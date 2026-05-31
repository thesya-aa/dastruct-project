#ifndef SUPERMARKET_H
#define SUPERMARKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_YELLOW  "\033[1;33m"

struct Item{
	int id;
	char name[50];
	char category[50];
	int price;
	int stock;
};

struct user{
	char username[50];
	char password[50];
	int role;
	struct User* next;
};

extern struct User* userTable[100];

void initHashTable();
int hashFunction(char* str);
void registerAccount();
int loginAccount();

#endif
