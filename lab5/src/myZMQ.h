#pragma once

#include <cassert>
#include <cerrno>
#include <cstring>
#include <string>
#include <zmq.hpp>
#include <random>

enum actions_t {
    FAIL = 0,
    SUCCESS = 1,
    CREATE,
    DESTROY,
    BIND,
    PING,
    EXEC_CHECK,
    EXEC_ADD
};

const char* NODE_EXECUTABLE_NAME = "calculationNode";
const int PORT_BASE = 8000;
const int WAIT_TIME = 1000;
const char SENTINEL = '$';

struct node_token_t {
    actions_t action;
    long long parentId, id;
};

namespace myZMQ{

void initPairSocket(void* & context, void* & socket) 
{
    int rc;
    context = zmq_ctx_new();
    socket = zmq_socket(context, ZMQ_PAIR);

    rc = zmq_setsockopt(socket, ZMQ_RCVTIMEO, &WAIT_TIME, sizeof(int));
    assert(rc == 0);

    rc = zmq_setsockopt(socket, ZMQ_SNDTIMEO, &WAIT_TIME, sizeof(int));
    assert(rc == 0);
}

template <typename T>
void receiveMsg(T & replyData, void* socket)
{
    int rc = 0;
    zmq_msg_t reply;
    zmq_msg_init(&reply);

    rc = zmq_msg_recv(&reply, socket, 0);
    assert(rc == sizeof(T));

    replyData = *(T* )zmq_msg_data(&reply);
    rc = zmq_msg_close(&reply);
    assert(rc == 0); 
}

template <typename T>
bool receiveMsgWait(T & replyData, void* socket)
{
    int rc = 0;
    zmq_msg_t reply;
    zmq_msg_init(&reply);

    rc = zmq_msg_recv(&reply, socket, 0);

    if (rc == -1) {
        zmq_msg_close(&reply);
        return false;
    }

    assert(rc == sizeof(T));
    replyData = *(T* )zmq_msg_data(&reply);
    rc = zmq_msg_close(&reply);
    assert(rc == 0);

    return true;
}

template <typename T>
void sendMsg(T* token, void* socket)
{
    int rc = 0;
    zmq_msg_t message;
    zmq_msg_init(&message);

    rc = zmq_msg_init_size(&message, sizeof(T));
    assert(rc == 0);
    
    rc = zmq_msg_init_data(&message, token, sizeof(T), NULL, NULL);
    assert(rc == 0);

    rc = zmq_msg_send(&message, socket, 0);
    assert(rc == sizeof(T));
}

template <typename T>
bool sendMsgNoWait(T* token, void* socket)
{
    int rc;
    zmq_msg_t message;
    zmq_msg_init(&message);

    rc = zmq_msg_init_size(&message, sizeof(T));
    assert(rc == 0);

    rc = zmq_msg_init_data(&message, token, sizeof(T), NULL, NULL);
    assert(rc == 0);

    rc = zmq_msg_send(&message, socket, ZMQ_DONTWAIT);

    if (rc == -1) {
        zmq_msg_close(&message);
        return false;
    }

    assert(rc == sizeof(T));
    return true;
}

template <typename T>
bool sendMsgWait(T* token, void* socket)
{
    int rc;
    zmq_msg_t message;
    zmq_msg_init(&message);

    rc = zmq_msg_init_size(&message, sizeof(T));
    assert(rc == 0);

    rc = zmq_msg_init_data(&message, token, sizeof(T), NULL, NULL);
    assert(rc == 0);

    rc = zmq_msg_send(&message, socket, 0);
    if (rc == -1) {
        zmq_msg_close(&message);
        return false;
    }

    assert(rc == sizeof(T));
    return true;
}

template <typename T>
bool sendReceiveWait(T* tokenSend, T & tokenReply, void* socket)
{
    if (sendMsgWait(tokenSend, socket)) {
        if (receiveMsgWait(tokenReply, socket)) {
            return true;
        }
    }

    return false;
}

} // namespace myZMQ