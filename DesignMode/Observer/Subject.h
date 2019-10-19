#ifndef SUBJECT_H_
#define SUBJECT_H_

#include <mutex>
#include <memory> // shared_ptr weak_ptr

#include <tools/base/noncopyable.h>

class Observer;

class Subject : public noncopyable {
public:
    void registerObserver(std::weak_ptr<Observer> ob){
        m_observers.push_back(ob);
    }

    // virtual void unregisterObserver(Observer*) = 0; 移除操作->notifyObserver

    virtual void notifyObserver(int, int) = 0;

protected:
    Subject() = default;

    virtual ~Subject() = default;

    std::mutex m_mutex;
    std::vector<std::weak_ptr<Observer>> m_observers;
};

#endif // SUBJECT_H_