#include "supermarket.h"

struct User* userTable[100];

void initHashTable() {
    for (int i = 0; i < 100; i++) {
        userTable[i] = NULL;
    }
}

int hashFunction(char* str) {
    int hashValue = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        hashValue += (int)str[i];
    }
    return hashValue % 100;
}

void registerAccount() {
    char user[50], pass[50];
    int role;

    printf(COLOR_CYAN "\n=== PENDAFTARAN AKUN ===" COLOR_RESET "\n");
    printf("Masukkan Username : ");
    scanf(" %[^\n]s", user);
    printf("Masukkan Password : ");
    scanf(" %[^\n]s", pass);
    printf("Pilih Role (0: Admin, 1: Customer): ");
    scanf("%d", &role);

    int index = hashFunction(user);

    struct User* newUser = (struct User*)malloc(sizeof(struct User));
    strcpy(newUser->username, user);
    strcpy(newUser->password, pass);
    newUser->role = role;
    newUser->next = NULL;

    if (userTable[index] == NULL) {
        userTable[index] = newUser;
    } else {
        newUser->next = userTable[index];
        userTable[index] = newUser;
    }

    printf(COLOR_GREEN "Registrasi Berhasil! Silakan Login." COLOR_RESET "\n");
}

int loginAccount() {
    char user[50], pass[50];

    printf(COLOR_CYAN "\n=== LOGIN SISTEM ===" COLOR_RESET "\n");
    printf("Username : ");
    scanf(" %[^\n]s", user);
    printf("Password : ");
    scanf(" %[^\n]s", pass);

    int index = hashFunction(user);
    struct User* curr = userTable[index];

    while (curr != NULL) {
        if (strcmp(curr->username, user) == 0 && strcmp(curr->password, pass) == 0) {
            printf(COLOR_GREEN "\nLogin Sukses! Selamat datang, %s." COLOR_RESET "\n", curr->username);
            return curr->role;
        }
        curr = curr->next;
    }

    printf(COLOR_RED "Error: Username atau Password salah!" COLOR_RESET "\n");
    return -1;
}
