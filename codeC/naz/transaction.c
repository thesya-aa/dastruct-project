#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "supermarket.h"
 
//  Global Variables 
CartNode  *headCart   = NULL;
UndoNode  *topUndo    = NULL;
OrderNode *frontQueue = NULL;
OrderNode *rearQueue  = NULL;

extern Item catalog[];
extern int  totalItems;
extern int  searchIndexInBST(int id);
 
void pushUndo(int id);
void enqueueOrder(char *name, int total);
void deleteFromCart(int id);
 
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
 
    printf("Berhasil masuk keranjang!\n");
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
    printf("Total: %d\n\n", grandTotal);
}
 
void deleteFromCart(int id) {
    CartNode *cur  = headCart;
    CartNode *prev = NULL;
 
    while (cur) {
        if (cur->id == id) {
            if (prev) prev->next = cur->next;
            else      headCart   = cur->next;
            free(cur);
            printf("Barang ID %d dihapus dari keranjang.\n", id);
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
 
    printf("Barang terakhir dibatalkan!\n");
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
        printf("Keranjang kosong!\n");
        return;
    }
 
    int       total = 0;
    CartNode *cur   = headCart;
    while (cur) {
        total += cur->price * cur->qty;
 
        int idx = searchIndexInBST(cur->id);
        if (idx != -1) {
            catalog[idx].stock -= cur->qty;
        }
        cur = cur->next;
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
 
    printf("Checkout Sukses, masuk antrian!\n");
    printf("Nama: %s | Total: %d\n", name, total);
}
