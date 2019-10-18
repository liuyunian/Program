#ifndef OBSERVER_H_
#define OBSERVER_H_

#include <tools/base/noncopyable.h>

class Observer : public noncopyable {
public:
    virtual void update(int, int) = 0;

protected:
    Observer() = default;

    virtual ~Observer() = default;
};

#endif // OBSERVER_H_