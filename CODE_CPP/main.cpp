#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cctype>
#include <cstdlib>
#include <cstdio>

using namespace std;

const int   ROWS             = 8;
const int   COLS             = 10;
const int   VIP_ROW_FROM     = 2;
const int   VIP_ROW_TO       = 5;
const int   MAX_SEATS_ORDER  = 8;
const long PRICE_NORMAL     = 80000;
const long PRICE_VIP        = 120000;
const int   REFUND_PERCENT   = 80;

const string MOVIE_NAME = "Phim demo - Chieu dac biet";
const string SHOW_TIME  = "19:30";
const string ROOM_NAME  = "Phong 01";

const string DB_FILE    = "database.txt";
const long   DB_VERSION = 1;

struct InputClosed {};

static string trim(const string& s) {
    size_t a = 0, b = s.size();
    while (a < b && isspace((unsigned char)s[a])) a++;
    while (b > a && isspace((unsigned char)s[b - 1])) b--;
    return s.substr(a, b - a);
}

static string readLine(const string& prompt) {
    cout << prompt;
    string s;
    if (!getline(cin, s)) throw InputClosed();
    return trim(s);
}

static string readNonEmpty(const string& prompt) {
    while (true) {
        string s = readLine(prompt);
        if (!s.empty()) return s;
        cout << "  ! Khong duoc de trong.\n";
    }
}

static long readLong(const string& prompt, long lo, long hi) {
    while (true) {
        string s = readLine(prompt);
        if (!s.empty()) {
            char* end = nullptr;
            long v = strtol(s.c_str(), &end, 10);
            if (*end == '\0' && v >= lo && v <= hi) return v;
        }
        cout << "  ! Gia tri khong hop le, vui long nhap so tu " << lo << " den " << hi << ".\n";
    }
}

static int readInt(const string& prompt, int lo, int hi) {
    return (int)readLong(prompt, lo, hi);
}

static bool readYesNo(const string& prompt) {
    while (true) {
        string s = readLine(prompt + " (y/n): ");
        if (!s.empty()) {
            char ch = (char)tolower((unsigned char)s[0]);
            if (ch == 'y' || ch == 'c') return true;
            if (ch == 'n' || ch == 'k') return false;
        }
        cout << "  ! Chi nhap y (dong y) hoac n (khong).\n";
    }
}

static string formatMoney(long v) {
    string s = to_string(v < 0 ? -v : v), out;
    int cnt = 0;
    for (int i = (int)s.size() - 1; i >= 0; --i) {
        out.insert(out.begin(), s[i]);
        if (++cnt % 3 == 0 && i > 0) out.insert(out.begin(), '.');
    }
    return (v < 0 ? "-" : "") + out + " VND";
}

static string nowString() {
    time_t t = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", localtime(&t));
    return buf;
}

static void line(char ch = '=', int n = 78) { cout << string(n, ch) << "\n"; }

enum CustomerType {
    CUS_NORMAL   = 1,
    CUS_PRIORITY = 2,
    CUS_REFUND   = 3
};

struct Customer {
    int         id;
    string       name;
    string       phone;
    CustomerType type;
    int         seatCount;
    bool         wantVip;
    int         refundTicketId;

    Customer() : id(0), type(CUS_NORMAL), seatCount(1), wantVip(false), refundTicketId(0) {}
};

static string typeText(CustomerType t) {
    switch (t) {
        case CUS_NORMAL:   return "Thuong";
        case CUS_PRIORITY: return "UU TIEN";
        case CUS_REFUND:   return "HOAN VE";
    }
    return "?";
}

class CustomerDeque {
public:
    struct Node {
        Customer data;
        Node*    prev;
        Node*    next;
        Node(const Customer& c) : data(c), prev(nullptr), next(nullptr) {}
    };

private:
    Node*  head;
    Node*  tail;
    size_t count;

public:
    CustomerDeque() : head(nullptr), tail(nullptr), count(0) {}
    ~CustomerDeque() { clear(); }

    CustomerDeque(const CustomerDeque&) = delete;
    CustomerDeque& operator=(const CustomerDeque&) = delete;

    bool   empty() const { return count == 0; }
    size_t size()  const { return count; }
    const Node* headNode() const { return head; }

    const Customer* front() const { return head ? &head->data : nullptr; }
    const Customer* back()  const { return tail ? &tail->data : nullptr; }

    void push_front(const Customer& c) {
        Node* n = new Node(c);
        if (head == nullptr) {
            head = tail = n;
        } else {
            n->next    = head;
            head->prev = n;
            head       = n;
        }
        ++count;
    }

    void push_back(const Customer& c) {
        Node* n = new Node(c);
        if (tail == nullptr) {
            head = tail = n;
        } else {
            n->prev    = tail;
            tail->next = n;
            tail       = n;
        }
        ++count;
    }

    bool pop_front(Customer& out) {
        if (head == nullptr) return false;
        Node* del = head;
        out  = del->data;
        head = head->next;
        if (head != nullptr) head->prev = nullptr;
        else                 tail = nullptr;
        delete del;
        --count;
        return true;
    }

    bool pop_back(Customer& out) {
        if (tail == nullptr) return false;
        Node* del = tail;
        out  = del->data;
        tail = tail->prev;
        if (tail != nullptr) tail->next = nullptr;
        else                 head = nullptr;
        delete del;
        --count;
        return true;
    }

    bool removeById(int id, Customer& out) {
        for (Node* p = head; p != nullptr; p = p->next) {
            if (p->data.id != id) continue;
            out = p->data;
            if (p->prev) p->prev->next = p->next; else head = p->next;
            if (p->next) p->next->prev = p->prev; else tail = p->prev;
            delete p;
            --count;
            return true;
        }
        return false;
    }

    size_t clear() {
        size_t freed = 0;
        Node* p = head;
        while (p != nullptr) {
            Node* nxt = p->next;
            delete p;
            p = nxt;
            ++freed;
        }
        head = tail = nullptr;
        count = 0;
        return freed;
    }
};

enum SeatType { SEAT_NORMAL = 0, SEAT_VIP = 1 };

struct SeatPos { int r, c; };

struct Seat {
    SeatType type;
    int      ticketId;
};

class Cinema {
    Seat seats[ROWS][COLS];

public:
    Cinema() {
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c) {
                seats[r][c].type = (r >= VIP_ROW_FROM && r <= VIP_ROW_TO) ? SEAT_VIP : SEAT_NORMAL;
                seats[r][c].ticketId = 0;
            }
    }

    static string seatName(const SeatPos& p) {
        return string(1, (char)('A' + p.r)) + to_string(p.c + 1);
    }

    bool     isFree(int r, int c) const { return seats[r][c].ticketId == 0; }
    int      ticketIdAt(int r, int c) const { return seats[r][c].ticketId; }
    SeatType typeAt(int r, int c) const { return seats[r][c].type; }
    long     priceAt(int r, int c) const { return seats[r][c].type == SEAT_VIP ? PRICE_VIP : PRICE_NORMAL; }
    void     book(const SeatPos& p, int ticketId) { seats[p.r][p.c].ticketId = ticketId; }
    void     release(const SeatPos& p)            { seats[p.r][p.c].ticketId = 0; }

    void reset() {
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c) seats[r][c].ticketId = 0;
    }

    int countFree(SeatType t) const {
        int n = 0;
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c)
                if (isFree(r, c) && seats[r][c].type == t) ++n;
        return n;
    }

    void display(const vector<SeatPos>& mark = vector<SeatPos>()) const {
        cout << "\n";
        cout << string(14, ' ') << "=========== MAN HINH ===========\n\n";
        cout << "     ";
        for (int c = 0; c < COLS; ++c) {
            string s = to_string(c + 1);
            cout << (s.size() == 1 ? " " + s + "  " : s + "  ");
        }
        cout << "\n";
        for (int r = 0; r < ROWS; ++r) {
            cout << " " << (char)('A' + r) << "   ";
            for (int c = 0; c < COLS; ++c) {
                bool marked = false;
                for (size_t k = 0; k < mark.size(); ++k)
                    if (mark[k].r == r && mark[k].c == c) marked = true;

                if (!isFree(r, c))                  cout << "[X] ";
                else if (marked)                    cout << "[*] ";
                else if (seats[r][c].type == SEAT_VIP) cout << "[V] ";
                else                                cout << "[T] ";
            }
            cout << " " << (seats[r][0].type == SEAT_VIP ? "VIP" : "Thuong") << "\n";
        }
        cout << "\n  Chu giai: [T] Thuong trong (" << formatMoney(PRICE_NORMAL) << ")   "
             << "[V] VIP trong (" << formatMoney(PRICE_VIP) << ")\n"
             << "             [X] Da dat                [*] Dang goi y / dang chon\n"
             << "  Con trong: " << countFree(SEAT_NORMAL) << " ghe thuong, "
             << countFree(SEAT_VIP) << " ghe VIP\n";
    }

    vector<SeatPos> suggest(int n, SeatType t) const {
        vector<SeatPos> best;
        double bestScore = 1e18;
        const double midRow = (ROWS - 1) / 2.0, midCol = (COLS - 1) / 2.0;

        for (int r = 0; r < ROWS; ++r) {
            for (int start = 0; start + n <= COLS; ++start) {
                bool ok = true;
                for (int k = 0; k < n && ok; ++k)
                    if (!isFree(r, start + k) || seats[r][start + k].type != t) ok = false;
                if (!ok) continue;
                double score = fabs(r - midRow) * 100 + fabs(start + (n - 1) / 2.0 - midCol) * 10;
                if (score < bestScore) {
                    bestScore = score;
                    best.clear();
                    for (int k = 0; k < n; ++k) best.push_back(SeatPos{r, start + k});
                }
            }
        }
        if (!best.empty()) return best;

        vector<SeatPos> all;
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c)
                if (isFree(r, c) && seats[r][c].type == t) all.push_back(SeatPos{r, c});
        if ((int)all.size() < n) return best;
        sort(all.begin(), all.end(), [&](const SeatPos& a, const SeatPos& b) {
            double sa = fabs(a.r - midRow) * 100 + fabs(a.c - midCol);
            double sb = fabs(b.r - midRow) * 100 + fabs(b.c - midCol);
            return sa < sb;
        });
        all.resize(n);
        return all;
    }
};

static bool parseSeat(const string& tok, SeatPos& p) {
    if (tok.size() < 2) return false;
    char ch = (char)toupper((unsigned char)tok[0]);
    if (ch < 'A' || ch >= 'A' + ROWS) return false;
    char* end = nullptr;
    long v = strtol(tok.c_str() + 1, &end, 10);
    if (*end != '\0' || v < 1 || v > COLS) return false;
    p.r = ch - 'A';
    p.c = (int)v - 1;
    return true;
}

struct Ticket {
    int             id;
    int             customerId;
    string          name;
    string          phone;
    vector<SeatPos> seats;
    long            total;
    string          payMethod;
    string          time;
    bool            active;
    bool            refundPending;

    Ticket() : id(0), customerId(0), total(0), active(true), refundPending(false) {}
};

static string escapeField(const string& s) {
    string o;
    for (size_t i = 0; i < s.size(); ++i) {
        char ch = s[i];
        if      (ch == '\\') o += "\\\\";
        else if (ch == '\t') o += "\\t";
        else if (ch == '\n') o += "\\n";
        else if (ch == '\r') o += "\\r";
        else                 o += ch;
    }
    return o;
}

static string unescapeField(const string& s) {
    string o;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char n = s[++i];
            if      (n == 't')  o += '\t';
            else if (n == 'n')  o += '\n';
            else if (n == 'r')  o += '\r';
            else if (n == '\\') o += '\\';
            else { o += '\\'; o += n; }
        } else {
            o += s[i];
        }
    }
    return o;
}

static vector<string> splitBy(const string& s, char sep) {
    vector<string> out;
    string cur;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == sep) { out.push_back(cur); cur.clear(); }
        else             cur += s[i];
    }
    out.push_back(cur);
    return out;
}

static bool parseLongIn(const string& s, long lo, long hi, long& out) {
    if (s.empty()) return false;
    char* end = nullptr;
    long v = strtol(s.c_str(), &end, 10);
    if (*end != '\0' || v < lo || v > hi) return false;
    out = v;
    return true;
}

static bool readDbLine(istream& in, string& ln, int& lineNo) {
    if (!getline(in, ln)) return false;
    ++lineNo;
    if (!ln.empty() && ln[ln.size() - 1] == '\r') ln.erase(ln.size() - 1);
    return true;
}

struct Snapshot {
    vector<Ticket>   tickets;
    vector<Customer> queue;
    int              nextCustomerId;
    int              nextTicketId;
    int              grid[ROWS][COLS];

    Snapshot() : nextCustomerId(1), nextTicketId(1) {
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c) grid[r][c] = 0;
    }
};

static bool parseDatabase(istream& in, Snapshot& s, string& err) {
    int ln = 0;
    string text;
    vector<string> f;
    long v = 0;
    const long BIG = 1000000000L;
    auto fail = [&](const string& m) -> bool {
        err = "dong " + to_string(ln) + ": " + m;
        return false;
    };

    if (!readDbLine(in, text, ln)) return fail("file rong");
    f = splitBy(text, '\t');
    if (f.size() != 2 || f[0] != "DT19DB" || !parseLongIn(f[1], DB_VERSION, DB_VERSION, v))
        return fail("sai tieu de hoac phien ban file");

    if (!readDbLine(in, text, ln)) return fail("thieu dong NEXT");
    f = splitBy(text, '\t');
    long nc = 0, nt = 0;
    if (f.size() != 3 || f[0] != "NEXT" || !parseLongIn(f[1], 1, BIG, nc) || !parseLongIn(f[2], 1, BIG, nt))
        return fail("dong NEXT khong hop le");
    s.nextCustomerId = (int)nc;
    s.nextTicketId   = (int)nt;

    if (!readDbLine(in, text, ln)) return fail("thieu dong TICKETS");
    f = splitBy(text, '\t');
    long nT = 0;
    if (f.size() != 2 || f[0] != "TICKETS" || !parseLongIn(f[1], 0, 1000000, nT))
        return fail("dong TICKETS khong hop le");
    for (long i = 0; i < nT; ++i) {
        if (!readDbLine(in, text, ln)) return fail("thieu dong ve");
        f = splitBy(text, '\t');
        long id, cid, total, act, pend;
        if (f.size() != 11 || f[0] != "T" ||
            !parseLongIn(f[1], 1, BIG, id) || !parseLongIn(f[2], 0, BIG, cid) ||
            !parseLongIn(f[3], 0, BIG, total) ||
            !parseLongIn(f[4], 0, 1, act) || !parseLongIn(f[5], 0, 1, pend))
            return fail("dong ve sai dinh dang");

        Ticket t;
        t.id            = (int)id;
        t.customerId    = (int)cid;
        t.total         = total;
        t.active        = (act == 1);
        t.refundPending = (pend == 1);
        t.time          = unescapeField(f[6]);
        t.payMethod     = unescapeField(f[7]);
        t.name          = unescapeField(f[8]);
        t.phone         = unescapeField(f[9]);
        if (t.name.empty() || t.phone.empty()) return fail("ve thieu ten hoac SDT");
        if (!t.active && t.refundPending)      return fail("ve da hoan nhung con co 'cho hoan'");
        for (size_t k = 0; k < s.tickets.size(); ++k)
            if (s.tickets[k].id == t.id) return fail("trung ma ve #" + to_string(t.id));

        vector<string> codes = splitBy(f[10], ',');
        for (size_t k = 0; k < codes.size(); ++k) {
            SeatPos p;
            if (!parseSeat(codes[k], p)) return fail("ma ghe '" + codes[k] + "' khong hop le");
            for (size_t j = 0; j < t.seats.size(); ++j)
                if (t.seats[j].r == p.r && t.seats[j].c == p.c) return fail("ghe bi lap trong ve #" + to_string(t.id));
            t.seats.push_back(p);
        }
        s.tickets.push_back(t);
    }

    if (!readDbLine(in, text, ln)) return fail("thieu dong SEATS");
    f = splitBy(text, '\t');
    long rr = 0, cc = 0;
    if (f.size() != 3 || f[0] != "SEATS" || !parseLongIn(f[1], ROWS, ROWS, rr) || !parseLongIn(f[2], COLS, COLS, cc))
        return fail("kich thuoc so do ghe khong khop " + to_string(ROWS) + "x" + to_string(COLS));
    for (int r = 0; r < ROWS; ++r) {
        if (!readDbLine(in, text, ln)) return fail("thieu dong so do ghe");
        f = splitBy(text, '\t');
        if (f.size() != 2 || f[0] != "S") return fail("dong so do ghe sai dinh dang");
        istringstream iss(f[1]);
        for (int c = 0; c < COLS; ++c) {
            long x;
            if (!(iss >> x) || x < 0 || x > BIG) return fail("gia tri ghe khong hop le");
            s.grid[r][c] = (int)x;
        }
        string extra;
        if (iss >> extra) return fail("thua du lieu o dong so do ghe");
    }

    if (!readDbLine(in, text, ln)) return fail("thieu dong QUEUE");
    f = splitBy(text, '\t');
    long nQ = 0;
    if (f.size() != 2 || f[0] != "QUEUE" || !parseLongIn(f[1], 0, 1000000, nQ))
        return fail("dong QUEUE khong hop le");
    for (long i = 0; i < nQ; ++i) {
        if (!readDbLine(in, text, ln)) return fail("thieu dong khach");
        f = splitBy(text, '\t');
        long id, type, cnt, vip, rtk;
        if (f.size() != 8 || f[0] != "Q" ||
            !parseLongIn(f[1], 1, BIG, id) || !parseLongIn(f[2], CUS_NORMAL, CUS_REFUND, type) ||
            !parseLongIn(f[3], 1, MAX_SEATS_ORDER, cnt) || !parseLongIn(f[4], 0, 1, vip) ||
            !parseLongIn(f[5], 0, BIG, rtk))
            return fail("dong khach sai dinh dang");

        Customer c;
        c.id             = (int)id;
        c.type           = (CustomerType)type;
        c.seatCount      = (int)cnt;
        c.wantVip        = (vip == 1);
        c.refundTicketId = (int)rtk;
        c.name           = unescapeField(f[6]);
        c.phone          = unescapeField(f[7]);
        if (c.name.empty() || c.phone.empty()) return fail("khach thieu ten hoac SDT");
        if (c.type == CUS_REFUND && c.refundTicketId < 1) return fail("yeu cau hoan ve thieu ma ve");
        if (c.type != CUS_REFUND && c.refundTicketId != 0) return fail("khach thuong/uu tien co ma ve hoan");
        for (size_t k = 0; k < s.queue.size(); ++k)
            if (s.queue[k].id == c.id) return fail("trung ma khach #" + to_string(c.id));
        s.queue.push_back(c);
    }

    if (!readDbLine(in, text, ln) || text != "END") return fail("thieu dau END (file co the bi cat cut)");

    int expected[ROWS][COLS];
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c) expected[r][c] = 0;
    for (size_t i = 0; i < s.tickets.size(); ++i) {
        const Ticket& t = s.tickets[i];
        if (!t.active) continue;
        for (size_t k = 0; k < t.seats.size(); ++k) {
            int& cell = expected[t.seats[k].r][t.seats[k].c];
            if (cell != 0) { err = "hai ve dang hieu luc cung giu mot ghe"; return false; }
            cell = t.id;
        }
    }
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            if (s.grid[r][c] != expected[r][c]) {
                err = "so do ghe khong khop danh sach ve (ghe " + Cinema::seatName(SeatPos{r, c}) + ")";
                return false;
            }

    for (size_t i = 0; i < s.queue.size(); ++i) {
        if (s.queue[i].type != CUS_REFUND) continue;
        bool found = false;
        for (size_t k = 0; k < s.tickets.size(); ++k)
            if (s.tickets[k].id == s.queue[i].refundTicketId) found = true;
        if (!found) { err = "yeu cau hoan tro toi ve khong ton tai"; return false; }
    }
    for (size_t k = 0; k < s.tickets.size(); ++k) {
        int reqs = 0;
        for (size_t i = 0; i < s.queue.size(); ++i)
            if (s.queue[i].type == CUS_REFUND && s.queue[i].refundTicketId == s.tickets[k].id) ++reqs;
        if (s.tickets[k].refundPending ? reqs != 1 : reqs != 0) {
            err = "trang thai 'cho hoan' cua ve #" + to_string(s.tickets[k].id) + " khong khop hang cho";
            return false;
        }
    }

    int maxCus = 0, maxTk = 0;
    for (size_t k = 0; k < s.tickets.size(); ++k) {
        maxTk  = max(maxTk,  s.tickets[k].id);
        maxCus = max(maxCus, s.tickets[k].customerId);
    }
    for (size_t i = 0; i < s.queue.size(); ++i) maxCus = max(maxCus, s.queue[i].id);
    if (s.nextCustomerId <= maxCus) s.nextCustomerId = maxCus + 1;
    if (s.nextTicketId   <= maxTk)  s.nextTicketId   = maxTk + 1;
    return true;
}

enum LoadResult { LOAD_OK, LOAD_NOFILE, LOAD_BAD };

class BookingSystem {
    Cinema          cinema;
    CustomerDeque   queue;
    vector<Ticket>  tickets;
    int             nextCustomerId;
    int             nextTicketId;

public:
    BookingSystem() : nextCustomerId(1), nextTicketId(1) {}

    static string seatCodes(const vector<SeatPos>& v) {
        string s;
        for (size_t i = 0; i < v.size(); ++i) {
            if (i) s += ",";
            s += Cinema::seatName(v[i]);
        }
        return s;
    }

    bool writeDatabase(const string& path) const {
        ofstream f(path.c_str(), ios::out | ios::trunc);
        if (!f) return false;

        f << "DT19DB\t" << DB_VERSION << "\n";
        f << "NEXT\t" << nextCustomerId << "\t" << nextTicketId << "\n";

        f << "TICKETS\t" << tickets.size() << "\n";
        for (size_t i = 0; i < tickets.size(); ++i) {
            const Ticket& t = tickets[i];
            f << "T\t" << t.id << "\t" << t.customerId << "\t" << t.total << "\t"
              << (t.active ? 1 : 0) << "\t" << (t.refundPending ? 1 : 0) << "\t"
              << escapeField(t.time) << "\t" << escapeField(t.payMethod) << "\t"
              << escapeField(t.name) << "\t" << escapeField(t.phone) << "\t"
              << seatCodes(t.seats) << "\n";
        }

        f << "SEATS\t" << ROWS << "\t" << COLS << "\n";
        for (int r = 0; r < ROWS; ++r) {
            f << "S\t";
            for (int c = 0; c < COLS; ++c) f << (c ? " " : "") << cinema.ticketIdAt(r, c);
            f << "\n";
        }

        f << "QUEUE\t" << queue.size() << "\n";
        for (const CustomerDeque::Node* p = queue.headNode(); p != nullptr; p = p->next) {
            const Customer& c = p->data;
            f << "Q\t" << c.id << "\t" << (int)c.type << "\t" << c.seatCount << "\t"
              << (c.wantVip ? 1 : 0) << "\t" << c.refundTicketId << "\t"
              << escapeField(c.name) << "\t" << escapeField(c.phone) << "\n";
        }
        f << "END\n";
        f.flush();
        return f.good();
    }

    bool saveToFile() const {
        const string tmp = DB_FILE + ".tmp";
        if (!writeDatabase(tmp)) { std::remove(tmp.c_str()); return false; }
        std::remove(DB_FILE.c_str());
        return std::rename(tmp.c_str(), DB_FILE.c_str()) == 0;
    }

    void autoSave() const {
        if (saveToFile()) cout << "   [Da tu dong luu -> " << DB_FILE << "]\n";
        else              cout << "  ! CANH BAO: khong ghi duoc '" << DB_FILE << "' (du lieu moi chi con trong bo nho).\n";
    }

    void applySnapshot(const Snapshot& s) {
        queue.clear();
        tickets = s.tickets;
        cinema.reset();
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c)
                if (s.grid[r][c] != 0) cinema.book(SeatPos{r, c}, s.grid[r][c]);
        for (size_t i = 0; i < s.queue.size(); ++i) queue.push_back(s.queue[i]);
        nextCustomerId = s.nextCustomerId;
        nextTicketId   = s.nextTicketId;
    }

    LoadResult loadFromFile(string& err) {
        ifstream in(DB_FILE.c_str());
        if (!in) {
            ifstream tmp((DB_FILE + ".tmp").c_str());
            if (!tmp) return LOAD_NOFILE;
            Snapshot s;
            if (!parseDatabase(tmp, s, err)) return LOAD_BAD;
            applySnapshot(s);
            return LOAD_OK;
        }
        Snapshot s;
        if (!parseDatabase(in, s, err)) return LOAD_BAD;
        applySnapshot(s);
        return LOAD_OK;
    }

    void startupLoad() {
        string err;
        switch (loadFromFile(err)) {
            case LOAD_OK:
                cout << "\n[Du lieu] Da khoi phuc tu '" << DB_FILE << "': " << tickets.size()
                     << " ve, " << queue.size() << " khach trong hang cho.\n";
                break;
            case LOAD_NOFILE:
                cout << "\n[Du lieu] Chua co '" << DB_FILE << "' - bat dau he thong moi.\n";
                break;
            case LOAD_BAD: {
                const string bak = DB_FILE + ".corrupt";
                std::remove(bak.c_str());
                bool moved = (std::rename(DB_FILE.c_str(), bak.c_str()) == 0);
                cout << "\n[Du lieu] File '" << DB_FILE << "' bi loi (" << err << ").\n";
                if (moved) cout << "          Da giu ban sao tai '" << bak << "'. ";
                cout << "Bat dau he thong moi.\n";
                break;
            }
        }
    }

    void manualSave() const {
        cout << "\n--- LUU DU LIEU ---\n";
        if (saveToFile())
            cout << "=> Da luu " << tickets.size() << " ve, " << queue.size()
                 << " khach cho, so do ghe vao '" << DB_FILE << "'.\n";
        else
            cout << "  ! Khong ghi duoc file '" << DB_FILE << "'. Kiem tra quyen ghi / dung luong dia.\n";
    }

    void manualLoad() {
        cout << "\n--- TAI LAI DU LIEU ---\n"
             << "Thao tac nay THAY THE toan bo trang thai hien tai (hang cho, ve, so do ghe)\n"
             << "bang noi dung trong '" << DB_FILE << "'.\n";
        if (!readYesNo("Tiep tuc?")) { cout << "  => Da bo qua.\n"; return; }
        string err;
        switch (loadFromFile(err)) {
            case LOAD_OK:
                cout << "=> Da tai lai: " << tickets.size() << " ve, " << queue.size()
                     << " khach trong hang cho.\n";
                break;
            case LOAD_NOFILE:
                cout << "  ! Chua co file '" << DB_FILE << "'. Trang thai hien tai duoc giu nguyen.\n";
                break;
            case LOAD_BAD:
                cout << "  ! File loi (" << err << "). Trang thai hien tai duoc giu nguyen.\n";
                break;
        }
    }

    Ticket* findTicket(int id) {
        for (size_t i = 0; i < tickets.size(); ++i)
            if (tickets[i].id == id) return &tickets[i];
        return nullptr;
    }

    string seatListText(const vector<SeatPos>& v) const {
        string s;
        for (size_t i = 0; i < v.size(); ++i) {
            if (i) s += ", ";
            s += Cinema::seatName(v[i]);
        }
        return s;
    }

    void onCustomerDropped(const Customer& c) {
        if (c.type == CUS_REFUND) {
            Ticket* t = findTicket(c.refundTicketId);
            if (t) t->refundPending = false;
        }
    }

    void showQueue() const {
        cout << "\n";
        line();
        cout << " HANG CHO KHACH HANG  (so luong: " << queue.size() << ")\n";
        line();
        if (queue.empty()) {
            cout << " (Hang cho trong)\n";
            line();
            return;
        }
        cout << left << setw(5) << " STT" << setw(6) << "Ma" << setw(20) << "Ho ten"
             << setw(13) << "SDT" << setw(10) << "Loai" << "Chi tiet\n";
        line('-');
        int pos = 1;
        for (const CustomerDeque::Node* p = queue.headNode(); p != nullptr; p = p->next, ++pos) {
            const Customer& c = p->data;
            string detail;
            if (c.type == CUS_REFUND)
                detail = "Hoan ve #" + to_string(c.refundTicketId);
            else
                detail = to_string(c.seatCount) + " ghe " + (c.wantVip ? "VIP" : "thuong");
            string mark;
            if (p == queue.headNode()) mark += " <-- DAU HANG (HEAD)";
            if (p->next == nullptr)    mark += " <-- CUOI HANG (TAIL)";
            cout << " " << left << setw(4) << pos << setw(6) << c.id
                 << setw(20) << c.name.substr(0, 19) << setw(13) << c.phone.substr(0, 12)
                 << setw(10) << typeText(c.type) << detail << mark << "\n";
        }
        line();
    }

    void showSeatMap() const { cinema.display(); }

    void addCustomer() {
        cout << "\n--- THEM KHACH VAO HANG CHO ---\n";
        Customer c;
        c.id    = nextCustomerId;
        c.name  = readNonEmpty("Ho ten khach: ");
        c.phone = readNonEmpty("So dien thoai: ");
        cout << "Loai ve:\n  1. Ve thuong  (xep vao CUOI hang  - push_back)\n"
                "  2. Ve uu tien (chen len DAU hang - push_front)\n";
        c.type = (CustomerType)readInt("Chon loai (1-2): ", 1, 2);
        c.seatCount = readInt("So ghe muon dat (1-" + to_string(MAX_SEATS_ORDER) + "): ", 1, MAX_SEATS_ORDER);
        c.wantVip = (readInt("Loai ghe: 1. Thuong  2. VIP  -> chon: ", 1, 2) == 2);

        SeatType want = c.wantVip ? SEAT_VIP : SEAT_NORMAL;
        if (cinema.countFree(want) < c.seatCount)
            cout << "  ! Canh bao: hien chi con " << cinema.countFree(want) << " ghe "
                 << (c.wantVip ? "VIP" : "thuong") << " trong, co the khong du cho khach nay.\n";

        ++nextCustomerId;
        if (c.type == CUS_PRIORITY) queue.push_front(c);
        else                        queue.push_back(c);

        cout << "=> Da them khach #" << c.id << " (" << c.name << ") vao "
             << (c.type == CUS_PRIORITY ? "DAU" : "CUOI") << " hang cho.\n";
        autoSave();
    }

    vector<SeatPos> selectSeatsManually(int n) {
        while (true) {
            string s = readLine("Nhap " + to_string(n) + " ma ghe cach nhau dau cach (vd: C4 C5), '0' de quay lai: ");
            if (s == "0") return vector<SeatPos>();
            istringstream iss(s);
            string tok;
            vector<SeatPos> v;
            string err;
            while (iss >> tok) {
                SeatPos p;
                if (!parseSeat(tok, p))                   { err = "Ma ghe '" + tok + "' khong hop le."; break; }
                if (!cinema.isFree(p.r, p.c))             { err = "Ghe " + tok + " da co nguoi dat."; break; }
                bool dup = false;
                for (size_t i = 0; i < v.size(); ++i) if (v[i].r == p.r && v[i].c == p.c) dup = true;
                if (dup)                                  { err = "Ghe " + tok + " bi nhap trung."; break; }
                v.push_back(p);
            }
            if (err.empty() && (int)v.size() != n)
                err = "Can chon dung " + to_string(n) + " ghe (ban nhap " + to_string(v.size()) + ").";
            if (err.empty()) return v;
            cout << "  ! " << err << "\n";
        }
    }

    void handleBooking(const Customer& c) {
        SeatType want = c.wantVip ? SEAT_VIP : SEAT_NORMAL;
        while (true) {
            vector<SeatPos> sugg = cinema.suggest(c.seatCount, want);
            cinema.display(sugg);

            cout << "\nKhach #" << c.id << " - " << c.name << " muon dat " << c.seatCount
                 << " ghe " << (c.wantVip ? "VIP" : "thuong") << ".\n";
            if (sugg.empty())
                cout << "Goi y: KHONG con du ghe loai nay de goi y.\n";
            else
                cout << "Goi y ghe: " << seatListText(sugg) << " (danh dau [*] tren so do)\n";
            cout << "  1. Chap nhan goi y\n  2. Tu chon ghe\n  0. Bo qua khach nay (khach roi di)\n";

            int ch = readInt("Chon (0-2): ", 0, 2);
            if (ch == 0) {
                cout << "=> Khach #" << c.id << " roi di, khong dat ve.\n";
                return;
            }
            vector<SeatPos> chosen;
            if (ch == 1) {
                if (sugg.empty()) { cout << "  ! Khong co goi y de chap nhan.\n"; continue; }
                chosen = sugg;
            } else {
                chosen = selectSeatsManually(c.seatCount);
                if (chosen.empty()) continue;
            }

            long total = 0;
            cout << "\n----- CHI TIET TIEN VE -----\n";
            for (size_t i = 0; i < chosen.size(); ++i) {
                long pr = cinema.priceAt(chosen[i].r, chosen[i].c);
                total += pr;
                cout << "  Ghe " << setw(4) << left << Cinema::seatName(chosen[i])
                     << (cinema.typeAt(chosen[i].r, chosen[i].c) == SEAT_VIP ? "VIP    " : "Thuong ")
                     << right << setw(14) << formatMoney(pr) << "\n";
            }
            cout << left << "  TONG CONG:" << right << setw(24) << formatMoney(total) << "\n";

            cout << "Phuong thuc thanh toan: 1. Tien mat  2. The/Chuyen khoan  0. Quay lai chon ghe\n";
            int pm = readInt("Chon (0-2): ", 0, 2);
            if (pm == 0) continue;
            string payText;
            if (pm == 1) {
                long given = readLong("Khach dua (VND, >= " + to_string(total) + "): ", total, 1000000000L);
                payText = "Tien mat (khach dua " + formatMoney(given) + ", thoi lai " + formatMoney(given - total) + ")";
                cout << "  Tien thoi lai: " << formatMoney(given - total) << "\n";
            } else {
                payText = "The/Chuyen khoan";
            }
            if (!readYesNo("Xac nhan thanh toan " + formatMoney(total) + "?")) {
                cout << "  => Da huy thanh toan, quay lai buoc chon ghe.\n";
                continue;
            }

            Ticket t;
            t.id         = nextTicketId++;
            t.customerId = c.id;
            t.name       = c.name;
            t.phone      = c.phone;
            t.seats      = chosen;
            t.total      = total;
            t.payMethod  = payText;
            t.time       = nowString();
            for (size_t i = 0; i < chosen.size(); ++i) cinema.book(chosen[i], t.id);
            tickets.push_back(t);

            cout << "\n=> DAT VE THANH CONG! Ma ve: #" << t.id << " - Ghe: " << seatListText(chosen) << "\n";
            exportTicket(t);
            return;
        }
    }

    void handleRefund(const Customer& c) {
        Ticket* t = findTicket(c.refundTicketId);
        if (t == nullptr || !t->active) {
            cout << "  ! Ve #" << c.refundTicketId << " khong ton tai hoac da duoc huy truoc do.\n";
            return;
        }
        long refund = t->total * REFUND_PERCENT / 100;
        for (size_t i = 0; i < t->seats.size(); ++i) cinema.release(t->seats[i]);
        t->active = false;
        t->refundPending = false;

        cout << "\n=> HOAN VE THANH CONG cho ve #" << t->id << " (" << t->name << ")\n"
             << "   Ghe tra lai: " << seatListText(t->seats) << "\n"
             << "   Gia tri ve : " << formatMoney(t->total) << "\n"
             << "   Phi huy " << (100 - REFUND_PERCENT) << "%: " << formatMoney(t->total - refund) << "\n"
             << "   TIEN HOAN  : " << formatMoney(refund) << "\n";
        exportRefund(*t, refund);
    }

    bool processNext() {
        Customer c;
        if (!queue.pop_front(c)) {
            cout << "\n  ! Hang cho dang trong.\n";
            return false;
        }
        cout << "\n";
        line();
        cout << " DANG PHUC VU: Khach #" << c.id << " - " << c.name << " - SDT " << c.phone
             << " - [" << typeText(c.type) << "]\n";
        line();
        if (c.type == CUS_REFUND) handleRefund(c);
        else                      handleBooking(c);
        autoSave();
        return true;
    }

    void processAll() {
        if (queue.empty()) { cout << "\n  ! Hang cho dang trong.\n"; return; }
        while (!queue.empty()) {
            processNext();
            if (queue.empty()) break;
            if (!readYesNo("\nTiep tuc phuc vu khach ke tiep (con " + to_string(queue.size()) + " khach)?")) break;
        }
        cout << "\n=> Ket thuc luot xu ly lien tuc.\n";
    }

    void dropLast() {
        Customer c;
        if (!queue.pop_back(c)) { cout << "\n  ! Hang cho dang trong.\n"; return; }
        onCustomerDropped(c);
        cout << "\n=> Khach cuoi hang #" << c.id << " (" << c.name << ", " << typeText(c.type)
             << ") da roi khoi hang cho.\n";
        autoSave();
    }

    void removeCustomer() {
        if (queue.empty()) { cout << "\n  ! Hang cho dang trong.\n"; return; }
        showQueue();
        int id = readInt("Nhap ma khach can xoa khoi hang cho (0 = quay lai): ", 0, 1000000);
        if (id == 0) return;
        Customer c;
        if (queue.removeById(id, c)) {
            onCustomerDropped(c);
            cout << "=> Da xoa khach #" << c.id << " (" << c.name << ") khoi hang cho.\n";
            autoSave();
        } else {
            cout << "  ! Khong tim thay khach co ma #" << id << " trong hang cho.\n";
        }
    }

    void suggestMenu() {
        cout << "\n--- GOI Y CHO NGOI TRONG ---\n";
        int n = readInt("So ghe can tim (1-" + to_string(MAX_SEATS_ORDER) + "): ", 1, MAX_SEATS_ORDER);
        SeatType t = (readInt("Loai ghe: 1. Thuong  2. VIP -> chon: ", 1, 2) == 2) ? SEAT_VIP : SEAT_NORMAL;
        vector<SeatPos> s = cinema.suggest(n, t);
        cinema.display(s);
        if (s.empty())
            cout << "\n=> Khong du " << n << " ghe " << (t == SEAT_VIP ? "VIP" : "thuong") << " trong.\n";
        else
            cout << "\n=> Goi y: " << seatListText(s) << "\n";
    }

    void requestCancel() {
        cout << "\n--- HUY VE ---\n";
        if (tickets.empty()) { cout << "  ! Chua co ve nao duoc ban.\n"; return; }
        int id = readInt("Nhap ma ve can huy (0 = quay lai): ", 0, 1000000);
        if (id == 0) return;
        Ticket* t = findTicket(id);
        if (t == nullptr)        { cout << "  ! Khong tim thay ve #" << id << ".\n"; return; }
        if (!t->active)          { cout << "  ! Ve #" << id << " da duoc huy/hoan truoc do.\n"; return; }
        if (t->refundPending)    { cout << "  ! Ve #" << id << " da co yeu cau hoan dang cho xu ly.\n"; return; }

        cout << "Ve #" << t->id << " - " << t->name << " - Ghe: " << seatListText(t->seats)
             << " - " << formatMoney(t->total) << "\n"
             << "Se hoan " << REFUND_PERCENT << "% = " << formatMoney(t->total * REFUND_PERCENT / 100) << "\n";
        if (!readYesNo("Xac nhan huy ve?")) { cout << "  => Da bo qua.\n"; return; }

        Customer r;
        r.id             = nextCustomerId++;
        r.name           = t->name;
        r.phone          = t->phone;
        r.type           = CUS_REFUND;
        r.refundTicketId = t->id;
        t->refundPending = true;
        queue.push_front(r);

        cout << "=> Da day yeu cau hoan ve #" << t->id << " len DAU hang cho (luot #" << r.id
             << "). Hay chon 'Xu ly khach tiep theo' de hoan tat.\n";
        autoSave();
    }

    void listTickets() const {
        cout << "\n";
        line();
        cout << " DANH SACH VE\n";
        line();
        if (tickets.empty()) { cout << " (Chua co ve nao)\n"; line(); return; }
        cout << left << setw(6) << " Ma" << setw(20) << "Khach hang" << setw(22) << "Ghe"
             << setw(16) << "Tong tien" << "Trang thai\n";
        line('-');
        for (size_t i = 0; i < tickets.size(); ++i) {
            const Ticket& t = tickets[i];
            string st = !t.active ? "Da hoan" : (t.refundPending ? "Cho hoan" : "Hieu luc");
            cout << " " << left << setw(5) << t.id << setw(20) << t.name.substr(0, 19)
                 << setw(22) << seatListText(t.seats).substr(0, 21)
                 << setw(16) << formatMoney(t.total) << st << "\n";
        }
        line();
    }

    bool exportTicket(const Ticket& t) const {
        string fn = "ve_" + to_string(t.id) + ".txt";
        ofstream f(fn.c_str());
        if (!f) { cout << "  ! Khong the tao file " << fn << "\n"; return false; }
        f << "==================================================\n"
          << "                VE XEM PHIM / HOA DON\n"
          << "==================================================\n"
          << "Ma ve      : #" << t.id << "\n"
          << "Phim       : " << MOVIE_NAME << "\n"
          << "Suat chieu : " << SHOW_TIME << " - " << ROOM_NAME << "\n"
          << "Khach hang : " << t.name << "\n"
          << "So dien thoai: " << t.phone << "\n"
          << "Ngay lap   : " << t.time << "\n"
          << "--------------------------------------------------\n"
          << "Danh sach ghe:\n";
        for (size_t i = 0; i < t.seats.size(); ++i) {
            const SeatPos& p = t.seats[i];
            f << "  - Ghe " << Cinema::seatName(p) << "  "
              << (cinema.typeAt(p.r, p.c) == SEAT_VIP ? "VIP    " : "Thuong ")
              << formatMoney(cinema.priceAt(p.r, p.c)) << "\n";
        }
        f << "--------------------------------------------------\n"
          << "TONG TIEN  : " << formatMoney(t.total) << "\n"
          << "Thanh toan : " << t.payMethod << "\n"
          << "Trang thai : " << (t.active ? "Hieu luc" : "Da hoan/huy") << "\n"
          << "==================================================\n"
          << "        Cam on quy khach - Chuc xem phim vui ve!\n";
        f.close();
        cout << "   [File ve da xuat: " << fn << "]\n";
        return true;
    }

    bool exportRefund(const Ticket& t, long refund) const {
        string fn = "hoanve_" + to_string(t.id) + ".txt";
        ofstream f(fn.c_str());
        if (!f) { cout << "  ! Khong the tao file " << fn << "\n"; return false; }
        f << "==================================================\n"
          << "              BIEN LAI HOAN/ HUY VE\n"
          << "Ma ve      : #" << t.id << "\n"
          << "Khach hang : " << t.name << " - " << t.phone << "\n"
          << "Ghe da tra : " << seatListText(t.seats) << "\n"
          << "Gia tri ve : " << formatMoney(t.total) << "\n"
          << "Phi huy " << (100 - REFUND_PERCENT) << "% : " << formatMoney(t.total - refund) << "\n"
          << "TIEN HOAN  : " << formatMoney(refund) << "\n"
          << "Ngay lap   : " << nowString() << "\n"
          << "==================================================\n";
        f.close();
        cout << "   [Bien lai hoan ve da xuat: " << fn << "]\n";
        return true;
    }

    void reprintTicket() const {
        cout << "\n--- XUAT LAI FILE VE ---\n";
        if (tickets.empty()) { cout << "  ! Chua co ve nao.\n"; return; }
        int id = readInt("Nhap ma ve (0 = quay lai): ", 0, 1000000);
        if (id == 0) return;
        for (size_t i = 0; i < tickets.size(); ++i)
            if (tickets[i].id == id) { exportTicket(tickets[i]); return; }
        cout << "  ! Khong tim thay ve #" << id << ".\n";
    }

    void printMenu() const {
        cout << "\n";
        line('=');
        cout << "   HE THONG DAT VE XEM PHIM & HANG CHO KHACH HANG\n"
             << "   " << MOVIE_NAME << " | " << SHOW_TIME << " | " << ROOM_NAME
             << " | Hang cho: " << queue.size() << " khach\n";
        line('=');
        cout << "  [HANG CHO - DEQUE]\n"
                "   1. Hien thi hang cho\n"
                "   2. Them khach vao hang cho (thuong: cuoi hang / uu tien: dau hang)\n"
                "   3. Xu ly khach tiep theo (pop_front)\n"
                "   4. Xu ly lien tuc toan bo hang cho\n"
                "   5. Khach cuoi hang roi di (pop_back)\n"
                "   6. Xoa khach bat ky khoi hang cho theo ma\n"
                "  [PHONG VE]\n"
                "   7. Xem so do ghe\n"
                "   8. Goi y cho ngoi trong\n"
                "   9. Huy ve (day yeu cau hoan ve len DAU hang cho)\n"
                "  10. Danh sach ve\n"
                "  11. Xuat lai file ve (.txt)\n"
                "  [DU LIEU - tu dong luu sau moi thay doi]\n"
                "  12. Luu du lieu ngay vao " << DB_FILE << "\n"
                "  13. Tai lai du lieu tu " << DB_FILE << "\n"
                "   0. Thoat\n";
        line('-');
    }

    void run() {
        startupLoad();
        while (true) {
            printMenu();
            int ch = readInt("Chon chuc nang (0-13): ", 0, 13);
            switch (ch) {
                case 1:  showQueue();       break;
                case 2:  addCustomer();     break;
                case 3:  processNext();     break;
                case 4:  processAll();      break;
                case 5:  dropLast();        break;
                case 6:  removeCustomer();  break;
                case 7:  showSeatMap();     break;
                case 8:  suggestMenu();     break;
                case 9:  requestCancel();   break;
                case 10: listTickets();     break;
                case 11: reprintTicket();   break;
                case 12: manualSave();      break;
                case 13: manualLoad();      break;
                case 0: {
                    if (saveToFile()) cout << "\nDa luu du lieu vao '" << DB_FILE << "'.";
                    else              cout << "\n  ! Khong ghi duoc '" << DB_FILE << "'.";
                    size_t freed = queue.clear();
                    cout << "\nDa giai phong " << freed << " nut con lai trong hang cho. Tam biet!\n";
                    return;
                }
            }
        }
    }
};

int main() {
    try {
        BookingSystem sys;
        sys.run();
    } catch (const InputClosed&) {
        cout << "\n\nLuong nhap da dong. Chuong trinh ket thuc (bo nho da duoc giai phong).\n";
    }
    return 0;
}
