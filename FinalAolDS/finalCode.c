// Klik DuoMart
// Gabungan modul:
// - Admin    : BST (Binary Search Tree) untuk indexing katalog
// - Auth     : Hash Table + Chaining untuk autentikasi
// - Transaksi: Linked List (Cart), Stack (Undo), Queue (Order)
// - Sorting  : Bubble Sort, Merge Sort, Quick Sort
// - Animasi  : Intro screen & portal login
// Persistensi Data:
// - databaseProduct.txt  : id|name|category|price|stock
// - databaseAccount.txt  : username|password|role
// - databaseCart.txt     : username|id|name|price|qty

// INCLUDES & PLATFORM DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#ifdef _WIN32
    #include <windows.h>
    #define DELAY(ms) Sleep(ms)
    #define CLEAR()   system("cls")
#else
    #include <unistd.h>
    #define DELAY(ms) usleep((ms) * 1000)
    #define CLEAR()   system("clear")
#endif

// COLOR / ANSI DEFINES
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define DIM         "\033[2m"
#define BRAND_BLUE  "\033[1;94m"
#define BRAND_CYAN  "\033[1;96m"
#define SUCCESS     "\033[1;92m"
#define WARNING     "\033[1;93m"
#define DANGER      "\033[1;91m"
#define TEXT_WHITE  "\033[1;97m"
#define BORDER_GRAY "\033[1;90m"

// CONSTANTS
#define MAX_ITEMS  50000
#define HASH_SIZE  100

// Path file database — relatif terhadap executable
#define DB_PRODUCT  "databaseProduct.txt"
#define DB_ACCOUNT  "databaseAccount.txt"
#define DB_CART     "databaseCart.txt"

// STRUCT DEFINITIONS

struct Item {
    int  id;
    char name[100];
    char category[50];
    int  price;
    int  stock;
};

typedef struct Item item;

struct BSTNode {
    int             id;
    int             catalogIndex;
    struct BSTNode *left;
    struct BSTNode *right;
};

typedef struct CartNode {
    int             id;
    char            name[100];
    int             price;
    int             qty;
    struct CartNode *next;
} CartNode;

typedef struct UndoNode {
    int              itemID;
    struct UndoNode *next;
} UndoNode;

typedef struct OrderNode {
    char              customerName[50];
    int               totalBill;
    struct OrderNode *next;
} OrderNode;

struct User {
    char         username[50];
    char         password[50];
    int          role;
    struct User *next;
};

// GLOBAL VARIABLES

struct Item    catalog[MAX_ITEMS];
int            totalItems = 0;

struct BSTNode *rootIndex = NULL;

struct User    *userTable[HASH_SIZE];
char           loggedInUser[50] = "";

CartNode  *headCart   = NULL;
UndoNode  *topUndo    = NULL;
OrderNode *frontQueue = NULL;
OrderNode *rearQueue  = NULL;

// FORWARD DECLARATIONS
void pushUndo(int id);
void enqueueOrder(char *name, int total);
void deleteFromCart(int id);
int  searchIndexInBST(int id);
struct BSTNode *insertBST(struct BSTNode *root, int id, int index);
int hashFunction(char *str);
void pauseScreen();
void adminMenu();
void customerMenu();
void saveProductDB();
void saveAccountDB();
void loadCartDB(char *username);
void saveCartDB(char *username);

// Algoritma Hashing FNV-1a 32-bit
uint32_t fnv1a_32(const char *str) {
    uint32_t hash = 2166136261u;
    for (int i = 0; str[i] != '\0'; i++) {
        hash ^= (uint8_t)str[i];
        hash *= 16777619u;
    }
    return hash;
}

// Fungsi jeda untuk refresh layar
void pauseScreen() {
    printf("\n\033[1;33m[ Tekan Enter untuk melanjutkan... ]\033[0m");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != -1);
    getchar();
}

// DATABASE I/O MODULE
// Format produk   : id|name|category|price|stock
// Format akun     : username|password|role
// Format keranjang: username|id|name|price|qty

void loadProductDB() {
    FILE *fp = fopen(DB_PRODUCT, "r");
    if (!fp) {
        printf(WARNING "[INFO] %s tidak ditemukan, mulai dengan katalog kosong.\n" RESET, DB_PRODUCT);
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        // Hapus newline
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        if (totalItems >= MAX_ITEMS) break;

        int  id, price, stock;
        char name[100], category[50];

        // Parse format: id|name|category|price|stock
        int parsed = sscanf(line, "%d|%99[^|]|%49[^|]|%d|%d",
                            &id, name, category, &price, &stock);

        if (parsed == 5) {
            catalog[totalItems].id    = id;
            strncpy(catalog[totalItems].name,     name,     99);
            strncpy(catalog[totalItems].category, category, 49);
            catalog[totalItems].price = price;
            catalog[totalItems].stock = stock;

            rootIndex = insertBST(rootIndex, id, totalItems);
            totalItems++;
        }
    }

    fclose(fp);
    printf(SUCCESS "[DB] %d produk berhasil dimuat dari %s\n" RESET, totalItems, DB_PRODUCT);
}

void saveProductDB() {
    FILE *fp = fopen(DB_PRODUCT, "w");
    if (!fp) {
        printf(DANGER "[ERROR] Gagal menyimpan ke %s\n" RESET, DB_PRODUCT);
        return;
    }

    for (int i = 0; i < totalItems; i++) {
        if (catalog[i].stock == -1) continue; // Lewati item yang dihapus (soft-delete)
        fprintf(fp, "%d|%s|%s|%d|%d\n",
                catalog[i].id,
                catalog[i].name,
                catalog[i].category,
                catalog[i].price,
                catalog[i].stock);
    }

    fclose(fp);
    printf(SUCCESS "[DB] Katalog berhasil disimpan ke %s\n" RESET, DB_PRODUCT);
}

void loadAccountDB() {
    FILE *fp = fopen(DB_ACCOUNT, "r");
    if (!fp) {
        printf(WARNING "[INFO] %s tidak ditemukan, mulai tanpa akun tersimpan.\n" RESET, DB_ACCOUNT);
        return;
    }

    char line[200];
    int  loaded = 0;
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        char username[50], password[50];
        int  role;

        int parsed = sscanf(line, "%49[^|]|%49[^|]|%d", username, password, &role);
        if (parsed != 3) continue;

        int index = hashFunction(username);

        struct User *newUser = (struct User *)malloc(sizeof(struct User));
        strncpy(newUser->username, username, 49);
        strncpy(newUser->password, password, 49);
        newUser->role = role;
        newUser->next = NULL;

        if (userTable[index] == NULL) {
            userTable[index] = newUser;
        } else {
            newUser->next    = userTable[index];
            userTable[index] = newUser;
        }
        loaded++;
    }

    fclose(fp);
    printf(SUCCESS "[DB] %d akun berhasil dimuat dari %s\n" RESET, loaded, DB_ACCOUNT);
}

void saveAccountDB() {
    FILE *fp = fopen(DB_ACCOUNT, "w");
    if (!fp) {
        printf(DANGER "[ERROR] Gagal menyimpan ke %s\n" RESET, DB_ACCOUNT);
        return;
    }

    for (int i = 0; i < HASH_SIZE; i++) {
        struct User *curr = userTable[i];
        while (curr != NULL) {
            fprintf(fp, "%s|%s|%d\n", curr->username, curr->password, curr->role);
            curr = curr->next;
        }
    }

    fclose(fp);
}

void loadCartDB(char *username) {
    FILE *fp = fopen(DB_CART, "r");
    if (!fp) return;

    CartNode *temp;
    while (headCart) {
        temp = headCart;
        headCart = headCart->next;
        free(temp);
    }
    headCart = NULL;

    CartNode *tail = NULL;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        char savedUser[50], itemName[100];
        int id, price, qty;
        int parsed = sscanf(line, "%49[^|]|%d|%99[^|]|%d|%d", savedUser, &id, itemName, &price, &qty);

        if (parsed == 5 && strcmp(savedUser, username) == 0) {
            CartNode *newNode = (CartNode *)malloc(sizeof(CartNode));
            newNode->id = id;
            strncpy(newNode->name, itemName, 99);
            newNode->price = price;
            newNode->qty = qty;
            newNode->next = NULL;

            if (!headCart) {
                headCart = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
        }
    }
    fclose(fp);
    printf(SUCCESS "[DB] Keranjang milik %s berhasil dimuat!\n" RESET, username);
}

void saveCartDB(char *username) {
    FILE *fp = fopen(DB_CART, "r");
    FILE *temp = fopen("temp_cart.txt", "w");
    if (!temp) return;

    if (fp) {
        char line[512];
        while (fgets(line, sizeof(line), fp)) {
            char savedUser[50];
            sscanf(line, "%49[^|]|", savedUser);
            if (strcmp(savedUser, username) != 0) {
                fputs(line, temp);
            }
        }
        fclose(fp);
    }

    CartNode *cur = headCart;
    while (cur) {
        fprintf(temp, "%s|%d|%s|%d|%d\n", username, cur->id, cur->name, cur->price, cur->qty);
        cur = cur->next;
    }

    fclose(temp);
    remove(DB_CART);
    rename("temp_cart.txt", DB_CART);
}

// AUTH MODULE
// Struktur: Hash Table (Division Method) + Chaining

void initHashTable() {
    for (int i = 0; i < HASH_SIZE; i++) {
        userTable[i] = NULL;
    }
}

int hashFunction(char *str) {
    int hashValue = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        hashValue += (int)str[i];
    }
    return hashValue % HASH_SIZE;
}

void registerAccount() {
    char user[50], pass[50];
    int  role;

    printf(BRAND_BLUE "\n=== PENDAFTARAN AKUN ===" RESET "\n");
    printf("Masukkan Username : ");
    scanf(" %[^\n]s", user);
    printf("Masukkan Password : ");
    scanf(" %[^\n]s", pass);
    printf("Pilih Role (0: Admin, 1: Customer): ");
    scanf("%d", &role);

    char hashedUser[50], hashedPass[50];
    sprintf(hashedUser, "%u", fnv1a_32(user));
    sprintf(hashedPass, "%u", fnv1a_32(pass));

    // Cek apakah username sudah ada
    int index = hashFunction(hashedUser);
    struct User *curr = userTable[index];
    while (curr != NULL) {
        if (strcmp(curr->username, hashedUser) == 0) {
            printf(DANGER "Username sudah digunakan!\n" RESET);
            return;
        }
        curr = curr->next;
    }

    struct User *newUser = (struct User *)malloc(sizeof(struct User));
    strncpy(newUser->username, hashedUser, 49);
    strncpy(newUser->password, hashedPass, 49);
    newUser->role = role;
    newUser->next = NULL;

    if (userTable[index] == NULL) {
        userTable[index] = newUser;
    } else {
        newUser->next    = userTable[index];
        userTable[index] = newUser;
    }

    saveAccountDB();
    printf(SUCCESS "Registrasi Berhasil! Silakan Login." RESET "\n");
}

int loginAccount() {
    char user[50], pass[50];

    printf(BRAND_BLUE "\n=== LOGIN SISTEM ===" RESET "\n");
    printf("Username : ");
    scanf(" %[^\n]s", user);
    printf("Password : ");
    scanf(" %[^\n]s", pass);

    char hashedUser[50], hashedPass[50];
    sprintf(hashedUser, "%u", fnv1a_32(user));
    sprintf(hashedPass, "%u", fnv1a_32(pass));

    int          index = hashFunction(hashedUser);
    struct User *curr  = userTable[index];

    while (curr != NULL) {
        if (strcmp(curr->username, hashedUser) == 0 &&
            strcmp(curr->password, hashedPass) == 0) {
            printf(SUCCESS "\nLogin Sukses! Selamat datang, %s." RESET "\n", user);
            strncpy(loggedInUser, user, 49);
            return curr->role;
        }
        curr = curr->next;
    }

    printf(DANGER "Error: Username atau Password salah!" RESET "\n");
    return -1;
}

// ADMIN MODULE
// Struktur: Binary Search Tree (BST) untuk indexing katalog

struct BSTNode *createBSTNode(int id, int index) {
    struct BSTNode *newNode = (struct BSTNode *)malloc(sizeof(struct BSTNode));
    newNode->id           = id;
    newNode->catalogIndex = index;
    newNode->left         = NULL;
    newNode->right        = NULL;
    return newNode;
}

struct BSTNode *insertBST(struct BSTNode *root, int id, int index) {
    if (root == NULL) {
        return createBSTNode(id, index);
    }
    if (id < root->id) {
        root->left  = insertBST(root->left,  id, index);
    } else if (id > root->id) {
        root->right = insertBST(root->right, id, index);
    }
    return root;
}

struct BSTNode *searchBST(struct BSTNode *root, int id) {
    if (root == NULL || root->id == id) {
        return root;
    }
    if (id < root->id) {
        return searchBST(root->left,  id);
    }
    return searchBST(root->right, id);
}

struct BSTNode *findMin(struct BSTNode *root) {
    while (root && root->left != NULL) {
        root = root->left;
    }
    return root;
}

struct BSTNode *deleteBST(struct BSTNode *root, int id) {
    if (root == NULL) return NULL;

    if (id < root->id) {
        root->left  = deleteBST(root->left,  id);
    } else if (id > root->id) {
        root->right = deleteBST(root->right, id);
    } else {
        if (root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        } else if (root->left == NULL) {
            struct BSTNode *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            struct BSTNode *temp = root->left;
            free(root);
            return temp;
        }
        struct BSTNode *successor = findMin(root->right);
        root->id           = successor->id;
        root->catalogIndex = successor->catalogIndex;
        root->right        = deleteBST(root->right, successor->id);
    }
    return root;
}

int searchIndexInBST(int id) {
    struct BSTNode *node = searchBST(rootIndex, id);
    return node ? node->catalogIndex : -1;
}

void createItem() {
    if (totalItems >= MAX_ITEMS) {
        printf(DANGER "Warehouse Full!\n" RESET);
        return;
    }

    int inputID;
    printf("Input Item ID : ");
    scanf("%d", &inputID);

    if (searchBST(rootIndex, inputID) != NULL) {
        printf(DANGER "ID Already Exists!\n" RESET);
        return;
    }

    catalog[totalItems].id = inputID;

    printf("Input Item Name : ");
    scanf(" %[^\n]", catalog[totalItems].name);

    printf("Input Category  : ");
    scanf(" %[^\n]", catalog[totalItems].category);

    printf("Input Price     : ");
    scanf("%d", &catalog[totalItems].price);

    printf("Input Stock     : ");
    scanf("%d", &catalog[totalItems].stock);

    rootIndex = insertBST(rootIndex, inputID, totalItems);
    totalItems++;

    saveProductDB();
    saveCartDB(loggedInUser);
    printf(SUCCESS "Item Successfully Added!\n" RESET);
}

// SORTING ALGORITHMS

static void swapItem(struct Item *a, struct Item *b) {
    struct Item temp = *a;
    *a = *b;
    *b = temp;
}

void bubbleSortCustom(struct Item arr[], int n, int (*cmp)(const void *, const void *)) {
    bool swapped;
    for (int i = 0; i < n - 1; i++) {
        swapped = false;
        for (int j = 0; j < n - 1 - i; j++) {
            if (cmp(&arr[j], &arr[j + 1]) > 0) {
                swapItem(&arr[j], &arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void mergeHelperCustom(struct Item arr[], int l, int m, int r, int (*cmp)(const void *, const void *)) {
    int n1 = m - l + 1;
    int n2 = r - m;
    struct Item *L = (struct Item *)malloc(n1 * sizeof(struct Item));
    struct Item *R = (struct Item *)malloc(n2 * sizeof(struct Item));
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];
    
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (cmp(&L[i], &R[j]) <= 0) arr[k++] = L[i++];
        else                        arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    free(L); free(R);
}

void mergeSortCustom(struct Item arr[], int l, int r, int (*cmp)(const void *, const void *)) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSortCustom(arr, l, m, cmp);
        mergeSortCustom(arr, m + 1, r, cmp);
        mergeHelperCustom(arr, l, m, r, cmp);
    }
}

int partitionCustom(struct Item arr[], int low, int high, int (*cmp)(const void *, const void *)) {
    struct Item pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (cmp(&arr[j], &pivot) <= 0) {
            i++;
            swapItem(&arr[i], &arr[j]);
        }
    }
    swapItem(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quickSortCustom(struct Item arr[], int low, int high, int (*cmp)(const void *, const void *)) {
    if (low < high) {
        int pi = partitionCustom(arr, low, high, cmp);
        quickSortCustom(arr, low, pi - 1, cmp);
        quickSortCustom(arr, pi + 1, high, cmp);
    }
}

// fungsi Comparator
int cmpNameAsc(const void *a, const void *b) {
    return strcmp(((struct Item *)a)->name, ((struct Item *)b)->name);
}
int cmpNameDesc(const void *a, const void *b) {
    return strcmp(((struct Item *)b)->name, ((struct Item *)a)->name);
}
int cmpPriceAsc(const void *a, const void *b) {
    return ((struct Item *)a)->price - ((struct Item *)b)->price;
}
int cmpPriceDesc(const void *a, const void *b) {
    return ((struct Item *)b)->price - ((struct Item *)a)->price;
}
int cmpStockDesc(const void *a, const void *b) {
    return ((struct Item *)b)->stock - ((struct Item *)a)->stock;
}
int cmpCategoryAsc(const void *a, const void *b) {
    int cmp = strcmp(((struct Item *)a)->category, ((struct Item *)b)->category);
    if (cmp == 0) return strcmp(((struct Item *)a)->name, ((struct Item *)b)->name);
    return cmp;
}
int cmpIDAsc(const void *a, const void *b) {
    return ((struct Item *)a)->id - ((struct Item *)b)->id;
}

void readCatalog() {
    if (totalItems <= 0) {
        printf(DANGER "\n[ERROR] Katalog kosong!\n" RESET);
        return;
    }

    int choice;
    printf(BRAND_BLUE "\n===== OPSI TAMPILAN KATALOG =====\n" RESET);
    printf("1. Alfabetis Nama (A-Z)\n");
    printf("2. Alfabetis Nama (Z-A)\n");
    printf("3. Harga (Low-High)\n");
    printf("4. Harga (High-Low)\n");
    printf("5. Stok Tersedia (High-Low)\n");
    printf("6. Pengelompokan Kategori (A-Z)\n");
    printf("7. Default (Berdasarkan ID)\n");
    printf("Pilih urutan : ");
    if (scanf("%d", &choice) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF);
        choice = 7;
    }

    int validCount = 0;
    for (int i = 0; i < totalItems; i++) {
        if (catalog[i].stock != -1) validCount++;
    }
    
    if (validCount == 0) {
        printf(DANGER "\nSemua barang habis atau telah dihapus!\n" RESET);
        return;
    }

    struct Item *arrDisplay = (struct Item *)malloc(validCount * sizeof(struct Item));
    struct Item *arrBubble  = (struct Item *)malloc(validCount * sizeof(struct Item));
    struct Item *arrMerge   = (struct Item *)malloc(validCount * sizeof(struct Item));
    struct Item *arrQuick   = (struct Item *)malloc(validCount * sizeof(struct Item));
    
    int idx = 0;
    for (int i = 0; i < totalItems; i++) {
        if (catalog[i].stock != -1) {
            arrDisplay[idx] = catalog[i];
            arrBubble[idx]  = catalog[i];
            arrMerge[idx]   = catalog[i];
            arrQuick[idx]   = catalog[i];
            idx++;
        }
    }

    int (*selectedCmp)(const void *, const void *) = cmpIDAsc;
    switch(choice) {
        case 1: selectedCmp = cmpNameAsc; break;
        case 2: selectedCmp = cmpNameDesc; break;
        case 3: selectedCmp = cmpPriceAsc; break;
        case 4: selectedCmp = cmpPriceDesc; break;
        case 5: selectedCmp = cmpStockDesc; break;
        case 6: selectedCmp = cmpCategoryAsc; break;
        case 7: selectedCmp = cmpIDAsc; break;
        default: 
            printf(DANGER "Invalid Option! Menggunakan urutan Default.\n" RESET);
            selectedCmp = cmpIDAsc;
            break;
    }

    //Sort using qsort for display
    qsort(arrDisplay, validCount, sizeof(struct Item), selectedCmp);

    // Print Catalog
    CLEAR();
    printf(BRAND_BLUE "\n========== KATALOG PRODUK DUOMART ==========\n" RESET);
    printf("%-5s | %-30s | %-20s | %-12s | %s\n", "ID", "NAMA", "KATEGORI", "HARGA", "STOK");
    printf("--------------------------------------------------------------------------------------\n");
    for (int i = 0; i < validCount; i++) {
        printf("%-5d | %-30.30s | %-20.20s | Rp%-10d | %d\n", 
               arrDisplay[i].id, arrDisplay[i].name, arrDisplay[i].category, arrDisplay[i].price, arrDisplay[i].stock);
    }
    printf("--------------------------------------------------------------------------------------\n");

    // Measure Bubble Sort
    clock_t start, end;
    start = clock();
    bubbleSortCustom(arrBubble, validCount, selectedCmp);
    end = clock();
    double timeBubble = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

    // Measure Merge Sort
    start = clock();
    mergeSortCustom(arrMerge, 0, validCount - 1, selectedCmp);
    end = clock();
    double timeMerge = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

    // Measure Quick Sort
    start = clock();
    quickSortCustom(arrQuick, 0, validCount - 1, selectedCmp);
    end = clock();
    double timeQuick = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

    double fastest = timeBubble;
    const char *winner = "Bubble Sort";
    if (timeMerge < fastest) { fastest = timeMerge; winner = "Merge Sort"; }
    if (timeQuick < fastest) { fastest = timeQuick; winner = "Quick Sort"; }

    printf(BRAND_BLUE "\n  [ LAPORAN EFISIENSI SORTING (%d ITEM) ]\n" RESET, validCount);
    printf("  %-15s : %.4f ms\n", "Bubble Sort", timeBubble);
    printf("  %-15s : %.4f ms\n", "Merge Sort",  timeMerge);
    printf("  %-15s : %.4f ms\n", "Quick Sort",  timeQuick);
    printf(SUCCESS "  >> Tercepat : %s (%.4f ms)\n" RESET, winner, fastest);
    
    free(arrDisplay);
    free(arrBubble);
    free(arrMerge);
    free(arrQuick);
}

void updateItem() {
    int id;
    printf("Input ID To Update : ");
    scanf("%d", &id);

    struct BSTNode *found = searchBST(rootIndex, id);
    if (found == NULL) {
        printf(DANGER "Item Not Found!\n" RESET);
        return;
    }

    int index = found->catalogIndex;

    printf("New Name     : ");
    scanf(" %[^\n]", catalog[index].name);

    printf("New Category : ");
    scanf(" %[^\n]", catalog[index].category);

    printf("New Price    : ");
    scanf("%d", &catalog[index].price);

    printf("New Stock    : ");
    scanf("%d", &catalog[index].stock);

    saveProductDB();
    saveCartDB(loggedInUser);
    printf(SUCCESS "Item Successfully Updated!\n" RESET);
}

void deleteItem() {
    int id;
    printf("Input ID To Delete : ");
    scanf("%d", &id);

    struct BSTNode *found = searchBST(rootIndex, id);
    if (found == NULL) {
        printf(DANGER "Item Not Found!\n" RESET);
        return;
    }

    int index = found->catalogIndex;
    catalog[index].stock = -1;

    rootIndex = deleteBST(rootIndex, id);

    saveProductDB();
    saveCartDB(loggedInUser);
    printf(WARNING "Item Successfully Deleted!\n" RESET);
}

void adminMenu() {
    int choice;
    do {
        CLEAR();
        printf(BRAND_BLUE "\n===== ADMIN MENU =====\n" RESET);
        printf("1. Add Item\n");
        printf("2. View Catalog\n");
        printf("3. Update Item\n");
        printf("4. Delete Item\n");
        printf("5. Exit\n");
        printf("Choose : ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: createItem();              break;
            case 2: readCatalog();             break;
            case 3: updateItem();              break;
            case 4: deleteItem();              break;
            
            
            case 5: printf(SUCCESS "Exiting Admin Module...\n" RESET); break;
            default: printf(DANGER "Invalid Menu!\n" RESET);
        }
        if (choice != 5) pauseScreen();
    } while (choice != 5);
}

// TRANSACTION MODULE
// Struktur: Linked List (Cart), Stack (Undo), Queue (Order)

void addToCart(int id, int qty) {
    int idx = searchIndexInBST(id);

    if (idx == -1 || catalog[idx].stock < qty) {
        printf("Stok tidak cukup/ID salah!\n");
        return;
    }

    CartNode *newNode = (CartNode *)malloc(sizeof(CartNode));
    if (!newNode) {
        printf("Error: malloc gagal!\n");
        return;
    }

    newNode->id    = catalog[idx].id;
    strncpy(newNode->name, catalog[idx].name, 100);
    newNode->price = catalog[idx].price;
    newNode->qty   = qty;
    newNode->next  = headCart;
    headCart       = newNode;

    pushUndo(id);
    printf("Berhasil masuk keranjang!\\n");
    saveCartDB(loggedInUser);
}

void displayCart(void) {
    if (!headCart) {
        printf("Keranjang kosong!\n");
        return;
    }

    printf("\n===== ISI KERANJANG =====\n");
    printf("%-5s %-20s %-10s %-5s %-12s\n",
           "ID", "Nama", "Harga", "Qty", "Subtotal");
    printf("--------------------------------------------------\n");

    CartNode *cur  = headCart;
    int grandTotal = 0;
    while (cur) {
        int subtotal = cur->price * cur->qty;
        grandTotal  += subtotal;
        printf("%-5d %-20s %-10d %-5d %-12d\n",
               cur->id, cur->name, cur->price, cur->qty, subtotal);
        cur = cur->next;
    }

    printf("--------------------------------------------------\n");
    printf("Total: Rp%d\n\n", grandTotal);
}

void deleteFromCart(int id) {
    CartNode *cur  = headCart;
    CartNode *prev = NULL;

    while (cur) {
        if (cur->id == id) {
            if (prev) prev->next = cur->next;
            else      headCart   = cur->next;
            free(cur);
            printf("Barang ID %d dihapus dari keranjang.\\n", id);
            saveCartDB(loggedInUser);
            return;
        }
        prev = cur;
        cur  = cur->next;
    }
    printf("Barang ID %d tidak ditemukan di keranjang.\n", id);
}

void pushUndo(int id) {
    UndoNode *newNode = (UndoNode *)malloc(sizeof(UndoNode));
    if (!newNode) return;
    newNode->itemID = id;
    newNode->next   = topUndo;
    topUndo         = newNode;
}

int popUndo(void) {
    if (!topUndo) return -1;
    UndoNode *temp = topUndo;
    int id         = temp->itemID;
    topUndo        = topUndo->next;
    free(temp);
    return id;
}

void executeUndo(void) {
    if (!topUndo) {
        printf("Belum ada barang di keranjang.\n");
        return;
    }
    int id = popUndo();
    deleteFromCart(id);
    printf("Barang terakhir dibatalkan!\\n");
    saveCartDB(loggedInUser);
}

void enqueueOrder(char *name, int total) {
    OrderNode *newNode = (OrderNode *)malloc(sizeof(OrderNode));
    if (!newNode) {
        printf("Error: malloc gagal!\n");
        return;
    }
    strncpy(newNode->customerName, name, 50);
    newNode->totalBill = total;
    newNode->next      = NULL;

    if (!rearQueue) {
        frontQueue = rearQueue = newNode;
    } else {
        rearQueue->next = newNode;
        rearQueue       = newNode;
    }
}

void checkout(char *name) {
    if (!headCart) {
        printf(DANGER "Keranjang kosong!\n" RESET);
        return;
    }

    int total = 0;
    CartNode *cur = headCart;
    
    CLEAR();
    printf(BRAND_BLUE "\n================ DUOMART RECEIPT ================\n" RESET);
    printf("Customer : %s\n", name);
    printf("-------------------------------------------------\n");
    printf("%-20s %-10s %-5s %-12s\n", "Nama Barang", "Harga", "Qty", "Subtotal");
    printf("-------------------------------------------------\n");

    while (cur) {
        int subtotal = cur->price * cur->qty;
        total += subtotal;
        
        printf("%-20.20s Rp%-9d %-5d Rp%-10d\n", 
               cur->name, cur->price, cur->qty, subtotal);
               
        int idx = searchIndexInBST(cur->id);
        if (idx != -1) {
            catalog[idx].stock -= cur->qty;
        }
        cur = cur->next;
    }
    printf("-------------------------------------------------\n");
    printf(SUCCESS "Total Belanja : Rp%d\n" RESET, total);
    printf(BRAND_BLUE "=================================================\n" RESET);

    int paymentChoice;
    char paymentMethod[30] = "Tunai";
    printf("\n" WARNING "Pilih Metode Pembayaran:" RESET "\n");
    printf("1. Tunai (Cash)\n");
    printf("2. Transfer Bank / Virtual Account\n");
    printf("3. E-Wallet (OVO/GoPay/Dana)\n");
    printf("Pilihan Anda : ");
    if (scanf("%d", &paymentChoice) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF);
        paymentChoice = 1;
    }
    switch(paymentChoice) {
        case 1: strcpy(paymentMethod, "Tunai"); break;
        case 2: strcpy(paymentMethod, "Transfer Bank"); break;
        case 3: strcpy(paymentMethod, "E-Wallet"); break;
        default: strcpy(paymentMethod, "Tunai"); break;
    }

    enqueueOrder(name, total);

    CartNode *cartTemp;
    while (headCart) {
        cartTemp = headCart;
        headCart = headCart->next;
        free(cartTemp);
    }
    headCart = NULL;

    UndoNode *undoTemp;
    while (topUndo) {
        undoTemp = topUndo;
        topUndo  = topUndo->next;
        free(undoTemp);
    }
    topUndo = NULL;

    saveProductDB();
    saveCartDB(loggedInUser);
    printf(SUCCESS "\nPembayaran menggunakan %s berhasil!\n" RESET, paymentMethod);
    printf(WARNING "Checkout Sukses! Pesanan %s telah masuk ke antrean (Queue).\n" RESET, name);
}

void customerMenu() {
    int choice;
    do {
        CLEAR();
        printf(BRAND_BLUE "\n===== CUSTOMER MENU =====\n" RESET);
        printf("1. Lihat Katalog\n");
        printf("2. Tambah ke Keranjang\n");
        printf("3. Lihat Keranjang\n");
        printf("4. Hapus dari Keranjang\n");
        printf("5. Undo Tambah Terakhir\n");
        printf("6. Checkout\n");
        printf("7. Exit\n");
        printf("Choose : ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                readCatalog();
                break;
            case 2: {
                int id, qty;
                printf("Input ID Barang : ");
                scanf("%d", &id);
                printf("Input Qty       : ");
                scanf("%d", &qty);
                addToCart(id, qty);
                break;
            }
            case 3:
                displayCart();
                break;
            case 4: {
                int id;
                printf("Input ID Barang yang dihapus : ");
                scanf("%d", &id);
                deleteFromCart(id);
                break;
            }
            case 5:
                executeUndo();
                break;
            case 6: {
                checkout(loggedInUser);
                break;
            }
            case 7:
                printf(SUCCESS "Exiting Customer Module...\n" RESET);
                while (headCart) {
                    CartNode *temp = headCart;
                    headCart = headCart->next;
                    free(temp);
                }
                while (topUndo) {
                    UndoNode *utemp = topUndo;
                    topUndo = topUndo->next;
                    free(utemp);
                }
                break;
            default:
                printf(DANGER "Invalid Menu!\n" RESET);
        }
        if (choice != 7) pauseScreen();
    } while (choice != 7);
}

// ANIMATION MODULE
// Diadaptasi dari anmsLogin.cpp ke C

const char *logo[] = {
    "    ____              __  ___          __  ",
    "   / __ \\__  ______  /  |/  /___ _____/ /_ ",
    "  / / / / / / / __ \\/ /|_/ / __ `/ ___/ __/",
    " / /_/ / /_/ / /_/ / /  / / /_/ / /  / /_  ",
    "/_____/\\__,_/\\____/_/  /_/\\__,_/_/   \\__/  "
};
const int logo_lines = 5;

void printCentered(const char *text, int totalWidth) {
    int len     = (int)strlen(text);
    int realLen = 0;
    for (int i = 0; i < len; i++) {
        if (text[i] == '\033') {
            while (text[i] != 'm' && i < len) i++;
        } else {
            realLen++;
        }
    }
    int padding = (totalWidth - realLen) / 2;
    if (padding > 0) printf("%*s", padding, "");
    printf("%s\n", text);
}

void printHeader(int screenWidth, int delay_ms) {
    printf(BORDER_GRAY);
    printCentered("|| | || || | || | | || | | || | || || | |", screenWidth);
    printf(RESET "\n");

    for (int i = 0; i < logo_lines; i++) {
        printf(BRAND_CYAN BOLD);
        printCentered(logo[i], screenWidth);
        printf(RESET);
        if (delay_ms > 0) {
            fflush(stdout);
            DELAY(delay_ms);
        }
    }

    printf("\n" DIM TEXT_WHITE);
    printCentered(">>> #1 Data Structure E-Commerce App <<<", screenWidth);
    printCentered("Shop Smart. Sort Fast. Secure Always.", screenWidth);
    printf(RESET "\n");
}

void playIntroAnimation() {
    int i, j;
    int screenWidth = 80;

    CLEAR();

    const char *bootLogs[] = {
        "[OK] Group 2 (Sorting, compare at least 3 different types of sorting and analyze the efficiencies.)...",
        "[OK] Hia, Carlen Janice - 2902574536...",
        "[OK] Muhammad Nabil Kurniawan - 2902648805...",
        "[OK] Syahnaz Sukma Annisa - 2902709460..."
    };

    printf("\n");
    for (i = 0; i < sizeof(bootLogs) / sizeof(bootLogs[0]); i++) {
        printf(DIM SUCCESS " %s\n" RESET, bootLogs[i]);
        fflush(stdout);
        DELAY(250);
    }
    DELAY(400);

    for (i = 0; i < 30; i += 2) {
        CLEAR();
        printf("\n\n\n\n\n");
        printf("%*s" WARNING "    \\_____" RESET "\n", i, "");
        printf("%*s" WARNING "    |____|  " BRAND_CYAN "DUO MART" RESET "\n", i, "");
        printf("%*s" WARNING "    (O)(O)" RESET "\n", i, "");
        DELAY(60);
    }

    CLEAR();
    printf("\n\n");
    printHeader(screenWidth, 100);

    int         barWidth = 40;
    const char *status;

    printf("\n");
    for (i = 0; i <= barWidth; i++) {
        if      (i < 10) status = "Preparing Cart... ";
        else if (i < 25) status = "Encrypting Data...";
        else if (i < 38) status = "Sorting Catalog...";
        else             status = "Ready to Shop!    ";

        printf("\r    " WARNING "%s " RESET, status);
        printf(BORDER_GRAY "[" RESET);

        for (j = 0; j < barWidth; j++) {
            if (j < i) printf(SUCCESS "#" RESET);
            else        printf(BORDER_GRAY "-" RESET);
        }
        printf(BORDER_GRAY "] " TEXT_WHITE "%3d%%" RESET, (i * 100) / barWidth);
        fflush(stdout);

        if (i == 25) DELAY(300);
        else         DELAY(40);
    }
    printf("\n");
    DELAY(500);

    CLEAR();
    printf("\n\n");
    printHeader(screenWidth, 0);
    printf("\n\n");

    for (i = 0; i < 3; i++) {
        printf(WARNING BOLD);
        printCentered("[ PRESS ENTER TO START SHOPPING ]", screenWidth);
        fflush(stdout);
        DELAY(400);
        printf("\r%*s\r", screenWidth, "");
        fflush(stdout);
        DELAY(200);
    }

    printf(SUCCESS BOLD);
    printCentered("[ PRESS ENTER TO START SHOPPING ]", screenWidth);
    printf(RESET "\n");

    getchar();
}

// MAIN

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    srand((unsigned int)time(NULL));

    // Inisialisasi Hash Table
    initHashTable();

    // Muat data dari file database
    loadAccountDB();
    loadProductDB();

    // Tampilkan animasi intro
    playIntroAnimation();
    CLEAR();

    int pilihan = 0;
    while (pilihan != 3) {
        CLEAR();
        printf("\n");
        printf(BRAND_CYAN "=================================================\n" RESET);
        printf(BOLD TEXT_WHITE "     DUOMART PORTAL - Welcome!\n" RESET);
        printf(BRAND_CYAN "=================================================\n" RESET);
        printf(SUCCESS " [1]" RESET " Registrasi Akun\n");
        printf(SUCCESS " [2]" RESET " Login\n");
        printf(DANGER   " [3]" RESET " Keluar Aplikasi\n");
        printf("-------------------------------------------------\n");
        printf("Pilih Menu: ");

        if (scanf("%d", &pilihan) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        if (pilihan == 1) {
            registerAccount();
            pauseScreen();
        } else if (pilihan == 2) {
            int role = loginAccount();
            if (role == 0) {
                pauseScreen();
                adminMenu();
            } else if (role == 1) {
                pauseScreen();
                loadCartDB(loggedInUser);
                customerMenu();
            } else {
                pauseScreen();
            }
        }
    }

    printf(BRAND_CYAN "\nTerima kasih telah berbelanja di DuoMart!\n" RESET);
    return 0;
}





