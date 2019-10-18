#ifndef SUBJECT_H_
#define SUBJECT_H_

#include <tools/base/noncopyable.h>

class Observer;

class Subject : public noncopyable {
public:
    virtual void registerObserver(Observer*) = 0;

    virtual void unregisterObserver(Observer*) = 0;

    virtual void notifyObserver(int, int) = 0;

protected:
    Subject() = default;

    virtual ~Subject() = default;
};

#endif // SUBJECT_H_