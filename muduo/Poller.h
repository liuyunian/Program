#ifndef POLLER_H_
#define POLLER_H_

#include <tools_cxx/noncopyable>

#include "EventLoop.h"

/**
 * IO复用基类
*/
class Poller : noncopyable{
public:
    Poller(EventLoop * loop);
};

#endif // POLLER_H_