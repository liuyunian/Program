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
    int state = SPRING;

    while(1){
        switch(state){
        case SPRING:
            spring_thing();
            state = SUMMER;
            break;
        case SUMMER:
            summer_thing();
            state = AUTUMN;
            break;
        case AUTUMN:
            autumn_thing();
            state = WINTER;
            break;
        case WINTER:
            winter_thing();
            state = SPRING;
            break;
        default:
            break;

        }
        
        sleep(1);
    }

    return 0;
}