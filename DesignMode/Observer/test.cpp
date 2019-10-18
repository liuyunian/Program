#include <iostream>
#include <vector>
#include <algorithm>

#include "Observer.h"
#include "Subject.h"

class WeatherData : public Subject {
public:
    WeatherData() : 
        m_wd(0),
        m_sd(0){}

    void registerObserver(Observer* ob) override {
        m_observers.push_back(ob);
    }

    void unregisterObserver(Observer* ob) override {
        auto iter = std::find(m_observers.begin(), m_observers.end(), ob);
        if(iter != m_observers.end()){
            std::swap(*iter, m_observers.back());
            m_observers.pop_back();
        }
    }

    void notifyObserver(int wd, int sd) override {
        for(auto& ob : m_observers){
            ob->update(wd, sd);
        }
    }

    void setWeatherData(int wd, int sd){
        m_wd = wd;
        m_sd = sd;

        notifyObserver(wd, sd);
    }

private:
    int m_wd;
    int m_sd;

    std::vector<Observer*> m_observers;
};

class PCDisplay : public Observer {
public:
    PCDisplay(WeatherData* wd) : 
        m_wd(wd)
    {
        m_wd->registerObserver(this);
    }

    ~PCDisplay(){
        m_wd->unregisterObserver(this);
    }

    void update(int wd, int sd){
        std::cout << "PC: 温度: " << wd << "湿度: " << sd << std::endl;
    }

private:
    WeatherData* m_wd;
};

class MobileDisplay : public Observer {
public:
    MobileDisplay(WeatherData* wd) : 
        m_wd(wd)
    {
        m_wd->registerObserver(this);
    }

    ~MobileDisplay(){
        m_wd->unregisterObserver(this);
    }

    void update(int wd, int sd){
        std::cout << "Mobile: 温度: " << wd << "湿度: " << sd << std::endl;
    }

private:
    WeatherData* m_wd;
};

int main(){
    WeatherData wd;
    PCDisplay* pc = new PCDisplay(&wd);
    MobileDisplay* mob = new MobileDisplay(&wd);

    wd.setWeatherData(23, 60);
    
    delete mob;
    wd.setWeatherData(30, 40);

    return 0;
}

