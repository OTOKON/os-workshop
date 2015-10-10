#include <setjmp.h>
#include <stdio.h>

/**
 *  Type definitions
 */
typedef void (task_func_t)(void);

typedef enum {
    TASK_STATE_NONE,
    TASK_STATE_IDLE,
    TASK_STATE_RUNNING,
    TASK_STATE_PAUSED,
    TASK_STATE_READY,
}task_state_t;

typedef struct {
    task_state_t state;
    task_func_t *p_func;
    jmp_buf env;
}task_t;


/**
 *  Global variables
 */
static jmp_buf root_env;
task_t *p_current_task = NULL;
task_t tasks[2];


/**
 *  Function declarations
 */

void task_root();
void task_init();
void task_add(task_func_t *p_func);
void task_yield();
task_t* task_search(task_func_t *p_func);
task_t* task_get_next();
void task_rerun(task_func_t *p_func);

/**
 *  Task functions (jobs)
 */
void task1(void);
void task2(void);


int main(void){
    task_init();
    task_add(&task1);
    task_add(&task2);
    task_root();
    
    return 0;
}

/**
 *  Function definitions
 */
task_t* task_get_next(){
    if (tasks[0].state == TASK_STATE_READY) {
        return &tasks[0];
    }else{
        return &tasks[1];
    }
}

void task_init(){
    tasks[0].state = TASK_STATE_NONE;
    tasks[1].state = TASK_STATE_NONE;
}

void task_yield(){
    /* TASK YIELD */
    // change the state of the task
    
    printf("\ntask_yield running");
    p_current_task->state = TASK_STATE_PAUSED;
    if(!setjmp(p_current_task->env)){
        longjmp(root_env, 1);
    }
    longjmp(p_current_task->env,1);
}

void task_root(){
    printf("\ntask_root running");
    setjmp(root_env);
    
    p_current_task = task_get_next();
    
    if (TASK_STATE_IDLE == p_current_task->state || TASK_STATE_READY == p_current_task->state) {
        p_current_task->state = TASK_STATE_RUNNING;
        p_current_task->p_func();
    }else{
        longjmp(p_current_task->env,1);
    }
}

task_t* task_search(task_func_t *p_func){
    if (tasks[0].p_func == p_func) {
        return &tasks[0];
    }else{
        return &tasks[1];
    }
}

void task_rerun(task_func_t *p_func){
    task_t* p_task = task_search(p_func);
    p_task->state = TASK_STATE_READY;
}

void task_add(task_func_t *p_func){
    // listeye pointerları yerleştir
    printf("\ntask_add()");
    if (tasks[0].state == TASK_STATE_NONE) {
        tasks[0].state = TASK_STATE_IDLE;
        tasks[0].p_func = p_func;
    }else if(tasks[1].state == TASK_STATE_NONE){
        tasks[1].state = TASK_STATE_IDLE;
        tasks[1].p_func = p_func;
    }else{
        printf("ERROR CANT ADD TASK!");
    }
}


void task1(void){
    while (1) {
        // do sth
        printf("\ntask_1()");
        task_rerun(&task2);
        task_yield();
    }
}

void task2(void){
    while (1) {
        // do sth
        printf("\ntask_2()");
        task_rerun(&task1);
        task_yield();
    }
}
