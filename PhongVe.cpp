#include "PhongVe.h"
#include <iostream>
#include <cctype>

using namespace std;

PhongVe::PhongVe() {
    for (int i = 0; i < SO_HANG; i++) {
        for (int j = 0; j < SO_COT; j++) {
            meTranGhe[i][j] = 0;
        }
    }
}

void PhongVe::hienThiSoDo() {
    cout << "\n\033[33m======== SO DO PHONG CHIEU PHIM ========\033[0m\n";
    cout << "     ";
    for (int j = 1; j <= SO_COT; j++) {
        cout << j << "   ";
    }
    cout << "\n";

    for (int i = 0; i < SO_HANG; i++) {
        char tenHang = 'A' + i;
        cout << " " << tenHang << "  ";
        for (int j = 0; j < SO_COT; j++) {
            if (meTranGhe[i][j] == 0) {
                cout << "\033[32m[ ] \033[0m";
            } else {
                cout << "\033[31m[X] \033[0m";
            }
        }
        cout << "\n";
    }
    cout << "\033[32m[ ] Ghe trong\033[0m | \033[31m[X] Ghe da dat\033[0m\n";
    cout << "========================================\n";
}

bool PhongVe::kiemTraGheTrong(string viTri) {
    if (viTri.length() < 2) return false;
    char hangChar = toupper(viTri[0]);
    int hang = hangChar - 'A';
    int cot = (viTri[1] - '0') - 1;
    if (viTri.length() == 3) {
        cot = ((viTri[1] - '0') * 10 + (viTri[2] - '0')) - 1;
    }

    if (hang < 0 || hang >= SO_HANG || cot < 0 || cot >= SO_COT) {
        return false;
    }
    return meTranGhe[hang][cot] == 0;
}

bool PhongVe::datGhe(string viTri) {
    if (viTri.length() < 2) return false;
    char hangChar = toupper(viTri[0]);
    int hang = hangChar - 'A';
    int cot = (viTri[1] - '0') - 1;
    if (viTri.length() == 3) {
        cot = ((viTri[1] - '0') * 10 + (viTri[2] - '0')) - 1;
    }

    if (hang < 0 || hang >= SO_HANG || cot < 0 || cot >= SO_COT) {
        return false;
    }
    if (meTranGhe[hang][cot] == 1) return false;

    meTranGhe[hang][cot] = 1;
    return true;
}

void PhongVe::huyGhe(string viTri) {
    if (viTri.length() < 2) return;
    char hangChar = toupper(viTri[0]);
    int hang = hangChar - 'A';
    int cot = (viTri[1] - '0') - 1;
    if (viTri.length() == 3) {
        cot = ((viTri[1] - '0') * 10 + (viTri[2] - '0')) - 1;
    }

    if (hang >= 0 && hang < SO_HANG && cot >= 0 && cot < SO_COT) {
        meTranGhe[hang][cot] = 0;
    }
}