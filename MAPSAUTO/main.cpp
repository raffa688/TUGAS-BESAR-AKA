#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <chrono>
#include <cstdlib> // Untuk rand()
#include <ctime>   // Untuk time()

using namespace std;

#define first(L) L.first
#define next(P) P->next
#define info(P) P->info

typedef string infotype;
typedef struct elmlist *address;

typedef struct Edge {
    infotype streetName;
    address destination;
} Edge;

struct elmlist {
    infotype placeName;
    vector<Edge> edges;
    address next;
};

struct List {
    address first;
};

void createList(List &L) {
    first(L) = nullptr;
}

address allocate(infotype placeName) {
    address P = new elmlist;
    P->placeName = placeName;
    P->next = nullptr;
    return P;
}

void insertLast(List &L, address P) {
    if (first(L) == nullptr) {
        first(L) = P;
    } else {
        address Q = first(L);
        while (next(Q) != nullptr) {
            Q = next(Q);
        }
        next(Q) = P;
    }
}

address findNode(List L, infotype placeName) {
    address P = first(L);
    while (P != nullptr && P->placeName != placeName) {
        P = next(P);
    }
    return P;
}

void addEdge(address from, address to, infotype streetName) {
    if (from != nullptr && to != nullptr) {
        Edge edge = {streetName, to};
        from->edges.push_back(edge);
    }
}

void printGraph(List L) {
    address P = first(L);
    while (P != nullptr) {
        cout << "Place: " << P->placeName << endl;
        for (const auto &edge : P->edges) {
            cout << "  -> " << edge.destination->placeName << " via " << edge.streetName << endl;
        }
        P = next(P);
    }
}

/// =======================
/// **Fitur Data Manual (Opsi 1 dan 2)**
/// =======================

void addManualPlace(List &cityMap) {
    string placeName;
    cout << "Enter place name: ";
    cin >> placeName;
    address newPlace = allocate(placeName);
    insertLast(cityMap, newPlace);
    cout << "Place " << placeName << " added successfully!" << endl;
}

void addManualStreet(List &cityMap) {
    string fromName, toName, streetName;
    cout << "Enter the starting place: ";
    cin >> fromName;
    address fromNode = findNode(cityMap, fromName);

    if (fromNode == nullptr) {
        cout << "Starting place not found!" << endl;
        return;
    }

    cout << "Enter the destination place: ";
    cin >> toName;
    address toNode = findNode(cityMap, toName);

    if (toNode == nullptr) {
        cout << "Destination place not found!" << endl;
        return;
    }

    cout << "Enter the street name: ";
    cin >> streetName;
    addEdge(fromNode, toNode, streetName);
    cout << "Street " << streetName << " added successfully!" << endl;
}

/// =======================
/// **Fitur Data Otomatis (Opsi 3)**
/// =======================

void addAutoPlaces(List &cityMap) {
    int numPlaces;
    cout << "Enter the number of places to generate: ";
    cin >> numPlaces;

    for (int i = 1; i <= numPlaces; i++) {
        string placeName = "Place_" + to_string(i);
        address newPlace = allocate(placeName);
        insertLast(cityMap, newPlace);
    }

    cout << numPlaces << " places have been added automatically!" << endl;
}

void addAutoStreets(List &cityMap) {
    int numStreets;
    cout << "Enter the number of streets for each place: ";
    cin >> numStreets;

    address fromNode = first(cityMap);
    srand(time(nullptr)); // Seed random generator

    while (fromNode != nullptr) {
        for (int i = 0; i < numStreets; i++) {
            address toNode = first(cityMap);
            int randomIndex = rand() % numStreets + 1; // Pilih node tujuan secara acak

            for (int j = 1; j < randomIndex && toNode != nullptr; j++) {
                toNode = next(toNode);
            }

            if (toNode != nullptr && toNode != fromNode) {
                string streetName = "Street_" + fromNode->placeName + "to" + toNode->placeName;
                addEdge(fromNode, toNode, streetName);
            }
        }
        fromNode = next(fromNode);
    }

    cout << "Streets have been added automatically for each place!" << endl;
}

void addAutoData(List &cityMap) {
    addAutoPlaces(cityMap);
    addAutoStreets(cityMap);
}

/// =======================
/// **Fungsi Shortest Path**
/// =======================

void findShortestPathIterative(List cityMap, string start, string destination) {
    auto start_time = chrono::high_resolution_clock::now();

    unordered_map<address, bool> visited;
    unordered_map<address, address> parent;
    unordered_map<address, string> edgeName;
    queue<address> q;

    address startNode = findNode(cityMap, start);
    address endNode = findNode(cityMap, destination);

    if (startNode == nullptr || endNode == nullptr) {
        cout << "Invalid start or destination place!" << endl;
        return;
    }

    q.push(startNode);
    visited[startNode] = true;

    while (!q.empty()) {
        address current = q.front();
        q.pop();

        if (current == endNode) {
            break;
        }

        for (const auto &edge : current->edges) {
            if (!visited[edge.destination]) {
                visited[edge.destination] = true;
                parent[edge.destination] = current;
                edgeName[edge.destination] = edge.streetName;
                q.push(edge.destination);
            }
        }
    }

    if (!visited[endNode]) {
        cout << "No path found between " << start << " and " << destination << "!" << endl;
        return;
    }

    vector<string> path;
    vector<string> streets;
    address current = endNode;

    while (current != nullptr) {
        path.insert(path.begin(), current->placeName);
        if (parent.find(current) != parent.end()) {
            streets.insert(streets.begin(), edgeName[current]);
        }
        current = parent[current];
    }

    cout << "Shortest path from " << start << " to " << destination << ":" << endl;
    for (size_t i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i < streets.size()) {
            cout << " via " << streets[i] << " -> ";
        }
    }
    cout << endl;

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - start_time;
    cout << "Execution Time (Iterative): " << elapsed.count() << " seconds" << endl;
}

bool findPathRecursiveHelper(address current, address endNode, unordered_map<address, bool> &visited,
                             vector<string> &path, vector<string> &streets, unordered_map<address, string> &edgeName) {
    visited[current] = true;
    path.push_back(current->placeName);

    if (current == endNode) {
        return true;
    }

    for (const auto &edge : current->edges) {
        if (!visited[edge.destination]) {
            streets.push_back(edge.streetName);
            if (findPathRecursiveHelper(edge.destination, endNode, visited, path, streets, edgeName)) {
                return true;
            }
            streets.pop_back(); // Backtrack jika tidak menemukan jalur
        }
    }

    path.pop_back(); // Backtrack
    return false;
}

void findShortestPathRecursive(List cityMap, string start, string destination) {
    auto start_time = chrono::high_resolution_clock::now(); // Mulai pengukuran waktu

    unordered_map<address, bool> visited;
    vector<string> path;
    vector<string> streets;
    unordered_map<address, string> edgeName;

    address startNode = findNode(cityMap, start);
    address endNode = findNode(cityMap, destination);

    if (startNode == nullptr || endNode == nullptr) {
        cout << "Invalid start or destination place!" << endl;
        return;
    }

    if (findPathRecursiveHelper(startNode, endNode, visited, path, streets, edgeName)) {
        cout << "Shortest path from " << start << " to " << destination << ":" << endl;
        for (size_t i = 0; i < path.size(); i++) {
            cout << path[i];
            if (i < streets.size()) {
                cout << " via " << streets[i] << " -> ";
            }
        }
        cout << endl;
    } else {
        cout << "No path found between " << start << " and " << destination << "!" << endl;
    }

    auto end_time = chrono::high_resolution_clock::now(); // Akhiri pengukuran waktu
    chrono::duration<double> elapsed = end_time - start_time;
    cout << "Execution Time (Recursive): " << elapsed.count() << " seconds" << endl;
}

/// =======================
/// **Tambahkan ke Menu Program**
/// =======================

int main() {
    List cityMap;
    createList(cityMap);

    int choice;
    do {
        cout << "\n--- Graph Menu ---" << endl;
        cout << "1. Add Place Manually" << endl;
        cout << "2. Add Street Manually" << endl;
        cout << "3. Add Data Automatically" << endl;
        cout << "4. Display Map" << endl;
        cout << "5. Find Shortest Path (Iterative)" << endl;
        cout << "6. Find Shortest Path (Recursive)" << endl;
        cout << "7. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            addManualPlace(cityMap);
        } else if (choice == 2) {
            addManualStreet(cityMap);
        } else if (choice == 3) {
            addAutoData(cityMap);
        } else if (choice == 4) {
            printGraph(cityMap);
        } else if (choice == 5) {
            string start, destination;
            cout << "Enter starting place: ";
            cin >> start;
            cout << "Enter destination place: ";
            cin >> destination;
            findShortestPathIterative(cityMap, start, destination);
        } else if (choice == 6) {
            string start, destination;
            cout << "Enter starting place: ";
            cin >> start;
            cout << "Enter destination place: ";
            cin >> destination;
            findShortestPathRecursive(cityMap, start, destination);
        } else if (choice == 7) {
            cout << "Exiting program..." << endl;
        } else {
            cout << "Invalid choice! Please try again." << endl;
        }
    } while (choice != 7);

    return 0;
}

