#ifndef OTHERS_H
#define OTHERS_H

#include <fstream>

#define MAX_DOCTORS 30
#define EMPTY 0
#define OCCUPIED 1
#define DELETED 2

#define ID_LEN 20
#define NAME_LEN 50
#define TEXT_LEN 40
#define PHONE_LEN 15
#define FIELD_LEN 64
#define LINE_LEN 256

#define ADMIN_PASSWORD "admin123"
#define MAX_LOGIN_ATTEMPTS 3

class Student;

void safeCopy(char dest[], const char src[], int size);
void readLine(const char prompt[], char out[], int maxLen);
int toInt(const char s[]);
int readInt(const char prompt[], int lo, int hi);
int compareIgnoreCase(const char a[], const char b[]);
bool containsIgnoreCase(const char text[], const char key[]);
int splitLine(const char line[], char parts[][FIELD_LEN], int maxParts);
unsigned int hashPassword(const char s[]);
bool replaceFile(const char tmpName[], const char destName[]);

class Person {
protected:
    char id[ID_LEN];
    char name[NAME_LEN];

public:
    Person();
    Person(const char i[], const char n[]);
    virtual ~Person();

    const char* getId() const;
    const char* getName() const;
    void setName(const char n[]);

    virtual void display() = 0;
    virtual void save(std::ofstream &out) = 0;
};

void saveAll();
void adminMenu();
void studentMenu(Student* me);
void studentLogin();
void adminLogin();

#endif
