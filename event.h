//
//  event.h
//  Tetris
//
//  Created by lipeng on 14-5-19.
//  Copyright (c) 2014年 lpstudy. All rights reserved.
//

#ifndef Tetris_event_h
#define Tetris_event_h

//keyboard ascii
//ATTENTION: VK_UP's ascii code is equal to 'A'. thougth i can make a switch in event.c, but i really dont want to do that because there are already so many ascii code you can use.  when you use 65-68, you must be carefull about it.

#define XCODE_TEST 0 // when using xcode, VK_UP's ascii code is not 65 but 128, not like in terminal's running.
#if XCODE_TEST
    #define VK    239
    #define VK2   156
    #define VK_UP    128
    #define VK_DOWN  129
    #define VK_RIGHT 131
    #define VK_LEFT  130
#else
    #define VK    27
    #define VK2   91
    #define VK_UP    65
    #define VK_DOWN  66
    #define VK_RIGHT 67
    #define VK_LEFT  68
#endif//XCODE_TEST
extern const char VK_ALL_DIRECTION[4];

//time function
unsigned long GetTickCount();
#define TIME_NOW (GetTickCount())
#define TIME_PER_MILLISECOND (1)
#define TIME_PER_SECOND (1000)
#define TIME_PER_MINITE (TIME_PER_SECOND*60)
#define TIME_PER_HOUR (TIME_PER_MINITE*60)

//event type
//char type
#define EVENT_TYPE_STDIN_ID_BEGIN 1
#define EVENT_TYPE_STDIN_ID_END 255
//once timer type
#define EVENT_TYPE_TIMER_ID_BEGIN 1001
#define EVENT_TYPE_TIMER_ID_END 2000
//repeat timer type
#define EVENT_TYPE_REPEAT_TIMER_ID_BEGIN 2001
#define EVENT_TYPE_REPEAT_TIMER_ID_END 3000

typedef enum EVENT_TYPE
{
    EVENT_TYPE_TIMER,
    EVENT_TYPE_REPEAT_TIMER,
    EVENT_TYPE_STDIN,
    EVNET_TYPE_OTHERS,
}EVENT_TYPE;

typedef struct event_mgr event_mgr;
typedef struct event event;
typedef struct event_list_node *event_list, event_list_node;
typedef int (*EVENT_CALLBACK) (event_mgr*, event*, void*);


/*
 有空的话需要将此event转换为继承模式。将timer继承event
 这样就可以创建repeat n次的timer了，而不是将变量都加入到event结构中
 */
struct event
{
    int ev_id;
    EVENT_TYPE ev_type;
    EVENT_CALLBACK ev_callback;
    int ev_to_delete;
    
    int ev_delay_time;//millseconds
    long ev_elapse_time;
};

//event function
void event_dealloc(event* ev);
event* event_timer_init(int ev_id, int ev_delay_time, EVENT_CALLBACK timer_callback);
void event_timer_init_and_add(event_mgr* mgr, int ev_id, int ev_delay_time, EVENT_CALLBACK timer_callback);
event* event_repeat_timer_init(int ev_id, int ev_delay_time, EVENT_CALLBACK timer_callback);
void event_timer_repeat_init_and_add(event_mgr* mgr, int ev_id, int ev_delay_time, EVENT_CALLBACK timer_callback);
event* event_stdin_init(int ev_id, EVENT_CALLBACK stdin_callback);
void event_stdin_init_and_add(event_mgr* mgr,int ev_id, EVENT_CALLBACK stdin_callback);
event_list event_stdin_init_many(const char* ev_ids, int ev_ids_num, EVENT_CALLBACK stdin_callback);
void event_stdin_init_many_and_add(event_mgr* mgr, const char* ev_ids, int ev_ids_num, EVENT_CALLBACK stdin_callback);
void event_set_to_delete(event* ev);


//event_list
//单向链表，带头结点，头结点的ev不可访问。
struct event_list_node
{
    event* ev;
    struct event_list_node* next;
};
//event_list function



//event_mgr type
struct event_mgr
{
    event_list work_list;
    event_list toadd_list;
};
//event_mgr function
event_mgr* event_mgr_init();//alloc and init
event_mgr* event_mgr_init_attr(event_mgr* ev_mgr);// only init
void event_mgr_add_event(event_mgr* mgr, event* ev);
void event_mgr_add_event_list(event_mgr* mgr, event_list list);
void event_mgr_destory(event_mgr* mgr);
void driver_work(event_mgr* mgr, void* data);

#endif
