#include <stdio.h>
#include <unistd.h> //sleep

// 状态
enum year_state{
    SPRING = 1,
    SUMMER,
    AUTUMN,
    WINTER
};

// 事件
enum year_event{
    EVENT1 = 1,
    EVENT2,
    EVENT3,
    EVENT4,
};

// 状态-动作表类型
typedef struct FsmTable_s{
    int event;   //事件
    int CurState;  //当前状态
    void (*eventActFun)();  //函数指针
    int NextState;  //下一个状态
}FsmTable_t;

// 状态机类型
typedef struct FSM_s{
    int curState;//当前状态
    FsmTable_t * pFsmTable;//状态表
    int size;//表的项数
}FSM_t;

/*状态机注册,给它一个状态表*/
void FSM_Regist(FSM_t* pFsm, FsmTable_t* pTable){
    pFsm->pFsmTable = pTable;
}

// 事件处理函数
void spring_thing(){
    printf("this is spring\n");
}

void summer_thing(){
    printf("this is summer\n");
}

void autumn_thing(){
    printf("this is autumn\n");
}

void winter_thing(){
    printf("this is winter\n");
}

/* 状态迁移 */
void FSM_StateTransfer(FSM_t* pFsm, int state){
    pFsm->curState = state;
}

/* 事件处理 */
void FSM_EventHandle(FSM_t* pFsm, int event){
    FsmTable_t * pActTable = pFsm->pFsmTable;
    void (*eventActFun)() = NULL;  // 函数指针初始化为空

    int CurState = pFsm->curState;
    int NextState;

    int flag = 0; // 标识是否满足条件

    /*获取当前动作函数*/
    for (int i = 0; i < pFsm->size; ++ i){
        if (event == pActTable[i].event && CurState == pActTable[i].CurState){ // 当且仅当当前状态下来个指定的事件，执行事件处理函数
            flag = 1;
            eventActFun = pActTable[i].eventActFun;
            NextState = pActTable[i].NextState;
            break;
        }
    }

    if(flag){
        if (eventActFun){
            eventActFun(); // 执行事件处理函数
        }
        
        FSM_StateTransfer(pFsm, NextState); // 跳转到下一个状态
    }
    else{
        printf("there is no match\n");
    }
}

// 状态-动作表
FsmTable_t year_table[] = {
    {EVENT1, SPRING, summer_thing, SUMMER},
    {EVENT2, SUMMER, autumn_thing, AUTUMN},
    {EVENT3, AUTUMN, winter_thing, WINTER},
    {EVENT4, WINTER, spring_thing, SPRING}
};

int main(){
    FSM_t year_fsm;
    FSM_Regist(&year_fsm, year_table);
    year_fsm.curState = SPRING;
    year_fsm.size = sizeof(year_table)/sizeof(FsmTable_t);
    while(1){
        FSM_EventHandle(&year_fsm, year_fsm.curState);
        sleep(1);
    }
    
    return 0;
}