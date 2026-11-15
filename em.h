#ifndef EM_H
#define EM_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <stdlib.h>

using namespace std;

#define MAX_EMERG 50
#define MAX_AMB 20
#define EMERGENCY_FILE "emergencies.txt"
#define AMBULANCE_FILE "ambulances.txt"

class EmergencyRequest {
private:
    int requestId;
    string studentName;
    string location;
    string description;
    int severity;
    string status;
    string assignedAmbulance;
    string* timestamp;

public:
    EmergencyRequest();
    EmergencyRequest(int id, string name, string loc, string desc, int sev,
                     string stat = "Pending", string amb = "None", string ts = "");
    EmergencyRequest(const EmergencyRequest& other);
    EmergencyRequest& operator=(const EmergencyRequest& other);
    ~EmergencyRequest();

    int getId();
    string getStudentName();
    string getLocation();
    string getDescription();
    int getSeverity();
    string getStatus();
    string getAssignedAmbulance();
    string getTimestamp();

    void setStatus(string newStatus);
    void setAssignedAmbulance(string amb);

    string getSeverityName();
    void display();
};

class Vehicle {
protected:
    int vehicleId;
    string vehicleNumber;

public:
    Vehicle();
    Vehicle(int id, string vNum);

    int getId();
    string getNumber();
};

class Ambulance : public Vehicle {
private:
    string driverName;
    string status;
    string currentDestination;
    double distanceKm;

public:
    Ambulance();
    Ambulance(int id, string vNum, string driver,
              string stat = "Available", string dest = "Campus Base", double distKm = 1.0);

    string getDriverName();
    string getStatus();
    string getDestination();
    string getAssignedLabel();
    double getDistanceKm();
    void setDistanceKm(double distKm);

    bool isAvailable();
    void dispatch(string dest);
    void returnToBase();
    void display();
};

class PriorityEmergencyQueue {
private:
    EmergencyRequest* requests[MAX_EMERG];
    int count;
    int currentId;

    void clear();
    void insertSorted(EmergencyRequest* newReq);

public:
    PriorityEmergencyQueue();
    ~PriorityEmergencyQueue();

    bool isFull();
    bool isEmpty();
    int getCount();

    void loadFromFile();
    void saveToFile();

    int addEmergency(string studentName, string location, string desc, int severity);
    EmergencyRequest* getTopPendingRequest();
    EmergencyRequest* findById(int id);
    bool removeById(int id);
    void resolveTop();
    int markAmbulanceReturned(string ambulanceLabel);
    bool hasActiveDispatchForAmbulance(string ambulanceLabel);
    void displayForStudent(string query, bool exactName = false);
    void displayAll();
};

class AmbulanceFleet {
private:
    Ambulance fleet[MAX_AMB];
    int totalAmbulances;

    int partition(int low, int high);
    void quickSort(int low, int high);

public:
    AmbulanceFleet();

    void loadFromFile();
    void saveToFile();

    int getTotalAmbulances();
    int getAvailableCount();
    int getDispatchedCount();

    void sortByProximityQuickSort();
    Ambulance* getAvailableAmbulance();
    Ambulance* getAt(int index);
    Ambulance* findByQuery(string query);
    void returnAmbulance(int id);
    bool addAmbulance(string vehicleNumber, string driverName, double distanceKm = 1.0);

    void displayFleet();
    void displayDispatched();
};

class EmergencySystem {
private:
    PriorityEmergencyQueue queue;
    AmbulanceFleet fleet;

public:
    EmergencySystem();

    void reportEmergency(string studentName = "");
    void viewStudentStatus(string studentName = "");

    void viewAll();
    void viewFleet();
    void dispatchTop();
    void returnAmbulance();
    void resolveEmergency();
    void addAmbulance();
    void adminMenu();
};

string findRegisteredStudentName(string input);
void emergencyMenu();

#endif
