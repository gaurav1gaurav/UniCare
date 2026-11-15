#include "Medicine.h"
#include "em.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

static string readText(string message)
{
    string s;
    cout << message;
    getline(cin, s);
    return s;
}

static int readNumber(string message)
{
    return atoi(readText(message).c_str());
}

const int MAX_REQUEST_QTY = 2;


MedicineInventory::MedicineInventory()
{
    count = 0;
    requestCount = 0;

    ifstream file("medicines.txt");
    string id, name, qty, price;
    while (count < 100 && getline(file, id, ','))
    {
        getline(file, name, ',');
        getline(file, qty, ',');
        getline(file, price);
        if (atoi(id.c_str()) > 0)
        {
            list[count++] = {atoi(id.c_str()), name, atoi(qty.c_str()), atof(price.c_str())};
        }
    }

    ifstream reqFile("requests.txt");
    string number, student, medId, medName;
    while (requestCount < 100 && getline(reqFile, number, ','))
    {
        getline(reqFile, student, ',');
        getline(reqFile, medId, ',');
        getline(reqFile, medName, ',');
        getline(reqFile, qty);
        if (atoi(number.c_str()) > 0)
        {
            requests[requestCount++] = {student, atoi(medId.c_str()), medName, atoi(qty.c_str())};
        }
    }
}

void MedicineInventory::save()
{
    ofstream file("medicines.txt");
    for (int i = 0; i < count; i++)
    {
        file << list[i].id << "," << list[i].name << ","
             << list[i].quantity << "," << list[i].price << "\n";
    }
}

void MedicineInventory::saveRequests()
{
    ofstream file("requests.txt");
    for (int i = 0; i < requestCount; i++)
    {
        file << i + 1 << "," << requests[i].studentName << ","
             << requests[i].medicineId << "," << requests[i].medicineName << ","
             << requests[i].quantity << "\n";
    }
}

int MedicineInventory::find(int id)
{
    for (int i = 0; i < count; i++)
    {
        if (list[i].id == id)
            return i;
    }
    return -1;
}

void MedicineInventory::view(bool isAdmin)
{
    if (count == 0)
        cout << "No medicines available.\n";

    for (int i = 0; i < count; i++)
    {
        cout << list[i].id << ". " << list[i].name << " | Stock: " << list[i].quantity;
        if (isAdmin)
            cout << " | Price: " << list[i].price;
        else
            cout << (list[i].quantity > 0 ? " | Available" : " | Out of stock");
        cout << "\n";
    }
}

void MedicineInventory::requestMedicine(string studentName)
{
    if (requestCount == 100)
    {
        cout << "Too many pending requests.\n";
        return;
    }

    string student = studentName.empty() ? readText("Enter your name: ") : studentName;
    if (student.empty() || student.find(',') != string::npos)
    {
        cout << "Invalid name.\n";
        return;
    }
    string registeredStudent = findRegisteredStudentName(student);
    if (registeredStudent.empty())
    {
        cout << "Student not found in students.txt. Only registered students can request medicine.\n";
        return;
    }
    student = registeredStudent;
    if (!studentName.empty())
    {
        cout << "\nRequesting medicine for: " << student << "\n";
    }

    view(false);
    int i = find(readNumber("Enter medicine ID: "));
    if (i == -1)
    {
        cout << "Medicine not found.\n";
        return;
    }

    int qty = readNumber("Enter quantity (1 to 2): ");
    if (qty < 1 || qty > MAX_REQUEST_QTY || qty > list[i].quantity)
    {
        cout << "Invalid quantity or not enough stock.\n";
        return;
    }

    requests[requestCount++] = {student, list[i].id, list[i].name, qty};
    saveRequests();
    cout << "Request sent. Please collect it after admin approval.\n";
}

void MedicineInventory::addMedicine()
{
    if (count == 100)
    {
        cout << "Inventory is full.\n";
        return;
    }

    string name = readText("Enter medicine name: ");
    int qty = readNumber("Enter quantity: ");
    double price = atof(readText("Enter price: ").c_str());

    if (name.empty() || name.find(',') != string::npos || qty < 0 || price < 0)
    {
        cout << "Invalid details.\n";
        return;
    }

    int id = (count == 0) ? 101 : list[count - 1].id + 1;
    list[count++] = {id, name, qty, price};
    save();
    cout << "Medicine added. ID: " << id << "\n";
}

void MedicineInventory::restockMedicine()
{
    int i = find(readNumber("Enter medicine ID: "));
    if (i == -1)
    {
        cout << "Medicine not found.\n";
        return;
    }

    int amount = readNumber("Enter quantity to add: ");
    if (amount <= 0)
    {
        cout << "Invalid quantity.\n";
        return;
    }

    list[i].quantity += amount;
    save();
    cout << list[i].name << " stock is now " << list[i].quantity << "\n";
}

void MedicineInventory::viewRequests()
{
    if (requestCount == 0)
        cout << "No pending requests.\n";

    for (int i = 0; i < requestCount; i++)
    {
        cout << i + 1 << ". " << requests[i].studentName << " wants "
             << requests[i].quantity << " x " << requests[i].medicineName << "\n";
    }
}

void MedicineInventory::handleRequest()
{
    viewRequests();
    if (requestCount == 0)
        return;

    int r = readNumber("Enter request number: ") - 1;
    if (r < 0 || r >= requestCount)
    {
        cout << "Request not found.\n";
        return;
    }

    int action = readNumber("1 = Approve, 2 = Reject: ");
    if (action == 1)
    {
        int m = find(requests[r].medicineId);
        if (m == -1 || list[m].quantity < requests[r].quantity)
        {
            cout << "Not enough stock (or medicine removed).\n";
            return;
        }
        list[m].quantity -= requests[r].quantity;
        save();
        cout << "Request approved.\n";
    }
    else if (action == 2)
    {
        cout << "Request rejected.\n";
    }
    else
    {
        cout << "Invalid choice.\n";
        return;
    }

    for (int i = r; i < requestCount - 1; i++)
        requests[i] = requests[i + 1];
    requestCount--;
    saveRequests();
}
