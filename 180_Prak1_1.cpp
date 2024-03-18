#include <iostream>
#include <fstream>
#include <conio.h>
#include <algorithm>
#include <cmath>
#include <regex>
using namespace std;

// GitHub repo https://github.com/asyary/Sem2_Prak1

// I can't help the urge to point out that siapapun developer monster
// yang beneran bikin aplikasi dengan ID serumit dan se-tidak-efisien
// ini ada baiknya memperbaiki diri T-T Like, just use NIK aja or something, why-

// Anyway, where was I? Oh, structure
struct Date { // kurma
	int day {0};
	int month {0};
	int year {0};
	bool validate() { // males kalo validasinya di bawah
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

struct Order {
	string id; // you know the gist
	string nama;
	string supir; // why don't u just use supir struct? prolly unnecessary
	string platNomor;
	string tujuan;
};

struct Mobil {
	string platNomor;
	string jenisMobil;
	string brandMobil;
};

struct Node {
	Supir data;
	Node* next {NULL}; // null default member init
	Node* prev {NULL};
}; // change to double circular
Node* head = NULL;
Node* tail = NULL;
int totalSupir;

// add node for order
struct NodeOrder {
	Order data;
	NodeOrder* next {NULL}; // null default member init
};
NodeOrder* orderHead = NULL;
NodeOrder* orderTail = NULL;
int totalOrder, ordAcc, ordRej, orderKeN;

struct NodeMobil {
	Mobil data;
	NodeMobil* next {NULL}; // yada yada
	NodeMobil* prev {NULL};
}; // let's do double linear, why not
NodeMobil* mobilHead = NULL;
NodeMobil* mobilTail = NULL;
int totalMobil;

enum MenuType {
	MAIN_MENU,
	ADMIN_MENU,
	USER_MENU
};

// Prototype
void menu(MenuType pilMenu);
bool isEmpty();
bool isOrderEmpty();
bool isMobilEmpty();

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
			head->prev = head;
		} else {
			tail->next = user;
			user->prev = tail;
			tail = user;
			user->next = head;
			head->prev = user;
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
	tulis.close();
}

void initOrderDB() {
	// lemme just, uh, ctrl+c ctrl+v real quick
	ifstream baca("./data/order.txt");
	if (baca.fail()) {
		return exit(0); // Waduh
	}
	baca >> totalOrder; // kalo total = 0 head nya udah null
	baca >> orderKeN;
	for (int i = 0; i < totalOrder; i++) {
		// Urutan: id -> nama -> supir (nama) -> plat -> tujuan
		NodeOrder* order = new NodeOrder;
		baca >> order->data.id;
		baca.ignore();
		getline(baca, order->data.nama);
		getline(baca, order->data.supir);
		getline(baca, order->data.platNomor);
		getline(baca, order->data.tujuan);
		if (i == 0) {
			orderHead = order;
			orderTail = order;
		} else {
			orderTail->next = order;
			orderTail = order;
		}
	}
	baca.close();
}

void updateOrderDB() {
	ofstream tulis("./data/order.txt", ios::trunc);
	tulis << totalOrder << "\n" << orderKeN << "\n";
	NodeOrder* helperNode = orderHead;
	if (isOrderEmpty()) {
		return;
	}
	do {
		tulis << helperNode->data.id << "\n" << helperNode->data.nama << "\n" <<
		helperNode->data.supir << "\n" << helperNode->data.platNomor << "\n" <<
		helperNode->data.tujuan << "\n";
		helperNode = helperNode->next;
	} while (helperNode != NULL);
	tulis.close();
}

void initMobilDB() {
	// literal ctrl+c ctrl+v all over again
	ifstream baca("./data/mobil.txt");
	if (baca.fail()) {
		return exit(0); // Waduh
	}
	baca >> totalMobil; // kalo total = 0 head nya udah null
	baca.ignore(); // took me way too long to figure out this bug, I'm getting rusty
	for (int i = 0; i < totalMobil; i++) {
		// Urutan: plat -> jenis -> brand
		NodeMobil* iniMobil = new NodeMobil;
		getline(baca, iniMobil->data.platNomor);
		getline(baca, iniMobil->data.jenisMobil);
		getline(baca, iniMobil->data.brandMobil);
		if (i == 0) {
			mobilHead = iniMobil;
			mobilTail = iniMobil;
		} else {
			mobilTail->next = iniMobil;
			iniMobil->prev = mobilTail;
			mobilTail = iniMobil;
		}
	}
	baca.close();
}

void updateMobilDB() {
	ofstream tulis("./data/mobil.txt", ios::trunc);
	tulis << totalMobil << "\n";
	NodeMobil* helperNode = mobilHead;
	if (isMobilEmpty()) {
		return;
	}
	do {
		tulis << helperNode->data.platNomor << "\n" <<
		helperNode->data.jenisMobil << "\n" <<
		helperNode->data.brandMobil << "\n";
		helperNode = helperNode->next;
	} while (helperNode != NULL);
	tulis.close();
}

void enqOrder(NodeOrder* newOrder) {
	if (isOrderEmpty()) {
		orderHead = newOrder;
		orderTail = newOrder;
	} else {
		orderTail->next = newOrder;
		orderTail = newOrder; // somehow seems too simple...
	}
	totalOrder++;
	updateOrderDB();
}

void deqOrder(bool isAcc) {
	// don't forget to check if empty BEFORE calling the func
	string filename = (isAcc ? "accepted.txt" : "rejected.txt");
	ofstream tulis("./data/" + filename, ios::app);
	tulis << orderHead->data.id << "\n" << orderHead->data.nama << "\n" <<
	orderHead->data.supir << "\n" << orderHead->data.platNomor << "\n" <<
	orderHead->data.tujuan << "\n\n"; // currently don't care how the data will be read
	NodeOrder* del = orderHead;
	orderHead = orderHead->next;
	delete del;
	totalOrder--;
	orderKeN++; // add this every dequeue
	tulis.close();
	updateOrderDB();
}

bool isEmpty() {
	if (head == NULL) {
		return true;
	} else {
		return false;
	}
}

bool isOrderEmpty() {
	if (orderHead == NULL) {
		return true;
	} else {
		return false;
	}
}

bool isMobilEmpty() {
	if (mobilHead == NULL) {
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
	if (isEmpty()) {
		gen += "0";
		return gen;
	}
	Node* iterateNode = tail;
	Node* latestOcc = NULL;
	do {
		if (iterateNode == supir) {
			continue;
		}
		string checkID = iterateNode->data.id;
		checkID = checkID.substr(0, 4);
		// Don't bother changing ID gen implementation, whatever lah
		if (checkID == gen) {
			latestOcc = iterateNode;
			break;
		}
		iterateNode = iterateNode->prev;
	} while (iterateNode != tail);
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

string idOrderGen(NodeOrder* order, string supirId) {
	// Btw, id can be > 10 digits karena id supir ga selalu 5 digits
	string gen;
	string plat = order->data.platNomor;
	string tujuan = order->data.tujuan;
	string nama = order->data.nama;
	for (char &c : plat) {
		c = tolower(c);
	}
	for (char &c : tujuan) {
		c = tolower(c);
	}
	int firstCh = plat.front() - 'a' + 1;
	gen += (to_string(firstCh).length() == 1 ? "0" + to_string(firstCh) : to_string(firstCh));
	gen += supirId;
	// Beware that this would result in an error kalo tujuan < 2 letters
	int lastTuj = tujuan.back() - 'a' + 1;
	tujuan.pop_back();
	int ingpokanNamingConvention = tujuan.back() - 'a' + 1;
	int hasil = lastTuj + ingpokanNamingConvention;
	gen += (to_string(hasil).length() == 1 ? "0" + to_string(hasil) : to_string(hasil));
	int hasilNama;
	for (char c : nama) { // dam this operator is op
		if (!isalpha(c)) {
			continue;
		}
		hasilNama += tolower(c) - 'a' + 1;
	}
	gen += to_string(hasilNama % 10);
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
			nodeHapus->next->prev = nodeCari;
			delete nodeHapus;
			totalSupir = totalSupir - 1;
			return;
		} else {
			nodeCari = nodeCari->next;
		}
	} while (nodeCari != head);
}

void addMobilHandler(NodeMobil* newMobil) {
	if (isMobilEmpty()) {
		mobilHead = newMobil;
		mobilTail = newMobil;
	} else {
		mobilTail->next = newMobil;
		newMobil->prev = mobilTail;
		mobilTail = newMobil;
	}
	totalMobil++;
	updateMobilDB();
}

void removeMobil() {
	if (!isMobilEmpty()) { // let's hope that wouldn't happen
		NodeMobil* del = mobilTail;
		if (totalMobil == 1) {
			// do I need to make a copy? actually not sure
			mobilHead = NULL;
			mobilTail = NULL;
		} else {
			mobilTail = mobilTail->prev;
			mobilTail->next = NULL;
		}
		delete del;
		totalMobil--;
		updateMobilDB();
	}
}

void addMobil() {
	system("cls");
	string platNomor, jenis, brand;
	cout << "==== Tambah Mobil ====\n\nMasukkan plat nomor mobil\t> ";
	getline(cin, platNomor);
	if (!regex_match(platNomor, regex("^[A-Z]{1,2}\\s\\d{1,4}\\s[A-Z]{1,3}$"))) {
		cout << "ERROR: Plat nomor tidak valdi!\n";
		system("pause");
		return menu(ADMIN_MENU);
	}
	cout << "Masukkan jenis mobil\t\t> ";
	getline(cin, jenis);
	cout << "Masukkan brand mobil\t\t> ";
	getline(cin, brand);
	NodeMobil* newMobil = new NodeMobil; // for readability's sake I'll put it down here
	newMobil->data.platNomor = platNomor;
	newMobil->data.jenisMobil = jenis;
	newMobil->data.brandMobil = brand;
	addMobilHandler(newMobil);
	cout << "\nData berhasil dimasukkan!\nIngin menambahkan unit lain? (Y/N)";
	char pil = '\0';
	pil = optionHandler();
	if (pil == 'Y' || pil == 'y') {
		addMobil();
	} else if (pil == 'N' || pil == 'n') {
		menu(ADMIN_MENU);
	}
}

void orderHandler(Node* supir) {
	system("cls");
	if (isMobilEmpty()) {
		cout << "ERROR: Unit mobil sedang kosong, harap menunggu...\n";
		system("pause");
		return menu(USER_MENU);
	}
	string nama, tujuan;
	cout << "==== Order ====\n\nMasukkan nama pelanggan\t> ";
	getline(cin, nama);
	cout << "Tujuan pelanggan\t> ";
	getline(cin, tujuan);
	if (tujuan.length() < 2) {
		cout << "ERROR: Tujuan tidak valdi!\n";
		system("pause");
		return;
	}
	NodeOrder* newOrder = new NodeOrder;
	newOrder->data.nama = nama;
	newOrder->data.tujuan = tujuan;
	newOrder->data.supir = supir->data.nama;
	// add plat nomor from mobilTail
	newOrder->data.platNomor = mobilTail->data.platNomor;
	removeMobil(); // and there you go, mobilnya lost forever WKWKWK
	string orderId = idOrderGen(newOrder, supir->data.id); // ew, unclean args
	newOrder->data.id = orderId;
	enqOrder(newOrder);
	system("cls");
	cout << "==== Order ====\n\nOrder telah dilakukan!\n";
	cout << "ID\t\t: " << orderId << "\nNama\t\t: " << nama << "\nSupir\t\t: " <<
	supir->data.nama << "\nPlat nomor\t: " << newOrder->data.platNomor << "\nTujuan\t\t: " <<
	tujuan << "\nIngin melakukan order lagi? (Y/N) > "; // I couldn't care less, just format it here
	char pil = '\0';
	pil = optionHandler();
	if (pil == 'Y' || pil == 'y') {
		orderHandler(supir);
	} else if (pil == 'N' || pil == 'n') {
		menu(USER_MENU);
	}
}

void prosesPesanan() {
	system("cls");
	if (isOrderEmpty()) {
		cout << "Tidak ada pesanan!\n";
		system("pause");
		return menu(ADMIN_MENU);
	}
	cout << "==== Pesanan ke-" << orderKeN << " ====\n\n";
	// Start from head, as always
	NodeOrder* currentNode = orderHead;
	cout << "ID\t\t: " << currentNode->data.id << "\nNama\t\t: " << currentNode->data.nama << "\nSupir\t\t: " <<
	currentNode->data.supir << "\nPlat nomor\t: " << currentNode->data.platNomor << "\nTujuan\t\t: " <<
	currentNode->data.tujuan << "\n\n1. Accept\n2. Reject\n3. Exit\n> ";
	char pil = '\0';
	pil = optionHandler();
	string currentID = currentNode->data.id;
	switch (pil) {
		case '1':
			deqOrder(true);
			cout << "Pesanan dengan ID " + currentID + " telah diterima!\n";
			system("pause");
			return prosesPesanan();
			break;
		case '2':
			deqOrder(false);
			cout << "Pesanan dengan ID " + currentID + " telah ditolak!\n";
			system("pause");
			return prosesPesanan();
			break;
		case '3':
			menu(ADMIN_MENU);
			break;
		case '\0':
			return;
		
		default:
			cout << "Pilihan invalid!\n";
			system("pause");
			return prosesPesanan();
			break;
	}
}

void userMenu() {
	// Just copy paste
	Node* helperNode = head;
	if (isEmpty()) {
		cout << "ERROR: Data supir kosong!\n";
		system("pause");
		return menu(MAIN_MENU);
	}
	while (true) {
		system("cls");
		printSupir(helperNode);
		cout << "\n\n1. Next\n2. Previous\n3. Order\n0. Exit\n> ";
		char pil = '\0';
		pil = optionHandler();
		switch (pil) {
			case '1':
				helperNode = helperNode->next;
				break;
			case '2':
				helperNode = helperNode->prev;
				break;
			case '3':
				orderHandler(helperNode);
				break;
			case '0':
				return menu(MAIN_MENU);
			case '\0':
				return;
			
			default:
				cout << "Pilihan invalid!\n";
				system("pause");
				break;
		}
	}
}

void ingpoDataSupir() {
	system("cls");
	if (isEmpty()) {
		cout << "ERROR: Data supir kosong!\n";
		system("pause");
		return menu(ADMIN_MENU);
	}
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
				helperNode = helperNode->prev;
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
		cout << "ERROR: Supir tidak ditemukan!\n";
		system("pause");
		cin.ignore();
		return menu(ADMIN_MENU);
	} else {
		cout << "\nSupir dengan data berikut akan dihapus:\n";
		printSupir(cariSupir);
		cout << "\n\nLanjutkan? (Y/N)\n> ";
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

void ubahSupir() { // Konotasinya kayak supirnya ganti
	system("cls");
	if (isEmpty()) {
		cout << "ERROR: Data supir kosong!\n";
		system("pause");
		return menu(ADMIN_MENU);
	}
	string id;
	cout << "Masukkan ID supir yang ingin diubah > ";
	cin >> id;
	Node* iniSupir = cariDataSupirHandler(id);
	if (iniSupir == NULL) {
		cout << "\nERROR: Supir tidak ditemukan!\n\n";
		system("pause");
		cin.ignore();
		return menu(ADMIN_MENU);
	}
	system("cls");
	cout << "- Mengubah Supir Dengan ID " << id << " -\n" <<
	"1. Ubah Nama\n2. Ubah Kelamin\n3. Ubah Alamat\n4. Ubah Tgl Lahir\n> ";
	char pil = '\0';
	pil = optionHandler();
	system("cls");
	cout << "Jangan keluar sebelum proses selesai!\n";
	switch (pil) {
		case '1': {
			string namaBaru;
			cout << "Masukkan nama baru > ";
			cin.ignore();
			getline(cin, namaBaru);
			iniSupir->data.nama = namaBaru;
			string idBaru = idGen(iniSupir);
			iniSupir->data.id = idBaru;
			break;
		}
		case '2': {
			char newGender;
			cout << "Masukkan kelamin baru > "; // Hah? Gimana gimana?
			cin >> newGender;
			if (newGender != 'L' && newGender != 'l' && newGender != 'P' && newGender != 'p') {
				cout << "Gender tidak valdi!\n";
				system("pause");
				cin.ignore();
				return menu(ADMIN_MENU);
			}
			iniSupir->data.kelamin = newGender;
			string idBaru = idGen(iniSupir);
			iniSupir->data.id = idBaru;
			break;
		}
		case '3' : {
			string alamatBaru;
			cout << "Masukkan alamat baru > ";
			cin.ignore();
			getline(cin, alamatBaru);
			iniSupir->data.alamat = alamatBaru; // Doesn't need new ID
			break;
		}
		case '4' : {
			string tglLahirBaru;
			cout << "Masukkan tgl lahir baru (dd/mm/yyyy) > ";
			cin >> tglLahirBaru;
			Date lahiran = dateSplitter(tglLahirBaru, '/');
			if (!lahiran.validate()) {
				cout << "Tanggal lahir tidak valdi!\n";
				system("pause");
				cin.ignore();
				return menu(ADMIN_MENU);
			}
			iniSupir->data.tglLahir = lahiran;
			string idBaru = idGen(iniSupir);
			iniSupir->data.id = idBaru;
			break;
		}
		case '\0':
			return;
		
		default:
			cout << "Pilihan invalid!\n";
			system("pause");
			return menu(ADMIN_MENU);
			break;
	}
	updateDB();
	cout << "Data telah diubah!\n";
	printSupir(iniSupir);
	cout << "\n\n";
	system("pause");
	return menu(ADMIN_MENU);
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
		cin.ignore();
		return menu(ADMIN_MENU);
	}
	cout << "Kelamin (L/P)          : ";
	cin >> gender;
	if (gender != 'L' && gender != 'l' && gender != 'P' && gender != 'p') {
		cout << "Gender tidak valdi!\n";
		system("pause");
		cin.ignore();
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
	if (isEmpty()) {
		head = newSupir;
		tail = newSupir;
		head->next = newSupir;
		head->prev = newSupir;
	} else {
		tail->next = newSupir;
		head->prev = newSupir;
		newSupir->prev = tail; // how did I not notice this
		tail = newSupir;
		newSupir->next = head;
	}
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
			cout << "1. Masuk Sebagai Admin\n2. Masuk Sebagai User\n0. Exit\n> ";
			char pil = '\0';
			pil = optionHandler();
			switch (pil) {
				case '1':
					menu(ADMIN_MENU);
					break;
				case '2':
					menu(USER_MENU);
					break;
				case '0':
					exit(0);
					return;
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
			"2. Menghapus Data Supir\n3. Mengubah Data Supir\n4. Menambah Data Supir" <<
			"\n5. Proses Pesanan\n6. Tambah Unit Mobil\n0. Exit\n> "; // unnecessarily long, ew
			char pil = '\0';
			pil = optionHandler();
			switch (pil) {
				case '1':
					// cariDataSupir(); wrong
					ingpoDataSupir();
					break;
				case '2':
					hapusSupir();
					break;
				case '3':
					ubahSupir();
					break;
				case '4':
					tambahSupir();
					break;
				case '5':
					prosesPesanan();
					break;
				case '6':
					addMobil();
					break;
				case '0':
					return menu(MAIN_MENU);
				case '\0':
					return;
				
				default:
					cout << "Pilihan invalid!\n";
					system("pause");
					menu(ADMIN_MENU);
					break;
			}}
			break;

		case USER_MENU:
				userMenu();
				break;
	}
}

void quit() {
	system("cls");
	cout << "Done ngabsku";
}

void init() {
	initDB();
	initOrderDB();
	initMobilDB();
	menu(MAIN_MENU);
}

int main() {
	atexit(quit);
	init();

	return 0;
}