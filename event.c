//
//  event.c
//  Tetris
//
//  Created by lipeng on 14-5-19.
//  Copyright (c) 2014年 lpstudy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "terminal.h"
#include "event.h"
#include "log.h"

const char VK_ALL_DIRECTION[4] = { VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN};

void event_list_add_event(event_list list, event* ev);
event_list event_list_init();


unsigned long GetTickCount()
{
    struct timeval tv;
    if( gettimeofday(&tv, NULL) != 0)
        return 0;
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
/**
 *  event定义
 *  timer_id >= 1000-2000
 */
event* event_init(int ev_id, EVENT_TYPE ev_type, int ev_delay_time, EVENT_CALLBACK ev_callback)
{
    event* ev = (event*)malloc(sizeof(struct event));
    assert(ev);
    ev->ev_id = ev_id;
    ev->ev_type = ev_type;
    ev->ev_callback = ev_callback;
    ev->ev_to_delete = 0;

    //stdin type Never Timeout
    if(ev_type == EVENT_TYPE_STDIN){
        ev->ev_elapse_time = -1;
    }else{
        ev->ev_delay_time = ev_delay_time;
        ev->ev_elapse_time = TIME_NOW + ev_delay_time;
    }
    trace_event(ev);
    return ev;
}
//释放event
void event_dealloc(event* ev)
{
    if(ev){
        trace_event(ev);
        free(ev);
    }
}
//初始化timer event
event* event_timer_init(int ev_id, int ev_delay_time, EVENT_CALLBACK timer_callback)
{
    assert(ev_id >= EVENT_TYPE_TIMER_ID_BEGIN && ev_id <= EVENT_TYPE_TIMER_ID_END );
    return event_init(ev_id, EVENT_TYPE_TIMER, ev_delay_time, timer_callback);
}
void event_timer_init_and_add(event_mgr* mgr, int ev_id, int ev_delay_time, EVENT_CALLBACK timer_callback)
{
    assert(mgr);
    event* ev = event_timer_init(ev_id, ev_delay_time, timer_callback);
    event_mgr_add_event(mgr, ev);
}
//初始化repeat_timer event
event* event_repeat_timer_init(int ev_id, int ev_delay_time, EVENT_CALLBACK timer_callback)
{
    assert(ev_id >= EVENT_TYPE_REPEAT_TIMER_ID_BEGIN && ev_id <= EVENT_TYPE_REPEAT_TIMER_ID_END );
    return event_init(ev_id, EVENT_TYPE_REPEAT_TIMER, ev_delay_time, timer_callback);
}
void event_timer_repeat_init_and_add(event_mgr* mgr, int ev_id, int ev_delay_time, EVENT_CALLBACK timer_callback)
{
    event* ev = event_repeat_timer_init(ev_id, ev_delay_time, timer_callback);
    event_mgr_add_event(mgr, ev);
}
//初始化stdin event
event* event_stdin_init(int ev_id, EVENT_CALLBACK stdin_callback)
{
    assert(ev_id >= EVENT_TYPE_STDIN_ID_BEGIN && ev_id <= EVENT_TYPE_STDIN_ID_END );
    return event_init(ev_id, EVENT_TYPE_STDIN,0, stdin_callback);
}
void event_stdin_init_and_add(event_mgr* mgr,int ev_id, EVENT_CALLBACK stdin_callback)
{
    event* ev = event_stdin_init(ev_id, stdin_callback);
    event_mgr_add_event(mgr, ev);
}
event_list event_stdin_init_many(const char* ev_ids, int ev_ids_num, EVENT_CALLBACK stdin_callback)
{
    int i = 0;
    int ev_id = 0;
    event_list list = event_list_init();

    for (; ev_ids[i] != '\0' &&  i < ev_ids_num; ++i) {
        ev_id = (unsigned char)ev_ids[i];
        assert(ev_id >= EVENT_TYPE_STDIN_ID_BEGIN && ev_id <= EVENT_TYPE_STDIN_ID_END );
        event* ev = event_init(ev_id, EVENT_TYPE_STDIN,0, stdin_callback);
        event_list_add_event(list, ev);
    }
    return list;
}
void event_stdin_init_many_and_add(event_mgr* mgr, const char* ev_ids, int ev_ids_num, EVENT_CALLBACK stdin_callback)
{
    event_list list = event_stdin_init_many(ev_ids, ev_ids_num, stdin_callback);
    event_mgr_add_event_list(mgr, list);
}
void event_set_to_delete(event* ev)
{
    ev->ev_to_delete = 1;
}


/**
 *  event_list
 */


//设置event_list的某一个节点的ev和next值
void event_list_node_set(event_list_node* node ,event* ev, event_list_node* next)
{
    node->ev = ev;
    node->next = next;
}
//设置ev值，next值为null
void event_list_node_set_ev(event_list_node* node ,event* ev)
{
    event_list_node_set(node, ev, NULL);
}
void event_list_node_destroy(event_list_node* node)
{
    trace_event_list_node(node);
    event_dealloc(node->ev);
    free(node);
}

event_list event_list_node_init()
{
    event_list_node* list = (event_list_node*)malloc(sizeof(struct event_list_node));
    assert(list);
    list->ev = NULL;
    list->next = NULL;
    trace_event_list_node(list);
    return list;
}
//头插法插入event
void event_list_add_event(event_list list, event* ev)
{
    assert(list && ev);
    event_list_node* node = event_list_node_init();
    event_list_node_set_ev(node, ev);
    node->next = list->next;
    list->next = node;
}
event_list event_list_init()
{
    return event_list_node_init();
}
void event_list_destory(event_list list)
{
    event_list head = list;
    event_list_node* cur_node;
    while (head) {
        cur_node = head;
        event_list_node_destroy(cur_node);
        head = head->next;
    }
}


/*
 event_mgr
 */
event_mgr* event_mgr_init()
{
    event_mgr* ev_mgr = (event_mgr*)malloc(sizeof(struct event_mgr));
    assert(ev_mgr);
    return event_mgr_init_attr(ev_mgr);
}
event_mgr* event_mgr_init_attr(event_mgr* ev_mgr)
{
    //set stdinput non block
    set_nonblock(STDIN_FILENO);
    set_no_buffer_std_input();
    set_no_buffer_std_output();

    ev_mgr->work_list = event_list_init();
    ev_mgr->toadd_list = event_list_init();
    return ev_mgr;
}
void event_mgr_add_event(event_mgr* mgr, event* ev)
{
    event_list_add_event(mgr->toadd_list, ev);
}
void event_mgr_add_event_list(event_mgr* mgr, event_list list)
{
    if (list != NULL) {
        event_list list_head = list;
        event_list toadd_list = mgr->toadd_list;

        while (list->next != NULL) {
            list = list->next;
        }
        list->next = toadd_list->next;
        toadd_list->next = list_head->next;
    }
}
void event_mgr_destory(event_mgr* mgr)
{
    event_list_destory(mgr->work_list);
    event_list_destory(mgr->toadd_list);
    free(mgr);

    //restore terminal buffer type
    restore_old_buffer_std_input();
}

int read_stdin_char(unsigned char* ch)
{
    int read_count = (int)read(STDIN_FILENO, ch, 1);
    if(*ch == VK)
    {
        if(getchar() == VK2)
        {
            switch(*ch = getchar())
            {
                case VK_UP:
                case VK_DOWN:
                case VK_LEFT:
                case VK_RIGHT:
                    break;
                default:
                {
                    //other kinds of VK_KEY not handled
                    read_count = 0;
                }
                    break;
            }
        }
    }

    return read_count;
}
void driver_work(event_mgr* mgr, void* data)
{
    //there are some events to add
    event_list toadd_list = mgr->toadd_list;

    if (toadd_list->next != NULL) {
        //add toadd_list to work_list
        event_list work_list = mgr->work_list;
        while (work_list->next != NULL) {
            work_list = work_list->next;
        }
        work_list->next = toadd_list->next;
        toadd_list->next = NULL;
    }

    //delete to_delete event
    event_list_node* prev = mgr->work_list;
    event_list_node* current = prev->next;

    //read from std input: O_NONBLOCK
    unsigned char read_char;
    int read_count = read_stdin_char(&read_char);
    while (current)
    {
        event* ev = current->ev;
        if(ev->ev_to_delete)
        {
            prev->next = current->next;
            event_list_node_destroy(current);
            current = prev->next;
        }
        else{
            prev = current;
            current = current->next;

            switch (ev->ev_type) {
                case EVENT_TYPE_TIMER:
                case EVENT_TYPE_REPEAT_TIMER:
                {
                    if(ev->ev_elapse_time <= TIME_NOW)
                    {
                        if(ev->ev_callback)
                        {
                            (ev->ev_callback)(mgr, ev, data);
                        }

                        if(ev->ev_type == EVENT_TYPE_TIMER){
                            ev->ev_to_delete = 1;
                        }
                        else{
                            ev->ev_elapse_time = TIME_NOW+ev->ev_delay_time;
                        }
                    }
                }
                    break;
                case EVENT_TYPE_STDIN:
                {
                    if(read_count >= 1 && ev->ev_id == read_char &&  ev->ev_callback)
                    {
                        (ev->ev_callback)(mgr, ev, data);
                    }
                }
                    break;
                default:
                    break;
            }


        }
    }
}
