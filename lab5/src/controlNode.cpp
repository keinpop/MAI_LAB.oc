#include <unistd.h>
#include <vector>
#include <zmq.hpp>

#include "myZMQ.h"
#include "topology.h"

using node_id_type = long long;

void deleteControlNode(node_id_type id, Topology_t<node_id_type> & controlNode, std::vector<std::pair<void* , void* >> children) 
{
    int ind = controlNode.find(id);
    int rc; 
    bool OK;

    if (ind != -1) {
        auto* token = new node_token_t({DESTROY, id, id});
        node_token_t reply({FAIL, id, id});
        OK = myZMQ::sendReceiveWait(token, reply, children[ind].second);

        if (reply.action == DESTROY && reply.parentId == id) {
            rc = zmq_close(children[ind].second);
            assert(rc == 0);

            rc = zmq_ctx_destroy(children[ind].first);
            assert(rc == 0);

            auto it = children.begin();

            while (ind--) {
                ++it;
            }

            children.erase(it);
        } else if (reply.action == BIND && reply.parentId == id) {
            rc = zmq_close(children[ind].second);
            assert(rc == 0);

            rc = zmq_ctx_term(children[ind].first);
            assert(rc == 0);

            myZMQ::initPairSocket(children[ind].first, children[ind].second);
            rc = zmq_bind(children[ind].second, ("tcp://*:" + std::to_string(PORT_BASE + id)).c_str());
            assert(rc == 0);
        }

        if (OK) {
            controlNode.erase(id);
            std::cout << "OK: " << id << std::endl;
        } else {
            std::cout << "Error: Node " << id << " is unavailable" << std::endl;
        }
    } else {
        std::cout << "Error: Not found" << std::endl;
    }
}

void help()
{
    std::cout << "\t\tUsage" << std::endl;
    std::cout << "Create id parent: create calculation node (use parent = -1 if parent is control node)" << std::endl;
    std::cout << "Ping id: ping calculation node with id $id" << std::endl;
    std::cout << "Remove id: delete calculation node with id $id" << std::endl;
    std::cout << "Exec id key val: add [key, val] add local dictionary" << std::endl;
    std::cout << "Exec id key: check local dictionary" << std::endl;
    std::cout << "Print 0: print topology" << std::endl;
}

int main()
{
    int rc;
    bool OK;
    Topology_t<node_id_type> controlNode;
    std::vector<std::pair<void* , void* >> children; // [context, socket]

    std::string str;
    node_id_type id;

    help();

    while (std::cin >> str >> id) {
        if ((str == "create") || (str == "Create")) {
            node_id_type parentID;
            std::cin >> parentID;

            int ind;

            if (parentID == -1) {
                void* newContext = nullptr;
                void* newSocket = nullptr;

                myZMQ::initPairSocket(newContext, newSocket);
                rc = zmq_bind(newSocket, ("tcp://*:" + std::to_string(PORT_BASE + id)).c_str());
                assert(rc == 0);

                int forkID = fork();

                if (forkID == 0) {
                    rc = execl(NODE_EXECUTABLE_NAME, NODE_EXECUTABLE_NAME, std::to_string(id).c_str(), nullptr);
                    assert (rc != -1);
                    return 0;
                } else {
                    auto* token = new node_token_t({PING, id, id});
                    node_token_t reply({FAIL, id, id});

                    if (myZMQ::sendReceiveWait(token, reply, newSocket) && (reply.action == SUCCESS)) {
                        children.emplace_back(std::make_pair(newContext, newSocket));
                        controlNode.insert(id);
                    } else {
                        rc = zmq_close(newSocket);
                        assert(rc == 0);
                        
                        rc = zmq_ctx_destroy(newContext);
                        assert(rc == 0); 
                    }
                }
            } else if ((ind = controlNode.find(parentID)) == -1) {
                std::cout << "Error: Not found" << std::endl;
                continue;
            } else {
                if (controlNode.find(id) != -1) {
                    std::cout << "Error: Already exists" << std::endl;
                    continue;
                }

                auto* token = new node_token_t({CREATE, parentID, id});
                node_token_t reply({FAIL, id, id});

                if (myZMQ::sendReceiveWait(token, reply, children[ind].second) && reply.action == SUCCESS) {
                    controlNode.insert(parentID, id);
                } else {
                    std::cout << "Error: Parent is unavailable" << std::endl;
                }
            }
        } else if (str == "remove" || str == "Remove") {
            deleteControlNode(id, controlNode, children);
        } else if (str == "ping" || str == "Ping") {
            int ind = controlNode.find(id);

            if (ind == -1) {
                std::cout << "Error: Not found" << std::endl;
                continue;
            }
            auto* token = new node_token_t({PING, id, id});
            node_token_t reply({FAIL, id, id});
            if (myZMQ::sendReceiveWait(token, reply, children[ind].second) && (reply.action == SUCCESS)) {
                std::cout << "OK: 1" << std::endl;
            } else {
                std::cout << "OK: 0" << std::endl;
            }
        } else if (str == "exec" || str == "Exec") {
            OK = true;
            std::string key;
            char c;
            int val = -1;
            bool add = false;

            std::cin >> key;
            if ((c = getchar()) == ' ') {
                add = true;
                std::cin >> val;
            }
            int ind = controlNode.find(id);
            if (ind == -1) {
                std::cout << "Error: Not found" << std::endl;
                continue;
            }

            key += SENTINEL;

            if (add) {
                for (auto i : key) {
                    auto* token = new node_token_t({EXEC_ADD, i, id});
                    node_token_t reply({FAIL, id, id});

                    if (!myZMQ::sendReceiveWait(token, reply, children[ind].second) || reply.action != SUCCESS) {
                        std::cout << "Fail: " << i << std::endl;
                        OK = false;
                        break;
                    }
                }
                auto* token = new node_token_t({EXEC_ADD, val, id});
                node_token_t reply({FAIL, id, id});

                if (!myZMQ::sendReceiveWait(token, reply, children[ind].second) || reply.action != SUCCESS) {
                    std::cout << "Fail: " << val << std::endl;
                    OK = false;
                }
            } else {
                for (auto i : key) {
                    auto* token = new node_token_t({EXEC_CHECK, i, id});
                    node_token_t reply({FAIL, id, id});

                    if (!myZMQ::sendReceiveWait(token, reply, children[ind].second) || (reply.action != SUCCESS)) {
                        OK = false;
                        std::cout << "Fail: " << i << std::endl;
                        break;
                    }
                }
            }
            if (!OK) {
                std::cout << "Error: Node is unavailable" << std::endl;
            }
        } else if (str == "print" || str == "Print") {
            std::cout << controlNode;
        }
       
    }

    std::cout << controlNode;

    for (auto i : controlNode._container) {
        for (size_t s = i.size(); s > 1; --s) {
            node_id_type last = i.back();
            deleteControlNode(last, controlNode, children);
            i.pop_back();
        }
    }

    std::vector<node_id_type> afterRoot;
    for (auto i : controlNode._container) {
        afterRoot.push_back(i.back());
    }

    for (auto i : afterRoot) {
        deleteControlNode(i, controlNode, children);
    }

    return 0;
}