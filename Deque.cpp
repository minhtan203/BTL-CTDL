#include "Deque.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

Deque::Deque() {
    head = NULL;
    tail = NULL;
}

Deque::~Deque() {
    while (!isEmpty()) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

bool Deque::isEmpty() {
    return head == NULL;
}

void Deque::themKhachVIP(KhachHang kh) {
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

void Deque::themKhachThuong(KhachHang kh) {
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

void Deque::xuLyVe(PhongVe &phongVe) {
    if (isEmpty()) {
        cout << "[!] Hang cho dang trong!\n";
        return;
    }
    Node* temp = head;
    cout << "=> XU LY THANH TOAN: " << temp->data.tenKH 
         << " | Ghe: " << temp->data.viTriGhe << "\n";
    
    phongVe.datGhe(temp->data.viTriGhe);

    head = head->next;
    if (head != NULL) {
        head->prev = NULL;
    } else {
        tail = NULL;
    }
    delete temp;
}

void Deque::khachCuoiHuy(PhongVe &phongVe) {
    if (isEmpty()) {
        cout << "[!] Hang cho dang trong!\n";
        return;
    }
    Node* temp = tail;
    cout << "=> Khach hang " << temp->data.tenKH << " da huy dang ky xep hang.\n";
    
    phongVe.huyGhe(temp->data.viTriGhe);

    tail = tail->prev;
    if (tail != NULL) {
        tail->next = NULL;
    } else {
        head = NULL;
    }
    delete temp;
}

void Deque::inHangCho() {
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

void Deque::timKiemKhachHang(string maOrTen) {
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

void Deque::ghiFileCSV() {
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

void Deque::docFileCSV(PhongVe &phongVe) {
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
        phongVe.datGhe(kh.viTriGhe);
    }
    file.close();
    cout << "[!] Da tai du lieu cu tu file CSV!\n";
}