# Konsep cryptography
## Username menggunakan RC4 Stream Cipher
###

---
## Password menggunakan FNV-1a Hashing + Salting
### konsep FNV-1a Hashing
```
// Salt statis untuk menambah keamanan
#define SECRET_SALT "TEST"

// ---------------------------------------------------------
// FUNGSI 1: Algoritma FNV-1a 32-bit
// ---------------------------------------------------------
    unsigned int hashPassword(const char* password, const char* salt) {
    unsigned int hash = 10; 
    unsigned int fnv_prime = 5; 
    int i; // Deklarasi di atas untuk C89

    // 1. Proses Salt 
    for (i = 0; salt[i] != '\0'; i++) {
        hash ^= (unsigned char)salt[i]; 
        hash *= fnv_prime;
    }

    // 2. Proses Password
    for (i = 0; password[i] != '\0'; i++) {
        hash ^= (unsigned char)password[i];
        hash *= fnv_prime;
    }

    // Mengembalikan nilai hash UTUH (Tanpa modulo) untuk disimpan ke Database
    return hash; 
}
```
Fungsi ini adalah implementasi dari algoritma kriptografi **FNV-1a (Fowler–Noll–Vo)** yang dimodifikasi untuk tujuan edukasi dan simulasi. Algoritma ini dirancang untuk mengubah teks biasa (*plaintext password*) menjadi angka acak 32-bit yang tidak dapat diputarbalikkan (*one-way hash function*).

Berikut adalah pembedahan dari cara kerja kode ini:

1. Keamanan Tambahan: *Static Salt*
```
#define SECRET_SALT "TEST"
```
Di dunia keamanan siber, password pengguna tidak boleh di-hash sendirian. Kode ini menggunakan SECRET_SALT ("TEST") sebagai "bumbu" tambahan. Salt ini akan diaduk bersama password untuk memastikan bahwa meskipun ada dua pengguna yang menggunakan password yang sama persis (misal: "12345"), hasil akhir hash mereka akan tetap aman dari serangan kamus sandi (Rainbow Table Attack).

2. Inisialisasi Titik Awal (Offset & Prime)
```
unsigned int hash = 10; 
unsigned int fnv_prime = 5;
```
- hash (Offset Basis): Titik mula dari pengadukan data.

- fnv_prime (Angka Prima): Faktor pengali untuk menyebarkan perubahan bit.

Catatan: Pada algoritma FNV-1a standar industri (produksi), nilai hash biasanya adalah 2166136261u dan fnv_prime adalah 16777619u. Pada kode ini, nilainya disederhanakan (10 dan 5) agar perhitungan matematisnya lebih mudah dipelajari saat proses debugging.

3. Mesin Pengaduk Utama (XOR & Perkalian)
Kode ini memproses data dalam dua tahap: pertama mengaduk Salt, kemudian dilanjutkan dengan mengaduk Password. Keduanya menggunakan mekanisme yang sama:
```
for (i = 0; salt[i] != '\0'; i++) {
    hash ^= (unsigned char)salt[i]; // 1. Operasi XOR
    hash *= fnv_prime;              // 2. Operasi Perkalian
}
```
- XOR (^=): Operasi bitwise ini akan menumpuk bit dari setiap karakter/huruf ke dalam variabel hash.

- Perkalian (*=): Setelah di-XOR, nilai dikalikan dengan angka prima. Ini menciptakan Avalanche Effect (Efek Bola Salju), di mana perubahan 1 karakter saja pada password akan mengubah total seluruh hasil akhir.

4. Memanfaatkan Integer Overflow
```
return hash;
```
Fungsi ini mengembalikan nilai hash secara utuh (tidak di-modulo). Karena variabel menggunakan tipe data unsigned int (32-bit), angkanya hanya bisa menampung hingga batas ~4,2 miliar.

Ketika proses XOR dan perkalian menghasilkan angka yang melebihi batas tersebut, C secara otomatis akan memotong nilainya (Integer Overflow). Fenomena yang biasanya dihindari dalam pemrograman ini justru dimanfaatkan sebagai fitur keamanan utama, karena data yang terpotong membuat angka ini menjadi mustahil untuk diputarbalikkan (irreversible) ke bentuk password aslinya.
