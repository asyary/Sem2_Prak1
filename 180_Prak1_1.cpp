#include <iostream>
#include <fstream>
#include <conio.h>
#include <algorithm>
#include <cmath>
using namespace std;

// GitHub repo (otw) https://github.com/asyary/Sem2_Prak1

// I can't help the urge to point out that siapapun developer monster
// yang beneran bikin aplikasi dengan ID serumit dan se-tidak-efisien
// ini ada baiknya memperbaiki diri T-T Like, just use NIK aja or something, why-

// Anyway, where was I? Oh, structure
struct Date { // kurma
	int day {0};
	int month {0};
	int year {0};
	int validate() { // males kalo validasinya di bawah
		if ((year < 1900 || year > 2024) || (month < 1 || month > 12) || (day < 1 || day > jumlahHari(month))) {
			return false;
		}
		return true;
	}
	
	private: // some fancy stuff
		bool tahunKabisat() {
			return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
		}
		int jumlahHari(int bulan) {
			int hariHariNT[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
			int hariPerBulan = hariHariNT[bulan-1];
			if (month == 2 && tahunKabisat()) {
				hariPerBulan++;
			}
			return hariPerBulan;
		}
};

struct Supir {
	string id; // can't really be integers, bermasalah kalo awalannya 0
	string nama;
	Date tglLahir;
	char kelamin;
	string alamat;
};

struct Node {
	Supir data;
	Node* next {NULL}; // null default member init
}; // note: this would be single circular
Node* head = NULL;
Node* tail = NULL;
int totalSupir;

enum MenuType {
	MAIN_MENU,
	ADMIN_MENU,
	USER_MENU
};

// Prototype
void menu(MenuType pilMenu);
bool isEmpty();

char optionHandler() {
	char pil;
	pil = _getch();
	if (pil == 3) {
		exit(0);
	}
	return pil;
};

void initDB() {
	ifstream baca("./data/userdata.txt");
	if (baca.fail()) {
		return exit(0); // Waduh
	}
	baca >> totalSupir; // kalo total = 0 head nya udah null
	baca.ignore();
	for (int i = 0; i < totalSupir; i++) {
		Node* user = new Node;
		// I really, really hate just stacking all the data in 1 file,
		// but I guess I have no other choice, terlalu ga efisien otherwise
		getline(baca, user->data.nama);
		baca >> user->data.id >> user->data.tglLahir.day >> user->data.tglLahir.month >>
		user->data.tglLahir.year >> user->data.kelamin;
		baca.ignore(); // remove whitespace (prolly newline) dari previous input
		getline(baca, user->data.alamat);
		if (i == 0) {
			head = user;
			tail = user;
			head->next = head;
		} else {
			tail->next = user;
			tail = user;
			user->next = head;
		} // some confusing stuff, whatever
	}
	baca.close();
}

void updateDB() {
	ofstream tulis("./data/userdata.txt", ios::trunc); // trunc hurts me in the meaw-meaw
	tulis << totalSupir << "\n";
	Node* helperNode = head;
	if (isEmpty()) {
		return;
	}
	do {
		tulis << helperNode->data.nama << "\n" << helperNode->data.id << "\n" <<
		helperNode->data.tglLahir.day << " " << helperNode->data.tglLahir.month << " " << helperNode->data.tglLahir.year <<
		"\n" << helperNode->data.kelamin << "\n" << helperNode->data.alamat << "\n"; // wew
		helperNode = helperNode->next;
	} while (helperNode != head);
}

bool isEmpty() {
	if (head == NULL) {
		return true;
	} else {
		return false;
	}
}

Date dateSplitter(string s, char delimiter) {
	Date newDate;
	string token;
	for (char c : s) {
		if (c != delimiter) {
			token += c;
		}
		else {
			if (newDate.day == 0) {
				newDate.day = stoi(token);
			} else {
				newDate.month = stoi(token);
			}
			token.clear();
		}
	}
	newDate.year = stoi(token);
	return newDate;
}

string idGen(Node* supir) {
	// Gen first 4 digits, yang terakhir for last
	string nama = supir->data.nama;
	string gen;
	for (char &c : nama) {
		c = tolower(c);
	}
	int awal = nama.front() - 'a' + 1;
	int akhir = nama.back() - 'a' + 1;
	int hasil = abs(awal-akhir);
	gen += (to_string(hasil).length() == 1 ? "0" + to_string(hasil) : to_string(hasil));
	if (supir->data.kelamin == 'L' || supir->data.kelamin == 'l') {
		gen += "1";
	} else {
		gen += "0";
	}
	int entahApalah = (supir->data.tglLahir.day % 10) + (supir->data.tglLahir.month % 10) + (supir->data.tglLahir.year % 10);
	entahApalah = entahApalah % 9;
	gen += to_string(entahApalah);
	// There's an inefficient way, O(n^2) based on N(N+1)/2 to just iterate each id (check if exists)
	// Also, ga dijelasin boleh/ngga ID yang pernah ada, dihapus, digunakan kembali (asumsi no)
	// Here goes nothing
	Node* iterateNode = head; // I just realized ini single bukan double, can't go backwards
	Node* latestOcc = NULL;
	do {
		string checkID = iterateNode->data.id;
		checkID = checkID.substr(0, 4);
		if (checkID == gen) {
			latestOcc = iterateNode;
		}
		iterateNode = iterateNode->next;
	} while (iterateNode != head);
	if (latestOcc == NULL) {
		gen += "0";
	} else {
		string latestOccID = latestOcc->data.id;
		latestOccID = latestOccID.erase(0, 4);
		int latestID = stoi(latestOccID);
		latestID += 1;
		gen += to_string(latestID);
	}
	return gen;
}

void printSupir(Node* nodeSupir) {
	// I didn't realize how complicated I'm making this
	string base = "|============"; // add - by the max length
	string nama = nodeSupir->data.nama;
	string id = nodeSupir->data.id;
	string hari = to_string(nodeSupir->data.tglLahir.day);
	string bulan = to_string(nodeSupir->data.tglLahir.month);
	string tahun = to_string(nodeSupir->data.tglLahir.year);
	if (hari.length() == 1) {
		hari.insert(0,1,'0'); // add padding 0
	}
	if (bulan.length() == 1) {
		bulan.insert(0,1,'0');
	}
	string tglLahir = hari + "/" + bulan + "/" + tahun;
	string gender = (nodeSupir->data.kelamin == 'L') ? "Laki-Laki" : "Perempuan";
	string alamat = nodeSupir->data.alamat;
	int biggest = max({nama.length(), id.length(), tglLahir.length(), gender.length(), alamat.length()});
	// Generate box and format based on string terpanjang
	base.insert(base.end(),biggest,'=');
	nama.insert(nama.end(),biggest-nama.length(),' ');
	nama.insert(0, "|Nama      : ");
	id.insert(id.end(),biggest-id.length(),' ');
	id.insert(0, "|ID        : ");
	tglLahir.insert(tglLahir.end(),biggest-tglLahir.length(),' ');
	tglLahir.insert(0, "|Tgl Lahir : ");
	gender.insert(gender.end(),biggest-gender.length(),' ');
	gender.insert(0, "|Kelamin   : ");
	alamat.insert(alamat.end(),biggest-alamat.length(),' ');
	alamat.insert(0, "|Alamat    : "); // I hate formatting
	cout << base + "|\n" + nama + "|\n" + id + "|\n" + tglLahir + "|\n" + gender + "|\n" + alamat + "|\n" + base + "|";
}

Node* cariDataSupirHandler(string idCari) {
	Node* nodeCari = head;
	do {
		if (nodeCari->data.id == idCari) {
			return nodeCari;
		} else {
			nodeCari = nodeCari->next;
		}
	} while (nodeCari != head);
	return NULL;
}

void hapusDataSupirHandler(Node* nodeHapus) {
	Node* nodeCari = head;
	if (head == tail && head == nodeHapus) {
		head = NULL;
		delete nodeHapus;
		totalSupir = totalSupir - 1;
		return;
	}
	do {
		if (nodeCari->next == nodeHapus) {
			if (nodeHapus == head) {
				head = head->next;
			}
			if (nodeHapus == tail) {
				tail = nodeCari;
			}
			nodeCari->next = nodeHapus->next;
			delete nodeHapus;
			totalSupir = totalSupir - 1;
			return;
		} else {
			nodeCari = nodeCari->next;
		}
	} while (nodeCari != head);
}

void cariDataSupir() { // wrong, ga diminta begini
	system("cls");
	if (isEmpty()) {
		cout << "ERROR: Data supir kosong!\n";
		system("pause");
		return menu(ADMIN_MENU);
	}
	cout << "==== Mencari Data Supir ====\n\nMasukkan ID Supir\n> ";
	string idCari;
	cin >> idCari;
	system("cls");
	Node* cariSupir = cariDataSupirHandler(idCari);
	if (cariSupir == NULL) {
		cout << "Supir tidak ditemukan!\n";
		system("pause");
		return menu(ADMIN_MENU);
	} else {
		printSupir(cariSupir);
		cout << "\n\n";
		system("pause");
		return menu(ADMIN_MENU);
	}
}

void ingpoDataSupir() {
	Node* helperNode = head;
	while (true) {
		system("cls");
		printSupir(helperNode);
		cout << "\n\n1. Next\n2. Previous\n0. Exit\n> ";
		char pil = '\0';
		pil = optionHandler();
		switch (pil) {
			case '1':
				helperNode = helperNode->next;
				break;
			case '2':

				break;
			case '0':
				return menu(ADMIN_MENU);
			case '\0':
				return;
			
			default:
				cout << "Pilihan invalid!\n";
				system("pause");
				break;
		}
	}
}

void hapusSupir() {
	system("cls");
	if (isEmpty()) {
		cout << "ERROR: Data supir kosong!\n";
		system("pause");
		return menu(ADMIN_MENU);
	}
	cout << "==== Menghapus Data Supir ====\n\nMasukkan ID Supir\n> ";
	string idCari;
	cin >> idCari;
	system("cls");
	Node* cariSupir = cariDataSupirHandler(idCari);
	if (cariSupir == NULL) {
		cout << "Supir tidak ditemukan!\n";
		system("pause");
		return menu(ADMIN_MENU);
	} else {
		cout << "\nSupir dengan data berikut akan dihapus:\n";
		printSupir(cariSupir);
		cout << "\n\nLanjutkan? (y/n)\n> ";
		char chC = 0;
		while (!(chC == 'y' || chC == 'Y' || chC == 'n' || chC == 'N')) {
			chC = optionHandler();
		}
		if (chC == 'y' || chC == 'Y') {
			system("cls");
			hapusDataSupirHandler(cariSupir);
			updateDB();
			cout << "Supir dengan ID " + idCari + " berhasil dihapus!\n\n";
			system("pause");
		}
		return menu(ADMIN_MENU);
	}
}

void tambahSupir() {
	system("cls");
	Node* newSupir = new Node;
	string nama, tanggalLahir, alamat;
	char gender;
	cout << "==== Menambah Data Supir ====\n\nNama                   : ";
	getline(cin, nama);
	cout << "Tgl Lahir (dd/mm/yyyy) : ";
	cin >> tanggalLahir;
	Date lahiran = dateSplitter(tanggalLahir, '/');
	if (!lahiran.validate()) {
		cout << "Tanggal lahir tidak valdi!\n";
		system("pause");
		return menu(ADMIN_MENU);
	}
	cout << "Kelamin (L/P)          : ";
	cin >> gender;
	if (gender != 'L' && gender != 'l' && gender != 'P' && gender != 'p') {
		cout << "Gender tidak valdi!\n";
		system("pause");
		return menu(ADMIN_MENU);
	}
	cout << "Alamat                 : ";
	cin.ignore();
	getline(cin, alamat);
	newSupir->data.nama = nama;
	newSupir->data.tglLahir.day = lahiran.day;
	newSupir->data.tglLahir.month = lahiran.month;
	newSupir->data.tglLahir.year = lahiran.year;
	newSupir->data.kelamin = gender;
	newSupir->data.alamat = alamat;
	string id = idGen(newSupir);
	newSupir->data.id = id;
	tail->next = newSupir;
	tail = newSupir;
	newSupir->next = head;
	totalSupir++;
	updateDB();
	cout << "\nData berhasil ditambahkan!\n";
	system("pause");
	return menu(ADMIN_MENU);
}

void menu(MenuType pilMenu) {
	system("cls");
	switch (pilMenu) {
		case MAIN_MENU: {
			cout << "====================\n  SELAMAT DATANG !\n====================\n";
			cout << "1. Masuk Sebagai Admin\n2. Masuk Sebagai User\n> ";
			char pil = '\0';
			pil = optionHandler();
			switch (pil) {
				case '1':
					menu(ADMIN_MENU);
					break;
				case '2':
					menu(USER_MENU);
					break;
				case '\0':
					return;
				
				default:
					cout << "Pilihan invalid!\n";
					system("pause");
					menu(MAIN_MENU);
					break;
			}}
			break;

		case ADMIN_MENU: {
			cout << "==== Dashboard Admin ====\n\n1. Mencari Data Supir\n" <<
			"2. Menghapus Data Supir\n3. Mengubah Data Supir\n4. Menambah Data Supir\n0. Exit\n> ";
			char pil = '\0';
			pil = optionHandler();
			switch (pil) {
				case '1':
					// cariDataSupir(); wrong
					break;
				case '2':
					hapusSupir();
					break;
				case '4':
					tambahSupir();
					break;
				case '\0':
					return;
				
				default:
					cout << "Pilihan invalid!\n";
					system("pause");
					menu(ADMIN_MENU);
					break;
			}}
			break;

		case USER_MENU: {
			cout << "Ingpo-ingpo-ingpokan cuy (gatau mau diisi apa)";
			}
	}
}

void quit() {
	system("cls");
	cout << "Done ngabsku";
}

void init() {
	initDB();
	// printSupir(head);
	menu(MAIN_MENU);
	// cout << ((head->next->next->next) == head);
}

int main() {
	atexit(quit);
	init();

	return 0;
}