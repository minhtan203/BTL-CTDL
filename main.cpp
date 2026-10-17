#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

struct KhachHang {
    string maKH;
    string tenKH;
    int loaiVe;
    string viTriGhe;
};

struct Node {
    KhachHang data;
    Node* prev;
    Node* next;

    Node(KhachHang kh) {
        data = kh;
        prev = NULL;
        next = NULL;
    }
};

class Deque {
private:
    Node* head;
    Node* tail;

public:
    Deque() {
        head = NULL;
        tail = NULL;
    }

    ~Deque() {
        while (!isEmpty()) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    bool isEmpty() {
        return head == NULL;
    }

    bool isGheDaDat(string ghe) {
        Node* curr = head;
        while (curr != NULL) {
            if (curr->data.viTriGhe == ghe) {
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    void themKhachVIP(KhachHang kh) {
        Node* newNode = new Node(kh);
        if (isEmpty()) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        cout << "[!] Da them khach VIP vao DAU hang!\n";
    }

    void themKhachThuong(KhachHang kh) {
        Node* newNode = new Node(kh);
        if (isEmpty()) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        cout << "[!] Da them khach Thuong vao CUOI hang.\n";
    }

    void hoanKhachVeDau(KhachHang kh) {
        Node* newNode = new Node(kh);
        if (isEmpty()) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        cout << "[!] Da hoan khach hang " << kh.tenKH << " ve DAU hang cho!\n";
    }

    void xuLyVe() {
        if (isEmpty()) {
            cout << "[!] Hang cho dang trong!\n";
            return;
        }
        Node* temp = head;
        cout << "=> XU LY THANH TOAN: " << temp->data.tenKH 
             << " | Ghe: " << temp->data.viTriGhe << "\n";

        string tenFileVe = "Ve_" + temp->data.maKH + ".txt";
        ofstream fVe(tenFileVe.c_str());
        if (fVe.is_open()) {
            fVe << "=======================================\n";
            fVe << "            VE XEM PHIM UTC2          \n";
            fVe << "=======================================\n";
            fVe << "Ma Khach Hang : " << temp->data.maKH << "\n";
            fVe << "Ten Khach Hang: " << temp->data.tenKH << "\n";
            fVe << "Loai Ve       : " << (temp->data.loaiVe == 1 ? "VIP" : "Thuong") << "\n";
            fVe << "Vi Tri Ghe    : " << temp->data.viTriGhe << "\n";
            fVe << "=======================================\n";
            fVe.close();
            cout << "[!] Da in va xet ve dien tu ra file: " << tenFileVe << "\n";
        }

        head = head->next;
        if (head != NULL) {
            head->prev = NULL;
        } else {
            tail = NULL;
        }
        delete temp;
    }

    void khachCuoiHuy() {
        if (isEmpty()) {
            cout << "[!] Hang cho dang trong!\n";
            return;
        }
        Node* temp = tail;
        cout << "=> Khach hang " << temp->data.tenKH << " da huy dang ky xep hang.\n";

        tail = tail->prev;
        if (tail != NULL) {
            tail->next = NULL;
        } else {
            head = NULL;
        }
        delete temp;
    }

    void inHangCho() {
        if (isEmpty()) {
            cout << "Hang cho trong.\n";
            return;
        }
        Node* curr = head;
        cout << "\n---------------- DANH SACH HANG CHO ----------------\n";
        while (curr != NULL) {
            string loai = (curr->data.loaiVe == 1) ? "VIP" : "Thuong";
            cout << "Ma: " << curr->data.maKH 
                 << " | Ten: " << curr->data.tenKH 
                 << " | Loai: " << loai 
                 << " | Ghe: " << curr->data.viTriGhe << "\n";
            curr = curr->next;
        }
        cout << "----------------------------------------------------\n";
    }

    void timKiemKhachHang(string maOrTen) {
        if (isEmpty()) {
            cout << "Hang cho trong.\n";
            return;
        }
        Node* curr = head;
        bool timThay = false;
        cout << "\n--- KET QUA TIM KIEM ---\n";
        while (curr != NULL) {
            if (curr->data.maKH == maOrTen || curr->data.tenKH.find(maOrTen) != string::npos) {
                string loai = (curr->data.loaiVe == 1) ? "VIP" : "Thuong";
                cout << "Tim thay -> Ma: " << curr->data.maKH 
                     << " | Ten: " << curr->data.tenKH 
                     << " | Loai: " << loai 
                     << " | Ghe: " << curr->data.viTriGhe << "\n";
                timThay = true;
            }
            curr = curr->next;
        }
        if (!timThay) {
            cout << "Khong tim thay khach hang voi tu khoa: " << maOrTen << "\n";
        }
        cout << "-----------------------\n";
    }

    void ghiFileCSV() {
        ofstream file("DanhSachKhachHang.csv");
        if (!file.is_open()) {
            cout << "Loi tao/mo file CSV!\n";
            return;
        }
        file << "MaKH,TenKH,LoaiVe,ViTriGhe\n";
        Node* curr = head;
        while (curr != NULL) {
            file << curr->data.maKH << ","
                 << curr->data.tenKH << ","
                 << curr->data.loaiVe << ","
                 << curr->data.viTriGhe << "\n";
            curr = curr->next;
        }
        file.close();
        cout << "[!] Da luu file CSV thanh cong!\n";
    }

    void docFileCSV() {
        ifstream file("DanhSachKhachHang.csv");
        if (!file.is_open()) return;

        string line;
        getline(file, line);
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            KhachHang kh;
            string loaiStr;

            getline(ss, kh.maKH, ',');
            getline(ss, kh.tenKH, ',');
            getline(ss, loaiStr, ',');
            getline(ss, kh.viTriGhe, ',');

            stringstream conv(loaiStr);
            conv >> kh.loaiVe;

            if (kh.loaiVe == 1) {
                themKhachVIP(kh);
            } else {
                themKhachThuong(kh);
            }
        }
        file.close();
        cout << "[!] Da tai du lieu cu tu file CSV!\n";
    }
};

int main() {
    Deque hangCho;
    hangCho.docFileCSV();

    int luaChon;
    while (true) {
        cout << "\n=========================================\n";
        cout << "     HE THONG DAT VE XEM PHIM UTC2      \n";
        cout << "=========================================\n";
        cout << "1. Khach VIP xep hang (Them DAU)\n";
        cout << "2. Khach Thuong xep hang (Them CUOI)\n";
        cout << "3. Hoan khach hang ve DAU hang cho\n";
        cout << "4. Xu ly thanh toan & In ve dien tu\n";
        cout << "5. Khach cuoi hang huy doi (Xoa CUOI)\n";
        cout << "6. Xem danh sach hang cho hien tai\n";
        cout << "7. Tim kiem khach hang\n";
        cout << "8. Luu du lieu & Thoat\n";
        cout << "-----------------------------------------\n";
        cout << "Nhap lua chon (1-8): ";

        if (!(cin >> luaChon)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Vui long nhap so hop le!\n";
            continue;
        }

        if (luaChon == 1 || luaChon == 2 || luaChon == 3) {
            KhachHang kh;
            cout << "Nhap Ma KH: "; cin >> kh.maKH;
            cin.ignore();
            cout << "Nhap Ten KH: "; getline(cin, kh.tenKH);

            do {
                cout << "Nhap Vi tri ghe (VD: A1, B3): "; 
                cin >> kh.viTriGhe;
                if (hangCho.isGheDaDat(kh.viTriGhe)) {
                    cout << "[!] Vi tri ghe " << kh.viTriGhe << " da duoc dat trong hang cho. Vui long chon ghe khac!\n";
                } else {
                    break;
                }
            } while (true);

            if (luaChon == 1) {
                kh.loaiVe = 1;
                hangCho.themKhachVIP(kh);
            } else if (luaChon == 2) {
                kh.loaiVe = 2;
                hangCho.themKhachThuong(kh);
            } else {
                kh.loaiVe = 1;
                hangCho.hoanKhachVeDau(kh);
            }
        } 
        else if (luaChon == 4) {
            hangCho.xuLyVe();
        } 
        else if (luaChon == 5) {
            hangCho.khachCuoiHuy();
        } 
        else if (luaChon == 6) {
            hangCho.inHangCho();
        } 
        else if (luaChon == 7) {
            string keyword;
            cout << "Nhap Ma KH hoac Ten KH can tim: ";
            cin.ignore();
            getline(cin, keyword);
            hangCho.timKiemKhachHang(keyword);
        }
        else if (luaChon == 8) {
            hangCho.ghiFileCSV();
            cout << "Da thoat chuong trinh!\n";
            break;
        } 
        else {
            cout << "Lua chon khong hop le!\n";
        }
    }
    return 0;
}