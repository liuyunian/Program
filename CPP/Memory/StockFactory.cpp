#include <functional>
#include <iostream>
#include <string>
#include <memory>
#include <mutex>
#include <map>

#include <tools/base/noncopyable.h>

class Stock : noncopyable {
public:
    Stock(const std::string& name) : m_name(name) {
        std::cout << "执行了Stock构造函数，name = " << m_name << std::endl;
    }

    ~Stock(){
        std::cout << "执行了Stock析构函数 name = " << m_name << std::endl;
    }

    const std::string& getName() const {
        return m_name;
    }

private:
    std::string m_name;
};

namespace version1 {
class StockFactory : noncopyable {
public:
    std::shared_ptr<Stock> get(const std::string& key){
        std::unique_lock<std::mutex> ul(m_mutex);
        std::shared_ptr<Stock>& pStock = m_stocks[key];
        if(!pStock){
            pStock.reset(new Stock(key));
        }

        return pStock;
    }

private:
    std::mutex m_mutex;
    std::map<std::string, std::shared_ptr<Stock>> m_stocks;
};
} // namespace version

namespace version2 {
class StockFactory : noncopyable {
public:
    std::shared_ptr<Stock> get(const std::string& key){
        std::shared_ptr<Stock> pStock;
        std::unique_lock<std::mutex> ul(m_mutex);
        std::weak_ptr<Stock>& wkStock = m_stocks[key];
        pStock = wkStock.lock();
        if(!pStock){
            pStock.reset(new Stock(key));
            wkStock = pStock; // 可以这样直接赋值么？
        }

        return pStock;
    }

private:
    std::mutex m_mutex;
    std::map<std::string, std::weak_ptr<Stock>> m_stocks;
};
} // namespace version2

namespace version3 {
class StockFactory : noncopyable {
public:
    std::shared_ptr<Stock> get(const std::string& key){
        std::shared_ptr<Stock> pStock;
        std::unique_lock<std::mutex> ul(m_mutex);
        std::weak_ptr<Stock>& wkStock = m_stocks[key];
        pStock = wkStock.lock();
        if(!pStock){
            pStock.reset(new Stock(key), std::bind(&StockFactory::deleteStock, this, std::placeholders::_1));
            wkStock = pStock; // 可以这样直接赋值么？
        }

        return pStock;
    }

private:
    void deleteStock(Stock* stock){
        if(stock){
            std::unique_lock<std::mutex> ul(m_mutex);
            m_stocks.erase(stock->getName());
        }

        delete stock;
    }

private:
    std::mutex m_mutex;
    std::map<std::string, std::weak_ptr<Stock>> m_stocks;
};
} // namespace version3

namespace version4 {
class StockFactory : public std::enable_shared_from_this<StockFactory>,
                     noncopyable {
public:
    std::shared_ptr<Stock> get(const std::string& key){
        std::shared_ptr<Stock> pStock;
        std::unique_lock<std::mutex> ul(m_mutex);
        std::weak_ptr<Stock>& wkStock = m_stocks[key];
        pStock = wkStock.lock();
        if(!pStock){
            pStock.reset(new Stock(key), std::bind(&StockFactory::deleteStock, shared_from_this(), std::placeholders::_1));
            wkStock = pStock; // 可以这样直接赋值么？
        }

        return pStock;
    }

private:
    void deleteStock(Stock* stock){
        if(stock){
            std::unique_lock<std::mutex> ul(m_mutex);
            m_stocks.erase(stock->getName());
        }

        delete stock;
    }

private:
    std::mutex m_mutex;
    std::map<std::string, std::weak_ptr<Stock>> m_stocks;
};
} // namespace version4

class StockFactory : public std::enable_shared_from_this<StockFactory>,
                     noncopyable {
public:
    std::shared_ptr<Stock> get(const std::string& key){
        std::shared_ptr<Stock> pStock;
        std::unique_lock<std::mutex> ul(m_mutex);
        std::weak_ptr<Stock>& wkStock = m_stocks[key];
        pStock = wkStock.lock();
        if(!pStock){
            pStock.reset(new Stock(key), std::bind(&StockFactory::weadDeleteCallback, std::weak_ptr<StockFactory>(shared_from_this()), std::placeholders::_1));
            wkStock = pStock; 
        }

        return pStock;
    }

private:
    static void weadDeleteCallback(const std::weak_ptr<StockFactory>& wkSF, Stock* stock){
        std::shared_ptr<StockFactory> spSF(wkSF.lock());
        if(spSF){
            spSF->deleteStock(stock);
        }

        delete stock;
    }

    void deleteStock(Stock* stock){
        if(stock){
            std::unique_lock<std::mutex> ul(m_mutex);
            m_stocks.erase(stock->getName());
        }
    }

private:
    std::mutex m_mutex;
    std::map<std::string, std::weak_ptr<Stock>> m_stocks;
};

int main(){
    // version1::StockFactory sf;
    // version2::StockFactory sf;
    // version3::StockFactory sf;
    // version4::StockFactory sf; // Aborted (core dumped)
    // std::shared_ptr<version4::StockFactory> sf(new version4::StockFactory);
    std::shared_ptr<StockFactory> sf(new StockFactory);
    sf->get("google");
    sf->get("MircroSoft");

    return 0;
}