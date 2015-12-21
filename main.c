//
//  main.c
//  snake
//
//  Created by lipeng on 14-5-20.
//  Copyright (c) 2014年 lpstudy. All rights reserved.
//


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "terminal.h"
#include "event.h"
#include "log.h"
#include "cursor.h"


#define snake_game_title_len (5)
typedef struct snake_game_size snake_game_size;
typedef struct snake_point snake_point;
typedef struct snake snake;
typedef struct snake_game snake_game;
typedef struct snake_point_list_node snake_point_list_node;
typedef struct snake_point_list snake_point_list;

typedef enum snake_point_color_type{
    snake_point_color_type_none,
    snake_point_color_type_red,
    snake_point_color_type_yellow,
}snake_point_color_type;

typedef enum snake_direction
{
    snake_direction_left,
    snake_direction_right,
    snake_direction_up,
    snake_direction_down,
    snake_direction_none,
}snake_direction;

typedef enum snake_game_run_state{
    snake_game_run_state_stoped = 0,
    snake_game_run_state_running,
    snake_game_run_state_pausing,
}snake_game_run_state;

struct snake_game_size
{
    int width;
    int height;
};
struct snake_point
{
    int x;
    int y;
};

snake_point snake_point_make(int x, int y);

void snake_game_print_line(int width)
{
    for (int i = 0; i < width; ++i) {
        printf("-");
    }
    printf("\n");
}
void snake_game_size_set(snake_game_size* sg_size, int width, int height)
{
    sg_size->width = width;
    sg_size->height = height;
    MOVETO_POINT(snake_point_make(0, 0));
    snake_game_print_line(width);
    const char* title = "Snake Game";
    int left = (width-(int)strlen(title)) / 2;
    if (left < 0) {
        left = 0;
    }
    char buf[1024];
    memset(buf, ' ', left);
    buf[left] = '\0';
    printf("\n%s%s\n\n", buf, title);
    snake_game_print_line(width);

    /*

     */

    MOVETO_POINT(snake_point_make(0, snake_game_title_len+height));
    snake_game_print_line(width);

    for (int i = 0; i <= snake_game_title_len+height; ++i) {
        MOVETO_POINT(snake_point_make(width, i));
        printf("|");
    }

}
int snake_point_is_equal(snake_point left, snake_point right)
{
    if( (left.x == right.x) && (left.y == right.y) )
        return 1;
    return  0;
}
void snake_point_set(snake_point* sna_point, int x, int y)
{
    sna_point->x = x;
    sna_point->y = y;
}
snake_point snake_point_make(int x, int y)
{
    snake_point point;
    snake_point_set(&point, x, y);
    return point;
}
struct snake_point_list_node
{
    snake_point point;
    struct snake_point_list_node* next;
};
struct snake_point_list
{
    //tail->next->next->next == head
    //便于删除尾部节点，修改其x，y值并将其加入到头结点
    snake_point_list_node* head;
    snake_point_list_node* tail;

    int length;
};
struct snake
{
    char sn_point_type;//用什么字符代表snake
    snake_direction sn_directin;//snake运行方向
    int  sn_point_num;
//    snake_point sn_point_array[1000];//组成snake的point array
    snake_point_list* sn_point_list;
    snake_point sn_to_eat_point; //随机生成的点
};

struct snake_game
{
    event_mgr sg_mgr;
    snake_game_size sg_size;
    snake *sg_snake;

    snake_game_run_state sg_is_run;
    int sg_is_change_direct;
};

snake_game *snake_game_init();
void snake_game_destroy(snake_game* sg_game);
void snake_game_set_size(snake_game* sg_game, snake_game_size sg_size);
void snake_game_set_snake(snake_game* sg_game, snake *sg_snake);
void snake_game_set_run_stop(snake_game* sg_game);
int snake_game_is_running(snake_game* sg_game);
snake_point snake_game_random_point(snake_game* sg_game);
void snake_game_set_pausing(snake_game* sg_game);
int snake_game_is_pausing(snake_game* sg_game);
void snake_game_set_running(snake_game* sg_game);
int snake_has_point(snake* sn, snake_point point);
int snake_game_conflict_point(snake_game* sg_game, snake_point point);

void snake_point_list_node_destory(snake_point_list_node* node);

// tail <-- A <-- B <-- C <-- head
snake_point_list* snake_point_list_init()
{
    snake_point_list* list = (snake_point_list*)malloc(sizeof(struct snake_point_list));
    assert(list);
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;

    trace_snake_list(list);
    return list;
}
void snake_point_list_destroy(snake_point_list* list)
{
    snake_point_list_node* head = list->head;
    snake_point_list_node* current;
    while (head)
    {
        current = head->next;
        trace_snake_list_node(head);
        snake_point_list_node_destory(head);
        head = current;
        --list->length;
    }

    assert(0 == list->length);
    trace_snake_list(list);
    free(list);
}
snake_point_list_node* snake_point_list_node_init(snake_point point)
{
    snake_point_list_node* node = (snake_point_list_node*)malloc(sizeof(struct snake_point_list_node));
    assert(node);
    node->point = point;
    node->next = NULL;

    trace_snake_list_node(node);
    return  node;
}
void snake_point_list_node_destory(snake_point_list_node* node)
{
    assert(node);
    trace_snake_list_node(node);
    free(node);
}
void snake_point_list_add_tail_node(snake_point_list* list, snake_point_list_node* node)
{
    if(list->tail == NULL){
        //add first node
        list->tail = node;
        assert(list->head == NULL);
        list->head = node;
    }
    else{
        list->tail->next = node;
        list->tail = node;
    }
    list->length++;
}
//not free removed node memory: because it will be added to tail
snake_point_list_node* snake_point_list_remove_head_node(snake_point_list* list)
{
    assert(list->length > 0);
    assert(list->head != NULL);
    snake_point_list_node* to_delete = list->head;
    list->head = list->head->next;
    if(list->head == NULL)
    {
        //list has only one node
        list->tail = NULL;
    }
    list->length--;
    return  to_delete;
}
snake_point_list_node* snake_point_list_tail_node(snake_point_list* list)
{
    assert(list);
    return list->tail;
}
snake_point_list_node* snake_point_list_head_node(snake_point_list* list)
{
    assert(list);
    return list->head;
}
void snake_point_list_node_set(snake_point_list_node* node, int x, int y)
{
    snake_point_set(&node->point, x, y);
}
//链表尾部加入一个节点，吃掉一个方块后
void snake_point_list_add_tail_point(snake_point_list* list, snake_point point)
{
    trace_snake_list(list);
    snake_point_list_node* node = snake_point_list_node_init(point);
    snake_point_list_add_tail_node(list, node);
}
int snake_point_list_contail_point(snake_point_list* list, snake_point point)
{
    int list_count = 0;
    snake_point_list_node* head = list->head;

    for (; head; head=head->next) {
        ++list_count;
        if(snake_point_is_equal(head->point, point)){
            return 1;//contain point
        }
    }

    assert(list_count == list->length);

    //not contain point
    return 0;
}
static int timer_call_quit_program(event_mgr* mgr, event* ev, void* data)
{
    snake_game* sg_game = (snake_game*)mgr;
//    trace_func("id:%d delay:%dms", ev->ev_id, ev->ev_delay_time);
    snake_game_set_run_stop(sg_game);
    return 0;
}
//monitor program
static int timer_call_monitor_program(event_mgr* mgr, event* ev, void* data)
{
    //trace_func("id:%d delay:%dms", ev->ev_id, ev->ev_delay_time);
    return 0;
}
int snake_game_size_contain_point(snake_game_size size, snake_point point)
{
    if(point.x >= size.width || point.x < 0 || point.y >= size.height || point.y < 0)
        return 0;
    return 1;
}
int snake_game_conflict_point(snake_game* sg_game, snake_point point)
{
    if(!snake_game_size_contain_point(sg_game->sg_size, point))
        return 1;
    if(snake_has_point(sg_game->sg_snake, point))
        return 1;
    return 0;
}
int snake_print_point(snake_point point, char type, snake_point_color_type color)
{
    point.y += snake_game_title_len;
    MOVETO_POINT(point);
    switch (color) {
        case snake_point_color_type_none:
            printf("%c", type);
            break;
        case snake_point_color_type_red:
            SHOW_READ_CHAR(type);
            break;
        case snake_point_color_type_yellow:
            SHOW_YELLOW_CHAR(type);
        default:
            break;
    }

    fflush(stdout);
    return 0;
}
//run forward a step to direction. 1:can run 0:game over
int snake_can_and_run_step(snake_game* sg_game, snake* sn)
{
    snake_game_size size = sg_game->sg_size;
    snake_point_list* list = sn->sn_point_list;

    //remove head, change it to tail, add it to tail
    snake_point_list_node* tail = snake_point_list_tail_node(list);
    assert(tail);
    snake_point tail_point = tail->point;


    switch (sn->sn_directin) {
        case snake_direction_down:
        {
            ++tail_point.y;
        }
            break;
        case snake_direction_up:
        {
            --tail_point.y;
        }
            break;
        case snake_direction_left:
        {
            --tail_point.x;
        }
            break;
        case snake_direction_right:
        {
            ++tail_point.x;
        }
            break;
        default:
            break;
    }

    //judge head is valid?
    if (snake_game_conflict_point(sg_game, tail_point)) {
        trace_func_sg_game(sg_game, "snake_game(%d:%d) conflict point(%d:%d)", size.width, size.height, tail_point.x, tail_point.y);
        return 0;
    }

    //valid: begin to judge i can eat
    if(snake_point_is_equal(sn->sn_to_eat_point, tail_point) ){
        //eat it and add to tail
        snake_point_list_add_tail_point(list, sn->sn_to_eat_point);
        snake_game_random_point(sg_game);
    }
    else{
        //remove head and add it to tail
        snake_point_list_node* head = snake_point_list_remove_head_node(list);
        snake_point to_delete_point = head->point;
        assert(head);

        //add head node to tail: this means the snake has ran a step.
        head->next = NULL;
        snake_point_list_add_tail_node(list, head);
        head->point = tail_point;//change it to tail
        snake_print_point(to_delete_point, ' ',  snake_point_color_type_none);
    }

    //valid: begin to refresh screen
    snake_print_point(tail_point, sn->sn_point_type, snake_point_color_type_red);
    //bug fix. remove head and add to tail. tail's color will be red
    snake_print_point(tail->point, sn->sn_point_type, snake_point_color_type_none);


    return 1;
}
//run forward a step

static int timer_call_run_steps(event_mgr* mgr, event* ev, void* data)
{
//    trace_func("id:%d delay:%dms", ev->ev_id, ev->ev_delay_time);
    snake_game* sg_game = (snake_game*)mgr;

    if(snake_game_is_pausing(sg_game))
        return 0;

    if(!snake_can_and_run_step(sg_game, (snake*)data)){
        sg_game->sg_is_run = snake_game_run_state_stoped;//stop the game
    }
    sg_game->sg_is_change_direct = 0;
    return 0;
}
char* snake_get_direction_string(snake_direction sn_dir)
{
    switch (sn_dir) {
        case snake_direction_down:
            return "snake Down";
            break;
        case snake_direction_up:
            return "snake Up";
            break;
        case snake_direction_left:
            return "snake Left";
            break;
        case snake_direction_right:
            return "snake Right";
            break;
        default:
            return "";
            break;
    }
}
//only change snake direction.  opposite direction won't work
snake_direction snake_direction_opposite_direction(snake_direction direction)
{
    switch (direction) {
        case snake_direction_up:
            return snake_direction_down;
        case snake_direction_down:
            return snake_direction_up;
        case snake_direction_left:
            return snake_direction_right;
        case snake_direction_right:
            return snake_direction_left;
        default:
            return snake_direction_none;
            break;
    }
}
static int stdin_callback_pausing(event_mgr* mgr, event* ev, void* data)
{
    //because this callback is also driver be event_mgr, so pausing is not supported.
    //one solution: in all callbacks add "ISPausing" judgement
    //another solution: change event_driver and add an attr in event_mgr
    snake_game* sg_game = (snake_game*)mgr;
    if(snake_game_is_pausing(sg_game)){
        snake_game_set_running(sg_game);
    }
    else if(snake_game_is_running(sg_game)){
        snake_game_set_pausing(sg_game);
    }
    return 0;
}
static int stdin_callback_keyboard(event_mgr* mgr, event* ev, void* data)
{
    snake_game* sg_game = (snake_game*)mgr;

    if(snake_game_is_pausing(sg_game))
        return 0;
    if(sg_game->sg_is_change_direct)
        return 0;

    snake* sn = (snake*)data;
    snake_direction to_direction;
    switch(ev->ev_id)
    {
        case 'w':
        case VK_UP:
            to_direction = snake_direction_up;
            break;
        case 's':
        case VK_DOWN:
            to_direction = snake_direction_down;
            break;
        case 'a':
        case VK_LEFT:
            to_direction = snake_direction_left;
            break;
        case 'd':
        case VK_RIGHT:
            to_direction = snake_direction_right;
            break;
        default:
            to_direction = snake_direction_none;
    }

    if(sn->sn_directin == to_direction){
        timer_call_run_steps(mgr, ev, data);//speed up
    }
    else if(to_direction!= snake_direction_none &&
            sn->sn_directin != snake_direction_opposite_direction(to_direction))
    {
        sg_game->sg_is_change_direct = 1;
        sn->sn_directin = to_direction;
    }
//    trace_func("id:%c direction:%s", ev->ev_id, snake_get_direction_string(sn->sn_directin));
    return 0;
}

snake *snake_init()
{
    //初始化三个***,向右运动
    snake* sna = (snake*)malloc(sizeof(struct snake));
    assert(sna);
    memset(sna, 0, sizeof(struct snake));
    sna->sn_point_type = '*';
    sna->sn_point_num = 6;
    sna->sn_directin = snake_direction_right;

    sna->sn_point_list = snake_point_list_init();
    snake_point point;
    for (int i = 0; i < sna->sn_point_num; ++i) {
        snake_point_set(&point, i, 0);
        snake_point_list_add_tail_point(sna->sn_point_list, point);
        snake_print_point(point, sna->sn_point_type, (i == sna->sn_point_num-1) ? snake_point_color_type_red:snake_point_color_type_none);
    }
    trace_snake(sna);
    return sna;
}
void snake_destroy(snake* sn)
{
    assert(sn);
    snake_point_list_destroy(sn->sn_point_list);
    trace_snake(sn);
    free(sn);
}
int snake_has_point(snake* sn, snake_point point)
{
    return snake_point_list_contail_point(sn->sn_point_list, point);
}
//get random point and show screen
snake_point snake_game_random_point(snake_game* sg_game)
{
    snake_point to_eat_point;
    snake* sn = sg_game->sg_snake;

    // until to get non conflict point
    do{
        to_eat_point.x = rand() % sg_game->sg_size.width;
        to_eat_point.y = rand() % sg_game->sg_size.height;
    }while (snake_has_point(sn, to_eat_point)) ;

    sg_game->sg_snake->sn_to_eat_point = to_eat_point;
    snake_print_point(to_eat_point, sg_game->sg_snake->sn_point_type, snake_point_color_type_yellow);
    return to_eat_point;
}
void snake_set_point_type(snake* sna, char point_type)
{
    sna->sn_point_type = point_type;
}
void snake_set_point_array(snake* sna, snake_point point_array)
{
    //not implements
}
snake_game *snake_game_init()
{
    CLEAR();
    HIDE_CURSOR();
    srand((unsigned int)time(NULL));
    snake_game* sg_game = (snake_game*)malloc(sizeof(struct snake_game));
    assert(sg_game);

    event_mgr_init_attr(&sg_game->sg_mgr);
    snake_game_size_set(&sg_game->sg_size, 50, 20);

    sg_game->sg_snake = snake_init();
    sg_game->sg_is_run = snake_game_run_state_running;
    sg_game->sg_is_change_direct = 0;
    snake_game_random_point(sg_game);//random point
    trace_snake_game(sg_game);
    return sg_game;
}
void snake_game_destroy(snake_game* sg_game)
{
    trace_snake_game(sg_game);
    snake_destroy(sg_game->sg_snake);
    event_mgr_destory((event_mgr*)sg_game);//this also free sg_game
}
void snake_game_set_size(snake_game* sg_game, snake_game_size sg_size)
{
    snake_game_size_set(&sg_game->sg_size, sg_size.width, sg_size.height);
}
void snake_game_set_snake(snake_game* sg_game, snake* sg_snake)
{
    assert(sg_game->sg_snake == NULL);
    sg_game->sg_snake = sg_snake;
}
void snake_game_set_run_stop(snake_game* sg_game)
{
    sg_game->sg_is_run = snake_game_run_state_stoped;
}
void snake_game_set_running(snake_game* sg_game)
{
    sg_game->sg_is_run = snake_game_run_state_running;
}
void snake_game_set_pausing(snake_game* sg_game)
{
    sg_game->sg_is_run = snake_game_run_state_pausing;
}
int snake_game_is_alive(snake_game* sg_game)
{
    return sg_game->sg_is_run;
}
int snake_game_is_pausing(snake_game* sg_game)
{
    return sg_game->sg_is_run == snake_game_run_state_pausing;
}
int snake_game_is_running(snake_game* sg_game)
{
    return sg_game->sg_is_run == snake_game_run_state_running;
}
int main(int argc, const char * argv[])
{
    //manager init
    snake_game* sg_game = snake_game_init();
    event_mgr* mgr = (event_mgr*)sg_game;

    //add monitor timer
    event_timer_repeat_init_and_add(mgr, 2004, 10*TIME_PER_SECOND, timer_call_monitor_program);

    //add run anoter step timer
    event_timer_repeat_init_and_add(mgr, 2005, 500, timer_call_run_steps);

    //add char SPACE run forward
    event_stdin_init_and_add(mgr, ' ', stdin_callback_pausing);

    //add char array monitor
    event_stdin_init_many_and_add(mgr, "wasd", 5, stdin_callback_keyboard);
    //add keyboard monitor
    event_list ev_key_direction = event_stdin_init_many(VK_ALL_DIRECTION, 4, stdin_callback_keyboard);
    event_mgr_add_event_list(mgr, ev_key_direction);

    while (snake_game_is_alive(sg_game)) {
        driver_work(mgr, sg_game->sg_snake);
        usleep(1000);
    }

    //manager destory
    snake_game_destroy(sg_game);
    return 0;
}

