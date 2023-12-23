#include <iostream>
#include <map>
#include <unistd.h>

#include "myZMQ.h"

long long NodeID;

int main(int argc, char* argv[])
{
    std::string key;
    int val;
    std::map<std::string, int> dictionary;

    int rc;
    assert(argc == 2);

    NodeID = std::stoll(std::string(argv[1]));

    void* nodeParentContext = zmq_ctx_new();
    void* nodeParentSocket = zmq_socket(nodeParentContext, ZMQ_PAIR);

    rc = zmq_connect(nodeParentSocket, ("tcp://localhost:" + std::to_string(PORT_BASE + NodeID)).c_str()); 
    assert(rc == 0);

    long long childID = -1;
    void* nodeContext = nullptr;
    void* nodeSocket = nullptr;
    std::cout << "OK: " << getpid() << std::endl;

    bool hasChild = false, awake = true, add = false;

    while (awake) {
        node_token_t token({FAIL, 0, 0});
        myZMQ::receiveMsg(token, nodeParentSocket);

        auto* reply = new node_token_t({FAIL, NodeID, NodeID});

        /*
            Привязка может быть получена, когда родительский узел создал узел
            и этот узел должен быть привязан к дочернему элементу родителя.
        */ 
        if (token.action == BIND && token.parentId == NodeID) {
            myZMQ::initPairSocket(nodeContext, nodeSocket);

            rc = zmq_bind(nodeSocket, ("tcp://*:" + std::to_string(PORT_BASE + token.id)).c_str());
            assert(rc == 0);

            hasChild = true;
            childID = token.id;

            auto* tokenPing = new node_token_t({PING, childID, childID});
            node_token_t replyPing({FAIL, childID, childID});

            if (myZMQ::sendReceiveWait(tokenPing, replyPing, nodeSocket) && replyPing.action == SUCCESS) {
                reply->action = SUCCESS;
            }
        } else if (token.action == CREATE) {
            if (token.parentId == NodeID) {
                if (hasChild) {
                    rc  = zmq_close(nodeSocket);
                    assert(rc == 0);
                    rc = zmq_ctx_term(nodeContext);
                    assert(rc == 0);
                }

                myZMQ::initPairSocket(nodeContext, nodeSocket);

                rc = zmq_bind(nodeSocket, ("tcp://*:" + std::to_string(PORT_BASE + token.id)).c_str());
                assert(rc == 0);

                int forkID = fork();

                if (forkID == 0) {
                    rc = execl(NODE_EXECUTABLE_NAME, NODE_EXECUTABLE_NAME, std::to_string(token.id).c_str(), nullptr);
                    assert(rc != -1);
                    return 0;
                } else {
                    bool OK = true;

                    if (hasChild) {
                        auto* tokenBind = new node_token_t({BIND, token.id, childID});
                        node_token_t replyBind({FAIL, token.id, token.id});

                        OK = myZMQ::sendReceiveWait(tokenBind, replyBind, nodeSocket);
                        OK = OK && (replyBind.action == SUCCESS);
                    }

                    if (OK) {
                        auto* tokenPing = new node_token_t({PING, token.id, token.id});
                        node_token_t replyPing({FAIL, token.id, token.id});

                        OK = myZMQ::sendReceiveWait(tokenPing, replyPing, nodeSocket);
                        OK = OK && (replyPing.action == SUCCESS);

                        if (OK) {
                            reply->action = SUCCESS;
                            childID = token.id;
                            hasChild = true;
                        } else {
                            rc = zmq_close(nodeSocket);
                            assert(rc == 0);

                            rc = zmq_ctx_term(nodeContext);
                            assert(rc == 0);
                        }
                    }
                }
            } else if (hasChild) {
                auto* tokenDown = new node_token_t(token);
                node_token_t replyDown(token);
                replyDown.action = FAIL;
                if (myZMQ::sendReceiveWait(tokenDown, replyDown, nodeSocket) && replyDown.action == SUCCESS) {
                    *reply = replyDown;
                }
            }
        } else if (token.action == PING) {
            if (token.id == NodeID) {
                reply->action = SUCCESS;
            } else if (hasChild) {
                auto* tokenDown = new node_token_t(token);
                node_token_t replyDown(token);
                replyDown.action = FAIL;

                if (myZMQ::sendReceiveWait(tokenDown, replyDown, nodeSocket) && replyDown.action == SUCCESS) {
                    *reply = replyDown;
                }
            }
        } else if (token.action == DESTROY) {
            if (hasChild) {
                if (token.id == childID) {
                    bool OK;
                    auto* tokenDown = new node_token_t({DESTROY, NodeID, childID});
                    node_token_t replyDown = {FAIL, childID, childID};
                    OK = myZMQ::sendReceiveWait(tokenDown, replyDown, nodeSocket);
                    if (replyDown.action == DESTROY) {
                        rc = zmq_close(nodeSocket);
                        assert(rc == 0);

                        rc = zmq_ctx_destroy(nodeContext);
                        assert(rc == 0);

                        hasChild = false;
                        childID = -1;
                    } else if (replyDown.action == BIND) {
                        rc = zmq_close(nodeSocket);
                        assert(rc == 0);

                        rc = zmq_ctx_destroy(nodeContext);
                        assert(rc == 0);

                        myZMQ::initPairSocket(nodeContext, nodeSocket);
                        rc = zmq_bind(nodeSocket, ("tcp://*:" + std::to_string(PORT_BASE + replyDown.id)).c_str());
                        assert(rc == 0);
                        childID = replyDown.id;
                        auto* tokenPing = new node_token_t({PING, childID, childID});
                        node_token_t replyPing({FAIL, childID, childID});
                        OK = myZMQ::sendReceiveWait(tokenPing, replyPing, nodeSocket) && (replyPing.action == SUCCESS);
                    }

                    if (OK) {
                        reply->action = SUCCESS;
                    }
                } else if (token.id == NodeID) {
                    rc = zmq_close(nodeSocket);
                    assert(rc == 0);

                    rc = zmq_ctx_destroy(nodeContext);
                    assert(rc == 0);

                    awake = false;
                    reply->action = BIND;
                    reply->id = childID;
                    reply->parentId = token.parentId;
                } else {
                    auto* tokenDown = new node_token_t(token);
                    node_token_t replyDown = token;
                    replyDown.action = FAIL;
                    if (myZMQ::sendReceiveWait(tokenDown, replyDown, nodeSocket) && replyDown.action == SUCCESS) {
                        *reply = replyDown;
                    }
                }
            } else if (token.id == NodeID) {
                reply->action = DESTROY;
                awake = false;
            }
        } else if (token.action == EXEC_CHECK) {
            if (token.id == NodeID) {
                char c = token.parentId;

                if (c == SENTINEL) {
                    if (dictionary.find(key) != dictionary.end()) {
                        std::cout << "OK:" << NodeID << ":" << dictionary[key] << std::endl;
                    } else {
                        std::cout << "OK:" << NodeID << ":'" << key << "' not found" << std::endl;
                    }

                    reply->action = SUCCESS;
                    key = "";
                } else {
                    key += c;
                    reply->action = SUCCESS;
                }
            } else if (hasChild) {
                auto* tokenDown = new node_token_t(token);
                node_token_t replyDown(token);
                replyDown.action = FAIL;
                if (myZMQ::sendReceiveWait(tokenDown, replyDown, nodeSocket) && replyDown.action == SUCCESS) {
                    *reply = replyDown;
                }
            }
        } else if (token.action == EXEC_ADD) {
            if (token.id == NodeID) {
                char c = token.parentId;

                if (c == SENTINEL) {
                    add = true;
                    reply->action = SUCCESS;
                } else if (add) {
                    val = token.parentId;
                    dictionary[key] = val;
                    std::cout << "OK:" << NodeID << std::endl;
                    add = false;
                    key = "";
                    reply->action = SUCCESS;
                } else {
                    key += c;
                    reply->action = SUCCESS;
                }
            } else if (hasChild) {
                auto* tokenDown = new node_token_t(token);
                node_token_t replyDown(token);
                replyDown.action = FAIL;
                if (myZMQ::sendReceiveWait(tokenDown, replyDown, nodeSocket) && replyDown.action == SUCCESS) {
                    *reply = replyDown;
                }
            } 
        }
        myZMQ::sendMsgNoWait(reply, nodeParentSocket);
    } 

    rc = zmq_close(nodeParentSocket);
    assert(rc == 0);

    rc = zmq_ctx_destroy(nodeParentContext);
    assert(rc == 0);

    return 0;
}