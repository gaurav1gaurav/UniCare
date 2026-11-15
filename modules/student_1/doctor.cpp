#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include "doctor.h"

using namespace std;

Doctor::Doctor() : Person() {
    specialization[0] = '\0';
    phone[0] = '\0';
    availability[0] = '\0';
}

Doctor::Doctor(const char i[], const char n[], const char spec[], const char ph[], const char avail[])
    : Person(i, n) {
    safeCopy(specialization, spec, TEXT_LEN);
    safeCopy(phone, ph, PHONE_LEN);
    safeCopy(availability, avail, TEXT_LEN);
}

const char* Doctor::getSpecialization() {
    return specialization;
}

void Doctor::setSpecialization(const char x[]) {
    safeCopy(specialization, x, TEXT_LEN);
}

void Doctor::setPhone(const char x[]) {
    safeCopy(phone, x, PHONE_LEN);
}

void Doctor::setAvailability(const char x[]) {
    safeCopy(availability, x, TEXT_LEN);
}

void Doctor::display() {
    cout << "  ID: " << id << " | ";
    if (strncmp(name, "Dr.", 3) != 0 && strncmp(name, "dr.", 3) != 0) cout << "Dr. ";
    cout << name << " | " << specialization
         << " | Phone: " << phone << " | Available: " << availability << endl;
}

void Doctor::save(ofstream &out) {
    out << id << "," << name << "," << specialization << "," << phone << "," << availability << "\n";
}

bool Doctor::loadFromLine(const char line[]) {
    char p[5][FIELD_LEN];
    if (splitLine(line, p, 5) != 5 || p[0][0] == '\0') return false;
    safeCopy(id, p[0], ID_LEN);
    safeCopy(name, p[1], NAME_LEN);
    safeCopy(specialization, p[2], TEXT_LEN);
    safeCopy(phone, p[3], PHONE_LEN);
    safeCopy(availability, p[4], TEXT_LEN);
    return true;
}

void quickSortDoctorsByName(Doctor a[], int lo, int hi) {
    if (lo >= hi) return;
    char pivot[NAME_LEN];
    safeCopy(pivot, a[(lo + hi) / 2].getName(), NAME_LEN);
    int i = lo, j = hi;
    while (i <= j) {
        while (compareIgnoreCase(a[i].getName(), pivot) < 0) i++;
        while (compareIgnoreCase(a[j].getName(), pivot) > 0) j--;
        if (i <= j) {
            Doctor t = a[i]; a[i] = a[j]; a[j] = t;
            i++; j--;
        }
    }
    quickSortDoctorsByName(a, lo, j);
    quickSortDoctorsByName(a, i, hi);
}

int DoctorList::binarySearchById(const char id[]) {
    int lo = 0, hi = count - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        int cmp = strcmp(doctors[mid].getId(), id);
        if (cmp == 0) return mid;
        if (cmp < 0) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

DoctorList::DoctorList() {
    count = 0;
}

int DoctorList::size() {
    return count;
}

bool DoctorList::add(Doctor d) {
    if (count >= MAX_DOCTORS) return false;
    if (binarySearchById(d.getId()) != -1) return false;
    int pos = count;
    while (pos > 0 && strcmp(doctors[pos - 1].getId(), d.getId()) > 0) {
        doctors[pos] = doctors[pos - 1];
        pos--;
    }
    doctors[pos] = d;
    count++;
    return true;
}

bool DoctorList::remove(const char id[]) {
    int i = binarySearchById(id);
    if (i == -1) return false;
    for (int j = i; j < count - 1; j++) doctors[j] = doctors[j + 1];
    count--;
    return true;
}

Doctor* DoctorList::findById(const char id[]) {
    int i = binarySearchById(id);
    if (i == -1) return NULL;
    return &doctors[i];
}

int DoctorList::searchByKeyword(const char key[]) {
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (containsIgnoreCase(doctors[i].getName(), key) ||
            containsIgnoreCase(doctors[i].getSpecialization(), key)) {
            doctors[i].display();
            found++;
        }
    }
    return found;
}

void DoctorList::displayAll(bool sortByName) {
    if (count == 0) { cout << "  No doctors added yet.\n"; return; }
    Doctor copy[MAX_DOCTORS];
    for (int i = 0; i < count; i++) copy[i] = doctors[i];
    if (sortByName) quickSortDoctorsByName(copy, 0, count - 1);
    for (int i = 0; i < count; i++) {
        cout << (i + 1) << ".";
        copy[i].display();
    }
}

bool DoctorList::saveToFile(const char filename[]) {
    ofstream out(filename);
    if (!out) return false;
    for (int i = 0; i < count; i++) doctors[i].save(out);
    return true;
}

void DoctorList::loadFromFile(const char filename[]) {
    ifstream in(filename);
    if (!in) return;
    char line[LINE_LEN];
    int skipped = 0;
    while (in.getline(line, LINE_LEN)) {
        if (line[0] == '\0') continue;
        Doctor d;
        if (!(d.loadFromLine(line) && add(d))) skipped++;
    }
    if (skipped > 0) cout << "  Warning: skipped " << skipped << " bad doctor line(s).\n";
}

DoctorList doctors;

void addDoctor() {
    char id[ID_LEN], name[NAME_LEN], spec[TEXT_LEN], phone[PHONE_LEN], avail[TEXT_LEN];
    cout << "\n--- Add Doctor ---\n";
    readLine("Doctor ID (e.g. D01): ", id, ID_LEN);
    if (doctors.findById(id) != NULL) { cout << "  A doctor with this ID already exists.\n"; return; }
    readLine("Name: ", name, NAME_LEN);
    readLine("Specialization: ", spec, TEXT_LEN);
    readLine("Phone: ", phone, PHONE_LEN);
    readLine("Availability (e.g. Mon-Fri 10-4): ", avail, TEXT_LEN);
    if (doctors.add(Doctor(id, name, spec, phone, avail))) {
        saveAll();
        cout << "  Doctor added.\n";
    } else cout << "  Doctor list is full.\n";
}

void searchDoctor() {
    cout << "\n1. Search by ID (binary search)\n2. Search by name / specialization (linear search)\n";
    int c = readInt("Choice: ", 1, 2);
    char key[NAME_LEN];
    if (c == 1) {
        readLine("Doctor ID: ", key, ID_LEN);
        Doctor* d = doctors.findById(key);
        if (d != NULL) d->display(); else cout << "  Doctor not found.\n";
    } else {
        readLine("Keyword: ", key, NAME_LEN);
        if (doctors.searchByKeyword(key) == 0)
            cout << "  No matching doctor found.\n";
    }
}

void updateDoctor() {
    char id[ID_LEN], text[NAME_LEN];
    readLine("\nDoctor ID to update: ", id, ID_LEN);
    Doctor* d = doctors.findById(id);
    if (d == NULL) { cout << "  Doctor not found.\n"; return; }
    d->display();
    cout << "1. Name\n2. Specialization\n3. Phone\n4. Availability\n";
    int c = readInt("What to update: ", 1, 4);
    if (c == 1) { readLine("New name: ", text, NAME_LEN); d->setName(text); }
    else if (c == 2) { readLine("New specialization: ", text, TEXT_LEN); d->setSpecialization(text); }
    else if (c == 3) { readLine("New phone: ", text, PHONE_LEN); d->setPhone(text); }
    else { readLine("New availability: ", text, TEXT_LEN); d->setAvailability(text); }
    saveAll();
    cout << "  Doctor updated.\n";
}

void deleteDoctor() {
    char id[ID_LEN], answer[10];
    readLine("\nDoctor ID to delete: ", id, ID_LEN);
    Doctor* d = doctors.findById(id);
    if (d == NULL) { cout << "  Doctor not found.\n"; return; }
    d->display();
    readLine("Type YES to confirm delete: ", answer, 10);
    if (strcmp(answer, "YES") == 0) {
        doctors.remove(id);
        saveAll();
        cout << "  Doctor deleted.\n";
    } else cout << "  Cancelled.\n";
}

void showDoctors() {
    cout << "\n--- Doctors ---\n1. Sorted by ID\n2. Sorted by name\n";
    doctors.displayAll(readInt("Choice: ", 1, 2) == 2);
}
