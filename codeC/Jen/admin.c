#include "supermarket.h"

struct Item catalog[MAX_ITEMS];
int totalItems = 0;

struct BSTNode* rootIndex = NULL;

struct BSTNode* createBSTNode(int id, int index){

  struct BSTNode* newNode =
    (struct BSTNode*)malloc(sizeof(struct BSTNode));

  newNode->id = id;
  newNode->catalogIndex = index;

  newNode->left = NULL;
  newNode->right = NULL;

  return newNode;
}

struct BSTNode* insertBST(
  struct BSTNode* root,
  int id,
  int index
){

  if(root == NULL){
    return createBSTNode(id, index);
  }

  if(id < root->id){
    root->left = insertBST(root->left, id, index);
  }
  else if(id > root->id){
    root->right = insertBST(root->right, id, index);
  }

  return root;
}

struct BSTNode* searchBST(
  struct BSTNode* root,
  int id
){

  if(root == NULL || root->id == id){
    return root;
  }

  if(id < root->id){
    return searchBST(root->left, id);
  }

  return searchBST(root->right, id);
}

struct BSTNode* findMin(
  struct BSTNode* root
){

  while(root && root->left != NULL){
    root = root->left;
  }

  return root;
}

struct BSTNode* deleteBST(
  struct BSTNode* root,
  int id
){

  if(root == NULL){
    return NULL;
  }

  if(id < root->id){
    root->left = deleteBST(root->left, id);
  }
  else if(id > root->id){
    root->right = deleteBST(root->right, id);
  }
  else{

    if(root->left == NULL && root->right == NULL){
      free(root);
      return NULL;
    }

    else if(root->left == NULL){
      struct BSTNode* temp = root->right;
      free(root):
      return temp;
    }

    else if(root->right == NULL){
      struct BSTNode* temp = root->left;
      free(root);
      return temp;
    }

    struct BSTNode* successor = findMin(root->right);

    root->id = successor->id;
    root->catalogIndex = successor->catalogIndex;

    root->right = deleteBST(root->right, successor->id);
  }

  return root;
}

void createItem(){

  if(totalItem >= MAX_ITEMS){

    printf(COLOR_RED "Warehouse Full!\n" COLOR_RESET);

    return;
  }

  int inputID;

  printf("Input Item ID : ");
  scanf("%d", &inputID);

  if(searchBST(rootIndex, inputID) != NULL){

    printf(COLOR_RED "ID Already Exists!\n" COLOR_RESET);

    return;
  }

  catalog[totalItems].id = inputID;

  printf("Input Item Name : ");
  scanf(" %[^\n]", catalog[totalItems].name);

  printf("Input Category : ");
  scanf(" %[^\n]", catalog[totalItems].category);

  printf("Input Price : ");
  scanf("%d", &catalog[totalItems].price);

  rootIndex = insertBST(
    rootIndex,
    inputID,
    totalItems
  );

  totalItems++;

  printf(COLOR_GREEN "Item Successfully Added!\n" COLOR_RESET);
}

void readCatalog(){

  if(totalItems == 0){

    printf(COLOR_RED "Catalog Empty!\n" COLOR_RESET);

    return;
  }

  printf(COLOR_CYAN "\n========== CATALOG ==========\n" COLOR_RESET);

  for(int i = 0; i < totalItems; i++){

    if(catalog[i].stock == -1){
      continue;
    }

    printf("ID       : %d\n", catalog[i].id);

    printf("Name     : %d\n", catalog[i].name);

    printf("Category : %d\n", catalog[i].category);

    printf("Price    : %d\n", catalog[i].price);

    printf("Stock    : %d\n", catalog[i].stock);

    printf("-----------------------------\n");
  }
}

void updateItem(){

  int id;

  printf("Input ID To Update : ");
  scanf("%d", &id);

  struct BSTNode* found = searchBST(rootIndex, id);

  if(found == NULL){

    printf(COLOR_RED "Item Not Found!\n" COLOR_RESET);

    return;
  }

  int index = found->catalogIndex;

  printf("New Name : ");
  scanf(" %[^\n]", catalog[index].name);

  printf("New Category : ");
  scanf(" %[^\n]", catalog[index].category);

  printf("New Price : ");
  scanf(" %[^\n]", catalog[index].price);

  printf("New Stock : ");
  scanf(" %[^\n]", catalog[index].stock);

  printf(COLOR_GREEN "Item Successfully Updated!\n" COLOR_RESET);
}

void deleteItem(){

  int id;

  printf("Input ID To Delete : ");
  scanf("%d", &id);

  struct BSTNode* found = searchBST(rootIndex, id);

  if(found == NULL){

    printf(COLOR_RED "Item Not Found!\n" COLOR_RESET);

    return;
  }

  int index = found->catalogIndex;

  catalong[index].stock = -1;

  rootIndex = deleteBST(
    rootIndex,
    id
  );

  printf(COLOR_YELLOW "Item Successfully Deleted!\n" COLOR_RESET);
}

void generateDummyData(){

  int amount;

  printf("How Many Dummy Data? ");
  scanf("%d", &amount);

  if(totalItems + amount >= MAX_ITEMS){

    printf(COLOR_RED "Exceeds Maximum Capacity!\n" COLOR_RESET);

    return;
  }

  for(int i = 0; i < amount; i++){

    int randomID = rand() % 100000 + 1;

    while(searchBST(rootIndex, randomID) != NULL){

      randomID = rand() % 100000 + 1;
    }

    catalog[totalItems].id = randomID;

     sprintf(
       catalog[totalItems].name,
       "Item%d",
       totalItems
      );

      sprintf(
            catalog[totalItems].category,
            "Category%d",
            rand()%10
        );

        catalog[totalItems].price =
            rand()%100000 + 1000;

        catalog[totalItems].stock =
            rand()%100 + 1;

        rootIndex = insertBST(
            rootIndex,
            randomID,
            totalItems
        );

        totalItems++;
    }

    printf(COLOR_GREEN
           "%d Dummy Data Generated!\n"
           COLOR_RESET,
           amount);
}

void adminMenu(){

    int choice;

    do{

        printf(COLOR_CYAN
               "\n===== ADMIN MENU =====\n"
               COLOR_RESET);

        printf("1. Add Item\n");
        printf("2. View Catalog\n");
        printf("3. Update Item\n");
        printf("4. Delete Item\n");
        printf("5. Generate Dummy Data\n");
        printf("6. Exit\n");

        printf("Choose : ");
        scanf("%d", &choice);

        switch(choice){

            case 1:
                createItem();
                break;

            case 2:
                readCatalog();
                break;

            case 3:
                updateItem();
                break;

            case 4:
                deleteItem();
                break;

            case 5:
                generateDummyData();
                break;

            case 6:
                printf(COLOR_GREEN
                       "Exiting Admin Module...\n"
                       COLOR_RESET);
                break;

            default:
                printf(COLOR_RED
                       "Invalid Menu!\n"
                       COLOR_RESET);
        }

    }while(choice != 6);
}
