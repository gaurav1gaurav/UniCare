
#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
using namespace std;

struct Appointment
{
    int id;
    string studentName;
    int doctorId;
    string doctorName;
};

class AppointmentQueue
{
    Appointment list[100];
    int count;
    void save();

public:
    AppointmentQueue();
    void book(string studentName = "");
    void view();
    void cancel();
};

#endif
            