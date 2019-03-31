#include <iostream>

class Student{
private:
    int num;
    std::string name;

public:
    explicit Student(int num){
        this->num = num;
        name = "zhangsan";
        std::cout << "单参数构造函数, num = " << num << std::endl;
    }

    Student(){}
};

void func(Student s){

}

int main(){
    Student s = Student(1);
    Student s2(2);

    Student * s3 = new Student;

    // Student s3 = 3;
    // func(4);
    return 0;
}