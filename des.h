#ifndef HEADER_FILE
#define HEADER_FILE

/*
 * des 需要用的所有函数定义，包括（逆）初始置换（initial permutation）、
 * 扩充置换(Expansion)、置换函数（expansion）Feistel 函数、置换紧缩、
 * 左移等
 *
 * */

#include <stdbool.h>
#include <stdio.h>
#define ENCRYPT 0
#define DECRYPT 1

//bool message[64];
//bool key[64];
//bool cipher[64];

void set_key(bool _key[]);
void set_message(bool _message[]);
void get_cipher(bool _processed[]);
void des(const int mode);
void print_sub_keys();
void print_cipher();
void clear_blocks();
#endif
