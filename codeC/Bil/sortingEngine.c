#include "supermarket.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct Item item;

void copyCatalog(item dest[], item stc[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = stc[i];
    }
}

static void swapItem(item* a, item* b) {
    item temp = *a;
    *a = *b;
    *b = temp;
}

void bubbleSort(item arr[], int n) {
    bool swapped;
    for (int i = 0; i < n - 1; i++) {
        swapped = false;
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j].price > arr[j + 1].price) {
                swapItem(&arr[j], &arr[j + 1]);
                swapped = true;
            }
        }
        if (swapped == false)
            break;
    }
}

static void merge(item arr[], int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    item* L = (item*)malloc(n1 * sizeof(item));
    item* R = (item*)malloc(n2 * sizeof(item));

    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i].price <= R[j].price) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(item arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

static int partition(item arr[], int low, int high) {
    int pivot = arr[high].price;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j].price <= pivot) {
            i++;
            swapItem(&arr[i], &arr[j]);
        }
    }
    swapItem(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quickSort(item arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

extern struct Item catalog[];
extern int totalItems;

void executeComparisonReport() {
    if (totalItems <= 0) {
        printf(COLOR_RED "\n[ERROR] Katalog kosong! Tidak dapat menjalankan laporan.\n" COLOR_RESET);
        return;
    }

    item* arrBubble = (item*)malloc(totalItems * sizeof(item));
    item* arrMerge  = (item*)malloc(totalItems * sizeof(item));
    item* arrQuick  = (item*)malloc(totalItems * sizeof(item));

    if (!arrBubble || !arrMerge || !arrQuick) {
        printf(COLOR_RED "\n[ERROR] Gagal mengalokasikan memori!\n" COLOR_RESET);
        free(arrBubble); free(arrMerge); free(arrQuick);
        return;
    }

    copyCatalog(arrBubble, catalog, totalItems);
    copyCatalog(arrMerge,  catalog, totalItems);
    copyCatalog(arrQuick,  catalog, totalItems);

    clock_t start, end;
    double timeBubble, timeMerge, timeQuick;

    start = clock();
    bubbleSort(arrBubble, totalItems);
    end = clock();
    timeBubble = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

    start = clock();
    mergeSort(arrMerge, 0, totalItems - 1);
    end = clock();
    timeMerge = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

    start = clock();
    quickSort(arrQuick, 0, totalItems - 1);
    end = clock();
    timeQuick = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

    printf(COLOR_CYAN "\n╔══════════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN   "║      LAPORAN EFISIENSI ALGORITMA SORTING     ║\n" COLOR_RESET);
    printf(COLOR_CYAN   "╚══════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("  Jumlah Data   : %d item\n\n", totalItems);

    printf("  %-15s %s\n", "Algoritma", "Waktu Eksekusi");
    printf("  %-15s %s\n", "─────────────", "──────────────");
    printf("  %-15s %.4f ms\n", "Bubble Sort",  timeBubble);
    printf("  %-15s %.4f ms\n", "Merge Sort",   timeMerge);
    printf("  %-15s %.4f ms\n", "Quick Sort",   timeQuick);
    printf("\n");

    double fastest = timeBubble;
    const char* winner = "Bubble Sort";
    if (timeMerge < fastest) { fastest = timeMerge; winner = "Merge Sort"; }
    if (timeQuick < fastest) { fastest = timeQuick; winner = "Quick Sort"; }

    printf(COLOR_GREEN "  >> Algoritma Tercepat : %s (%.4f ms)\n" COLOR_RESET, winner, fastest);
    printf(COLOR_CYAN "══════════════════════════════════════════════════\n\n" COLOR_RESET);

    int preview = (totalItems < 5) ? totalItems : 5;
    printf(COLOR_YELLOW "  [Preview] 5 Item Harga Terendah (Merge Sort):\n" COLOR_RESET);
    printf("  %-5s %-30s %s\n", "ID", "Nama", "Harga");
    printf("  %-5s %-30s %s\n", "──", "────────────────────────────", "──────");
    for (int i = 0; i < preview; i++) {
        printf("  %-5d %-30s Rp%d\n",
               arrMerge[i].id,
               arrMerge[i].name,
               arrMerge[i].price);
    }
    printf("\n");

    free(arrBubble);
    free(arrMerge);
    free(arrQuick);
}
