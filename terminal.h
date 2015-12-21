//
//  terminal.h
//  Tetris
//
//  Created by lipeng on 14-5-19.
//  Copyright (c) 2014年 lpstudy. All rights reserved.
//

#ifndef Tetris_terminal_h
#define Tetris_terminal_h

//设置fileno为非阻塞模式
int set_nonblock(int fileno);
//设置为阻塞模式
int set_block(int fileno);
//设置标准输入为终端无缓冲：用户输入后会立刻返回
void set_no_buffer_std_input();
//stdout 立刻输出到屏幕上，无缓冲
void set_no_buffer_std_output();
//恢复终端的状态
void restore_old_buffer_std_input();

#endif
