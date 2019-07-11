#include <stdio.h>
#include <unistd.h> // sleep

enum year_state {
    SPRING,
    SUMMER,
    AUTUMN,
    WINTER
};

void spring_thing(){
    printf("hello spring\n");
}

void summer_thing(){
    printf("hello summer\n");
}

void autumn_thing(){
    printf("hello autumn\n");
}

void winter_thing(){
    printf("hello winter\n");
}

int main(){
    int state = SPRING; //　初始状态
    while (1){
        if (state == SPRING){
            spring_thing(); //　相应的处理
            state = SUMMER; //　状态改变
        }
        else if (state == SUMMER){
            summer_thing();
            state = AUTUMN;
        }
        else if (state == AUTUMN){
            autumn_thing();
            state = WINTER;
        }
        else if (state == WINTER){
            winter_thing();
            state = SPRING;
        }

        sleep(1);
    }

    return 0;
}