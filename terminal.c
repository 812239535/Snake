//
//  terminal.c
//  Tetris
//
//  Created by lipeng on 14-5-19.
//  Copyright (c) 2014年 lpstudy. All rights reserved.
//

#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include "log.h"

/*
 有缓冲情况下:
 read直到用户输入了缓冲区终止符才会返回。1，如果用户设置的size比较大，那么少于size的缓冲区数据会返回。2，如果设置的readsize比较小，它会先从缓冲区读取一部分值，再返回。即使read设置1个字符，它也会等到用户输入了enter等结束符后才返回read结果。然后进入下一遍read 1个的循环，再次读取剩余的值，此时会立刻返回。
 无缓冲情况下：
 setbuf(stdin, NULL); 这个函数的调用看起来没有起作用，这个仅仅是库函数不使用缓冲

 非阻塞模式下：（O_NONBLOCK)
 read会不断尝试读取，如果没有则返回-1，同时置位errno为 EAGAIN or EWOULDBLOCK。它和上面的有缓冲情况没有太大的区别，仅仅就在于没有数据也返回，不会阻塞。如果有数据的话，和上面的处理一致。

 terminal无缓冲：
 用户输入任何内容，会立刻被程序读取出来。又由于设置了非阻塞模式，因此可以实现有输入就读取，无输入就返回-1的情况。
 */

static struct termios old_term;

static int set_block_with_flag(int fileno, int is_block)
{
    int flags = fcntl(fileno, F_GETFL, 0);
    if(is_block){
        flags &= ~O_NONBLOCK;
    }
    else{
        flags |= O_NONBLOCK;
    }
    int res = fcntl(fileno, F_SETFL, flags);
    if(res < 0){
        trace_func("res=%d errno=%d", res, errno);
        return res;
    }
    return 0;
}
int set_nonblock(int fileno)
{
    return set_block_with_flag(fileno, 0);
}
int set_block(int fileno)
{
    return set_block_with_flag(fileno, 1);
}
void set_no_buffer_std_input()
{
    struct termios new_term;

    /* Get old terminal settings for further restoration */
    tcgetattr(0, &old_term);

    /* Copy the settings to the new value */
    new_term = old_term;

    /* Disable echo of the character and line buffering */
    new_term.c_lflag &= (~ICANON & ~ECHO);
    /* Set new settings to the terminal */
    tcsetattr(0, TCSANOW, &new_term);
}
void set_no_buffer_std_output()
{
    setbuf(stdout, NULL);
}
void restore_old_buffer_std_input()
{
    /* Restore old settings */
    tcsetattr(0, TCSANOW, &old_term);
}
