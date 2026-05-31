#ifndef SUPERMARKET_H
#define SUPERMARKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_CYAN    "\033[1;36m"

#define MAX_ITEMS 50000

struct Item{
  int id;
  char name[100];
  char category[50];
  int price;
  int stock;
};

struct BSTNode{
  int id;
  int catalogIndex;

  struct BSTNode* left;
  struct BSTNode* right;
};

extern struct Item catalog[MAX_ITEMS];
extern int totalItems;

extern struct BSTNode* rootIndex;

struct BSTNode* createBSTNode(int id, int index);

struct BSTNode* insertBST(
  struct BSTNode* root,
  int id,
  int index
);

struct BSTNode* searchBST(
  struct BSTNode* root,
  int id
);

struct BSTNode* deleteBST(
  struct BSTNode* root,
  int id
);

struct BSTNode* findMin(
  struct BSTNode* root
);

void createItem();

void readCatalog();

void updateItem();

void deleteItem();

void generateDummyData();

void adminMenu();

#endif
