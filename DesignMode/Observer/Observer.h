#ifndef OBSERVER_H_
#define OBSERVER_H_

#include <memory>

#include <tools/base/noncopyable.h>

class Subject;

class Observer : public std::enable_shared_from_this<Observer>,
                 noncopyable {
public:
    void setSubject(Subject* sub){
        sub->registerObserver(shared_from_this());
        m_subject = sub;
    }

    virtual void update(int, int) = 0;

protected:
    Observer() = default;

    virtual ~Observer() = default;

    Subject* m_subject;
};

#endif // OBSERVER_H_