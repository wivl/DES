#include "des.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*
 * des 需要用的所有函数定义，包括（逆）初始置换（initial permutation）、
 * 扩充置换(Expansion)、置换函数（expansion）Feistel 函数、置换紧缩、
 * 左移等
 *
 * */

// 初始置换函数
const int IP_TABLE[64] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53,45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7	
};

// 逆初始置换函数
const int INVERSE_IP_TABLE[64] = {
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25
};

// 扩充置换
const int EXPANSION_TABLE[48] = {
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1
};

// 置换函数
const int PERMUTATION_TABLE[32] = {
	16, 7, 20, 21, 29, 12, 28, 17,
	1, 15, 23, 26, 5, 18, 31, 10,
	2, 8, 24, 14, 32, 27, 3, 9,
	19, 13, 30, 6, 22, 11, 4, 25
};

// s 盒
const int SBOXS[8][4][16] = {
	{	// s1
		{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
		{0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
		{4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
		{15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
	},
	{	// s2
		{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
		{3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
		{0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
		{13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
	},
	{	// s3
		{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
		{13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
		{13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
		{1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
	},
	{	// s4
		{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
		{13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
		{10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
		{3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
	},
	{	// s5
		{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
		{14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
		{4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
		{11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
	},
	{	// s6
		{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
		{10, 15, 4, 2, 7, 12, 0, 5, 6, 1, 13, 14, 0, 11, 3, 8},
		{9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
		{4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
	},
	{	// s7
		{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
		{13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
		{1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
		{6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
	},
	{	// s8
		{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
		{1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
		{7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
		{2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
	}
};

// 左移次数
const int LEFT_SHIFT[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

// 置换选择 1
const int PC_1_TABLE[56] = {
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4
};

// 置换选择 2
const int PC_2_TABLE[48] = {
	14, 17, 11, 24, 1, 5, 3, 28,
	15, 6, 21, 10, 23, 19, 12, 4,
	26, 8, 16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55, 30, 40,
	51, 45, 33, 48, 44, 49, 39, 56,
	34, 53, 46, 42, 50, 36, 29, 32
};

bool sub_key[16][48];

bool left[32], right[32], expanded[48], left_backup[32];

void set_key(bool _key[]) {
	for (int i = 0; i < 64; i++) {
		key[i] = _key[i];
	}
}

void set_message(bool _message[]) {
	for (int i = 0; i < 64; i++) {
		message[i] = _message[i];
	}
}

void initial_permutation() {
	bool temp[64];
	for (int i = 0; i < 64; i++) {
		temp[i] = message[IP_TABLE[i] - 1]; // IP_TABLE starts from index 1.
	}
	memcpy(message, temp, sizeof(message));
}

void inverse_initial_permutation() {
	bool temp[64];
	for (int i = 0; i < 64; i++) {
		temp[i] = message[INVERSE_IP_TABLE[i] - 1];
	}
	memcpy(message, temp, sizeof(message));
}

void generate_sub_keys() {
	bool temp[56];
	for (int j = 0; j < 56; j++) {
		temp[j] = key[PC_1_TABLE[j] - 1];
	}
	for (int i = 0; i < 16; i++) {
		// left shift
		// left
		int buff[2], j, left = 0, right = 28;
		for (int k = 0; k < LEFT_SHIFT[i]; k++, left++) {
			buff[k] = temp[left];
		}
		for (j = 0; j < 28 - LEFT_SHIFT[i]; j++) {
			temp[j] = temp[j + LEFT_SHIFT[i]];
		}
		for (int k = 0; k < LEFT_SHIFT[i]; k++, j++) {
			temp[j] = buff[k];
		}
		// right
		for (int k = 0; k < LEFT_SHIFT[i]; k++, right++) {
			buff[k] = temp[right];
		}
		for (j = 28; j < 56 - LEFT_SHIFT[i]; j++) {
			temp[j] = temp[j + LEFT_SHIFT[i]];
		}
		for (int k = 0; k < LEFT_SHIFT[i]; k++, j++) {
			temp[j] = buff[k];
		}
		// pc 2
		for (int k = 0; k < 48; k++) {
			sub_key[i][k] = temp[PC_2_TABLE[k] - 1];
		}
	}
}


void print_sub_keys() {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 48; j++) {
			printf("%d", sub_key[i][j]);
		}
		puts("");
	}
}

void message_split() {
	memcpy(left, message, sizeof(left));
	memcpy(right, message + 32, sizeof(right));
}

void expansion() {
	for (int i = 0; i < 48; i++) {
		expanded[i] = right[EXPANSION_TABLE[i] - 1];
	}
}

void expended_xor_sub_key(int rou) {
	bool temp[48];
	for (int i = 0; i < 48; i++) {
		temp[i] = expanded[i] ^ sub_key[rou][i];
	}
	memcpy(expanded, temp, sizeof(temp));
}

void des(const int mode) {
	generate_sub_keys();
	initial_permutation();
	message_split();
	for (int i = 0; i < 16; i++) {
		// feistel function starts.
		memcpy(left_backup, left, sizeof(left));
		memcpy(left, right, sizeof(right));
		expansion();
		if (mode == ENCRYPT) {
			expended_xor_sub_key(i);
		} else {
			expended_xor_sub_key(15 - i);
		}
		int ptr = 0, rptr = 0;
		for (int j = 0; j < 8; j++) {
			// locating row
			int row = 0;
			row += expanded[ptr];
			row <<= 1;
			row += expanded[ptr + 5];
			// locating column
			int column = 0;
			for (int k = ptr + 1; k < ptr + 5; k++) {
				column <<= 1;
				column += expanded[k];
			}
			int ans = SBOXS[j][row][column];


			// fill the right
			for (int k = rptr + 3; k >= rptr; k--) {
				right[k] = ans & 1;
				ans >>= 1;
			}

			ptr += 6, rptr += 4;
		}
		// p
		bool temp[32];
		for (int k = 0; k < 32; k++) {
			temp[k] = right[PERMUTATION_TABLE[k] - 1];
		}
		// temp: f(r, subkey)
		// feistel function ends.

		// temp (pre-right) xor left_backup
		for (int k = 0; k < 32; k++) {
			right[k] = temp[k] ^ left_backup[k];
		}
	}
	// swap left and right
	memcpy(message, right, sizeof(right));
	memcpy(message + 32, left, sizeof(left));

	// inverse initial permutation
	inverse_initial_permutation();

	memcpy(cipher, message, sizeof(message));
}

void get_cipher(bool *_processed) {
	memcpy(_processed, cipher, sizeof(cipher));
}

void print_cipher() {
	for (int i = 0; i < 64; i++) {
		printf("%d ", cipher[i]);
		if (i % 8 == 7) {
			puts("");
		}
	}

}
