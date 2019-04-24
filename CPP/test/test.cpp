#include <iostream>
#include <memory>
#include <vector>

// std::shared_ptr<int> test(int a){

//     return new int(a);
// }

// void test(std::shared_ptr<int> sp){

// }

// void myDelete(int * p){
//     std::cout << "执行了自定义删除器" << std::endl;
//     delete p;
// }

// class A{
// public:
//     A(){}
//     ~A(){}
// };

// void test(std::shared_ptr<int> sp){
//     std::cout << "执行了test() " << sp.use_count() << std::endl;
// }

// void test(int * p){
//     std::shared_ptr<int> sp(p);
// }

// class A : public std::enable_shared_from_this<A>{
// public:
//     std::shared_ptr<A> getThis(){
//         return shared_from_this();
//     }
// };

// auto test(int data){
//     return std::make_unique<int>(data);
// }

// int main(){
    // std::shared_ptr<int> p(new int{2});
    // *p = 2;
    // std::shared_ptr<int> p = new int(2);
    // std::shared_ptr<int> sp = std::make_shared<int>();
    // *sp = 1;

    // auto sp = std::make_shared<std::string>("hello");
    // auto sp = test(1);

    // auto sp = std::make_shared<int>(10);
    // std::cout << sp.unique() << std::endl;

    // auto sp2(sp);
    // std::cout << sp.unique() << std::endl;
    // std::cout << sp2.unique() << std::endl;
    // int count = sp.use_count();
    // std::cout << count << std::endl;
    // auto sp2(sp);
    // count = sp.use_count();
    // int count2 = sp2.use_count();
    // std::cout << count << std::endl;
    // std::cout << count2 << std::endl;

    // auto sp = std::make_shared<int>(10);
    // std::cout << sp.use_count() << std::endl;
    // std::cout << *sp << std::endl;
    // sp.reset(new int(100));
    // // if(sp == nullptr){
    // //     std::cout << "sp == nullptr" << std::endl;
    // // }
    // std::cout << sp.use_count() << std::endl;
    // std::cout << *sp << std::endl;

    // auto sp1 = std::make_shared<std::string>("I");
    // sp1 = nullptr;
    // if(sp1){
    //     std::cout << "sp1不为空" << std::endl;
    // }
    // else{
    //     std::cout << "sp1为空" << std::endl;
    // }

    // std::cout << sp1.use_count() << std::endl;
    // sp1 = nullptr;
    // std::cout << sp1.use_count() << std::endl;
    // auto sp2 = std::make_shared<std::string>("you");
    // std::cout << *sp1 << ", " << *sp2 << std::endl;

    // sp1.swap(sp2);
    // std::cout << *sp1 << ", " << *sp2 << std::endl;

    // std::shared_ptr<int> sp(new int(100), [](int * p){
    //     std::cout << "执行了自定义删除器" << std::endl;
    //     delete p;
    // });

    // auto sp2(sp);
    // sp.reset();
    // std::cout << sp2.use_count() << std::endl;
    // sp2.reset();
    // std::cout << sp2.use_count() << std::endl;
    // int * p = new int[10];
    // delete p;

    // A * ap = new A[10];
    // delete ap;

    // std::shared_ptr<A> ap(new A[10], [](A * p){
    //     delete[] p;
    // });

    // std::shared_ptr<A> ap(new A[10], std::default_delete<A[]>());

    // std::shared_ptr<int> sp1(new int(10), [](int * p){
    //     std::cout << "执行sp1的自定义删除器" << std::endl;
    //     delete p;
    // });

    // std::shared_ptr<int> sp2(new int(10), [](int * p){
    //     std::cout << "执行sp2的自定义删除器" << std::endl;
    //     delete p;
    // });

    // std::vector<std::shared_ptr<int>> v{sp1, sp2};
    // auto sp = std::make_shared<int>(100);
    // std::weak_ptr<int> wp(sp);
    // // std::cout << *wp << std::endl;

    // auto sp2 = wp.lock();
    // if(sp2 != nullptr){
    //     std::cout << *sp2 << std::endl;
    // }
    // else{
    //     std::cout << "lock失败" << std::endl;
    // }

    // std::weak_ptr<int> wp;
    // wp = sp;

    // std::cout << sp.use_count() << std::endl;

    // auto sp1 = std::make_shared<std::string>("hello");
    // std::shared_ptr<std::string> sp2(sp1);

    // std::weak_ptr<std::string> wp(sp2);
    // std::cout << wp.use_count() << std::endl;

    // std::cout << wp.expired() << std::endl;

    // sp1.reset();
    // sp2.reset();

    // std::cout << wp.expired() << std::endl;

    // int * p = new int(1);
    // std::shared_ptr<int> sp(p);
    // std::weak_ptr<int> wp(sp);

    // std::cout << sizeof(p) << std::endl;
    // std::cout << sizeof(sp) << std::endl;
    // std::cout << sizeof(wp) << std::endl;

    // int * p = new int(10);
    // test(std::shared_ptr<int>(p));

    // *p = 20;
    // std::cout << *p << std::endl;

    // std::shared_ptr<int> sp1(p);
    // std::cout << sp1.use_count() << std::endl;

    // std::shared_ptr<int> sp2(p);
    // std::cout << sp2.use_count() << std::endl;

    // auto sp = std::make_shared<int>(100);
    // test(sp.get());
    // auto sp = std::make_shared<A>();
    // std::cout << sp.use_count() << std::endl;
    // auto sp2 = sp->getThis();
    // std::cout << sp.use_count() << std::endl;
    // auto sp = std::make_shared<int>(100);
    // auto sp2(std::move(sp));

    // std::cout << sp.use_count() << std::endl;
    // std::cout << sp2.use_count() << std::endl;

    // auto sp3 = std::move(sp2);
    // std::cout << sp3.use_count() << std::endl;
    // std::cout << sp2.use_count() << std::endl;

    // std::unique_ptr<int> up(new int(100));
    // std::unique_ptr<int> up2 = std::make_unique<int>(10);
    // auto up3 = std::make_unique<int>(1);

    // auto up = std::make_unique<std::string>("hello");
    // std::unique_ptr<std::string> up2(up);

    // std::unique_ptr<std::string> up3 = up;

    // std::unique_ptr<std::string> up4;
    // up4 = up;


    // auto up = std::make_unique<std::string>("hello");
    // std::unique_ptr<std::string> up2(std::move(up));
    // std::unique_ptr<std::string> up3 = std::move(up2);
    // std::unique_ptr<std::string> up4;
    // up4 = std::move(up3);

    // auto up = std::make_unique<std::string>("hello");
    // // std::cout << *(up.release()) << std::endl;
    // std::unique_ptr<std::string> up2(up.release());

    // // std::string * str_p = up2.release();
    // // delete str_p;

    // std::shared_ptr<std::string> sp(up2.release());

    // auto up = std::make_unique<std::string>("hello");

    // up.reset(new std::string("lyn"));
    // std::cout << *up << std::endl;

    // std::unique_ptr<int[]> up(new int[10]);
    // std::shared_ptr<int[]> sp(new int[10]);

    // auto up = std::make_unique<int>(10);
    // // std::shared_ptr<int> sp(std::move(up));

    // std::shared_ptr<int> sp;
    // sp = std::move(up);


    // std::cout << sp.use_count() << std::endl;
    // std::cout << *sp << std::endl;

    // auto up(test(1));
    // std::cout << *up << std::endl;
//     return 0;
// }

void myDelete(std::string * p){
    std::cout << "执行了自定义删除器" << std::endl;
    delete p;
}

int main(){
    typedef void (*Func)(std::string *);
    std::unique_ptr<std::string, Func> up(new std::string("hello"), myDelete);

    using Func = void(*)(std::string *);
    std::unique_ptr<std::string, Func> up2(new std::string("hello"), myDelete);

    std::unique_ptr<std::string, std::function<void(std::string *)>> up3(new std::string("hello"), myDelete);

    std::unique_ptr<std::string, decltype(myDelete) *> up4(new std::string("hello"), myDelete);

    std::unique_ptr<std::string, std::function<void(std::string *)>> up5(new std::string("hello"), [](std::string * p){
        std::cout << "执行了自定义删除器" << std::endl;
        delete p;
    });

    // std::string * p = new std::string("hello");
    // std::cout << sizeof(p) << std::endl;
    
    // std::unique_ptr<std::string> up(p);

    std::cout << sizeof(up) << std::endl;
    std::cout << sizeof(up2) << std::endl;
    std::cout << sizeof(up3) << std::endl;
    std::cout << sizeof(up4) << std::endl;
    std::cout << sizeof(up5) << std::endl;
    return 0;
}