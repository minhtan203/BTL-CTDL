# 🎬 HỆ THỐNG ĐẶT VÉ XEM PHIM & HÀNG CHỜ KHÁCH HÀNG SỬ DỤNG DEQUE

> **Bài tập lớn môn Cấu trúc Dữ liệu & Giải thuật - Đề tài 19**  
> **Trường:** Đại học Giao thông Vận tải Phân hiệu tại TP. Hồ Chí Minh (UTC2)  
> **Khoa:** Điện - Điện tử | **Bộ môn:** Cấu trúc dữ liệu giải thuật 

---

## 📌 Giới thiệu Đề tài
Dự án mô phỏng hệ thống quản lý hàng chờ bán vé xem phim tại quầy rạp chiếu phim. Hệ thống áp dụng Cấu trúc Dữ liệu **Deque (Double-Ended Queue - Hàng đợi hai đầu)** được tự cài đặt bằng **Danh sách liên kết đôi (Doubly Linked List)** để tối ưu hóa thời gian xử lý ưu tiên cho khách VIP, khách thường và các thao tác hủy/hoàn vé ở hai đầu hàng chờ.

---

## 👥 Danh sách Thành viên Nhóm
| STT | Họ và Tên | MSSV | Vai trò | Nhánh GitHub (Branch) |
|---|---|---|---|---|
| 1 | Nguyễn Minh Tân | 6651060055 | Trưởng nhóm / Architect | `main` |
| 2 | Phạm Lê Ngọc Tài | 6651060053  | C++ Core Developer | `nhanh-tv2-core` |
| 3 | Lê Nguyễn Huỳnh Phúc | 6651060045  | Business Logic & File I/O | `nhanh-tv3-logic` |
| 4 | Thái Lê Hữu Tài | 6651060054 | UI Console & Test | `nhanh-tv4-ui` |

---

## 🚀 Các Tính năng Chính
1. **Xếp hàng ưu tiên (Push):**
   * **Khách VIP:** Được chèn vào **ĐẦU** Deque (`push_front`) để ưu tiên phục vụ trước.
   * **Khách Thường:** Được thêm vào **CUỐI** Deque (`push_back`) theo thứ tự xếp hàng truyền thống.
2. **Xử lý Bán vé & Thanh toán (Pop):**
   * Chọn khách hàng ở đầu hàng chờ (`pop_front`) để giao dịch.
   * Kiểm tra và chống trùng lặp vị trí ghế ngồi.
   * Tự động xuất vé điện tử cá nhân ra file `.txt` (`Ve_[MaKH].txt`).
3. **Hoàn & Hủy vé linh hoạt:**
   * **Hoàn khách về đầu:** Đẩy lại khách vừa đổi ý về đầu Deque (`push_front`).
   * **Khách hủy xếp hàng:** Loại bỏ khách ở cuối hàng chờ (`pop_back`).
4. **Lưu trữ dữ liệu vĩnh viễn (File I/O):**
   * Đọc danh sách khởi tạo từ file `DanhSachKhachHang.csv`.
   * Ghi lịch sử giao dịch bán vé ra file `DanhSachKhachHang.csv`.

---

## 🛠️ Công nghệ & Cấu trúc Dữ liệu Sử dụng
* **Ngôn ngữ lập trình:** C++ (Biên dịch chuẩn trên Dev-C++ / GCC).
* **Cấu trúc dữ liệu cốt lõi:**
  * `struct KhachHang`: Lưu thông tin Mã KH, Họ tên, SĐT, Loại khách (VIP/Thường), Mã ghế.
  * `struct Node`: Chứa thông tin khách hàng và 2 con trỏ `next`, `prev`.
  * `class Deque`: Cài đặt hàng đợi 2 đầu bằng Danh sách liên kết đôi (`head`, `tail`, `count`).

---

## 💻 Hướng dẫn Biên dịch & Chạy Chương trình

### Cách 1: Sử dụng Dev-C++ (Khuyên dùng)
1. Mở phần mềm **Dev-C++**.
2. Chọn `File` $\rightarrow$ `Open Project/File...` $\rightarrow$ Chọn file project `BTL_Datvephim.dev` (hoặc mở file `main.cpp`).
3. Nhấn phím **`F11`** (Compile & Run) để biên dịch và chạy chương trình.

### Cách 2: Sử dụng G++ Terminal / Command Prompt
```bash
# Biên dịch tất cả các file source code
g++ -o main main.cpp Deque.cpp PhongVe.cpp

# Chạy chương trình
./main
