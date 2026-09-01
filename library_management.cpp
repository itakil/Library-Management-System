#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <ctime>
#include <map>

using namespace std;

// ─────────────────────────────────────────────
//  Utility
// ─────────────────────────────────────────────
string currentDate() {
    time_t now = time(nullptr);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
    return string(buf);
}

string toLowerCase(const string& s) {
    string r = s;
    transform(r.begin(), r.end(), r.begin(), ::tolower);
    return r;
}

// ─────────────────────────────────────────────
//  Class: Book
// ─────────────────────────────────────────────
class Book {
private:
    int    bookId;
    string title;
    string author;
    string genre;
    int    totalCopies;
    int    availableCopies;

public:
    Book() : bookId(0), totalCopies(0), availableCopies(0) {}

    Book(int id, const string& t, const string& a,
         const string& g, int copies)
        : bookId(id), title(t), author(a), genre(g),
          totalCopies(copies), availableCopies(copies) {}

    // Getters
    int    getBookId()          const { return bookId;          }
    string getTitle()           const { return title;           }
    string getAuthor()          const { return author;          }
    string getGenre()           const { return genre;           }
    int    getTotalCopies()     const { return totalCopies;     }
    int    getAvailableCopies() const { return availableCopies; }
    bool   isAvailable()        const { return availableCopies > 0; }

    // Issue / Return
    bool issueBook() {
        if (availableCopies <= 0) return false;
        availableCopies--;
        return true;
    }
    bool returnBook() {
        if (availableCopies >= totalCopies) return false;
        availableCopies++;
        return true;
    }

    // Serialization  (fields separated by '|')
    string serialize() const {
        ostringstream oss;
        oss << bookId << "|" << title << "|" << author << "|"
            << genre << "|" << totalCopies << "|" << availableCopies;
        return oss.str();
    }
    static Book deserialize(const string& line) {
        Book b;
        istringstream iss(line);
        string tok;
        getline(iss, tok, '|'); b.bookId          = stoi(tok);
        getline(iss, tok, '|'); b.title           = tok;
        getline(iss, tok, '|'); b.author          = tok;
        getline(iss, tok, '|'); b.genre           = tok;
        getline(iss, tok, '|'); b.totalCopies     = stoi(tok);
        getline(iss, tok, '|'); b.availableCopies = stoi(tok);
        return b;
    }
};

// ─────────────────────────────────────────────
//  Class: Member
// ─────────────────────────────────────────────
class Member {
private:
    int    memberId;
    string name;
    string email;
    string phone;
    string joinDate;

public:
    Member() : memberId(0) {}
    Member(int id, const string& n, const string& e, const string& p)
        : memberId(id), name(n), email(e), phone(p), joinDate(currentDate()) {}

    int    getMemberId() const { return memberId; }
    string getName()     const { return name;     }
    string getEmail()    const { return email;    }
    string getPhone()    const { return phone;    }
    string getJoinDate() const { return joinDate; }

    string serialize() const {
        return to_string(memberId) + "|" + name + "|" + email + "|" + phone + "|" + joinDate;
    }
    static Member deserialize(const string& line) {
        Member m;
        istringstream iss(line);
        string tok;
        getline(iss, tok, '|'); m.memberId = stoi(tok);
        getline(iss, tok, '|'); m.name     = tok;
        getline(iss, tok, '|'); m.email    = tok;
        getline(iss, tok, '|'); m.phone    = tok;
        getline(iss, tok, '|'); m.joinDate = tok;
        return m;
    }
};

// ─────────────────────────────────────────────
//  Class: BorrowRecord
// ─────────────────────────────────────────────
class BorrowRecord {
public:
    int    recordId;
    int    memberId;
    int    bookId;
    string issueDate;
    string returnDate;   // empty = not yet returned
    bool   returned;

    BorrowRecord() : recordId(0), memberId(0), bookId(0), returned(false) {}
    BorrowRecord(int rid, int mid, int bid)
        : recordId(rid), memberId(mid), bookId(bid),
          issueDate(currentDate()), returnDate(""), returned(false) {}

    string serialize() const {
        ostringstream oss;
        oss << recordId << "|" << memberId << "|" << bookId << "|"
            << issueDate << "|" << (returnDate.empty() ? "-" : returnDate)
            << "|" << (returned ? "1" : "0");
        return oss.str();
    }
    static BorrowRecord deserialize(const string& line) {
        BorrowRecord r;
        istringstream iss(line);
        string tok;
        getline(iss, tok, '|'); r.recordId   = stoi(tok);
        getline(iss, tok, '|'); r.memberId   = stoi(tok);
        getline(iss, tok, '|'); r.bookId     = stoi(tok);
        getline(iss, tok, '|'); r.issueDate  = tok;
        getline(iss, tok, '|'); r.returnDate = (tok == "-") ? "" : tok;
        getline(iss, tok, '|'); r.returned   = (tok == "1");
        return r;
    }
};

// ─────────────────────────────────────────────
//  Class: Library
// ─────────────────────────────────────────────
class Library {
private:
    vector<Book>         books;
    vector<Member>       members;
    vector<BorrowRecord> records;

    const string BOOKS_FILE   = "library_books.dat";
    const string MEMBERS_FILE = "library_members.dat";
    const string RECORDS_FILE = "library_records.dat";

    int lastBookId   = 100;
    int lastMemberId = 200;
    int lastRecordId = 300;

    // ── File I/O ─────────────────────────────
    void loadBooks() {
        ifstream fin(BOOKS_FILE);
        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            try {
                Book b = Book::deserialize(line);
                lastBookId = max(lastBookId, b.getBookId());
                books.push_back(b);
            } catch (...) {}
        }
    }
    void saveBooks() {
        ofstream fout(BOOKS_FILE, ios::trunc);
        for (const auto& b : books) fout << b.serialize() << "\n";
    }
    void loadMembers() {
        ifstream fin(MEMBERS_FILE);
        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            try {
                Member m = Member::deserialize(line);
                lastMemberId = max(lastMemberId, m.getMemberId());
                members.push_back(m);
            } catch (...) {}
        }
    }
    void saveMembers() {
        ofstream fout(MEMBERS_FILE, ios::trunc);
        for (const auto& m : members) fout << m.serialize() << "\n";
    }
    void loadRecords() {
        ifstream fin(RECORDS_FILE);
        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            try {
                BorrowRecord r = BorrowRecord::deserialize(line);
                lastRecordId = max(lastRecordId, r.recordId);
                records.push_back(r);
            } catch (...) {}
        }
    }
    void saveRecords() {
        ofstream fout(RECORDS_FILE, ios::trunc);
        for (const auto& r : records) fout << r.serialize() << "\n";
    }

    // ── Finders ──────────────────────────────
    Book*   findBook(int id) {
        for (auto& b : books) if (b.getBookId() == id) return &b;
        return nullptr;
    }
    Member* findMember(int id) {
        for (auto& m : members) if (m.getMemberId() == id) return &m;
        return nullptr;
    }

    // ── UI helpers ───────────────────────────
    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
    void printHeader(const string& title) {
        cout << "\n"
             << "╔══════════════════════════════════════════════════════╗\n"
             << "║          LIBRARY MANAGEMENT SYSTEM  v1.0             ║\n"
             << "╠══════════════════════════════════════════════════════╣\n"
             << "║  " << left << setw(52) << title << "║\n"
             << "╚══════════════════════════════════════════════════════╝\n";
    }
    void pause() {
        cout << "\n  Press Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
    int getInt(const string& prompt) {
        int val;
        while (true) {
            cout << prompt;
            if (cin >> val) { cin.ignore(); return val; }
            cout << "  ✗ Invalid. Enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    string getString(const string& prompt) {
        string val;
        cout << prompt;
        getline(cin, val);
        return val;
    }

    void printBookRow(const Book& b) {
        cout << "  " << left
             << setw(6)  << b.getBookId()
             << setw(30) << b.getTitle().substr(0, 28)
             << setw(20) << b.getAuthor().substr(0, 18)
             << setw(14) << b.getGenre()
             << setw(7)  << b.getAvailableCopies()
             << b.getTotalCopies() << "\n";
    }
    void printBookHeader() {
        cout << "\n  " << left
             << setw(6)  << "ID"
             << setw(30) << "Title"
             << setw(20) << "Author"
             << setw(14) << "Genre"
             << setw(7)  << "Avail"
             << "Total\n";
        cout << "  " << string(77, '-') << "\n";
    }

public:
    Library() { loadBooks(); loadMembers(); loadRecords(); }
    ~Library() { saveBooks(); saveMembers(); saveRecords(); }

    // ══════════════════════════════════════════
    //  BOOK OPERATIONS
    // ══════════════════════════════════════════
    void addBook() {
        clearScreen();
        printHeader("ADD NEW BOOK");

        string title  = getString("  Title         : ");
        string author = getString("  Author        : ");
        string genre  = getString("  Genre         : ");
        int copies    = getInt   ("  No. of Copies : ");
        if (copies < 1) copies = 1;

        int id = ++lastBookId;
        books.emplace_back(id, title, author, genre, copies);
        saveBooks();

        cout << "\n  ✓ Book added! (ID = " << id << ")\n";
        pause();
    }

    void displayAllBooks() {
        clearScreen();
        printHeader("ALL BOOKS");
        if (books.empty()) { cout << "\n  No books in library.\n"; pause(); return; }

        printBookHeader();
        for (const auto& b : books) printBookRow(b);
        cout << "\n  Total: " << books.size() << " book(s).\n";
        pause();
    }

    void searchBooks() {
        clearScreen();
        printHeader("SEARCH BOOKS");

        cout << "  Search by:\n  1. Title\n  2. Author\n  3. Book ID\n";
        int choice = getInt("  Choose (1/2/3): ");

        vector<Book*> results;

        if (choice == 3) {
            int id = getInt("  Enter Book ID : ");
            Book* b = findBook(id);
            if (b) results.push_back(b);
        } else {
            string query = getString(choice == 1 ? "  Enter Title  : " : "  Enter Author : ");
            string lq = toLowerCase(query);
            for (auto& b : books) {
                string field = toLowerCase(choice == 1 ? b.getTitle() : b.getAuthor());
                if (field.find(lq) != string::npos)
                    results.push_back(&b);
            }
        }

        if (results.empty()) {
            cout << "\n  ✗ No matching books found.\n";
        } else {
            printBookHeader();
            for (auto* b : results) printBookRow(*b);
            cout << "\n  Found: " << results.size() << " result(s).\n";
        }
        pause();
    }

    void updateBook() {
        clearScreen();
        printHeader("UPDATE BOOK");

        int id = getInt("  Enter Book ID : ");
        Book* b = findBook(id);
        if (!b) { cout << "  ✗ Book not found.\n"; pause(); return; }

        printBookHeader();
        printBookRow(*b);
        cout << "\n  (Leave blank to keep current value)\n";

        // We rebuild the book since fields are private — use temp values
        string title  = getString("  New Title  [" + b->getTitle()  + "]: ");
        string author = getString("  New Author [" + b->getAuthor() + "]: ");
        string genre  = getString("  New Genre  [" + b->getGenre()  + "]: ");

        int copies = b->getTotalCopies();
        cout << "  New Copies [" << copies << "]: ";
        string copStr;
        getline(cin, copStr);
        if (!copStr.empty()) copies = stoi(copStr);

        // Replace entry
        int issued = b->getTotalCopies() - b->getAvailableCopies();
        *b = Book(id,
                  title.empty()  ? b->getTitle()  : title,
                  author.empty() ? b->getAuthor() : author,
                  genre.empty()  ? b->getGenre()  : genre,
                  copies);
        // Restore issued count
        for (int i = 0; i < issued; ++i) b->issueBook();

        saveBooks();
        cout << "\n  ✓ Book updated.\n";
        pause();
    }

    void deleteBook() {
        clearScreen();
        printHeader("DELETE BOOK");

        int id = getInt("  Enter Book ID : ");
        auto it = find_if(books.begin(), books.end(),
                          [id](const Book& b){ return b.getBookId() == id; });
        if (it == books.end()) { cout << "  ✗ Book not found.\n"; pause(); return; }

        // Check if any copies are issued
        if (it->getAvailableCopies() < it->getTotalCopies()) {
            cout << "  ✗ Cannot delete — some copies are currently issued.\n";
            pause(); return;
        }

        printBookHeader();
        printBookRow(*it);
        string confirm = getString("\n  Type 'DELETE' to confirm: ");
        if (confirm == "DELETE") {
            books.erase(it);
            saveBooks();
            cout << "  ✓ Book deleted.\n";
        } else {
            cout << "  – Cancelled.\n";
        }
        pause();
    }

    // ══════════════════════════════════════════
    //  MEMBER OPERATIONS
    // ══════════════════════════════════════════
    void addMember() {
        clearScreen();
        printHeader("ADD NEW MEMBER");

        string name  = getString("  Name   : ");
        string email = getString("  Email  : ");
        string phone = getString("  Phone  : ");

        int id = ++lastMemberId;
        members.emplace_back(id, name, email, phone);
        saveMembers();

        cout << "\n  ✓ Member added! (Member ID = " << id << ")\n";
        pause();
    }

    void displayAllMembers() {
        clearScreen();
        printHeader("ALL MEMBERS");
        if (members.empty()) { cout << "\n  No members registered.\n"; pause(); return; }

        cout << "\n  " << left
             << setw(8)  << "ID"
             << setw(22) << "Name"
             << setw(25) << "Email"
             << setw(14) << "Phone"
             << "Joined\n";
        cout << "  " << string(80, '-') << "\n";
        for (const auto& m : members) {
            cout << "  " << left
                 << setw(8)  << m.getMemberId()
                 << setw(22) << m.getName().substr(0, 20)
                 << setw(25) << m.getEmail().substr(0, 23)
                 << setw(14) << m.getPhone()
                 << m.getJoinDate() << "\n";
        }
        cout << "\n  Total: " << members.size() << " member(s).\n";
        pause();
    }

    // ══════════════════════════════════════════
    //  BORROW / RETURN
    // ══════════════════════════════════════════
    void issueBook() {
        clearScreen();
        printHeader("ISSUE BOOK");

        int mid = getInt("  Member ID : ");
        Member* m = findMember(mid);
        if (!m) { cout << "  ✗ Member not found.\n"; pause(); return; }

        // Check how many books this member has active
        int active = 0;
        for (const auto& r : records)
            if (r.memberId == mid && !r.returned) active++;
        if (active >= 3) {
            cout << "  ✗ Member already has 3 books issued (maximum limit).\n";
            pause(); return;
        }

        int bid = getInt("  Book ID   : ");
        Book* b = findBook(bid);
        if (!b) { cout << "  ✗ Book not found.\n"; pause(); return; }
        if (!b->isAvailable()) {
            cout << "  ✗ No copies available at the moment.\n";
            pause(); return;
        }

        // Check member doesn't already have this book
        for (const auto& r : records)
            if (r.memberId == mid && r.bookId == bid && !r.returned) {
                cout << "  ✗ Member already has this book.\n";
                pause(); return;
            }

        b->issueBook();
        records.emplace_back(++lastRecordId, mid, bid);
        saveBooks();
        saveRecords();

        cout << "\n  ✓ \"" << b->getTitle() << "\" issued to " << m->getName() << "\n";
        cout << "  Issue Date  : " << currentDate() << "\n";
        cout << "  Return by   : (within 14 days)\n";
        pause();
    }

    void returnBook() {
        clearScreen();
        printHeader("RETURN BOOK");

        int mid = getInt("  Member ID : ");
        Member* m = findMember(mid);
        if (!m) { cout << "  ✗ Member not found.\n"; pause(); return; }

        // Show active issues for this member
        cout << "\n  Active issues for " << m->getName() << ":\n";
        bool any = false;
        for (const auto& r : records) {
            if (r.memberId == mid && !r.returned) {
                Book* b = findBook(r.bookId);
                cout << "  Record #" << r.recordId
                     << " → Book ID " << r.bookId
                     << " [" << (b ? b->getTitle() : "Unknown") << "]"
                     << "  Issued: " << r.issueDate << "\n";
                any = true;
            }
        }
        if (!any) { cout << "  No books currently issued.\n"; pause(); return; }

        int rid = getInt("\n  Enter Record # to return: ");
        BorrowRecord* rec = nullptr;
        for (auto& r : records)
            if (r.recordId == rid && r.memberId == mid && !r.returned) { rec = &r; break; }

        if (!rec) { cout << "  ✗ Record not found or already returned.\n"; pause(); return; }

        Book* b = findBook(rec->bookId);
        if (b) b->returnBook();
        rec->returned   = true;
        rec->returnDate = currentDate();

        saveBooks();
        saveRecords();

        cout << "\n  ✓ Book returned successfully!\n";
        cout << "  Book    : " << (b ? b->getTitle() : "Unknown") << "\n";
        cout << "  Returned: " << currentDate() << "\n";
        pause();
    }

    // ══════════════════════════════════════════
    //  BORROW HISTORY
    // ══════════════════════════════════════════
    void viewBorrowRecords() {
        clearScreen();
        printHeader("BORROW RECORDS");

        cout << "  Filter:\n  1. All Records\n  2. Active (not returned)\n  3. By Member ID\n";
        int choice = getInt("  Choose: ");

        int filterMember = -1;
        bool activeOnly  = false;
        if (choice == 2) activeOnly = true;
        if (choice == 3) filterMember = getInt("  Member ID: ");

        cout << "\n  " << left
             << setw(8)  << "Rec#"
             << setw(9)  << "Member"
             << setw(7)  << "Book"
             << setw(30) << "Book Title"
             << setw(12) << "Issued"
             << setw(12) << "Returned"
             << "Status\n";
        cout << "  " << string(85, '-') << "\n";

        int count = 0;
        for (const auto& r : records) {
            if (activeOnly && r.returned) continue;
            if (filterMember != -1 && r.memberId != filterMember) continue;

            Book* b = findBook(r.bookId);
            string title = b ? b->getTitle().substr(0, 28) : "Unknown";

            cout << "  " << left
                 << setw(8)  << r.recordId
                 << setw(9)  << r.memberId
                 << setw(7)  << r.bookId
                 << setw(30) << title
                 << setw(12) << r.issueDate
                 << setw(12) << (r.returnDate.empty() ? "-" : r.returnDate)
                 << (r.returned ? "Returned" : "ACTIVE") << "\n";
            count++;
        }
        cout << "\n  Showing " << count << " record(s).\n";
        pause();
    }

    // ══════════════════════════════════════════
    //  STATISTICS
    // ══════════════════════════════════════════
    void showStats() {
        clearScreen();
        printHeader("LIBRARY STATISTICS");

        int totalBooks = 0, availBooks = 0;
        for (const auto& b : books) {
            totalBooks += b.getTotalCopies();
            availBooks += b.getAvailableCopies();
        }
        int activeIssues = 0;
        for (const auto& r : records) if (!r.returned) activeIssues++;

        cout << "\n"
             << "  Unique Titles     : " << books.size()   << "\n"
             << "  Total Copies      : " << totalBooks      << "\n"
             << "  Available Copies  : " << availBooks      << "\n"
             << "  Issued Copies     : " << (totalBooks - availBooks) << "\n"
             << "  Registered Members: " << members.size() << "\n"
             << "  Active Issues     : " << activeIssues    << "\n"
             << "  Total Borrow Logs : " << records.size() << "\n";

        // Most borrowed book
        if (!records.empty()) {
            map<int,int> freq;
            for (const auto& r : records) freq[r.bookId]++;
            auto maxIt = max_element(freq.begin(), freq.end(),
                [](const pair<int,int>& a, const pair<int,int>& b){ return a.second < b.second; });
            Book* top = findBook(maxIt->first);
            cout << "  Most Borrowed     : "
                 << (top ? top->getTitle() : "N/A")
                 << " (" << maxIt->second << " times)\n";
        }
        pause();
    }

    // ══════════════════════════════════════════
    //  MAIN MENU
    // ══════════════════════════════════════════
    void run() {
        int choice;
        do {
            clearScreen();
            printHeader("MAIN MENU");
            cout << "\n"
                 << "  ── Books ─────────────────────────\n"
                 << "  1.  Add Book\n"
                 << "  2.  Display All Books\n"
                 << "  3.  Search Books\n"
                 << "  4.  Update Book\n"
                 << "  5.  Delete Book\n"
                 << "\n"
                 << "  ── Members ───────────────────────\n"
                 << "  6.  Add Member\n"
                 << "  7.  Display All Members\n"
                 << "\n"
                 << "  ── Borrowing ─────────────────────\n"
                 << "  8.  Issue Book\n"
                 << "  9.  Return Book\n"
                 << "  10. View Borrow Records\n"
                 << "\n"
                 << "  ── Reports ───────────────────────\n"
                 << "  11. Library Statistics\n"
                 << "\n"
                 << "  0.  Exit\n\n";

            choice = getInt("  Enter choice: ");

            switch (choice) {
                case 1:  addBook();           break;
                case 2:  displayAllBooks();   break;
                case 3:  searchBooks();        break;
                case 4:  updateBook();         break;
                case 5:  deleteBook();         break;
                case 6:  addMember();          break;
                case 7:  displayAllMembers();  break;
                case 8:  issueBook();          break;
                case 9:  returnBook();         break;
                case 10: viewBorrowRecords();  break;
                case 11: showStats();          break;
                case 0:
                    clearScreen();
                    cout << "\n  Goodbye! All data saved.\n\n";
                    break;
                default:
                    cout << "\n  ✗ Invalid option.\n";
                    pause();
            }
        } while (choice != 0);
    }
};

// ─────────────────────────────────────────────
//  Entry point
// ─────────────────────────────────────────────
int main() {
    Library lib;
    lib.run();
    return 0;
}
