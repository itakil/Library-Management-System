# Library Management System (C++)

A console-based Library Management System using **Object-Oriented Programming** and **file handling** in C++.

## OOP Design

| Class | Responsibility |
|---|---|
| `Book` | Stores book details; handles copy availability tracking |
| `Member` | Stores member details and registration info |
| `BorrowRecord` | Tracks each issue/return transaction |
| `Library` | Manages all collections, file I/O, and menu |

## Features

| Feature | Details |
|---|---|
| **Add / Update / Delete Book** | Full CRUD for books with copy management |
| **Search Books** | By title (partial), author (partial), or book ID |
| **Add Member** | Register members with name, email, phone |
| **Issue Book** | PIN-free issue — validates availability, max 3 books/member |
| **Return Book** | Shows active issues for member, returns by record number |
| **Borrow Records** | Filter by all / active / member ID |
| **Statistics** | Titles, copies, issued, members, most borrowed book |
| **Persistent Storage** | 3 separate `.dat` files for books, members, records |

## Build & Run

```bash
# Compile
make
# or
g++ -std=c++17 -Wall -O2 -o library_management library_management.cpp

# Run
./library_management        # Linux / macOS
.\library_management.exe    # Windows

# Clean
make clean
```

## Data Files

| File | Contents |
|---|---|
| `library_books.dat` | All book records with copy counts |
| `library_members.dat` | All registered members |
| `library_records.dat` | Full borrow/return history |

## File Structure

```
library_management/
├── library_management.cpp   # Full OOP source
├── Makefile
├── README.md
├── library_books.dat        # created on first run
├── library_members.dat      # created on first run
└── library_records.dat      # created on first run
```

## Business Rules
- Maximum **3 books** per member at a time
- A member cannot borrow the **same book twice** simultaneously
- Books with **issued copies** cannot be deleted
- Search supports **partial, case-insensitive** matching
