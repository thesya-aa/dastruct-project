#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    #define DELAY(ms) Sleep(ms)
    #define CLEAR() system("cls")
#else
    #include <unistd.h>
    #define DELAY(ms) usleep((ms) * 1000)
    #define CLEAR() system("clear")
#endif

// --- ANSI COLOR CODES (Dark Mode Friendly) ---
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define DIM         "\033[2m"
#define RED         "\033[1;31m"
#define GREEN       "\033[1;32m"
#define YELLOW      "\033[1;33m"
#define CYAN        "\033[1;36m"
#define WHITE       "\033[1;37m"
#define DARK_GRAY   "\033[1;30m"

// ASCII Art Logo DuoMart
const char *logo[] = {
    "    ____              __  ___          __  ",
    "   / __ \\__  ______  /  |/  /___ _____/ /_ ",
    "  / / / / / / / __ \\/ /|_/ / __ `/ ___/ __/",
    " / /_/ / /_/ / /_/ / /  / / /_/ / /  / /_  ",
    "/_____/\\__,_/\\____/_/  /_/\\__,_/_/   \\__/  "
};
const int logo_lines = 5;

// ==========================================================
// FUNGSI HELPER (Modularisasi)
// ==========================================================

// 1. Cetak teks di tengah layar
void printCentered(const char* text, int totalWidth) {
    int len = strlen(text);
    int realLen = 0;
    for(int i = 0; i < len; i++) {
        if(text[i] == '\033') {
            while(text[i] != 'm' && i < len) i++;
        } else {
            realLen++;
        }
    }
    int padding = (totalWidth - realLen) / 2;
    if (padding > 0) printf("%*s", padding, "");
    printf("%s\n", text);
}

// 2. Cetak Barcode, Logo, dan Tagline (Menghindari kode berulang)
void printHeader(int screenWidth, int delay_ms) {
    printf(DARK_GRAY);
    printCentered("|| | || || | || | | || | | || | || || | |", screenWidth);
    printf(RESET "\n");

    for (int i = 0; i < logo_lines; i++) {
        printf(CYAN BOLD);
        printCentered(logo[i], screenWidth);
        printf(RESET);
        if (delay_ms > 0) {
            fflush(stdout);
            DELAY(delay_ms);
        }
    }

    printf("\n" DIM WHITE);
    printCentered(">>> #1 Data Structure E-Commerce App <<<", screenWidth);
    printCentered("Shop Smart. Sort Fast. Secure Always.", screenWidth);
    printf(RESET "\n");
}

// ==========================================================
// FUNGSI UTAMA ANIMASI
// ==========================================================
// ==========================================================
// FUNGSI UTAMA ANIMASI (FIXED ASCII VERSION)
// ==========================================================
void playIntroAnimation() {
    int i, j;
    int screenWidth = 80;

    CLEAR();

    // FASE 1: LOG E-COMMERCE BOOTING
    const char* bootLogs[] = {
        "[OK] Connecting to DuoMart Cloud Servers...",
        "[OK] Fetching 1000+ Product Database...",
        "[OK] Initializing Binary Search Tree Indexing...",
        "[OK] Securing Gateway with FNV-1a Hashing...",
        "[OK] Applying RC4 Symmetric Encryption to Cart..."
    };
    
    printf("\n");
    for (i = 0; i < 5; i++) {
        printf(DIM GREEN " %s\n" RESET, bootLogs[i]);
        fflush(stdout);
        DELAY(250);
    }
    DELAY(400);

    // FASE 2: ANIMASI TROLI BELANJA BERJALAN
    for (i = 0; i < 30; i += 2) {
        CLEAR();
        printf("\n\n\n\n\n");
        printf("%*s" YELLOW "    \\_____" RESET "\n", i, "");
        printf("%*s" YELLOW "    |____|  " CYAN " ~ FAST DELIVERY ~" RESET "\n", i, "");
        printf("%*s" YELLOW "    (O)(O)" RESET "\n", i, "");
        DELAY(60);
    }

    // FASE 3: MUNCUL HEADER DENGAN EFEK TURUN
    CLEAR();
    printf("\n\n");
    printHeader(screenWidth, 100); 

    // ==========================================================
    // FASE 4: DYNAMIC LOADING BAR (100% TERMINAL SAFE)
    // ==========================================================
    int barWidth = 40;
    const char *status;
    
    printf("\n");
    for (i = 0; i <= barWidth; i++) {
        // Penentuan teks status dengan panjang yang SAMA (18 karakter) agar tidak balapan
        if (i < 10) status = "Preparing Cart... ";
        else if (i < 25) status = "Encrypting Data...";
        else if (i < 38) status = "Sorting Catalog...";
        else status = "Ready to Shop!    ";
        
        // \r mengembalikan kursor. Spasi di awal untuk padding tengah.
        printf("\r    " YELLOW "%s " RESET, status);
        printf(DARK_GRAY "[" RESET);
        
        // Loop untuk balok loading (menggunakan karakter # dan - yang aman di semua OS)
        for (j = 0; j < barWidth; j++) {
            if (j < i) printf(GREEN "#" RESET);
            else printf(DARK_GRAY "-" RESET); 
        }
        
        // %3d memastikan angka persentase selalu mengambil 3 digit ruang (misal: "  5%", " 50%", "100%")
        printf(DARK_GRAY "] " WHITE "%3d%%" RESET, (i * 100) / barWidth);
        fflush(stdout);
        
        if (i == 25) DELAY(300);
        else DELAY(40);
    }
    printf("\n");
    DELAY(500);

    // FASE 5: BLINKING "PRESS ENTER TO SHOP"
    CLEAR();
    printf("\n\n");
    printHeader(screenWidth, 0); 
    printf("\n\n");
    
    for (i = 0; i < 3; i++) {
        printf(YELLOW BOLD);
        printCentered("[ PRESS ENTER TO START SHOPPING ]", screenWidth);
        fflush(stdout);
        DELAY(400);
        
        printf("\r%*s\r", screenWidth, ""); 
        fflush(stdout);
        DELAY(200);
    }
    
    printf(GREEN BOLD);
    printCentered("[ PRESS ENTER TO START SHOPPING ]", screenWidth);
    printf(RESET "\n");

    getchar(); 
}

// ==========================================================
// FUNGSI MAIN
// ==========================================================
int main() {
    playIntroAnimation();
    CLEAR();

    int pilihan = 0;
    while (pilihan != 3) {
        printf("\n");
        printf(CYAN "=================================================\n" RESET);
        printf(BOLD WHITE " 🛒  DUOMART PORTAL - Welcome, Shopper! \n" RESET);
        printf(CYAN "=================================================\n" RESET);
        printf(GREEN " [1]" RESET " Registrasi Akun (Secure FNV-1a & RC4)\n");
        printf(GREEN " [2]" RESET " Login Member\n");
        printf(RED   " [3]" RESET " Keluar Aplikasi\n");
        printf("-------------------------------------------------\n");
        printf("Pilih Menu: ");
        
        if (scanf("%d", &pilihan) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue; 
        }

        if (pilihan == 1) {
            printf("\n>>> Membuka Menu Registrasi...\n");
            // registerUser();
        } 
        else if (pilihan == 2) {
            printf("\n>>> Membuka Menu Login...\n");
            // loginUser();
        }
    }

    printf(CYAN "\nTerima kasih telah berbelanja di DuoMart!\n" RESET);
    return 0;
}