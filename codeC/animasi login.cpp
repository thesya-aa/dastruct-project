#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Deteksi OS untuk fungsi Delay (Tidur) dan Clear Screen
#ifdef _WIN32
    #include <windows.h>
    #define DELAY(ms) Sleep(ms)
    #define CLEAR() system("cls")
#else
    #include <unistd.h>
    #define DELAY(ms) usleep((ms) * 1000)
    #define CLEAR() system("clear")
#endif

// --- ANSI COLOR CODES ---
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define DIM         "\033[2m"
#define RED         "\033[1;31m"
#define GREEN       "\033[1;32m"
#define YELLOW      "\033[1;33m"
#define BLUE        "\033[1;34m"
#define MAGENTA     "\033[1;35m"
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

// Fungsi untuk membuat teks di tengah (Padding)
void printCentered(const char* text, int totalWidth) {
    int len = strlen(text);
    // Mengabaikan karakter ANSI saat menghitung panjang teks asli
    int realLen = 0;
    for(int i = 0; i < len; i++) {
        if(text[i] == '\033') {
            while(text[i] != 'm' && i < len) i++;
        } else {
            realLen++;
        }
    }
    
    int padding = (totalWidth - realLen) / 2;
    if (padding > 0) {
        printf("%*s", padding, ""); // Print spasi
    }
    printf("%s\n", text);
}

// Fungsi utama Animasi Intro
void playIntroAnimation() {
    int i, j, frame;
    int screenWidth = 80; // Asumsi lebar terminal 80 karakter

    CLEAR();

    // 1. EFEK SLIDE DOWN & TYPEWRITER UNTUK LOGO
    for (i = 0; i < logo_lines; i++) {
        printf(CYAN BOLD);
        printCentered(logo[i], screenWidth);
        printf(RESET);
        DELAY(150); // Delay per baris (150 ms)
    }
    
    DELAY(500);

    // 2. EFEK GLOWING / COLOR CYCLING
    // Mengubah warna logo secara bergantian
    const char* glowColors[] = {DARK_GRAY, BLUE, CYAN, WHITE, CYAN, BLUE, DARK_GRAY};
    int numColors = 7;

    for (frame = 0; frame < numColors; frame++) {
        CLEAR();
        // Print beberapa spasi kosong (jarak dari atas)
        printf("\n\n"); 
        
        for (i = 0; i < logo_lines; i++) {
            printf("%s", glowColors[frame]);
            printCentered(logo[i], screenWidth);
        }
        
        printf("\n" DIM);
        printCentered("Your Ultimate E-Commerce Experience", screenWidth);
        printf(RESET "\n");
        
        DELAY(150);
    }

    // 3. EFEK LOADING BAR PROGRESS
    printf("\n\n");
    int barWidth = 40;
    int padding = (screenWidth - barWidth - 10) / 2; 
    
    printf("%*s" CYAN "Booting DuoMart System...\n" RESET, padding + 5, "");
    
    printf("%*s" DARK_GRAY "[", padding, ""); // Kurung buka loading
    
    for (i = 0; i < barWidth; i++) {
        // Cetak blok progress
        printf(GREEN "■" RESET);
        
        // Memaksa output segera dicetak ke terminal tanpa menunggu (SANGAT PENTING UNTUK ANIMASI C)
        fflush(stdout); 
        
        // Percepat di awal, agak melambat di akhir agar realistis
        if (i < barWidth / 2) {
            DELAY(30); 
        } else if (i == barWidth - 5) {
            DELAY(400); // Simulasi "lag" loading sebentar
        } else {
            DELAY(60);
        }
    }
    printf(DARK_GRAY "] " WHITE "100%%\n" RESET);
    DELAY(500);

    // 4. EFEK BLINK "PRESS ENTER TO CONTINUE"
    CLEAR();
    printf("\n\n");
    for (i = 0; i < logo_lines; i++) {
        printf(CYAN BOLD);
        printCentered(logo[i], screenWidth);
    }
    printf("\n" DIM);
    printCentered("Your Ultimate E-Commerce Experience", screenWidth);
    printf(RESET "\n\n\n");
    
    for (i = 0; i < 3; i++) {
        printf(YELLOW BOLD);
        printCentered(">>> SYSTEM READY <<<", screenWidth);
        fflush(stdout);
        DELAY(400);
        
        // Hapus tulisan (Blink efek)
        printf("\r%*s\r", screenWidth, ""); 
        fflush(stdout);
        DELAY(200);
    }
    
    printf(GREEN BOLD);
    printCentered(">>> SYSTEM READY <<<", screenWidth);
    printf(RESET "\n\n");
    
    // Bebas tekan enter / delay otomatis
    DELAY(1000); 
}

// ---------------------------------------------------------
// MAIN FUNCTION (Contoh Integrasi)
// ---------------------------------------------------------
int main() {
    // 1. Panggil animasi pembuka tepat saat program dijalankan
    playIntroAnimation();

    // 2. Clear screen untuk masuk ke menu asli
    CLEAR();

    // 3. Menu utama aplikasi Anda
    printf(CYAN "===================================\n" RESET);
    printf(WHITE "   WELCOME TO DUOMART PORTAL\n" RESET);
    printf(CYAN "===================================\n" RESET);
    printf("1. Registrasi (Encrypt Data)\n");
    printf("2. Login (Decrypt Data)\n");
    printf("3. Keluar\n");
    printf("Pilihan Anda: ");
    
    // ... Taruh logika while loop / scanf Anda di sini ...

    return 0;
}