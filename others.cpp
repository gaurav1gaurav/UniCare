#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "others.h"
#include "student.h"
#include "doctor.h"

using namespace std;

void safeCopy(char dest[], const char src[], int size) {
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}

void readLine(const char prompt[], char out[], int maxLen) {
    while (true) {
        cout << prompt;
        cin.getline(out, maxLen);

        if (cin.eof() && strlen(out) == 0) {
            cout << "\nInput closed. Exiting.\n";
            exit(0);
        }
        if (cin.fail() && !cin.eof()) {
            cin.clear();
            cin.ignore(10000, '\n');
        }

        int len = strlen(out);
        for (int i = 0; i < len; i++)
            if (out[i] == ',') out[i] = ' ';

        int a = 0, b = len - 1;
        while (a < len && (out[a] == ' ' || out[a] == '\t' || out[a] == '\r')) a++;
        while (b >= a && (out[b] == ' ' || out[b] == '\t' || out[b] == '\r')) b--;
        int n = b - a + 1;
        memmove(out, out + a, n);
        out[n] = '\0';

        if (n > 0) return;
        cout << "  This field cannot be empty.\n";
    }
}

int toInt(const char s[]) {
    int len = strlen(s);
    if (len == 0 || len > 9) return -1;
    int n = 0;
    for (int i = 0; i < len; i++) {
        if (s[i] < '0' || s[i] > '9') return -1;
        n = n * 10 + (s[i] - '0');
    }
    return n;
}

int readInt(const char prompt[], int lo, int hi) {
    char buf[20];
    while (true) {
        readLine(prompt, buf, 20);
        int n = toInt(buf);
        if (n >= lo && n <= hi) return n;
        cout << "  Please enter a number between " << lo << " and " << hi << ".\n";
    }
}

int compareIgnoreCase(const char a[], const char b[]) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        int x = tolower(a[i]), y = tolower(b[i]);
        if (x != y) return x - y;
        i++;
    }
    return tolower(a[i]) - tolower(b[i]);
}

bool containsIgnoreCase(const char text[], const char key[]) {
    char t[LINE_LEN], k[LINE_LEN];
    safeCopy(t, text, LINE_LEN);
    safeCopy(k, key, LINE_LEN);
    for (int i = 0; t[i] != '\0'; i++) t[i] = tolower(t[i]);
    for (int i = 0; k[i] != '\0'; i++) k[i] = tolower(k[i]);
    return strstr(t, k) != NULL;
}

int splitLine(const char line[], char parts[][FIELD_LEN], int maxParts) {
    int n = 0, pos = 0;
    for (int i = 0; i < maxParts; i++) parts[i][0] = '\0';
    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] == ',') {
            n++;
            pos = 0;
        } else if (line[i] != '\r' && n < maxParts && pos < FIELD_LEN - 1) {
            parts[n][pos++] = line[i];
            parts[n][pos] = '\0';
        }
    }
    return n + 1;
}

unsigned int hashPassword(const char s[]) {
    unsigned int h = 5381;
    for (int i = 0; s[i] != '\0'; i++) h = h * 33 + (unsigned char)s[i];
    return h;
}

bool replaceFile(const char tmpName[], const char destName[]) {
    if (rename(tmpName, destName) == 0) return true;
    remove(destName);
    return rename(tmpName, destName) == 0;
}

Person::Person() {
    id[0] = '\0';
    name[0] = '\0';
}

Person::Person(const char i[], const char n[]) {
    safeCopy(id, i, ID_LEN);
    safeCopy(name, n, NAME_LEN);
}

Person::~Person() {}

const char* Person::getId() const {
    return id;
}

const char* Person::getName() const {
    return name;
}

void Person::setName(const char n[]) {
    safeCopy(name, n, NAME_LEN);
}

void saveAll() {
    if (!students.saveToFile("students.txt")) cout << "  Warning: could not save students.\n";
    if (!doctors.saveToFile("doctors.txt")) cout << "  Warning: could not save doctors.\n";
}

void adminMenu() {
    while (true) {
        cout << "\n===== ADMIN MENU =====\n"
             << "1. Register student\n2. Search student\n3. Update student\n4. Delete student\n"
             << "5. View all students\n6. Add doctor\n7. Search doctor\n8. View doctors\n9. Update doctor\n10. Delete doctor\n0. Back\n";
        int c = readInt("Choice: ", 0, 10);
        if (c == 1) registerStudent(true);
        else if (c == 2) searchStudent();
        else if (c == 3) updateStudent();
        else if (c == 4) deleteStudent();
        else if (c == 5) {
            cout << "\n--- Students (" << students.size() << ") ---\n";
            students.displayAllSorted();
        }
        else if (c == 6) addDoctor();
        else if (c == 7) searchDoctor();
        else if (c == 8) showDoctors();
        else if (c == 9) updateDoctor();
        else if (c == 10) deleteDoctor();
        else return;
    }
}

void studentMenu(Student* me) {
    char text[NAME_LEN];
    while (true) {
        cout << "\n===== WELCOME, " << me->getName() << " =====\n"
             << "1. My profile\n2. View doctors\n3. Search doctor\n4. Change password\n0. Logout\n";
        int c = readInt("Choice: ", 0, 4);
        if (c == 1) me->display();
        else if (c == 2) showDoctors();
        else if (c == 3) searchDoctor();
        else if (c == 4) {
            readLine("Current password: ", text, NAME_LEN);
            if (!me->checkPassword(text)) cout << "  Wrong password.\n";
            else {
                readLine("New password: ", text, NAME_LEN);
                me->setPassword(text);
                saveAll();
                cout << "  Password changed.\n";
            }
        }
        else return;
    }
}

void studentLogin() {
    char roll[ID_LEN], pass[NAME_LEN];
    cout << "\n--- Student Login ---\n";
    readLine("Roll No: ", roll, ID_LEN);
    for (int attempt = 1; attempt <= MAX_LOGIN_ATTEMPTS; attempt++) {
        readLine("Password: ", pass, NAME_LEN);
        Student* s = students.authenticate(roll, pass);
        if (s != NULL) { studentMenu(s); return; }
        cout << "  Invalid roll number or password (" << attempt << "/" << MAX_LOGIN_ATTEMPTS << ").\n";
    }
    cout << "  Too many failed attempts.\n";
}

void adminLogin() {
    char pass[NAME_LEN];
    cout << "\n--- Admin Login ---\n";
    for (int attempt = 1; attempt <= MAX_LOGIN_ATTEMPTS; attempt++) {
        readLine("Admin password: ", pass, NAME_LEN);
        if (strcmp(pass, ADMIN_PASSWORD) == 0) { adminMenu(); return; }
        cout << "  Wrong admin password (" << attempt << "/" << MAX_LOGIN_ATTEMPTS << ").\n";
    }
    cout << "  Too many failed attempts.\n";
}
