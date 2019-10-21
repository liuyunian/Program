#include <fcntl.h>  // open
#include <errno.h>  // errno

#include <tools/log/log.h>
#include <tools/socket/Socket.h>
#include <tools/socket/SocketsOps.h>

#include "muduo/Acceptor.h"
#include "muduo/EventLoop.h"

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reusePort) : 
    m_loop(loop),
    m_acceptSocket(sockets::create_nonblocking_socket(listenAddr.get_family())),
    m_acceptChannel(loop, m_acceptSocket.get_sockfd()),
    m_listening(false),
    m_idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    assert(m_idleFd >= 0);
    m_acceptSocket.set_reuse_address(true);
    m_acceptSocket.set_reuse_port(reusePort);
    m_acceptSocket.bind(listenAddr);
    
    m_acceptChannel.set_read_callback(std::bind(&Acceptor::handle_read, this));
}

Acceptor::~Acceptor(){
    m_acceptChannel.disable_all();
    m_acceptChannel.remove();
    sockets::close(m_idleFd);
}

void Acceptor::listen(){
    m_loop->assert_in_loop_thread();

    m_listening = true;
    m_acceptSocket.listen();
    m_acceptChannel.enable_reading();
}

void Acceptor::handle_read(){
    m_loop->assert_in_loop_thread();

    InetAddress peerAddr;
    Socket* connSocket = m_acceptSocket.accept_nonblocking(&peerAddr);
    if(connSocket == nullptr){
        if(errno == EMFILE){
            sockets::close(m_idleFd);
            connSocket = m_acceptSocket.accept_nonblocking(nullptr);
            delete connSocket;

            m_idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
            return;
        }

        LOG_SYSERR("Failed to accept in Acceptor::hand_read()");
        return;
    }

    if(m_connectionCallback){
        m_connectionCallback(connSocket, peerAddr);
    }
    else{
        delete connSocket;
    }
}