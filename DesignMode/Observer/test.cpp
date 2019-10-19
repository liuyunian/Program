#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>

#include "Subject.h"
#include "Observer.h"

class WeatherData : public Subject {
public:
    WeatherData() : 
        m_wd(0),
        m_sd(0){}

    void notifyObserver(int wd, int sd) override {
        std::unique_lock<std::mutex> ul(m_mutex);
        auto iter = m_observers.begin();
        while(iter != m_observers.end()){
            std::shared_ptr<Observer> ob(iter->lock());
            if(ob){ // 可以保证指向的Observer对象一定有效
                ob->update(wd, sd);
                ++ iter;
            }
            else{
                iter = m_observers.erase(iter);
            }
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
};

class PCDisplay : public Observer {
    void update(int wd, int sd){
        std::cout << "PC: 温度: " << wd << "湿度: " << sd << std::endl;
    }
};

class MobileDisplay : public Observer {
    void update(int wd, int sd){
        std::cout << "Mobile: 温度: " << wd << "湿度: " << sd << std::endl;
    }
};

int main(){
    WeatherData wd;
    std::shared_ptr<PCDisplay> pcd(new PCDisplay);
    std::shared_ptr<MobileDisplay> mod(new MobileDisplay);
    pcd->setSubject(&wd);
    mod->setSubject(&wd);
    wd.setWeatherData(23, 60);
    
    mod.reset();
    wd.setWeatherData(30, 40);

    return 0;
}

