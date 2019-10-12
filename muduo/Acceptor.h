#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_

#include <functional>

#include <tools/base/noncopyable.h>
#include <tools/socket/Socket.h>
#include <tools/socket/InetAddress.h>
#include <tools/socket/ServerSocket.h>

#include "EventLoop.h"
#include "Channel.h"

class Acceptor : public noncopyable {
public:
    typedef std::function<void(Socket*, const InetAddress&)> NewConnCallback;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reusePort);
    ~Acceptor();

    void set_newConnCallback(const NewConnCallback& cb){
        m_newConnCallback = cb;
    }

    bool is_listening() const {
        return m_listening;
    }

    void listen();

private:
    void handle_read();

private:
    EventLoop* m_loop;
    ServerSocket m_acceptSocket;
    Channel m_acceptChannel;
    NewConnCallback m_newConnCallback;
    bool m_listening;
    int m_idleFd;
};

#endif // ACCEPTOR_H_