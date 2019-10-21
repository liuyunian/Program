#ifndef CALLBACK_H_
#define CALLBACK_H_

#include <functional>
#include <memory>

// Timer
typedef std::function<void()> TimerCallback;

// TcpConnection
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&, const char* data, ssize_t len)> MessageCallback;


#endif // CALLBACK_H_