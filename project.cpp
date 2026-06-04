#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstring>
using namespace std;

// ============================================================
// 1. STRUCT - Definisi struktur data
// ============================================================

struct Barang {
    int    id;
    char   nama[50];
    char   kategori[30];
    double harga;
    int    stok;
};

struct Transaksi {
    int    idTransaksi;
    int    idBarang;
    char   namaBarang[50];
    int    jumlah;
    double totalHarga;
    char   tanggal[20];
};

// Array multi dimensi untuk menyimpan riwayat penjualan per kategori per bulan
// [kategori][bulan] -> total penjualan
// kategori: 0=Makanan, 1=Minuman, 2=Elektronik, 3=Pakaian, 4=Lainnya
// bulan   : 0..11
double rekapPenjualan[5][12];
const char* namaKategori[5] = {"Makanan", "Minuman", "Elektronik", "Pakaian", "Lainnya"};

// ============================================================
// Global Data
// ============================================================
const int MAX_BARANG     = 100;
const int MAX_TRANSAKSI  = 500;

Barang    daftarBarang[MAX_BARANG];
Transaksi daftarTransaksi[MAX_TRANSAKSI];
int       jumlahBarang    = 0;
int       jumlahTransaksi = 0;
int       idTransaksiNext = 1;

// ============================================================
// 2. FUNGSI REKURSIF
// ============================================================

// Hitung total harga dengan diskon bertingkat
double hitungDiskonRekursif(double harga, int jumlah, int levelDiskon) {
    if (levelDiskon == 0 || jumlah == 0) {
        return harga * jumlah;
    }
    // Setiap 5 item dapat diskon 5% 
    if (jumlah >= 5) {
        double hargaDiskon = harga * 5 * 0.95; // diskon 5% per 5 item
        return hargaDiskon + hitungDiskonRekursif(harga, jumlah - 5, levelDiskon - 1);
    }
    return harga * jumlah;
}

// Hitung faktorial untuk keperluan kombinasi promo
long long faktorial(int n) {
    if (n <= 1) return 1;
    return n * faktorial(n - 1);
}

// Binary Search 
int binarySearchRekursif(Barang* arr, int low, int high, int targetId) {
    if (low > high) return -1;
    int mid = (low + high) / 2;
    if (arr[mid].id == targetId) return mid;
    if (arr[mid].id < targetId)
        return binarySearchRekursif(arr, mid + 1, high, targetId);
    else
        return binarySearchRekursif(arr, low, mid - 1, targetId);
}

// ============================================================
// 3. SEARCHING
// ============================================================

// Linear Search by nama barang
int linearSearchNama(const char* keyword) {
    for (int i = 0; i < jumlahBarang; i++) {
        // Case-insensitive search
        string namaBrg(daftarBarang[i].nama);
        string key(keyword);
        // konversi lowercase sederhana
        for (char& c : namaBrg) if (c >= 'A' && c <= 'Z') c += 32;
        for (char& c : key)     if (c >= 'A' && c <= 'Z') c += 32;
        if (namaBrg.find(key) != string::npos) return i;
    }
    return -1;
}

// Binary Search by ID
int binarySearchById(int targetId) {
    return binarySearchRekursif(daftarBarang, 0, jumlahBarang - 1, targetId);
}

// ============================================================
// 4. SORTING
// ============================================================

// Bubble Sort - urutkan barang by harga ascending
void bubbleSortByHarga() {
    for (int i = 0; i < jumlahBarang - 1; i++) {
        for (int j = 0; j < jumlahBarang - 1 - i; j++) {
            if (daftarBarang[j].harga > daftarBarang[j+1].harga) {
                Barang temp    = daftarBarang[j];
                daftarBarang[j]   = daftarBarang[j+1];
                daftarBarang[j+1] = temp;
            }
        }
    }
}

// Selection Sort - urutkan barang by nama ascending
void selectionSortByNama() {
    for (int i = 0; i < jumlahBarang - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < jumlahBarang; j++) {
            if (strcmp(daftarBarang[j].nama, daftarBarang[minIdx].nama) < 0) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            Barang temp       = daftarBarang[i];
            daftarBarang[i]   = daftarBarang[minIdx];
            daftarBarang[minIdx] = temp;
        }
    }
}

// Sort by ID ascending 
void sortById() {
    for (int i = 0; i < jumlahBarang - 1; i++) {
        for (int j = 0; j < jumlahBarang - 1 - i; j++) {
            if (daftarBarang[j].id > daftarBarang[j+1].id) {
                Barang temp    = daftarBarang[j];
                daftarBarang[j]   = daftarBarang[j+1];
                daftarBarang[j+1] = temp;
            }
        }
    }
}

// ============================================================
// 5. POINTER - Fungsi menggunakan pointer
// ============================================================

// Update stok menggunakan pointer
void updateStok(Barang* barang, int jumlahBeli) {
    if (barang->stok >= jumlahBeli) {
        barang->stok -= jumlahBeli;
    } else {
        cout << "[ERROR] Stok tidak mencukupi!\n";
    }
}

// Tambah stok menggunakan pointer
void tambahStok(Barang* barang, int jumlahTambah) {
    barang->stok += jumlahTambah;
}

// Cari barang dengan pointer, return pointer ke barang atau NULL
Barang* cariBarangById(int id) {
    for (int i = 0; i < jumlahBarang; i++) {
        if (daftarBarang[i].id == id) {
            return &daftarBarang[i]; // return pointer ke elemen array
        }
    }
    return NULL;
}

// Update harga via pointer
void updateHarga(Barang* barang, double hargaBaru) {
    barang->harga = hargaBaru;
}

// ============================================================
// 6. FILE - Simpan & Baca Data
// ============================================================

void simpanBarangKeFile() {
    ofstream file("barang.txt");
    if (!file.is_open()) {
        cout << "[ERROR] Gagal membuka file barang.txt\n";
        return;
    }
    file << jumlahBarang << "\n";
    for (int i = 0; i < jumlahBarang; i++) {
        file << daftarBarang[i].id      << "|"
             << daftarBarang[i].nama    << "|"
             << daftarBarang[i].kategori << "|"
             << daftarBarang[i].harga   << "|"
             << daftarBarang[i].stok    << "\n";
    }
    file.close();
    cout << "[OK] Data barang berhasil disimpan ke barang.txt\n";
}

void bacaBarangDariFile() {
    ifstream file("barang.txt");
    if (!file.is_open()) {
        cout << "[INFO] File barang.txt tidak ditemukan, mulai data kosong.\n";
        return;
    }
    file >> jumlahBarang;
    file.ignore();
    for (int i = 0; i < jumlahBarang; i++) {
        string line;
        getline(file, line);
        // Parse delimiter '|'
        int pos = 0;
        auto nextField = [&]() {
            int end = line.find('|', pos);
            string field = (end == (int)string::npos)
                           ? line.substr(pos)
                           : line.substr(pos, end - pos);
            pos = (end == (int)string::npos) ? line.size() : end + 1;
            return field;
        };
        daftarBarang[i].id    = stoi(nextField());
        strncpy(daftarBarang[i].nama,     nextField().c_str(), 49);
        strncpy(daftarBarang[i].kategori, nextField().c_str(), 29);
        daftarBarang[i].harga = stod(nextField());
        daftarBarang[i].stok  = stoi(nextField());
    }
    file.close();
}

void simpanTransaksiKeFile() {
    ofstream file("transaksi.txt", ios::app); 
    if (!file.is_open()) {
        cout << "[ERROR] Gagal membuka file transaksi.txt\n";
        return;
    }

    if (jumlahTransaksi > 0) {
        int i = jumlahTransaksi - 1;
        file << daftarTransaksi[i].idTransaksi << "|"
             << daftarTransaksi[i].idBarang    << "|"
             << daftarTransaksi[i].namaBarang  << "|"
             << daftarTransaksi[i].jumlah      << "|"
             << daftarTransaksi[i].totalHarga  << "|"
             << daftarTransaksi[i].tanggal     << "\n";
    }
    file.close();
}

void bacaTransaksiDariFile() {
    ifstream file("transaksi.txt");
    if (!file.is_open()) {
        cout << "[INFO] File transaksi.txt tidak ditemukan.\n";
        return;
    }
    jumlahTransaksi = 0;
    string line;
    while (getline(file, line) && jumlahTransaksi < MAX_TRANSAKSI) {
        int pos = 0;
        auto nextField = [&]() {
            int end = line.find('|', pos);
            string field = (end == (int)string::npos)
                           ? line.substr(pos)
                           : line.substr(pos, end - pos);
            pos = (end == (int)string::npos) ? line.size() : end + 1;
            return field;
        };
        int i = jumlahTransaksi;
        daftarTransaksi[i].idTransaksi = stoi(nextField());
        daftarTransaksi[i].idBarang    = stoi(nextField());
        strncpy(daftarTransaksi[i].namaBarang, nextField().c_str(), 49);
        daftarTransaksi[i].jumlah      = stoi(nextField());
        daftarTransaksi[i].totalHarga  = stod(nextField());
        strncpy(daftarTransaksi[i].tanggal,    nextField().c_str(), 19);
        jumlahTransaksi++;
    }
    if (jumlahTransaksi > 0) {
        idTransaksiNext = daftarTransaksi[jumlahTransaksi-1].idTransaksi + 1;
    }
    file.close();
}

void simpanRekapKeFile() {
    ofstream file("rekap_penjualan.txt");
    if (!file.is_open()) { cout << "[ERROR] Gagal simpan rekap.\n"; return; }
    file << "=== REKAP PENJUALAN PER KATEGORI PER BULAN ===\n";
    file << setw(15) << "Kategori";
    for (int b = 1; b <= 12; b++) file << setw(10) << "Bln-" << b;
    file << "\n";
    for (int k = 0; k < 5; k++) {
        file << setw(15) << namaKategori[k];
        for (int b = 0; b < 12; b++) {
            file << setw(12) << fixed << setprecision(0) << rekapPenjualan[k][b];
        }
        file << "\n";
    }
    file.close();
    cout << "[OK] Rekap penjualan disimpan ke rekap_penjualan.txt\n";
}

// ============================================================
// 7. KICK START - Inisialisasi data awal
// ============================================================
void kickStart() {

    // Inisialisasi array multi dimensi rekap penjualan
    for (int k = 0; k < 5; k++)
        for (int b = 0; b < 12; b++)
            rekapPenjualan[k][b] = 0.0;

    // Coba baca dari file terlebih dahulu
    bacaBarangDariFile();
    bacaTransaksiDariFile();

    // Jika tidak ada data barang, isi data sample
    if (jumlahBarang == 0) {
        // Data sample menggunakan pointer untuk pengisian
        Barang* p = daftarBarang;

        p->id = 1; strcpy(p->nama, "Beras Premium 5kg");
        strcpy(p->kategori, "Makanan");   p->harga = 75000;  p->stok = 50; p++;
        p->id = 2; strcpy(p->nama, "Mie Goreng Instan");
        strcpy(p->kategori, "Makanan");   p->harga = 3500;   p->stok = 200; p++;
        p->id = 3; strcpy(p->nama, "Air Mineral 600ml");
        strcpy(p->kategori, "Minuman");   p->harga = 4000;   p->stok = 150; p++;
        p->id = 4; strcpy(p->nama, "Kopi Susu Kaleng");
        strcpy(p->kategori, "Minuman");   p->harga = 8500;   p->stok = 80; p++;
        p->id = 5; strcpy(p->nama, "Lampu LED 10W");
        strcpy(p->kategori, "Elektronik"); p->harga = 35000; p->stok = 30; p++;
        p->id = 6; strcpy(p->nama, "Sabun Mandi");
        strcpy(p->kategori, "Lainnya");   p->harga = 6000;   p->stok = 100; p++;
        p->id = 7; strcpy(p->nama, "Kaos Polos");
        strcpy(p->kategori, "Pakaian");   p->harga = 50000;  p->stok = 40; p++;
        p->id = 8; strcpy(p->nama, "Snack Chitato");
        strcpy(p->kategori, "Makanan");   p->harga = 12000;  p->stok = 60; p++;

        jumlahBarang = 8;
        simpanBarangKeFile();
    }

}

// ============================================================
// TAMPILAN & MENU
// ============================================================

void tampilGaris(char c = '=', int len = 60) {
    for (int i = 0; i < len; i++) cout << c;
    cout << "\n";
}

void tampilHeader() {
    tampilGaris();
    cout << "          SISTEM MANAJEMEN TOKO.\n";
    tampilGaris();
}

void tampilDaftarBarang() {
    tampilGaris('-');
    cout << left
         << setw(5)  << "ID"
         << setw(25) << "Nama Barang"
         << setw(12) << "Kategori"
         << setw(12) << "Harga"
         << setw(8)  << "Stok"
         << "\n";
    tampilGaris('-');
    for (int i = 0; i < jumlahBarang; i++) {
        cout << left
             << setw(5)  << daftarBarang[i].id
             << setw(25) << daftarBarang[i].nama
             << setw(12) << daftarBarang[i].kategori
             << "Rp " << setw(10) << fixed << setprecision(0) << daftarBarang[i].harga
             << setw(8)  << daftarBarang[i].stok
             << "\n";
    }
    tampilGaris('-');
}

void tampilTransaksi() {
    if (jumlahTransaksi == 0) {
        cout << "[INFO] Belum ada transaksi.\n";
        return;
    }
    tampilGaris('-');
    cout << left
         << setw(5)  << "No"
         << setw(8)  << "ID Trx"
         << setw(22) << "Nama Barang"
         << setw(8)  << "Jml"
         << setw(15) << "Total"
         << setw(12) << "Tanggal"
         << "\n";
    tampilGaris('-');
    for (int i = 0; i < jumlahTransaksi; i++) {
        cout << left
             << setw(5)  << (i+1)
             << setw(8)  << daftarTransaksi[i].idTransaksi
             << setw(22) << daftarTransaksi[i].namaBarang
             << setw(8)  << daftarTransaksi[i].jumlah
             << "Rp " << setw(12) << fixed << setprecision(0) << daftarTransaksi[i].totalHarga
             << setw(12) << daftarTransaksi[i].tanggal
             << "\n";
    }
    tampilGaris('-');
}

// ============================================================
// FITUR UTAMA
// ============================================================

void tambahBarang() {
    if (jumlahBarang >= MAX_BARANG) {
        cout << "[ERROR] Data barang penuh!\n"; return;
    }
    Barang* p = &daftarBarang[jumlahBarang]; 
    p->id = (jumlahBarang > 0) ? daftarBarang[jumlahBarang-1].id + 1 : 1;

    cout << "\n=== TAMBAH BARANG BARU ===\n";
    cout << "ID otomatis: " << p->id << "\n";
    cout << "Nama Barang : "; cin.ignore(); cin.getline(p->nama, 50);
    cout << "Kategori\n";
    cout << "  1. Makanan  2. Minuman  3. Elektronik  4. Pakaian  5. Lainnya\n";
    cout << "Pilih (1-5): ";
    int kat; cin >> kat;
    if (kat < 1 || kat > 5) kat = 5;
    strncpy(p->kategori, namaKategori[kat-1], 29);
    cout << "Harga       : Rp "; cin >> p->harga;
    cout << "Stok Awal   : ";     cin >> p->stok;

    jumlahBarang++;
    simpanBarangKeFile();
    cout << "[OK] Barang berhasil ditambahkan!\n";
}

void editBarang() {
    cout << "\n=== EDIT BARANG ===\n";
    cout << "Masukkan ID barang yang diedit: ";
    int id; cin >> id;

    Barang* p = cariBarangById(id); 
    if (p == NULL) {
        cout << "[ERROR] Barang dengan ID " << id << " tidak ditemukan.\n";
        return;
    }
    cout << "Barang ditemukan: " << p->nama << "\n";
    cout << "Harga baru (sekarang Rp " << p->harga << "): Rp ";
    double hBaru; cin >> hBaru;
    updateHarga(p, hBaru); 

    cout << "Tambah stok? (sekarang " << p->stok << "), tambah: ";
    int tambah; cin >> tambah;
    if (tambah > 0) tambahStok(p, tambah); 

    simpanBarangKeFile();
    cout << "[OK] Data barang berhasil diupdate!\n";
}

void hapusBarang() {
    cout << "\n=== HAPUS BARANG ===\n";
    cout << "Masukkan ID barang: ";
    int id; cin >> id;
    int idx = -1;
    for (int i = 0; i < jumlahBarang; i++)
        if (daftarBarang[i].id == id) { idx = i; break; }
    if (idx == -1) { cout << "[ERROR] Barang tidak ditemukan.\n"; return; }
    cout << "Hapus '" << daftarBarang[idx].nama << "'? (y/n): ";
    char conf; cin >> conf;
    if (conf != 'y' && conf != 'Y') { cout << "[BATAL]\n"; return; }
    for (int i = idx; i < jumlahBarang - 1; i++)
        daftarBarang[i] = daftarBarang[i+1];
    jumlahBarang--;
    simpanBarangKeFile();
    cout << "[OK] Barang berhasil dihapus.\n";
}

void transaksiPembelian() {
    cout << "\n=== TRANSAKSI PENJUALAN ===\n";
    tampilDaftarBarang();
    cout << "Masukkan ID barang: ";
    int id; cin >> id;

    Barang* p = cariBarangById(id); 
    if (p == NULL) { cout << "[ERROR] Barang tidak ditemukan.\n"; return; }
    if (p->stok == 0) { cout << "[ERROR] Stok habis!\n"; return; }

    cout << "Barang : " << p->nama << " | Harga: Rp " << p->harga << " | Stok: " << p->stok << "\n";
    cout << "Jumlah beli: ";
    int jml; cin >> jml;
    if (jml <= 0 || jml > p->stok) {
        cout << "[ERROR] Jumlah tidak valid atau melebihi stok.\n"; return;
    }

    // Hitung total dengan diskon 
    double total = hitungDiskonRekursif(p->harga, jml, jml / 5 + 1);
    double normal = p->harga * jml;
    double diskon = normal - total;

    cout << "\n--- Rincian ---\n";
    cout << "Harga normal : Rp " << fixed << setprecision(0) << normal << "\n";
    if (diskon > 0)
        cout << "Diskon       : Rp " << diskon << " (tiap 5 item diskon 5%)\n";
    cout << "Total bayar  : Rp " << total << "\n";
    cout << "Konfirmasi? (y/n): ";
    char conf; cin >> conf;
    if (conf != 'y' && conf != 'Y') { cout << "[BATAL]\n"; return; }

    updateStok(p, jml); 

    // Simpan transaksi
    Transaksi* t = &daftarTransaksi[jumlahTransaksi];
    t->idTransaksi = idTransaksiNext++;
    t->idBarang    = id;
    strncpy(t->namaBarang, p->nama, 49);
    t->jumlah      = jml;
    t->totalHarga  = total;
    strncpy(t->tanggal, "2025-01-01", 19); 

    // Update rekap 
    int katIdx = 4; // default Lainnya
    for (int k = 0; k < 5; k++)
        if (strcmp(p->kategori, namaKategori[k]) == 0) { katIdx = k; break; }
    rekapPenjualan[katIdx][0] += total; // bulan ke-1 (index 0)

    jumlahTransaksi++;
    simpanBarangKeFile();
    simpanTransaksiKeFile();
    cout << "[OK] Transaksi #" << t->idTransaksi << " berhasil!\n";
}

void menuSearching() {
    cout << "\n=== PENCARIAN BARANG ===\n";
    cout << "1. Linear Search by Nama\n";
    cout << "2. Binary Search by ID\n";
    cout << "Pilih: ";
    int pilih; cin >> pilih;

    if (pilih == 1) {
        cout << "Masukkan kata kunci nama: ";
        char keyword[50]; cin.ignore(); cin.getline(keyword, 50);
        int idx = linearSearchNama(keyword);
        if (idx == -1) cout << "[INFO] Barang tidak ditemukan.\n";
        else {
            cout << "[DITEMUKAN] ";
            cout << "ID:" << daftarBarang[idx].id
                 << " | " << daftarBarang[idx].nama
                 << " | Rp " << daftarBarang[idx].harga
                 << " | Stok: " << daftarBarang[idx].stok << "\n";
        }
    } else if (pilih == 2) {
        cout << "Masukkan ID barang: ";
        int id; cin >> id;
        sortById(); 
        int idx = binarySearchById(id);
        if (idx == -1) cout << "[INFO] Barang ID " << id << " tidak ditemukan.\n";
        else {
            cout << "[DITEMUKAN via Binary Search] ";
            cout << daftarBarang[idx].nama
                 << " | Rp " << daftarBarang[idx].harga
                 << " | Stok: " << daftarBarang[idx].stok << "\n";
        }
    }
}

void menuSorting() {
    cout << "\n=== PENGURUTAN BARANG ===\n";
    cout << "1. Urutkan by Harga\n";
    cout << "2. Urutkan by Nama\n";
    cout << "3. Urutkan by ID\n";
    cout << "Pilih: ";
    int pilih; cin >> pilih;
    if (pilih == 1) {
        bubbleSortByHarga();
        cout << "[OK] Diurutkan by Harga (murah ke mahal):\n";
    } else if (pilih == 2) {
        selectionSortByNama();
        cout << "[OK] Diurutkan by Nama (A-Z):\n";
    } else {
        sortById();
        cout << "[OK] Diurutkan by ID:\n";
    }
    tampilDaftarBarang();
}

void tampilRekapPenjualan() {
    cout << "\n=== REKAP PENJUALAN ===\n";
    tampilGaris('-');
    cout << left << setw(14) << "Kategori";
    for (int b = 1; b <= 12; b++) cout << right << setw(9) << "Bln-" + to_string(b);
    cout << "\n";
    tampilGaris('-');
    for (int k = 0; k < 5; k++) {
        cout << left << setw(14) << namaKategori[k];
        for (int b = 0; b < 12; b++) {
            cout << right << setw(9) << fixed << setprecision(0) << rekapPenjualan[k][b];
        }
        cout << "\n";
    }
    tampilGaris('-');
    simpanRekapKeFile();
}

void tampilInfoRekursif() {
    double harga;
    int jumlah;

    cout << "\n=== DEMO DISKON REKURSIF ===\n";

    cout << "Masukkan Harga Barang : Rp ";
    cin >> harga;

    cout << "Masukkan Jumlah Beli  : ";
    cin >> jumlah;

    double normal = harga * jumlah;
    double total = hitungDiskonRekursif(harga, jumlah, jumlah / 5 + 1);
    double diskon = normal - total;

    cout << fixed << setprecision(0);

	cout << "\n===== HASIL PERHITUNGAN =====\n";
	cout << "Harga Normal : Rp " << normal << "\n";
	cout << "Diskon       : Rp " << diskon << "\n";
	cout << "Total Bayar  : Rp " << total << "\n";
}

// ============================================================
// MAIN PROGRAM
// ============================================================
int main() {
    // KICK START - inisialisasi program
    kickStart();

    int pilihan;
    do {
        tampilHeader();
        cout << "  1. Lihat Daftar Barang\n";
        cout << "  2. Tambah Barang\n";
        cout << "  3. Edit Barang (Harga/Stok)\n";
        cout << "  4. Hapus Barang\n";
        cout << "  5. Transaksi Penjualan\n";
        cout << "  6. Lihat Riwayat Transaksi\n";
        cout << "  7. Pencarian Barang\n";
        cout << "  8. Pengurutan Barang\n";
        cout << "  9. Rekap Penjualan\n";
        cout << " 10. Demo Diskon\n";
        cout << "  0. Keluar\n";
        tampilGaris('-');
        cout << "Pilihan: ";
        cin >> pilihan;

        switch (pilihan) {
            case 1: tampilDaftarBarang();    break;
            case 2: tambahBarang();           break;
            case 3: editBarang();             break;
            case 4: hapusBarang();            break;
            case 5: transaksiPembelian();     break;
            case 6: tampilTransaksi();        break;
            case 7: menuSearching();          break;
            case 8: menuSorting();            break;
            case 9: tampilRekapPenjualan();   break;
            case 10: tampilInfoRekursif(); break; // Demo Diskon Rekursif
            case 0: cout << "\n[BYE] Program selesai. Data tersimpan.\n"; break;
            default: cout << "[ERROR] Pilihan tidak valid.\n";
        }
        if (pilihan != 0) {
            cout << "\nTekan Enter untuk lanjut...";
            cin.ignore(); cin.get();
        }
    } while (pilihan != 0);

    return 0;
} 
