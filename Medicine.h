#ifndef MEDICINE_H
#define MEDICINE_H

#include <string>
using namespace std;

struct Medicine
{
    int id;
    string name;
    int quantity;
    double price;
};

struct Request
{
    string studentName;
    int medicineId;
    string medicineName;
    int quantity;
};

class MedicineInventory
{
    Medicine list[100];
    int count;
    Request requests[100];
    int requestCount;

    void save();
    void saveRequests();
    int find(int id);

public:
    MedicineInventory();

    void view(bool isAdmin);
    void requestMedicine(string studentName = "");

    void addMedicine();
    void restockMedicine();
    void viewRequests();
    void handleRequest();
};

#endif
