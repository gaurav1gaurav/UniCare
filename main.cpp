#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <new>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace HospitalMapModule {
#define main mapMain
#include "map.c"
#undef main
#undef MAX_NODES
#undef INF
#undef NAME_LEN
}

namespace PadDispatchModule {
#define main padDispatchMain
#include "pad dispatch.cpp"
#undef main
}

namespace Member1Module {
#include "others.h"
#include "student.h"
#include "doctor.h"
#include "student.cpp"
#include "doctor.cpp"
#include "others.cpp"

Student* authenticateStudentSession() {
    char roll[ID_LEN], pass[NAME_LEN];
    cout << "\n--- STUDENT LOGIN ---\n";
    readLine("Roll No: ", roll, ID_LEN);
    if (students.search(roll) == NULL) {
        cout << "  Roll No not found in students.txt. Please register first.\n";
        return NULL;
    }
    for (int attempt = 1; attempt <= MAX_LOGIN_ATTEMPTS; attempt++) {
        readLine("Password: ", pass, NAME_LEN);
        Student* s = students.authenticate(roll, pass);
        if (s != NULL) {
            return s;
        }
        cout << "  Invalid password (" << attempt << "/" << MAX_LOGIN_ATTEMPTS << ").\n";
    }
    cout << "  Too many failed attempts.\n";
    return NULL;
}

void changeStudentPassword(Student* me) {
    if (me == NULL) {
        return;
    }
    char currentPass[NAME_LEN], newPass[NAME_LEN];
    readLine("Current password: ", currentPass, NAME_LEN);
    if (!me->checkPassword(currentPass)) {
        cout << "  Wrong password.\n";
        return;
    }
    readLine("New password: ", newPass, NAME_LEN);
    me->setPassword(newPass);
    saveAll();
    cout << "  Password changed.\n";
}
}

#undef MAX_DOCTORS
#undef EMPTY
#undef OCCUPIED
#undef DELETED
#undef ID_LEN
#undef NAME_LEN
#undef TEXT_LEN
#undef PHONE_LEN
#undef FIELD_LEN
#undef LINE_LEN
#undef ADMIN_PASSWORD
#undef MAX_LOGIN_ATTEMPTS

#include "Appointment.h"
#include "Medicine.h"
#include "em.h"

#define ADMIN_PASSWORD "admin123"

static string readText(string prompt) {
    string s;
    cout << prompt;
    if (!getline(cin, s)) {
        return "";
    }
    return s;
}

static int readNumber(string prompt) {
    string s = readText(prompt);
    int start = 0;
    while (start < (int)s.length() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r')) {
        start++;
    }
    int end = (int)s.length() - 1;
    while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r')) {
        end--;
    }
    if (start > end) {
        return -1;
    }
    string clean = s.substr(start, end - start + 1);
    for (int i = 0; i < (int)clean.length(); i++) {
        if (clean[i] < '0' || clean[i] > '9') {
            return -1;
        }
    }
    return atoi(clean.c_str());
}

void initMember1Directory() {
    Member1Module::students.loadFromFile("students.txt");
    Member1Module::doctors.loadFromFile("doctors.txt");

    if (Member1Module::doctors.size() == 0) {
        Member1Module::doctors.add(Member1Module::Doctor("D01", "Arun Sharma", "General Physician", "9000000001", "Mon-Sat 9-5"));
        Member1Module::doctors.add(Member1Module::Doctor("D02", "Vikram Verma", "Dentist", "9000000002", "Mon-Sat 10-4"));
        Member1Module::doctors.add(Member1Module::Doctor("D03", "Neha Gupta", "Counsellor", "9000000003", "Mon-Fri 10-5"));
        Member1Module::doctors.add(Member1Module::Doctor("D04", "Anita Rawat", "General Physician", "9000000004", "Mon-Sat 9-5"));
        Member1Module::doctors.add(Member1Module::Doctor("D05", "Rajesh Negi", "Psychiatrist", "9000000005", "Mon-Fri 11-4"));
        Member1Module::doctors.add(Member1Module::Doctor("D06", "Meera Joshi", "Career Counsellor", "9000000006", "Tue-Thu 10-3"));
        Member1Module::saveAll();
    }
}

void runHospitalMap() {
    HospitalMapModule::mapMain();
    int ch = getchar();
    while (ch != '\n' && ch != EOF) {
        ch = getchar();
    }
}

void runPadDispatchStudent(Member1Module::Student* loggedIn) {
    PadDispatchModule::PadDispatch dispatch;
    string studentId = (loggedIn != NULL) ? string(loggedIn->getId()) : "";
    if (!PadDispatchModule::validStudentId(studentId)) {
        cout << "Invalid student ID.\n";
        return;
    }
    PadDispatchModule::studentMenu(studentId, dispatch);
}

void runPadDispatchStaff() {
    PadDispatchModule::PadDispatch dispatch;
    PadDispatchModule::staffMenu("ADMIN", dispatch);
}

void runDoctorsDirectory() {
    while (true) {
        cout << "\n--- DOCTORS DIRECTORY ---\n"
             << "1. View Doctors List\n"
             << "2. Search Doctor\n"
             << "0. Back\n";
        int c = readNumber("Choice: ");
        if (c == 1) Member1Module::showDoctors();
        else if (c == 2) Member1Module::searchDoctor();
        else if (c == 0) break;
        else cout << "Invalid choice!\n";
    }
}

bool adminLogin() {
    for (int tries = 0; tries < 3; tries++) {
        if (readText("Admin password: ") == ADMIN_PASSWORD) {
            return true;
        }
        cout << "Wrong password!\n";
    }
    cout << "Too many attempts.\n";
    return false;
}

void studentMenu(AppointmentQueue& appts, MedicineInventory& meds, EmergencySystem& emergencies, Member1Module::Student* loggedIn) {
    while (true) {
        cout << "\n===== STUDENT PORTAL : " << loggedIn->getName()
             << " (Roll No: " << loggedIn->getId() << ") =====\n"
             << "1. Book Appointment\n"
             << "2. View Available Medicines\n"
             << "3. Request Medicine\n"
             << "4. Report Medical Emergency\n"
             << "5. Check My Emergency Status\n"
             << "6. Find Nearest Hospital Route & Map\n"
             << "7. Emergency Sanitary Pad Dispatch\n"
             << "8. Doctors Directory (View / Search)\n"
             << "9. My Profile\n"
             << "10. Change Password\n"
             << "0. Logout\n";

        int c = readNumber("Choice: ");
        if (c == 1) appts.book(loggedIn->getName());
        else if (c == 2) meds.view(false);
        else if (c == 3) meds.requestMedicine(loggedIn->getName());
        else if (c == 4) emergencies.reportEmergency(loggedIn->getName());
        else if (c == 5) emergencies.viewStudentStatus(loggedIn->getName());
        else if (c == 6) runHospitalMap();
        else if (c == 7) runPadDispatchStudent(loggedIn);
        else if (c == 8) runDoctorsDirectory();
        else if (c == 9) loggedIn->display();
        else if (c == 10) Member1Module::changeStudentPassword(loggedIn);
        else if (c == 0) break;
        else cout << "Invalid choice!\n";
    }
}

void adminMenu(AppointmentQueue& appts, MedicineInventory& meds, EmergencySystem& emergencies) {
    if (!adminLogin()) {
        return;
    }

    while (true) {
        cout << "\n--- ADMIN / MEDICAL STAFF PORTAL ---\n"
             << "1. View Appointments\n"
             << "2. Cancel Appointment\n"
             << "3. View Medicines\n"
             << "4. Add Medicine\n"
             << "5. Restock Medicine\n"
             << "6. View Medicine Requests\n"
             << "7. Approve / Reject Medicine Request\n"
             << "8. Emergency & Ambulance Dispatch Console\n"
             << "9. Sanitary Pad Dispatch (Staff Portal)\n"
             << "10. Student & Doctor Management\n"
             << "0. Back\n";

        int c = readNumber("Choice: ");
        if (c == 1) appts.view();
        else if (c == 2) appts.cancel();
        else if (c == 3) meds.view(true);
        else if (c == 4) meds.addMedicine();
        else if (c == 5) meds.restockMedicine();
        else if (c == 6) meds.viewRequests();
        else if (c == 7) meds.handleRequest();
        else if (c == 8) emergencies.adminMenu();
        else if (c == 9) runPadDispatchStaff();
        else if (c == 10) Member1Module::adminMenu();
        else if (c == 0) break;
        else cout << "Invalid choice!\n";
    }
}

int main() {
    initMember1Directory();

    AppointmentQueue appts;
    MedicineInventory meds;
    EmergencySystem emergencies;

    while (true) {
        cout << "\n=============================================\n"
             << "   UNI-CARE : CAMPUS HEALTH & EMERGENCY HUB\n"
             << "=============================================\n"
             << "1. Student Login\n"
             << "2. Student Registration\n"
             << "3. Doctor Registration\n"
             << "4. Admin / Medical Staff Portal\n"
             << "0. Exit\n";

        int c = readNumber("Choice: ");
        if (c == 1) {
            Member1Module::Student* s = Member1Module::authenticateStudentSession();
            if (s != NULL) {
                studentMenu(appts, meds, emergencies, s);
            }
        }
        else if (c == 2) Member1Module::registerStudent(false);
        else if (c == 3) {
            if (adminLogin()) {
                Member1Module::addDoctor();
            }
        }
        else if (c == 4) adminMenu(appts, meds, emergencies);
        else if (c == 0) {
            Member1Module::saveAll();
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice!\n";
        }
    }
    return 0;
}
