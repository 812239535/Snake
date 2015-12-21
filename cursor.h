//
//  cursor.h
//  snake
//
//  Created by lipeng on 14-5-20.
//  Copyright (c) 2014年 lpstudy. All rights reserved.
//

#ifndef snake_cursor_h
#define snake_cursor_h

#include <stdio.h>
#include <unistd.h>

// 清除屏幕
#define CLEAR() printf("\033[2J")

// 上移光标
#define MOVEUP(x) printf("\033[%dA", (x))

// 下移光标
#define MOVEDOWN(x) printf("\033[%dB", (x))

// 左移光标
#define MOVELEFT(y) printf("\033[%dD", (y))

// 右移光标
#define MOVERIGHT(y) printf("\033[%dC",(y))

// 定位光标
#define MOVETO(x,y) printf("\033[%d;%dH", (y), (x))

// 定位point
#define MOVETO_POINT(point) MOVETO(point.x+1, point.y+1)

// 光标复位
#define RESET_CURSOR() printf("\033[H")

// 隐藏光标
#define HIDE_CURSOR() printf("\033[?25l")

// 显示光标
#define SHOW_CURSOR() printf("\033[?25h")

//反显
#define HIGHT_LIGHT() printf("\033[7m")
#define UN_HIGHT_LIGHT() printf("\033[27m")

//清空属性
#define CURSOR_CLEAR_ATTR() printf("\033[0m")

//打印红色
#define SHOW_READ_CHAR(ch) printf("\033[31m%c\033[0m",ch);

//打印黄色
#define SHOW_YELLOW_CHAR(ch) printf("\033[33m%c\033[0m",ch);
#endif
