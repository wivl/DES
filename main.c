#include "des.h"
#include <stdio.h>
#include <string.h>

#define FILE_NOT_FOUND 404
#define FLAG_ERROR 405

#define INVALID_MODE 406
#define READ 403

#define MAX_SIZE 8

const char HEX[16] = "0123456789abcdef";

// help
void print_help() {
	printf("Usage:\n");
	printf("\t./des [-flag] [(optional)input file name]\n\n");
	printf("-flag:\n");
	printf("\t -E\t\t\t= encrypt .txt file\n");
	printf("\t -D\t\t\t= decrypt .des file\n");
	printf("\t -R\t\t\t= read .des file\n\n");
	printf("the default file name:\n");
	printf("\t -E\t\t\t= input.txt\n");
	printf("\t -D -R\t\t\t= output.des\n");
}

// char to bin
void char_to_bin(unsigned char *ptr, bool *bin) {
	int bptr = 0;
	for (; *ptr != 0; ++ptr) {
		for(int i = 7; i >= 0; --i) {
			bin[bptr++] = (*ptr & 1 << i) ? 1 : 0;
		} 
	}
}

// bin to hex
void bin_to_hex(const bool *bin, unsigned char *chr) {
	int dec = 0, ptr = 0;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 4; j++) {
			dec += bin[i * 4 + j] << (3 - j);
		}
		chr[ptr++] = HEX[dec];
		dec = 0;
	}
}


// bin to char
void bin_to_char(const bool *bin, unsigned char *chr) {
	int dec = 0, ptr = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			dec += bin[i * 8 + j] << (7 - j);
		}
		chr[ptr++] = (char)dec;
		dec = 0;
	}
}

int main(int argc, char **argv) {
	bool message_bin[MAX_SIZE * 8], key_bin[MAX_SIZE * 8], processed_bin[MAX_SIZE * 8], cipher_bin[MAX_SIZE * 8];
	unsigned char message_char[MAX_SIZE], key_char[MAX_SIZE], processed_hex[MAX_SIZE * 2], processed_char[MAX_SIZE], cipher_hex[MAX_SIZE * 2];
	FILE *fi = NULL, *fo = NULL, *ki = NULL;

	// defaule mode is encrypt
	int mode = ENCRYPT;

	if (*++argv != NULL && **argv == '-') {
		switch (*++*argv) {
			case 'E':
				mode = ENCRYPT;
				break;

			case 'D':
				mode = DECRYPT;
				break;

			case 'R':
				mode = READ;
				break;

			case 'H':
				print_help();
				return 0;
				break;

			default:
				mode = INVALID_MODE;
		}
	}


	int char_count = 0;

	// read mode
	if (mode == READ) {
		char file_name[64] = "output.des";
		if (*++argv != NULL) {
			memset(file_name, 0, sizeof(file_name));
			strcpy(file_name, *argv);
		}
		fo = fopen(file_name, "r");
		unsigned char read_buffer[MAX_SIZE * 2];
		bool read_bin[MAX_SIZE * 8];
		while ((char_count = fread(read_bin, sizeof(bool), MAX_SIZE * 8, fo)) != 0) {
			bin_to_hex(read_bin, read_buffer);
			for (int i = 0; i < char_count / 4; i++) {
				printf("%c", read_buffer[i]);
			}
			printf("\n");
			memset(read_bin, 0, sizeof(read_bin));
			memset(read_buffer, 0, sizeof(read_buffer));
		}
		return 0;
	}


	// read and set key
	ki = fopen("key.txt", "r");
	if (!ki) {
		printf("[ERROR]No available key file found.\n");
		return FILE_NOT_FOUND;
	} else {
		fread(key_char, sizeof(unsigned char), MAX_SIZE, ki);
		char_to_bin(key_char, key_bin);
		set_key(key_bin);
		printf("[Key]Key file found.\n");
	}
	fclose(ki);

	// encrypt
	if (mode == ENCRYPT) {
		char file_name[64] = "input.txt";
		if (*++argv != NULL) {
			memset(file_name, 0, sizeof(file_name));
			strcpy(file_name, *argv);
			printf("%s", file_name);
		}
		fi = fopen(file_name, "r");
		if (!fi) {
			printf("[ERROR]No available input file found.\n");
			return FILE_NOT_FOUND;
		}
		fo = fopen("output.des", "w");
		printf("[ENCRYPT]Processing...\n");
		char message_buffer[MAX_SIZE];
		clear_blocks();
		memset(processed_hex, 0, sizeof(processed_hex));
		memset(processed_bin, 0, sizeof(processed_bin));
		memset(cipher_bin, 0, sizeof(cipher_bin));
		memset(cipher_hex, 0, sizeof(cipher_hex));
		while ((char_count = fread(message_buffer, sizeof(unsigned char), MAX_SIZE, fi)) != 0) {
			for (int i = 0; i < char_count; i++) {
				message_char[i] = message_buffer[i];
			}
			char_to_bin(message_char, message_bin);
			set_message(message_bin);
			des(mode);
			get_cipher(processed_bin);
			fwrite(processed_bin, sizeof(bool), MAX_SIZE * 8, fo);
			clear_blocks();
			memset(message_char, 0, sizeof(message_char));
			memset(message_bin, 0, sizeof(message_bin));
			memset(processed_hex, 0, sizeof(processed_hex));
			memset(processed_bin, 0, sizeof(processed_bin));
		}
		fclose(fi);
		fclose(fo);
		printf("[Done]File has been writen to output.des.\n");
	// decrypt
	} else if (mode == DECRYPT) {
		char file_name[64] = "output.des";
		if (*++argv != NULL) {
			memset(file_name, 0, sizeof(file_name));
			strcpy(file_name, *argv);
		}
		fi = fopen(file_name, "r");
		if (!fi) {
			printf("[ERROR]No available .des file found.\n");
			return FILE_NOT_FOUND;
		}
		fo = fopen("message.txt", "w");
		printf("[DECRYPT]Processing...\n");
		clear_blocks();
		memset(processed_hex, 0, sizeof(processed_hex));
		memset(processed_bin, 0, sizeof(processed_bin));
		memset(cipher_bin, 0, sizeof(cipher_bin));
		memset(cipher_hex, 0, sizeof(cipher_hex));

		while ((char_count = fread(cipher_bin, sizeof(bool), MAX_SIZE * 8, fi)) != 0) {
			set_message(cipher_bin);
			des(mode);
			get_cipher(processed_bin);
			bin_to_char(processed_bin, processed_char);
			fwrite(processed_char, sizeof(unsigned char), MAX_SIZE, fo);
			clear_blocks();
			memset(processed_hex, 0, sizeof(processed_hex));
			memset(processed_bin, 0, sizeof(processed_bin));
			memset(cipher_bin, 0, sizeof(cipher_bin));
			memset(cipher_hex, 0, sizeof(cipher_hex));
		}
		fclose(fi);
		fclose(fo);
		printf("[Done]File has been writen to message.txt.\n");

	} else {
		printf("[ERROR] Invalid mode flag.\n");
		return FLAG_ERROR;
	}
}
