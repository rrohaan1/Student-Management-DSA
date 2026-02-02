# Student Management System: Hybrid DSA Architecture

A high-performance student information system developed in C++ as part of the **Data Structures and Algorithms (DSA)** curriculum. This project demonstrates advanced memory management using a hybrid of multi-level linked lists and hash tables to optimize for both categorical organization and global search efficiency.

## 🏗️ System Architecture

This system utilizes three primary interconnected data structures to manage information:

### 1. User Authentication Layer
Users are managed via a singly linked list, providing a secure gateway for system access.
* **Logic:** Each `User` node stores credentials and a pointer to the next user.
![User Linked List](Screenshot%202026-02-02%20181145.png)

### 2. Categorical Hierarchy (Linked Lists)
Data is organized logically to reflect a university's structural hierarchy:
* **Semesters:** A top-level linked list manages different academic terms.
* **Departments:** Each semester node contains a nested linked list of departments (e.g., CS, Cys, SE, AI).
* **Students:** Each department maintains a dedicated linked list of its students.

**Semester & Department Chain:**
![Semester Hierarchy](Screenshot%202026-02-02%20181145.png)

**Departmental Detail:**
![Department Chain](Screenshot%202026-02-02%20181155.png)

### 3. High-Speed Global Search (Hash Table)
To achieve **O(1) average time complexity** for student lookups by ID, a global hash table is implemented.
* **Collision Handling:** Uses **Separate Chaining** via the `hashNext` pointer within the student structure.
* **Functionality:** This allows for instantaneous retrieval regardless of which semester or department the student belongs to.
![Hash Table Chaining](Screenshot%202026-02-02%20181217.png)

---

## 🛠️ Data Structure: The Student Node
The "Hybrid" nature of this project is found within the `Student` structure itself. Every student exists in two structures simultaneously through multiple pointers.
* **`next`**: Connects to the next student in the same **department**.
* **`hashNext`**: Connects to the next student in the same **hash bucket**.
* **Back-pointers**: Every student maintains pointers back to their specific `dept` and `sem` for data integrity.

![Student Node Detail](Screenshot%202026-02-02%20181202.png)

---

## 🚀 Key Technical Implementation
- **Data Persistence:** Implements robust File I/O to save/load student and user records (`students.txt`, `users.txt`).
- **Memory Management:** Efficient use of raw pointers and manual memory allocation/deallocation in C++.
- **Validation Suite:** Strict checks for GPA ranges (0.0 - 4.0), ID numericality, and valid department/semester entries.

## 📂 Repository Contents
- `Student Management System.cpp`: The complete source code.
- `students.txt` / `users.txt`: Sample database files.
- `Screenshots/`: Detailed architectural diagrams of the DSA implementation.
