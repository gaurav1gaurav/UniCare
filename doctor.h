#ifndef DOCTOR_H
#define DOCTOR_H

#include <fstream>
#include "others.h"

class Doctor : public Person {
private:
    char specialization[TEXT_LEN];
    char phone[PHONE_LEN];
    char availability[TEXT_LEN];

public:
    Doctor();
    Doctor(const char i[], const char n[], const char spec[], const char ph[], const char avail[]);

    const char* getSpecialization();
    void setSpecialization(const char x[]);
    void setPhone(const char x[]);
    void setAvailability(const char x[]);

    void display();
    void save(std::ofstream &out);
    bool loadFromLine(const char line[]);
};

void quickSortDoctorsByName(Doctor a[], int lo, int hi);

class DoctorList {
private:
    Doctor doctors[MAX_DOCTORS];
    int count;

    int binarySearchById(const char id[]);

public:
    DoctorList();

    int size();
    bool add(Doctor d);
    bool remove(const char id[]);
    Doctor* findById(const char id[]);
    int searchByKeyword(const char key[]);
    void displayAll(bool sortByName);
    bool saveToFile(const char filename[]);
    void loadFromFile(const char filename[]);
};

extern DoctorList doctors;

void addDoctor();
void searchDoctor();
void updateDoctor();
void deleteDoctor();
void showDoctors();

#endif
