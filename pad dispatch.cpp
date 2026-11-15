#include <algorithm>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

const string REQUEST_FILE = "pad_requests.txt";
const string LOCATION_FILE = "washrooms.txt";
const string STAFF_FILE = "staff.txt";

vector<string> splitLine(const string& line, char separator) {
    vector<string> parts;
    string part;
    stringstream stream(line);
    while (getline(stream, part, separator)) parts.push_back(part);
    return parts;
}

bool hasBadCharacter(const string& text) {
    return text.find(',') != string::npos || text.find('\n') != string::npos ||
           text.find('\r') != string::npos;
}

string formatTime(long seconds) {
    if (seconds <= 0) return "unknown time";
    time_t value = static_cast<time_t>(seconds);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M", localtime(&value));
    return buffer;
}

string readLine(const string& prompt) {
    cout << prompt;
    string answer;
    if (!getline(cin, answer)) {
        cout << "\nInput closed.\n";
        exit(0);
    }
    return answer;
}

int readNumber(const string& prompt, int minimum, int maximum) {
    while (true) {
        string text = readLine(prompt);
        try {
            size_t used;
            int number = stoi(text, &used);
            if (used == text.size() && number >= minimum && number <= maximum)
                return number;
        } catch (...) {
        }
        cout << "Please enter a number from " << minimum << " to " << maximum << ".\n";
    }
}

bool validStudentId(const string& id) {
    if (id.empty() || hasBadCharacter(id)) return false;
    for (char ch : id) {
        if (ch == '|' || isspace(static_cast<unsigned char>(ch))) return false;
    }
    return true;
}

enum class Status { Pending, Accepted, Dispatched, Delivered, Cancelled, Failed };

string statusName(Status status) {
    switch (status) {
        case Status::Pending: return "PENDING";
        case Status::Accepted: return "ACCEPTED";
        case Status::Dispatched: return "DISPATCHED";
        case Status::Delivered: return "DELIVERED";
        case Status::Cancelled: return "CANCELLED";
        case Status::Failed: return "FAILED";
    }
    return "UNKNOWN";
}

Status readStatus(const string& name) {
    for (Status status : {Status::Pending, Status::Accepted, Status::Dispatched,
                          Status::Delivered, Status::Cancelled, Status::Failed}) {
        if (statusName(status) == name) return status;
    }
    throw invalid_argument("Unknown status: " + name);
}

class PadRequest {
private:
    int id;
    string studentId;
    string location;
    string staffId;
    Status status;
    long created;

public:
    PadRequest(int requestId, const string& student, const string& place,
               Status currentStatus = Status::Pending, const string& staff = "-",
               long creationTime = -1)
        : id(requestId), studentId(student), location(place), staffId(staff),
          status(currentStatus),
          created(creationTime < 0 ? static_cast<long>(time(nullptr)) : creationTime) {
        if (id <= 0) throw invalid_argument("Bad request ID");
        if (studentId.empty() || hasBadCharacter(studentId)) throw invalid_argument("Bad student ID");
        if (location.empty() || hasBadCharacter(location)) throw invalid_argument("Bad location");
        if (staffId.empty() || hasBadCharacter(staffId)) throw invalid_argument("Bad staff ID");
    }

    int getId() const { return id; }
    const string& getStudentId() const { return studentId; }
    const string& getLocation() const { return location; }
    Status getStatus() const { return status; }
    long getCreatedTime() const { return created; }

    bool isActive() const {
        return status == Status::Pending || status == Status::Accepted ||
               status == Status::Dispatched;
    }

    void assignStaff(const string& staff) {
        if (staff.empty() || hasBadCharacter(staff)) throw invalid_argument("Bad staff ID");
        staffId = staff;
    }

    void changeStatus(Status next) {
        bool allowed = false;
        if (status == Status::Pending)
            allowed = next == Status::Accepted || next == Status::Cancelled;
        else if (status == Status::Accepted)
            allowed = next == Status::Dispatched || next == Status::Cancelled;
        else if (status == Status::Dispatched)
            allowed = next == Status::Delivered || next == Status::Failed;

        if (!allowed)
            throw logic_error("Illegal status change: " + statusName(status) + " -> " + statusName(next));
        status = next;
    }

    string toLine() const {
        return to_string(id) + "," + studentId + "," + location + "," +
               statusName(status) + "," + staffId + "," + to_string(created);
    }

    static PadRequest fromLine(const string& line) {
        vector<string> fields = splitLine(line, ',');
        if (fields.size() != 5 && fields.size() != 6)
            throw invalid_argument("Malformed record");
        long timeCreated = fields.size() == 6 ? stol(fields[5]) : 0;
        return PadRequest(stoi(fields[0]), fields[1], fields[2], readStatus(fields[3]),
                          fields[4], timeCreated);
    }
};

class PadDispatch {
private:
    vector<PadRequest> requests;
    queue<int> pendingIds;
    vector<string> locations;
    int nextId = 1;

    int findIndex(int requestId) const {
        for (int i = 0; i < static_cast<int>(requests.size()); ++i) {
            if (requests[i].getId() == requestId) return i;
        }
        throw out_of_range("No request with ID " + to_string(requestId));
    }

    int nextPendingId() {
        while (!pendingIds.empty()) {
            int requestId = pendingIds.front();
            if (requests[findIndex(requestId)].getStatus() == Status::Pending)
                return requestId;
            pendingIds.pop();
        }
        return -1;
    }

    bool saveRequests() const {
        string temporaryFile = REQUEST_FILE + ".tmp";
        {
            ofstream output(temporaryFile);
            if (!output) {
                cerr << "Warning: cannot write " << temporaryFile << "\n";
                return false;
            }
            for (const PadRequest& request : requests) output << request.toLine() << "\n";
            if (!output) {
                cerr << "Warning: write failed\n";
                return false;
            }
        }
        remove(REQUEST_FILE.c_str());
        if (rename(temporaryFile.c_str(), REQUEST_FILE.c_str()) != 0) {
            cerr << "Warning: could not replace file\n";
            return false;
        }
        return true;
    }

    void loadLocations() {
        ifstream input(LOCATION_FILE);
        if (!input) {
            ofstream output(LOCATION_FILE);
            for (int floor = 1; floor <= 6; ++floor) {
                output << "Floor " << floor << " - Washroom A\n";
                output << "Floor " << floor << " - Washroom B\n";
            }
            output.close();
            input.open(LOCATION_FILE);
        }

        string line;
        while (getline(input, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (!line.empty() && !hasBadCharacter(line)) locations.push_back(line);
        }
        if (locations.empty()) locations.push_back("Medical Room");
    }

public:
    PadDispatch() {
        loadLocations();
        ifstream input(REQUEST_FILE);
        if (!input) return;

        string line;
        int largestId = 0;
        while (getline(input, line)) {
            if (line.empty()) continue;
            try {
                PadRequest request = PadRequest::fromLine(line);
                largestId = max(largestId, request.getId());
                requests.push_back(request);
            } catch (const exception& error) {
                cerr << "Skipping bad line: " << error.what() << "\n";
            }
        }
        nextId = largestId + 1;
        for (const PadRequest& request : requests) {
            if (request.getStatus() == Status::Pending) pendingIds.push(request.getId());
        }
    }

    const vector<string>& getLocations() const { return locations; }

    int createRequest(const string& student, const string& place) {
        if (find(locations.begin(), locations.end(), place) == locations.end())
            throw invalid_argument("Invalid washroom location");
        for (const PadRequest& request : requests) {
            if (request.getStudentId() == student && request.isActive())
                throw logic_error("You already have an active request (#" +
                                  to_string(request.getId()) + ")");
        }

        int requestId = nextId++;
        requests.push_back(PadRequest(requestId, student, place));
        pendingIds.push(requestId);
        saveRequests();
        return requestId;
    }

    string getStatus(int requestId, const string& student) const {
        const PadRequest& request = requests[findIndex(requestId)];
        if (request.getStudentId() != student) throw invalid_argument("That request is not yours");
        return statusName(request.getStatus());
    }

    void cancelRequest(int requestId, const string& student) {
        PadRequest& request = requests[findIndex(requestId)];
        if (request.getStudentId() != student) throw invalid_argument("That request is not yours");
        request.changeStatus(Status::Cancelled);
        saveRequests();
    }

    void showStudentRequests(const string& student) const {
        bool found = false;
        for (const PadRequest& request : requests) {
            if (request.getStudentId() == student) {
                cout << "#" << request.getId() << " | " << request.getLocation() << " | "
                     << statusName(request.getStatus()) << " | "
                     << formatTime(request.getCreatedTime()) << "\n";
                found = true;
            }
        }
        if (!found) cout << "You have no pad requests.\n";
    }

    void showNextRequest() {
        int requestId = nextPendingId();
        if (requestId == -1) {
            cout << "No pending requests.\n";
            return;
        }
        const PadRequest& request = requests[findIndex(requestId)];
        cout << "Next: #" << requestId << " at " << request.getLocation() << " ("
             << formatTime(request.getCreatedTime()) << ")\n";
    }

    int acceptNextRequest(const string& staff) {
        int requestId = nextPendingId();
        if (requestId == -1) throw runtime_error("No pending requests");
        PadRequest& request = requests[findIndex(requestId)];
        request.changeStatus(Status::Accepted);
        request.assignStaff(staff);
        pendingIds.pop();
        saveRequests();
        return requestId;
    }

    void dispatchRequest(int id) { requests[findIndex(id)].changeStatus(Status::Dispatched); saveRequests(); }
    void deliverRequest(int id) { requests[findIndex(id)].changeStatus(Status::Delivered); saveRequests(); }
    void failRequest(int id) { requests[findIndex(id)].changeStatus(Status::Failed); saveRequests(); }

    void showAllRequests() const {
        if (requests.empty()) {
            cout << "(no pad requests)\n";
            return;
        }
        for (const PadRequest& request : requests) {
            cout << "#" << request.getId() << " | " << request.getLocation() << " | "
                 << statusName(request.getStatus()) << " | "
                 << formatTime(request.getCreatedTime()) << "\n";
        }
    }
};

vector<pair<string, string>> loadStaff() {
    ifstream checkFile(STAFF_FILE);
    if (!checkFile) {
        ofstream output(STAFF_FILE);
        output << "STAFF01,graphicera123\n";
    }

    ifstream input(STAFF_FILE);
    vector<pair<string, string>> staff;
    string line;
    while (getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        vector<string> fields = splitLine(line, ',');
        if (fields.size() == 2 && !fields[0].empty())
            staff.push_back({fields[0], fields[1]});
    }
    return staff;
}

string staffLogin(const vector<pair<string, string>>& staff) {
    for (int attempt = 1; attempt <= 3; ++attempt) {
        string id = readLine("Staff ID: ");
        string password = readLine("Password: ");
        for (const auto& person : staff) {
            if (person.first == id && person.second == password) return id;
        }
        cout << "Wrong ID or password (" << attempt << "/3).\n";
    }
    return "";
}

void studentMenu(const string& studentId, PadDispatch& dispatch) {
    while (true) {
        cout << "\n=== STUDENT (" << studentId << ") ===\n"
             << "1. Request emergency pad\n2. Check request status\n"
             << "3. My requests\n4. Cancel request\n0. Back\n";
        int choice = readNumber("Choice: ", 0, 4);
        try {
            if (choice == 0) return;
            if (choice == 1) {
                const vector<string>& places = dispatch.getLocations();
                for (int i = 0; i < static_cast<int>(places.size()); ++i)
                    cout << i + 1 << ". " << places[i] << "\n";
                int placeNumber = readNumber("Washroom number: ", 1, static_cast<int>(places.size()));
                int id = dispatch.createRequest(studentId, places[placeNumber - 1]);
                cout << "Request created. Your request ID is " << id << "\n";
            } else if (choice == 2) {
                int id = readNumber("Request ID: ", 1, 1000000);
                cout << "Status: " << dispatch.getStatus(id, studentId) << "\n";
            } else if (choice == 3) {
                dispatch.showStudentRequests(studentId);
            } else {
                int id = readNumber("Request ID: ", 1, 1000000);
                dispatch.cancelRequest(id, studentId);
                cout << "Request cancelled.\n";
            }
        } catch (const exception& error) {
            cout << "Error: " << error.what() << "\n";
        }
    }
}

void staffMenu(const string& staffId, PadDispatch& dispatch) {
    while (true) {
        cout << "\n=== STAFF (" << staffId << ") ===\n"
             << "1. View next pending\n2. Accept next\n3. Mark dispatched\n"
             << "4. Mark delivered\n5. Mark failed\n6. View all requests\n0. Back\n";
        int choice = readNumber("Choice: ", 0, 6);
        try {
            switch (choice) {
                case 0: return;
                case 1: dispatch.showNextRequest(); break;
                case 2: cout << "Accepted request #" << dispatch.acceptNextRequest(staffId) << "\n"; break;
                case 3: dispatch.dispatchRequest(readNumber("Request ID: ", 1, 1000000)); cout << "Marked dispatched.\n"; break;
                case 4: dispatch.deliverRequest(readNumber("Request ID: ", 1, 1000000)); cout << "Marked delivered.\n"; break;
                case 5: dispatch.failRequest(readNumber("Request ID: ", 1, 1000000)); cout << "Marked failed.\n"; break;
                case 6: dispatch.showAllRequests(); break;
            }
        } catch (const exception& error) {
            cout << "Error: " << error.what() << "\n";
        }
    }
}

int main() {
    PadDispatch dispatch;
    vector<pair<string, string>> staff = loadStaff();
    cout << "=== UNI-CARE Pad Dispatch (prototype: records and coordinates only) ===\n";

    while (true) {
        cout << "\n1. Student login\n2. Staff login\n0. Exit\n";
        int choice = readNumber("Choice: ", 0, 2);
        if (choice == 0) break;

        if (choice == 1) {
            string studentId = readLine("Student ID (11 digits): ");
            if (!validStudentId(studentId)) {
                cout << "Invalid student ID.\n";
                continue;
            }
            studentMenu(studentId, dispatch);
        } else {
            string staffId = staffLogin(staff);
            if (staffId.empty()) {
                cout << "Too many failed attempts.\n";
                continue;
            }
            staffMenu(staffId, dispatch);
        }
    }

    cout << "Goodbye.\n";
    return 0;
}
