#include <iostream>
#include <fstream>
#include <new>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "student.h"

using namespace std;

Student::Student() : Person() {
    department[0] = '\0';
    year = 1;
    passHash = 0;
}

Student::Student(const char roll[], const char n[], const char dept[], int yr, const char password[])
    : Person(roll, n) {
    safeCopy(department, dept, TEXT_LEN);
    year = (yr >= 1 && yr <= 4) ? yr : 1;
    passHash = hashPassword(password);
}

const char* Student::getDepartment() const {
    return department;
}

int Student::getYear() const {
    return year;
}

void Student::setDepartment(const char d[]) {
    safeCopy(department, d, TEXT_LEN);
}

void Student::setYear(int y) {
    if (y >= 1 && y <= 4) year = y;
}

void Student::setPassword(const char p[]) {
    passHash = hashPassword(p);
}

bool Student::checkPassword(const char p[]) const {
    return passHash == hashPassword(p);
}

void Student::display() {
    cout << "  Roll No: " << id << " | Name: " << name
         << " | Dept: " << department << " | Year: " << year << endl;
}

void Student::save(ofstream &out) {
    out << id << "," << name << "," << department << "," << year << "," << passHash << "\n";
}

bool Student::loadFromLine(const char line[]) {
    char p[5][FIELD_LEN];
    if (splitLine(line, p, 5) != 5) return false;
    int yr = toInt(p[3]);
    if (yr < 1 || yr > 4 || p[0][0] == '\0') return false;

    if (p[4][0] == '\0') return false;
    for (int i = 0; p[4][i] != '\0'; i++)
        if (p[4][i] < '0' || p[4][i] > '9') return false;
    errno = 0;
    unsigned long h = strtoul(p[4], NULL, 10);
    if (errno == ERANGE || h > 0xFFFFFFFFUL) return false;

    safeCopy(id, p[0], ID_LEN);
    safeCopy(name, p[1], NAME_LEN);
    safeCopy(department, p[2], TEXT_LEN);
    year = yr;
    passHash = (unsigned int)h;
    return true;
}

void quickSortStudents(Student a[], int lo, int hi) {
    if (lo >= hi) return;
    char pivot[ID_LEN];
    safeCopy(pivot, a[(lo + hi) / 2].getId(), ID_LEN);
    int i = lo, j = hi;
    while (i <= j) {
        while (strcmp(a[i].getId(), pivot) < 0) i++;
        while (strcmp(a[j].getId(), pivot) > 0) j--;
        if (i <= j) {
            Student t = a[i]; a[i] = a[j]; a[j] = t;
            i++; j--;
        }
    }
    quickSortStudents(a, lo, j);
    quickSortStudents(a, i, hi);
}

int StudentTable::hashFn(const char key[], int cap) {
    unsigned long h = 0;
    for (int i = 0; key[i] != '\0'; i++)
        h = (h * 31 + (unsigned char)key[i]) % cap;
    return (int)h;
}

bool StudentTable::isPrime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; i++)
        if (n % i == 0) return false;
    return true;
}

int StudentTable::nextPrime(int n) {
    while (!isPrime(n)) n++;
    return n;
}

StudentTable::StudentTable() {
    capacity = INITIAL_TABLE_SIZE;
    count = 0;
    used = 0;
    table = new Student[capacity];
    state = new int[capacity];
    for (int i = 0; i < capacity; i++) state[i] = EMPTY;
}

StudentTable::~StudentTable() {
    delete[] table;
    delete[] state;
}

StudentTable::StudentTable(const StudentTable &other) {
    capacity = other.capacity;
    count = other.count;
    used = other.used;
    table = new Student[capacity];
    state = new int[capacity];
    for (int i = 0; i < capacity; i++) {
        table[i] = other.table[i];
        state[i] = other.state[i];
    }
}

StudentTable& StudentTable::operator=(const StudentTable &other) {
    if (this == &other) return *this;
    Student* newTable = new Student[other.capacity];
    int* newState = new int[other.capacity];
    for (int i = 0; i < other.capacity; i++) {
        newTable[i] = other.table[i];
        newState[i] = other.state[i];
    }
    delete[] table;
    delete[] state;
    table = newTable;
    state = newState;
    capacity = other.capacity;
    count = other.count;
    used = other.used;
    return *this;
}

int StudentTable::findIndex(const char rollNo[]) {
    int idx = hashFn(rollNo, capacity);
    for (int i = 0; i < capacity; i++) {
        if (state[idx] == EMPTY) return -1;
        if (state[idx] == OCCUPIED && strcmp(table[idx].getId(), rollNo) == 0) return idx;
        idx = (idx + 1) % capacity;
    }
    return -1;
}
bool StudentTable::rehash(int newCap) {
    Student* newTable = new (nothrow) Student[newCap];
    int* newState = new (nothrow) int[newCap];
    if (newTable == NULL || newState == NULL) {
        delete[] newTable;
        delete[] newState;
        return false;
    }
    for (int i = 0; i < newCap; i++) newState[i] = EMPTY;

    for (int i = 0; i < capacity; i++) {
        if (state[i] != OCCUPIED) continue;
        int idx = hashFn(table[i].getId(), newCap);
        while (newState[idx] == OCCUPIED) idx = (idx + 1) % newCap;
        newTable[idx] = table[i];
        newState[idx] = OCCUPIED;
    }

    delete[] table;
    delete[] state;
    table = newTable;
    state = newState;
    capacity = newCap;
    used = count;
    return true;
}

int StudentTable::size() {
    return count;
}

bool StudentTable::insert(Student s) {
    if (findIndex(s.getId()) != -1) return false;

    if ((used + 1) * 2 > capacity) {
        int newCap = nextPrime((count + 1) * 4);
        if (newCap < capacity) newCap = capacity; 
        if (!rehash(newCap)) return false;
    }

    int idx = hashFn(s.getId(), capacity);
    while (state[idx] == OCCUPIED) idx = (idx + 1) % capacity;
    if (state[idx] == EMPTY) used++;  
    table[idx] = s;
    state[idx] = OCCUPIED;
    count++;
    return true;
}

Student* StudentTable::search(const char rollNo[]) {
    int idx = findIndex(rollNo);
    if (idx == -1) return NULL;
    return &table[idx];
}

bool StudentTable::remove(const char rollNo[]) {
    int idx = findIndex(rollNo);
    if (idx == -1) return false;
    state[idx] = DELETED;
    count--;
    return true;
}

Student* StudentTable::authenticate(const char rollNo[], const char password[]) {
    Student* s = search(rollNo);
    if (s != NULL && s->checkPassword(password)) return s;
    return NULL;
}

void StudentTable::displayAllSorted() {
    if (count == 0) { cout << "  No students registered.\n"; return; }

    Student* all = new (nothrow) Student[count];
    if (all == NULL) { cout << "  Not enough memory to list students.\n"; return; }

    int n = 0;
    for (int i = 0; i < capacity; i++)
        if (state[i] == OCCUPIED) all[n++] = table[i];

    quickSortStudents(all, 0, n - 1);
    for (int i = 0; i < n; i++) {
        cout << (i + 1) << ".";
        all[i].display();
    }
    delete[] all;
}

bool StudentTable::saveToFile(const char filename[]) {
    char tmpName[LINE_LEN];
    safeCopy(tmpName, filename, LINE_LEN - 4);
    strcat(tmpName, ".tmp");

    ofstream out(tmpName);
    if (!out) return false;
    for (int i = 0; i < capacity; i++)
        if (state[i] == OCCUPIED) table[i].save(out);
    out.flush();
    bool ok = out.good();
    out.close();

    if (!ok) { remove(tmpName); return false; }
    return replaceFile(tmpName, filename);
}

void StudentTable::loadFromFile(const char filename[]) {
    ifstream in(filename);
    if (!in) return;
    char line[LINE_LEN];
    int skipped = 0;
    while (in.getline(line, LINE_LEN)) {
        if (line[0] == '\0') continue;
        Student s;
        if (!(s.loadFromLine(line) && insert(s))) skipped++;
    }
    if (skipped > 0) cout << "  Warning: skipped " << skipped << " bad student line(s).\n";
}

StudentTable students;
void registerStudent(bool byAdmin) {
    char roll[ID_LEN], name[NAME_LEN], dept[TEXT_LEN], pass[NAME_LEN];
    cout << (byAdmin ? "\n--- Register Student ---\n" : "\n--- Student Registration ---\n");
    readLine("Roll No: ", roll, ID_LEN);
    if (students.search(roll) != NULL) { cout << "  This roll number is already registered.\n"; return; }
    readLine("Full name: ", name, NAME_LEN);
    readLine("Department: ", dept, TEXT_LEN);
    int year = readInt("Year (1-4): ", 1, 4);
    readLine(byAdmin ? "Initial password: " : "Choose a password: ", pass, NAME_LEN);

    if (students.insert(Student(roll, name, dept, year, pass))) {
        saveAll();
        cout << (byAdmin ? "  Student registered.\n"
                         : "  Registration successful. You can now log in.\n");
    } else {
        cout << "  Could not register student (out of memory).\n";
    }
}

void searchStudent() {
    char roll[ID_LEN];
    readLine("\nRoll No to search: ", roll, ID_LEN);
    Student* s = students.search(roll);
    if (s != NULL) s->display(); else cout << "  Student not found.\n";
}

void updateStudent() {
    char roll[ID_LEN], text[NAME_LEN];
    readLine("\nRoll No to update: ", roll, ID_LEN);
    Student* s = students.search(roll);
    if (s == NULL) { cout << "  Student not found.\n"; return; }
    s->display();
    cout << "1. Name\n2. Department\n3. Year\n4. Reset password\n";
    int c = readInt("What to update: ", 1, 4);
    if (c == 1) { readLine("New name: ", text, NAME_LEN); s->setName(text); }
    else if (c == 2) { readLine("New department: ", text, TEXT_LEN); s->setDepartment(text); }
    else if (c == 3) s->setYear(readInt("New year (1-4): ", 1, 4));
    else { readLine("New password: ", text, NAME_LEN); s->setPassword(text); }
    saveAll();
    cout << "  Record updated.\n";
}

void deleteStudent() {
    char roll[ID_LEN], answer[10];
    readLine("\nRoll No to delete: ", roll, ID_LEN);
    Student* s = students.search(roll);
    if (s == NULL) { cout << "  Student not found.\n"; return; }
    s->display();
    readLine("Type YES to confirm delete: ", answer, 10);
    if (strcmp(answer, "YES") == 0) {
        students.remove(roll);
        saveAll();
        cout << "  Student deleted.\n";
    } else cout << "  Cancelled.\n";
}
