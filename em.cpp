#include "em.h"

static string trimSpaces(string s) {
    int start = 0;
    while (start < (int)s.length() &&
           (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n')) {
        start++;
    }
    int end = (int)s.length() - 1;
    while (end >= start &&
           (s[end] == ' ' || s[end] == '\t' || s[end] == '\r' || s[end] == '\n')) {
        end--;
    }
    if (start > end) {
        return "";
    }
    return s.substr(start, end - start + 1);
}

static string toLowerStr(string s) {
    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] >= 'A' && s[i] <= 'Z') {
            s[i] = (char)(s[i] - 'A' + 'a');
        }
    }
    return s;
}

static bool hasComma(string s) {
    return s.find(',') != string::npos;
}

string findRegisteredStudentName(string input) {
    string clean = trimSpaces(input);
    if (clean.empty()) {
        return "";
    }
    string cleanLower = toLowerStr(clean);
    ifstream file("students.txt");
    if (!file.is_open()) {
        return "";
    }
    string line;
    while (getline(file, line)) {
        line = trimSpaces(line);
        if (line.empty()) {
            continue;
        }
        stringstream ss(line);
        string roll, name;
        getline(ss, roll, ',');
        getline(ss, name, ',');
        name = trimSpaces(name);
        if (!name.empty() && toLowerStr(name) == cleanLower) {
            return name;
        }
    }
    return "";
}

static string readText(string prompt) {
    string s;
    cout << prompt;
    if (!getline(cin, s)) {
        return "";
    }
    return trimSpaces(s);
}

static int readNumber(string prompt) {
    string s = readText(prompt);
    if (s.empty()) {
        return -1;
    }
    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] < '0' || s[i] > '9') {
            return -1;
        }
    }
    return atoi(s.c_str());
}



static double readDouble(string prompt) {
    string s = readText(prompt);
    if (s.empty()) {
        return -1.0;
    }
    int dots = 0;
    int digits = 0;
    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] == '.') {
            dots++;
            if (dots > 1) {
                return -1.0;
            }
        } else if (s[i] >= '0' && s[i] <= '9') {
            digits++;
        } else {
            return -1.0;
        }
    }
    if (digits == 0) {
        return -1.0;
    }
    return atof(s.c_str());
}

static string intToText(int value) {
    stringstream ss;
    ss << value;
    return ss.str();
}

static string getCurrentTimestamp() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    if (ltm == NULL) {
        return "Unknown";
    }
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ltm);
    return string(buf);
}

EmergencyRequest::EmergencyRequest() {
    requestId = 0;
    studentName = "";
    location = "";
    description = "";
    severity = 3;
    status = "Pending";
    assignedAmbulance = "None";
    timestamp = new string(getCurrentTimestamp());
}

EmergencyRequest::EmergencyRequest(int id, string name, string loc, string desc,
                                   int sev, string stat, string amb, string ts) {
    requestId = id;
    studentName = name;
    location = loc;
    description = desc;
    severity = (sev >= 1 && sev <= 3) ? sev : 3;
    status = stat.empty() ? "Pending" : stat;
    assignedAmbulance = amb.empty() ? "None" : amb;
    timestamp = new string(ts.empty() ? getCurrentTimestamp() : ts);
}

EmergencyRequest::EmergencyRequest(const EmergencyRequest& other) {
    requestId = other.requestId;
    studentName = other.studentName;
    location = other.location;
    description = other.description;
    severity = other.severity;
    status = other.status;
    assignedAmbulance = other.assignedAmbulance;
    timestamp = new string(other.timestamp != NULL ? *other.timestamp : getCurrentTimestamp());
}

EmergencyRequest& EmergencyRequest::operator=(const EmergencyRequest& other) {
    if (this == &other) {
        return *this;
    }
    requestId = other.requestId;
    studentName = other.studentName;
    location = other.location;
    description = other.description;
    severity = other.severity;
    status = other.status;
    assignedAmbulance = other.assignedAmbulance;
    string* newTs = new string(other.timestamp != NULL ? *other.timestamp : getCurrentTimestamp());
    delete timestamp;
    timestamp = newTs;
    return *this;
}

EmergencyRequest::~EmergencyRequest() {
    delete timestamp;
    timestamp = NULL;
}

int EmergencyRequest::getId() { return requestId; }
string EmergencyRequest::getStudentName() { return studentName; }
string EmergencyRequest::getLocation() { return location; }
string EmergencyRequest::getDescription() { return description; }
int EmergencyRequest::getSeverity() { return severity; }
string EmergencyRequest::getStatus() { return status; }
string EmergencyRequest::getAssignedAmbulance() { return assignedAmbulance; }
string EmergencyRequest::getTimestamp() { return timestamp != NULL ? *timestamp : ""; }

void EmergencyRequest::setStatus(string newStatus) { status = newStatus; }
void EmergencyRequest::setAssignedAmbulance(string amb) { assignedAmbulance = amb; }

string EmergencyRequest::getSeverityName() {
    if (severity == 1) return "CRITICAL";
    if (severity == 2) return "MODERATE";
    if (severity == 3) return "MINOR";
    return "UNKNOWN";
}

void EmergencyRequest::display() {
    cout << " [#" << requestId << "] [" << getSeverityName() << "]"
         << " | Student: " << studentName
         << " | Loc: " << location
         << " | Issue: " << description
         << " | Status: " << status
         << " (" << assignedAmbulance << ")"
         << " | Time: " << getTimestamp() << endl;
}

Vehicle::Vehicle() {
    vehicleId = 0;
    vehicleNumber = "";
}

Vehicle::Vehicle(int id, string vNum) {
    vehicleId = id;
    vehicleNumber = vNum;
}

int Vehicle::getId() { return vehicleId; }
string Vehicle::getNumber() { return vehicleNumber; }

Ambulance::Ambulance() : Vehicle() {
    driverName = "";
    status = "Available";
    currentDestination = "Campus Base";
    distanceKm = 1.0;
}

Ambulance::Ambulance(int id, string vNum, string driver, string stat, string dest, double distKm)
    : Vehicle(id, vNum) {
    driverName = driver;
    status = stat.empty() ? "Available" : stat;
    currentDestination = dest.empty() ? "Campus Base" : dest;
    distanceKm = distKm >= 0.0 ? distKm : 1.0;
}

string Ambulance::getDriverName() { return driverName; }
string Ambulance::getStatus() { return status; }
string Ambulance::getDestination() { return currentDestination; }
double Ambulance::getDistanceKm() { return distanceKm; }

void Ambulance::setDistanceKm(double distKm) {
    if (distKm >= 0.0) {
        distanceKm = distKm;
    }
}

string Ambulance::getAssignedLabel() {
    return "Ambulance #" + intToText(vehicleId) + " (" + vehicleNumber + ")";
}

bool Ambulance::isAvailable() {
    return status == "Available";
}

void Ambulance::dispatch(string dest) {
    status = "Dispatched";
    currentDestination = dest;
}

void Ambulance::returnToBase() {
    status = "Available";
    currentDestination = "Campus Base";
}

void Ambulance::display() {
    cout << " [Amb #" << vehicleId << "] " << vehicleNumber
         << " | Driver: " << driverName
         << " | Dist: " << distanceKm << " km"
         << " | Status: " << status
         << " | Location: " << currentDestination << endl;
}

PriorityEmergencyQueue::PriorityEmergencyQueue() {
    for (int i = 0; i < MAX_EMERG; i++) {
        requests[i] = NULL;
    }
    count = 0;
    currentId = 501;
    loadFromFile();
}

PriorityEmergencyQueue::~PriorityEmergencyQueue() {
    clear();
}

void PriorityEmergencyQueue::clear() {
    for (int i = 0; i < count; i++) {
        delete requests[i];
        requests[i] = NULL;
    }
    count = 0;
}

bool PriorityEmergencyQueue::isFull() { return count >= MAX_EMERG; }
bool PriorityEmergencyQueue::isEmpty() { return count == 0; }
int PriorityEmergencyQueue::getCount() { return count; }

void PriorityEmergencyQueue::insertSorted(EmergencyRequest* newReq) {
    if (newReq == NULL) {
        return;
    }
    if (isFull()) {
        delete newReq;
        return;
    }

    int i = count - 1;
    while (i >= 0 && requests[i]->getSeverity() > newReq->getSeverity()) {
        requests[i + 1] = requests[i];
        i--;
    }
    requests[i + 1] = newReq;
    count++;
}

void PriorityEmergencyQueue::loadFromFile() {
    clear();
    currentId = 501;

    ifstream file(EMERGENCY_FILE);
    if (!file.is_open()) {
        return;
    }

    string line;
    while (getline(file, line)) {
        line = trimSpaces(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        stringstream ss(line);
        string parts[8];
        int partCount = 0;
        while (partCount < 8 && getline(ss, parts[partCount], ',')) {
            parts[partCount] = trimSpaces(parts[partCount]);
            partCount++;
        }

        if (partCount < 7) {
            continue;
        }

        int id = atoi(parts[0].c_str());
        int sev = atoi(parts[4].c_str());
        if (id <= 0) {
            continue;
        }

        string ts = (partCount >= 8 && !parts[7].empty()) ? parts[7] : "Recorded";
        EmergencyRequest* req = new EmergencyRequest(id, parts[1], parts[2], parts[3], sev, parts[5], parts[6], ts);
        insertSorted(req);

        if (id >= currentId) {
            currentId = id + 1;
        }
    }
    file.close();
}

void PriorityEmergencyQueue::saveToFile() {
    ofstream file(EMERGENCY_FILE);
    if (!file.is_open()) {
        cout << "Error: Could not save emergency records.\n";
        return;
    }

    for (int i = 0; i < count; i++) {
        file << requests[i]->getId() << ","
             << requests[i]->getStudentName() << ","
             << requests[i]->getLocation() << ","
             << requests[i]->getDescription() << ","
             << requests[i]->getSeverity() << ","
             << requests[i]->getStatus() << ","
             << requests[i]->getAssignedAmbulance() << ","
             << requests[i]->getTimestamp() << "\n";
    }
    file.close();
}

int PriorityEmergencyQueue::addEmergency(string studentName, string location, string desc, int severity) {
    if (isFull()) {
        cout << "Error: Emergency queue is full! Maximum capacity reached.\n";
        return -1;
    }

    int assignedId = currentId++;
    EmergencyRequest* newReq = new EmergencyRequest(assignedId, studentName, location, desc, severity, "Pending", "None");
    insertSorted(newReq);
    saveToFile();
    return assignedId;
}

EmergencyRequest* PriorityEmergencyQueue::getTopPendingRequest() {
    for (int i = 0; i < count; i++) {
        if (requests[i]->getStatus() == "Pending") {
            return requests[i];
        }
    }
    return NULL;
}

EmergencyRequest* PriorityEmergencyQueue::findById(int id) {
    for (int i = 0; i < count; i++) {
        if (requests[i]->getId() == id) {
            return requests[i];
        }
    }
    return NULL;
}

bool PriorityEmergencyQueue::removeById(int id) {
    int idx = -1;
    for (int i = 0; i < count; i++) {
        if (requests[i]->getId() == id) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        return false;
    }

    delete requests[idx];
    for (int i = idx; i < count - 1; i++) {
        requests[i] = requests[i + 1];
    }
    requests[count - 1] = NULL;
    count--;
    saveToFile();
    return true;
}

void PriorityEmergencyQueue::resolveTop() {
    if (isEmpty()) {
        cout << "No emergencies in queue to resolve.\n";
        return;
    }

    int resolvedId = requests[0]->getId();
    string resolvedStudent = requests[0]->getStudentName();
    delete requests[0];
    for (int i = 0; i < count - 1; i++) {
        requests[i] = requests[i + 1];
    }
    requests[count - 1] = NULL;
    count--;
    saveToFile();

    cout << "Resolved Emergency #" << resolvedId
         << " for student " << resolvedStudent << ".\n";
}

int PriorityEmergencyQueue::markAmbulanceReturned(string ambulanceLabel) {
    int updated = 0;
    for (int i = 0; i < count; i++) {
        if (requests[i]->getStatus() == "Ambulance Dispatched" &&
            requests[i]->getAssignedAmbulance() == ambulanceLabel) {
            requests[i]->setStatus("Patient Transported to Clinic");
            updated++;
        }
    }
    if (updated > 0) {
        saveToFile();
    }
    return updated;
}

bool PriorityEmergencyQueue::hasActiveDispatchForAmbulance(string ambulanceLabel) {
    for (int i = 0; i < count; i++) {
        if (requests[i]->getStatus() == "Ambulance Dispatched" &&
            requests[i]->getAssignedAmbulance() == ambulanceLabel) {
            return true;
        }
    }
    return false;
}

void PriorityEmergencyQueue::displayForStudent(string query, bool exactName) {
    string qLower = toLowerStr(trimSpaces(query));
    if (qLower.empty()) {
        cout << "Please enter a valid Request ID or Name.\n";
        return;
    }

    bool isNumeric = true;
    for (int i = 0; i < (int)qLower.length(); i++) {
        if (qLower[i] < '0' || qLower[i] > '9') {
            isNumeric = false;
            break;
        }
    }
    int searchId = isNumeric ? atoi(qLower.c_str()) : -1;

    int matches = 0;
    cout << "\n--- Matching Emergency Requests ---\n";
    for (int i = 0; i < count; i++) {
        bool hit = false;
        string recLower = toLowerStr(requests[i]->getStudentName());
        if (!exactName && isNumeric && requests[i]->getId() == searchId) {
            hit = true;
        } else if (exactName && recLower == qLower) {
            hit = true;
        } else if (!exactName && !isNumeric && recLower.find(qLower) != string::npos) {
            hit = true;
        }
        if (hit) {
            requests[i]->display();
            matches++;
        }
    }
    if (matches == 0) {
        cout << "No matching emergency request found.\n";
    }
    cout << "-----------------------------------\n";
}

void PriorityEmergencyQueue::displayAll() {
    cout << "\n==========================================================\n";
    cout << "      PRIORITY EMERGENCY(" << count << " total)\n";
    cout << "==========================================================\n";
    if (isEmpty()) {
        cout << " No emergency requests currently recorded.\n";
    } else {
        for (int i = 0; i < count; i++) {
            requests[i]->display();
        }
    }
    cout << "==========================================================\n";
}

AmbulanceFleet::AmbulanceFleet() {
    totalAmbulances = 0;
    loadFromFile();
}

int AmbulanceFleet::partition(int low, int high) {
    double pivotDist = fleet[high].getDistanceKm();
    int pivotId = fleet[high].getId();
    int i = low - 1;

    for (int j = low; j < high; j++) {
        double dist = fleet[j].getDistanceKm();
        if (dist < pivotDist || (dist == pivotDist && fleet[j].getId() < pivotId)) {
            i++;
            Ambulance temp = fleet[i];
            fleet[i] = fleet[j];
            fleet[j] = temp;
        }
    }

    Ambulance temp = fleet[i + 1];
    fleet[i + 1] = fleet[high];
    fleet[high] = temp;
    return i + 1;
}

void AmbulanceFleet::quickSort(int low, int high) {
    if (low < high) {
        int pi = partition(low, high);
        quickSort(low, pi - 1);
        quickSort(pi + 1, high);
    }
}

void AmbulanceFleet::sortByProximityQuickSort() {
    if (totalAmbulances > 1) {
        quickSort(0, totalAmbulances - 1);
    }
}

void AmbulanceFleet::loadFromFile() {
    totalAmbulances = 0;
    ifstream file(AMBULANCE_FILE);

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            line = trimSpaces(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }

            stringstream ss(line);
            string parts[6];
            int partCount = 0;
            while (partCount < 6 && getline(ss, parts[partCount], ',')) {
                parts[partCount] = trimSpaces(parts[partCount]);
                partCount++;
            }

            if (partCount < 5 || totalAmbulances >= MAX_AMB) {
                continue;
            }

            int id = atoi(parts[0].c_str());
            if (id <= 0) {
                continue;
            }

            double dist = 1.0;
            if (partCount >= 6 && !parts[5].empty()) {
                dist = atof(parts[5].c_str());
            } else {
                if (id == 1) dist = 1.8;
                else if (id == 2) dist = 0.6;
                else if (id == 3) dist = 1.2;
            }

            fleet[totalAmbulances++] = Ambulance(id, parts[1], parts[2], parts[3], parts[4], dist);
        }
        file.close();
    }

    if (totalAmbulances == 0) {
        fleet[totalAmbulances++] = Ambulance(1, "DL-01-AMB-101", "Ramesh Kumar", "Available", "Campus Base", 1.8);
        fleet[totalAmbulances++] = Ambulance(2, "DL-01-AMB-102", "Suresh Singh", "Available", "Campus Base", 0.6);
        fleet[totalAmbulances++] = Ambulance(3, "DL-01-AMB-103", "Vikram Rathore", "Available", "Campus Base", 1.2);
        saveToFile();
    }
}

void AmbulanceFleet::saveToFile() {
    ofstream file(AMBULANCE_FILE);
    if (!file.is_open()) {
        cout << "Error: Could not save ambulance fleet data.\n";
        return;
    }

    for (int i = 0; i < totalAmbulances; i++) {
        file << fleet[i].getId() << ","
             << fleet[i].getNumber() << ","
             << fleet[i].getDriverName() << ","
             << fleet[i].getStatus() << ","
             << fleet[i].getDestination() << ","
             << fleet[i].getDistanceKm() << "\n";
    }
    file.close();
}

int AmbulanceFleet::getTotalAmbulances() { return totalAmbulances; }

int AmbulanceFleet::getAvailableCount() {
    int avail = 0;
    for (int i = 0; i < totalAmbulances; i++) {
        if (fleet[i].isAvailable()) {
            avail++;
        }
    }
    return avail;
}

int AmbulanceFleet::getDispatchedCount() {
    return totalAmbulances - getAvailableCount();
}

Ambulance* AmbulanceFleet::getAvailableAmbulance() {
    sortByProximityQuickSort();
    for (int i = 0; i < totalAmbulances; i++) {
        if (fleet[i].isAvailable()) {
            return &fleet[i];
        }
    }
    return NULL;
}

Ambulance* AmbulanceFleet::getAt(int index) {
    if (index < 0 || index >= totalAmbulances) {
        return NULL;
    }
    return &fleet[index];
}

Ambulance* AmbulanceFleet::findByQuery(string query) {
    string q = trimSpaces(query);
    string qLower = toLowerStr(q);

    for (int i = 0; i < totalAmbulances; i++) {
        if (intToText(fleet[i].getId()) == q ||
            toLowerStr(fleet[i].getNumber()) == qLower ||
            toLowerStr(fleet[i].getAssignedLabel()) == qLower) {
            return &fleet[i];
        }
    }
    return NULL;
}

void AmbulanceFleet::returnAmbulance(int id) {
    for (int i = 0; i < totalAmbulances; i++) {
        if (fleet[i].getId() == id) {
            if (!fleet[i].isAvailable()) {
                fleet[i].returnToBase();
                saveToFile();
                cout << "Ambulance #" << id << " (" << fleet[i].getNumber()
                     << ") has returned to Campus Base and is now Available.\n";
            } else {
                cout << "Ambulance #" << id << " is already Available at Campus Base.\n";
            }
            return;
        }
    }
    cout << "Ambulance ID #" << id << " not found.\n";
}

bool AmbulanceFleet::addAmbulance(string vehicleNumber, string driverName, double distanceKm) {
    if (totalAmbulances >= MAX_AMB) {
        cout << "Ambulance fleet is at maximum capacity (" << MAX_AMB << ").\n";
        return false;
    }

    string plateLower = toLowerStr(trimSpaces(vehicleNumber));
    for (int i = 0; i < totalAmbulances; i++) {
        if (toLowerStr(fleet[i].getNumber()) == plateLower) {
            cout << "An ambulance with vehicle number " << vehicleNumber << " already exists.\n";
            return false;
        }
    }

    int nextId = 1;
    for (int i = 0; i < totalAmbulances; i++) {
        if (fleet[i].getId() >= nextId) {
            nextId = fleet[i].getId() + 1;
        }
    }

    fleet[totalAmbulances++] = Ambulance(nextId, vehicleNumber, driverName, "Available", "Campus Base", distanceKm);
    sortByProximityQuickSort();
    saveToFile();
    cout << "Added Ambulance #" << nextId << " (" << vehicleNumber
         << ") - Driver: " << driverName << " | Dist: " << distanceKm << " km\n";
    return true;
}

void AmbulanceFleet::displayFleet() {
    sortByProximityQuickSort();
    cout << "\n----------------------------------------------------------\n";
    cout << "    CAMPUS AMBULANCE FLEET STATUS (" << getAvailableCount()
         << "/" << totalAmbulances << " Available, Sorted Nearest First)\n";
    cout << "----------------------------------------------------------\n";
    for (int i = 0; i < totalAmbulances; i++) {
        fleet[i].display();
    }
    cout << "----------------------------------------------------------\n";
}

void AmbulanceFleet::displayDispatched() {
    sortByProximityQuickSort();
    cout << "\n--- Currently Dispatched Ambulances ---\n";
    int shown = 0;
    for (int i = 0; i < totalAmbulances; i++) {
        if (!fleet[i].isAvailable()) {
            fleet[i].display();
            shown++;
        }
    }
    if (shown == 0) {
        cout << " None currently dispatched.\n";
    }
    cout << "---------------------------------------\n";
}

EmergencySystem::EmergencySystem() {
}

void EmergencySystem::reportEmergency(string studentName) {
    if (queue.isFull()) {
        cout << "Emergency queue is full! Please call campus medical helpline directly.\n";
        return;
    }

    cout << "\n--- REPORT MEDICAL EMERGENCY ---\n";
    string name = studentName.empty() ? readText("Student Name: ") : studentName;
    if (name.empty() || hasComma(name)) {
        cout << "Invalid name (cannot be empty or contain ',').\n";
        return;
    }
    string registeredStudent = findRegisteredStudentName(name);
    if (registeredStudent.empty()) {
        cout << "Student not found in students.txt. Only registered students can report an emergency.\n";
        return;
    }
    name = registeredStudent;
    if (!studentName.empty()) {
        cout << "Student: " << name << "\n";
    }

    string loc = readText("Exact Campus Location (e.g. Hostel 3 / Library): ");
    if (loc.empty() || hasComma(loc)) {
        cout << "Invalid location (cannot be empty or contain ',').\n";
        return;
    }

    string desc = readText("Nature of Emergency (symptoms / injury): ");
    if (desc.empty() || hasComma(desc)) {
        cout << "Invalid description (cannot be empty or contain ',').\n";
        return;
    }

    cout << "Severity Level:\n"
         << "  1. CRITICAL (Unconscious, severe bleeding, chest pain)\n"
         << "  2. MODERATE (Fracture, deep cut, high fever)\n"
         << "  3. MINOR (Dizziness, sprain, mild injury)\n";
    int sev = readNumber("Select Severity (1-3): ");
    if (sev < 1 || sev > 3) {
        cout << "Invalid severity level! Please select 1, 2, or 3.\n";
        return;
    }

    int newId = queue.addEmergency(name, loc, desc, sev);
    if (newId > 0) {
        cout << "\nEMERGENCY ALERT LOGGED!\n";
        cout << "Request ID: #" << newId << " (keep this ID to track status)\n";
        cout << "Available Campus Ambulances: " << fleet.getAvailableCount()
             << "/" << fleet.getTotalAmbulances() << "\n";
        if (sev == 1) {
            cout << "CRITICAL priority assigned - placed at top of triage queue!\n";
        }
    }
}

void EmergencySystem::viewStudentStatus(string studentName) {
    if (queue.isEmpty()) {
        cout << "\nNo emergency requests currently in system.\n";
        return;
    }
    if (!studentName.empty()) {
        queue.displayForStudent(studentName, true);
        return;
    }
    string query = readText("\nEnter your Request ID (e.g., 502) or Student Name: ");
    queue.displayForStudent(query, false);
}

void EmergencySystem::viewAll() {
    queue.displayAll();
}

void EmergencySystem::viewFleet() {
    fleet.displayFleet();
}

void EmergencySystem::dispatchTop() {
    EmergencyRequest* top = queue.getTopPendingRequest();
    if (top == NULL) {
        cout << "\nNo pending emergency requests waiting for dispatch.\n";
        return;
    }

    Ambulance* amb = fleet.getAvailableAmbulance();
    if (amb == NULL) {
        cout << "\nALERT: All campus ambulances are currently dispatched!\n";
        return;
    }

    amb->dispatch(top->getLocation());
    top->setStatus("Ambulance Dispatched");
    top->setAssignedAmbulance(amb->getAssignedLabel());

    fleet.saveToFile();
    queue.saveToFile();

    cout << "\nDISPATCH CONFIRMED (Closest Available Selected via Quick Sort)!\n";
    cout << " Emergency #" << top->getId() << " (" << top->getSeverityName() << ") - "
         << top->getStudentName() << " at " << top->getLocation() << "\n";
    cout << " Assigned: " << amb->getAssignedLabel()
         << " | Driver: " << amb->getDriverName()
         << " | Proximity: " << amb->getDistanceKm() << " km\n";
}

void EmergencySystem::returnAmbulance() {
    if (fleet.getDispatchedCount() == 0) {
        cout << "\nAll ambulances are already Available at Campus Base.\n";
        return;
    }

    fleet.displayDispatched();
    string query = readText("Enter Ambulance ID, Vehicle Number, or 'all' (0 to cancel): ");
    if (query.empty() || query == "0") {
        return;
    }

    if (toLowerStr(query) == "all") {
        int returnedCount = 0;
        int transportedCount = 0;
        for (int i = 0; i < fleet.getTotalAmbulances(); i++) {
            Ambulance* a = fleet.getAt(i);
            if (a != NULL && !a->isAvailable()) {
                string label = a->getAssignedLabel();
                a->returnToBase();
                transportedCount += queue.markAmbulanceReturned(label);
                returnedCount++;
            }
        }
        fleet.saveToFile();
        cout << "\nReturned " << returnedCount << " ambulance(s) to Campus Base.\n";
        if (transportedCount > 0) {
            cout << "Updated " << transportedCount
                 << " emergency request(s) to 'Patient Transported to Clinic'.\n";
        }
        return;
    }

    Ambulance* amb = fleet.findByQuery(query);
    if (amb == NULL) {
        cout << "No ambulance matching '" << query << "' found.\n";
        return;
    }
    if (amb->isAvailable()) {
        cout << amb->getAssignedLabel() << " is already Available at Campus Base.\n";
        return;
    }

    string label = amb->getAssignedLabel();
    amb->returnToBase();
    fleet.saveToFile();
    int updated = queue.markAmbulanceReturned(label);

    cout << "\n" << label << " marked Available at Campus Base.\n";
    if (updated > 0) {
        cout << "Updated " << updated
             << " linked emergency request(s) to 'Patient Transported to Clinic'.\n";
    }
}

void EmergencySystem::resolveEmergency() {
    if (queue.isEmpty()) {
        cout << "\nNo emergency requests to resolve.\n";
        return;
    }

    queue.displayAll();
    int id = readNumber("Enter Request ID to resolve/remove (0 to cancel): ");
    if (id <= 0) {
        return;
    }

    EmergencyRequest* target = queue.findById(id);
    if (target == NULL) {
        cout << "Emergency Request #" << id << " not found.\n";
        return;
    }

    string oldStatus = target->getStatus();
    string ambLabel = target->getAssignedAmbulance();
    string student = target->getStudentName();

    if (queue.removeById(id)) {
        cout << "Resolved and removed Emergency #" << id << " (" << student << ").\n";
        if (oldStatus == "Ambulance Dispatched" &&
            !queue.hasActiveDispatchForAmbulance(ambLabel)) {
            Ambulance* amb = fleet.findByQuery(ambLabel);
            if (amb != NULL && !amb->isAvailable()) {
                amb->returnToBase();
                fleet.saveToFile();
                cout << "Auto-returned " << ambLabel << " to Available at Campus Base.\n";
            }
        }
    }
}

void EmergencySystem::addAmbulance() {
    cout << "\n--- ADD CAMPUS AMBULANCE ---\n";
    string plate = readText("Vehicle Number (e.g. DL-01-AMB-104): ");
    if (plate.empty() || hasComma(plate)) {
        cout << "Invalid vehicle number (cannot be empty or contain ',').\n";
        return;
    }

    string driver = readText("Driver Name: ");
    if (driver.empty() || hasComma(driver)) {
        cout << "Invalid driver name (cannot be empty or contain ',').\n";
        return;
    }

    double dist = readDouble("Current Distance from Campus Base (km, e.g., 0.5): ");
    if (dist < 0.0) {
        cout << "Invalid distance.\n";
        return;
    }

    fleet.addAmbulance(plate, driver, dist);
}

void EmergencySystem::adminMenu() {
    while (true) {
        cout << "\n--- EMERGENCY & AMBULANCE DISPATCH CONSOLE ---\n"
             << "1. View Emergency Triage Queue\n"
             << "2. View Ambulance Fleet Status\n"
             << "3. Dispatch Nearest Ambulance to Top Priority\n"
             << "4. Mark Ambulance Returned to Base\n"
             << "5. Resolve / Remove Emergency Request\n"
             << "6. Add New Ambulance to Fleet\n"
             << "0. Back\n";

        int choice = readNumber("Choice: ");
        if (choice == 1) viewAll();
        else if (choice == 2) viewFleet();
        else if (choice == 3) dispatchTop();
        else if (choice == 4) returnAmbulance();
        else if (choice == 5) resolveEmergency();
        else if (choice == 6) addAmbulance();
        else if (choice == 0) break;
        else cout << "Invalid choice!\n";
    }
}

void emergencyMenu() {
    EmergencySystem sys;
    while (true) {
        cout << "\n=============================================\n";
        cout << "     UNICARE - EMERGENCY & AMBULANCE MODULE  \n";
        cout << "=============================================\n";
        cout << "1. Student: Report Medical Emergency\n";
        cout << "2. Student: Check My Emergency Status\n";
        cout << "3. Admin: Open Emergency Dispatch Console\n";
        cout << "0. Back / Exit\n";

        int choice = readNumber("Choice: ");
        if (choice == 1) sys.reportEmergency();
        else if (choice == 2) sys.viewStudentStatus();
        else if (choice == 3) sys.adminMenu();
        else if (choice == 0) break;
        else cout << "Invalid choice!\n";
    }
}
