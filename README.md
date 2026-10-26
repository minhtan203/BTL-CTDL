# 🎬 BÁO CÁO ĐỒ ÁN MÔN CẤU TRÚC DỮ LIỆU & GIẢI THUẬT (UTC2)
## 📌 ĐỀ TÀI 19: HỆ THỐNG ĐẶT VÉ XEM PHIM & HÀNG CHỜ KHÁCH HÀNG
> **Đơn vị:** Khoa Công nghệ Thông tin – Trường Đại học Giao thông Vận tải Phân hiệu tại TP.HCM (UTC2)  
> **Môn học:** Cấu trúc Dữ liệu và Giải thuật  
> **Cấu trúc dữ liệu cốt lõi:** Deque (Hàng đợi hai đầu) cài đặt bằng Danh sách liên kết đôi thủ công (không dùng STL `std::deque` hay `std::list`).

---

## 👨‍💻 NHÓM THỰC HIỆN (NHÓM 6)

| STT | Họ và tên | Mã sinh viên | Vai trò trong dự án | Tỷ lệ đóng góp |
| :---: | :--- | :---: | :--- | :---: |
| 1 | **Nguyễn Minh Tân** | **6651060055** | **Trưởng nhóm** – Lập trình C++/JS, Kiểm thử bộ nhớ, Tổng hợp Báo cáo Word 40–50 trang | 100% |
| 2 | **Phạm Lê Ngọc Tài** | **6651060053** | Soạn Báo cáo Word (Chương 1: Nghiệp vụ & Phụ lục A, B, C) | 100% |
| 3 | **Lê Nguyễn Huỳnh Phúc** | **6651060045** | Soạn Báo cáo Word (Chương 3 & 4), Xây dựng bộ 12 Kịch bản Test Cases | 100% |
| 4 | **Thái Lê Hữu Tài** | **6651060054** | Soạn Báo cáo Word (Chương 2: Thiết kế CTDL), Đồng chủ trì thiết kế Slide | 100% |
| 5 | **Lê Phúc Trường** | **6651060054** | Chủ trì thiết kế Slide PowerPoint, Quay Video Demo & Kiểm thử Giao diện Web | 100% |

---

## 🌟 ĐẶC ĐIỂM NỔI BẬT & QUY TẮC NGHIỆP VỤ

### 1. Quản lý Hàng chờ bằng Deque Danh sách liên kết đôi (DLL)
* **Khách thường (`CUS_NORMAL`):** Xếp hàng vào **CUỐI** hàng đợi $\rightarrow$ Thao tác `push_back`.
* **Khách ưu tiên (`CUS_PRIORITY`):** Được chen lên **ĐẦU** hàng đợi $\rightarrow$ Thao tác `push_front`.
* **Yêu cầu hoàn vé (`CUS_REFUND`):** Khi hủy vé, hệ thống tạo một yêu cầu hoàn và đẩy thẳng lên **ĐẦU** hàng chờ $\rightarrow$ Thao tác `push_front` để giải phóng ghế sớm nhất.
* **Phục vụ khách kế tiếp:** Lấy khách ở **ĐẦU** hàng $\rightarrow$ Thao tác `pop_front`.
* **Khách cuối hàng bỏ về:** Gỡ phần tử ở **CUỐI** hàng $\rightarrow$ Thao tác `pop_back`.
* **Xóa khách bất kỳ theo ID:** Tìm và nối con trỏ `prev`/`next` gỡ nút trong $O(1)$ sau khi tìm thấy $\rightarrow$ Thao tác `removeById`.

### 2. Sơ đồ Phòng chiếu & Thuật toán Gợi ý Chỗ ngồi Tối ưu
* **Sơ đồ phòng chiếu:** Ma trận 8 hàng $\times$ 10 cột = 80 ghế[cite: 10, 13].
  * Ghế VIP (Hàng C đến F / Chỉ số 2–5): Giá 120.000 VNĐ[cite: 10, 13].
  * Ghế Thường (Hàng A, B, G, H): Giá 80.000 VNĐ[cite: 10, 13].
* **Thuật toán chấm điểm vị trí:** Tự động đề xuất nhóm $n$ ghế liền kề tốt nhất dựa trên khoảng cách hình học tới tâm phòng chiếu ($midRow = 3.5, midCol = 4.5$)[cite: 10, 13].
$$\text{Score} = \vert{}r - midRow\vert{} \times 100 + \vert{}start + (n - 1)/2.0 - midCol\vert{} \times 10$$
* **Chính sách hoàn tiền:** Hủy vé thành công được hoàn **80%** giá trị vé (phí hủy 20%), ghế lập tức được trả về trạng thái trống[cite: 10, 13].

---

## 📂 CẤU TRÚC THƯ MỤC DỰ ÁN

```text
BTL-CTDL/
├── Code_CPP/
│   └── main.cpp                   # Toàn bộ mã nguồn C++ (794 dòng) chạy Console
├── Web_App/
│   ├── index.html                 # Giao diện Web mô phỏng phòng vé
│   ├── app.js                     # Cấu trúc Deque & Nghiệp vụ viết bằng JavaScript
│   └── style.css                  # Style CSS giao diện trực quan
├── Bao_Cao/
│   ├── Bao_Cao_Do_An_CTDLGT_De19.docx # Báo cáo Word 40–50 trang chuẩn định dạng
│   └── Slide_Bao_Ve_Do_An.pptx    # Slide PowerPoint thuyết trình bảo vệ
└── README.md                      # Tài liệu hướng dẫn & Giới thiệu dự án
