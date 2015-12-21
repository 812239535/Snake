//
//  log.h
//  Tetris
//
//  Created by lipeng on 14-5-19.
//  Copyright (c) 2014年 lpstudy. All rights reserved.
//

#ifndef Tetris_log_h
#define Tetris_log_h

#include <stdio.h>
#include "cursor.h"

#define log_event (0)
#define log_event_list (0)
#define log_snake_list_node (0)
#define log_snake_list (0)
#define log_snake (0)
#define log_snake_game (0)

#define trace_func(format, ...)  printf("[%30s:%3d]: ("format")\n",__func__, __LINE__, ##__VA_ARGS__)

#if (log_event)
    #define trace_event(ev) trace_func("id=%d, type=%d, callback=%p", ev->ev_id, ev->ev_type, ev->ev_callback)
#else
    #define trace_event(ev)
#endif//log_event

#if (log_event_list)
    #define trace_event_list_node(ev_list) trace_func("list_node:%p", ev_list)
#else
    #define trace_event_list_node(ev_list)
#endif//log_event_list

#if (log_snake_list_node)
    #define trace_snake_list_node(list_node) trace_func("list_node:%p (%d:%d)", list_node, list_node->point.x, list_node->point.y)
#else
    #define trace_snake_list_node(list_node)
#endif//log_snake_list_node

#if (log_snake_list)
    #define trace_snake_list(list) trace_func("list:%p len:%d", list, list->length)
#else
    #define trace_snake_list(list)
#endif//log_snake_list

#if (log_snake)
    #define trace_snake(snake) trace_func("snake:%p", snake)
#else
    #define trace_snake(snake)
#endif//log_snake

#if (log_snake_game)
    #define trace_snake_game(game) trace_func("game:%p", game)
#else
    #define trace_snake_game(game) 
#endif//log_snake_game


#define trace_func_line(line,format, ...)  MOVETO(1,line); printf("[%30s:%3d]: ("format")\n",__func__, __LINE__, ##__VA_ARGS__)
#define trace_func_sg_game(sg_game,format, ...)  MOVETO(1,sg_game->sg_size.height+snake_game_title_len+3); printf("[%30s:%3d]: ("format")\n",__func__, __LINE__, ##__VA_ARGS__)

#endif//Tetris_log_h
