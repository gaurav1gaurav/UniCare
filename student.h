#ifndef STUDENT_H
#define STUDENT_H

#include <fstream>
#include "others.h"

#define INITIAL_TABLE_SIZE 11

class Student : public Person {
private:
    char department[TEXT_LEN];
    int year;
    unsigned int passHash;

public:
    Student();
    Student(const char roll[], const char n[], const char dept[], int yr, const char password[]);

    const char* getDepartment() const;
    int getYear() const;
    void setDepartment(const char d[]);
    void setYear(int y);
    void setPassword(const char p[]);
    bool checkPassword(const char p[]) const;

    void display();
    void save(std::ofstream &out);
    bool loadFromLine(const char line[]);
};

void quickSortStudents(Student a[], int lo, int hi);

class StudentTable {
private:
    Student* table;    
    int* state;        
    int capacity;     
    int count;        
    int used;        

    static int hashFn(const char key[], int cap);
    static bool isPrime(int n);
    static int nextPrime(int n);

    int findIndex(const char rollNo[]);
    bool rehash(int newCap);

public:
    StudentTable();
    ~StudentTable();
    StudentTable(const StudentTable &other);
    StudentTable& operator=(const StudentTable &other);

    int size();
    bool insert(Student s);
    Student* search(const char rollNo[]);
    bool remove(const char rollNo[]);
    Student* authenticate(const char rollNo[], const char password[]);
    void displayAllSorted();
    bool saveToFile(const char filename[]);
    void loadFromFile(const char filename[]);
};

extern StudentTable students;

void registerStudent(bool byAdmin);
void searchStudent();
void updateStudent();
void deleteStudent();

#endif
