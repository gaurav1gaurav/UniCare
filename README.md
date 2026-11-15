# UniCare

UniCare is a student health and emergency management system developed in C++ as a Project-Based Learning (PBL) project at Graphic Era Hill University. It uses core Data Structures and Algorithms to manage student health records and handle emergency cases efficiently.

## Features

### Student portal
- Student registration and login (passwords are stored as hashes)
- Book appointments with doctors
- View available medicines and request medicine
- Report a medical emergency and check its status
- Find the nearest hospital with a route and map
- Emergency sanitary pad dispatch (request, track and cancel)
- Doctors directory with sorting and search
- View your profile and change your password

### Admin / medical staff portal
- View and manage appointments
- Approve medicine requests and manage inventory
- Emergency triage console with ambulance dispatch
- Manage emergency sanitary pad requests

## Data Structures and Algorithms Used

| Area | Concept |
|---|---|
| Emergency triage | Priority queue, ordered by severity |
| Ambulance dispatch | Quick sort by distance to pick the closest ambulance |
| Hospital route | Graph (adjacency list) with Dijkstra's shortest path |
| Doctors directory | Quick sort, binary search by ID, linear search by name or specialization |
| Pad dispatch | Queue of pending requests |
| Appointments | Array-based appointment queue |
| Login | Password hashing |

## Project Structure

```
UniCare/
├── main.cpp            # Main menu; includes the other modules
├── Appointment.h/.cpp  # Appointment booking
├── Medicine.h/.cpp     # Medicine inventory and requests
├── em.h/.cpp           # Emergency triage and ambulance dispatch
├── student.h/.cpp      # Student registration and login
├── doctor.h/.cpp       # Doctors directory
├── others.h/.cpp       # Profile, admin login and shared helpers
├── map.c               # Hospital route and map
├── pad dispatch.cpp    # Emergency sanitary pad dispatch
├── CMakeLists.txt
└── *.txt               # Sample data (doctors, medicines, ambulances, emergencies, washrooms)
```

## How to Build and Run

You need a C++ compiler (g++ or clang++).

```
g++ -std=c++14 -Wall main.cpp Appointment.cpp Medicine.cpp em.cpp -o UniCare
./UniCare
```

Run the program from the project folder so it can find the `.txt` data files.

**Using CMake instead:**

```
mkdir build && cd build
cmake ..
make
```

The executable is placed in the project folder.

> `main.cpp` includes `student.cpp`, `doctor.cpp`, `others.cpp`, `map.c` and `pad dispatch.cpp` directly. Do not compile them separately, or you will get duplicate symbol errors.

## Demo Login

- **Student:** register a new account from the main menu, then log in.
- **Admin:** password `admin123` (demo only).

## Data Files

- `doctors.txt`, `medicines.txt`, `ambulances.txt`, `emergencies.txt` and `washrooms.txt` hold the sample data the program starts with.
- `students.txt`, `appointment.txt`, `requests.txt` and `pad_requests.txt` are created automatically while the program is used. They are listed in `.gitignore` and are not part of the repository.

## Team

- Gaurav Singh Bora
- Lakshit Goswami
- Priyasa Gusain
- Shambhavi Chauhan
