#include "Appointment.h"
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

static bool sameIgnoreCase(string a, string b)
{
    if (a.length() != b.length())
        return false;
    for (int i = 0; i < (int)a.length(); i++)
    {
        char ca = a[i];
        char cb = b[i];
        if (ca >= 'A' && ca <= 'Z') ca = (char)(ca - 'A' + 'a');
        if (cb >= 'A' && cb <= 'Z') cb = (char)(cb - 'A' + 'a');
        if (ca != cb)
            return false;
    }
    return true;
}

static int extractDigitsId(string rawId, int fallbackId)
{
    int value = 0;
    bool found = false;
    for (int i = 0; i < (int)rawId.length(); i++)
    {
        if (rawId[i] >= '0' && rawId[i] <= '9')
        {
            value = value * 10 + (rawId[i] - '0');
            found = true;
        }
    }
    return found ? value : fallbackId;
}

AppointmentQueue::AppointmentQueue()
{
    count = 0;
    ifstream file("appointment.txt");
    string id, student, doctorId, doctorName;

    while (count < 100 && getline(file, id, ','))
    {
        getline(file, student, ',');
        getline(file, doctorId, ',');
        getline(file, doctorName);
        if (atoi(id.c_str()) > 0)
        {
            list[count++] = {atoi(id.c_str()), student, atoi(doctorId.c_str()), doctorName};
        }
    }
}

void AppointmentQueue::save()
{
    ofstream file("appointment.txt");
    if (!file)
        cout << "Error: could not save appointment.txt\n";
    for (int i = 0; i < count; i++)
    {
        file << list[i].id << "," << list[i].studentName << ","
             << list[i].doctorId << "," << list[i].doctorName << "\n";
    }
}

void AppointmentQueue::book(string studentName)
{
    if (count == 100)
    {
        cout << "Appointment list is full.\n";
        return;
    }

    string rawIds[30], names[30], specs[30], avails[30];
    int ids[30], total = 0;
    ifstream file("doctors.txt");
    string line;

    while (total < 30 && getline(file, line))
    {
        if (!line.empty() && line[line.length() - 1] == '\r')
            line.erase(line.length() - 1);
        if (line.empty())
            continue;

        string p[5];
        int pCount = 0;
        size_t start = 0;
        while (pCount < 5)
        {
            size_t pos = line.find(',', start);
            if (pos == string::npos)
            {
                p[pCount++] = line.substr(start);
                break;
            }
            p[pCount++] = line.substr(start, pos - start);
            start = pos + 1;
        }
        if (pCount >= 2 && !p[0].empty() && !p[1].empty())
        {
            rawIds[total] = p[0];
            ids[total] = extractDigitsId(p[0], total + 1);
            names[total] = p[1];
            specs[total] = (pCount >= 3) ? p[2] : "";
            avails[total] = (pCount >= 5) ? p[4] : "";
            total++;
        }
    }

    if (total == 0)
    {
        cout << "No doctors available.\n";
        return;
    }

    string student = studentName.empty() ? readText("Enter student name: ") : studentName;
    if (student.empty() || student.find(',') != string::npos)
    {
        cout << "Invalid name.\n";
        return;
    }
    string registeredStudent = findRegisteredStudentName(student);
    if (registeredStudent.empty())
    {
        cout << "Student not found in students.txt. Only registered students can book appointments.\n";
        return;
    }
    student = registeredStudent;
    if (!studentName.empty())
    {
        cout << "\nBooking appointment for: " << student << "\n";
    }

    cout << "\nAvailable Doctors:\n";
    for (int i = 0; i < total; i++)
    {
        cout << rawIds[i] << ". ";
        if (names[i].find("Dr.") != 0)
            cout << "Dr. ";
        cout << names[i];
        if (!specs[i].empty())
            cout << " (" << specs[i] << ")";
        if (!avails[i].empty())
            cout << " [" << avails[i] << "]";
        cout << "\n";
    }
    string docInput = readText("Enter doctor ID (e.g. D01 or 1): ");
    int numericInput = extractDigitsId(docInput, -1);

    for (int i = 0; i < total; i++)
    {
        if (sameIgnoreCase(rawIds[i], docInput) || (numericInput > 0 && ids[i] == numericInput && isdigit((unsigned char)docInput[0])))
        {
            int newId = (count == 0) ? 1001 : list[count - 1].id + 1;
            string docTitle = (names[i].find("Dr.") == 0) ? names[i] : ("Dr. " + names[i]);
            list[count++] = {newId, student, ids[i], docTitle};
            save();
            cout << "Appointment booked. ID: " << newId << ", Doctor: " << docTitle << "\n";
            return;
        }
    }
    cout << "Doctor ID not found.\n";
}

void AppointmentQueue::view()
{
    if (count == 0)
        cout << "No appointments available.\n";
    for (int i = 0; i < count; i++)
    {
        cout << "\nAppointment ID: " << list[i].id
             << "\nStudent: " << list[i].studentName
             << "\nDoctor: " << list[i].doctorName << "\n";
    }
}

void AppointmentQueue::cancel()
{
    int id = readNumber("Enter appointment ID to cancel: ");

    for (int i = 0; i < count; i++)
    {
        if (list[i].id == id)
        {
            for (int j = i; j < count - 1; j++)
                list[j] = list[j + 1];
            count--;
            save();
            cout << "Appointment cancelled.\n";
            return;
        }
    }
    cout << "Appointment not found.\n";
}