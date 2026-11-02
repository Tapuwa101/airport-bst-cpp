
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

// Task 1(a): Store airport information
class Airport {
public:
    string code;
    string name;
    string city;
    string country;
    string icao;
    double latitude;
    double longitude;
};

// Read CSV fields, including quoted commas
vector<string> splitCSV(string line) {
    vector<string> fields;
    string field = "";
    bool inQuotes = false;

    for (int i = 0; i < (int)line.size(); i++) {
        char c = line[i];

        if (c == '"') {
            if (inQuotes && i + 1 < (int)line.size()
                && line[i + 1] == '"') {
                field += '"';
                i++;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field = "";
        } else {
            field += c;
        }
    }

    fields.push_back(field);
    return fields;
}

// Task 1(b): Build Binary Search Tree
class AirportBST {
private:
    struct Node {
        Airport airport;
        Node* left;
        Node* right;

        Node(Airport a) {
            airport = a;
            left = nullptr;
            right = nullptr;
        }
    };

    Node* root;
    int inserted;
    int duplicates;

    // Task 1(c): Calculate tree height
    int calculateHeight(Node* node) {
        if (node == nullptr) {
            return -1;
        }

        int leftHeight = calculateHeight(node->left);
        int rightHeight = calculateHeight(node->right);

        return 1 + max(leftHeight, rightHeight);
    }

    // Task 1(c): Calculate total depth
    void calculateDepth(Node* node, int depth,
                        long long& totalDepth, int& count) {
        if (node == nullptr) {
            return;
        }

        totalDepth += depth;
        count++;

        calculateDepth(node->left, depth + 1,
                       totalDepth, count);
        calculateDepth(node->right, depth + 1,
                       totalDepth, count);
    }

    void deleteTree(Node* node) {
        if (node == nullptr) {
            return;
        }

        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

public:
    AirportBST() {
        root = nullptr;
        inserted = 0;
        duplicates = 0;
    }

    ~AirportBST() {
        deleteTree(root);
    }

    // Insert airport into BST
    void insert(Airport a) {
        if (root == nullptr) {
            root = new Node(a);
            inserted++;
            return;
        }

        Node* current = root;

        while (true) {
            if (a.code == current->airport.code) {
                duplicates++;
                cout << "Duplicate: " << a.code << endl;
                return;
            }

            if (a.code < current->airport.code) {
                if (current->left == nullptr) {
                    current->left = new Node(a);
                    inserted++;
                    return;
                }
                current = current->left;
            } else {
                if (current->right == nullptr) {
                    current->right = new Node(a);
                    inserted++;
                    return;
                }
                current = current->right;
            }
        }
    }

    void printCounts() {
        cout << "Airports inserted: " << inserted << endl;
        cout << "Duplicates: " << duplicates << endl;
    }

    void printStats() {
        int height = calculateHeight(root);

        long long totalDepth = 0;
        int count = 0;

        calculateDepth(root, 0, totalDepth, count);

        double average = 0.0;

        if (count > 0) {
            average = (double)totalDepth / count;
        }

        cout << "Tree height: " << height << endl;
        cout << "Average depth: "
             << fixed << setprecision(1)
             << average << endl;
    }
};

// Main program: Task 1(a), 1(b), and 1(c)
int main() {
    ifstream file("airports.dat");
    vector<Airport> airports;
    string line;
    int skipped = 0;

    if (!file.is_open()) {
        cout << "Error opening airports.dat" << endl;
        return 1;
    }

    // Task 1(a): Load airport records
    while (getline(file, line)) {
        vector<string> fields = splitCSV(line);

        if (fields.size() < 8 ||
            fields[4].size() != 3 ||
            fields[4] == "\\N") {
            skipped++;
            continue;
        }

        Airport a;
        a.code = fields[4];
        a.name = fields[1];
        a.city = fields[2];
        a.country = fields[3];
        a.icao = fields[5];
        a.latitude = stod(fields[6]);
        a.longitude = stod(fields[7]);

        airports.push_back(a);
    }

    file.close();

    cout << "Airports loaded: "
         << airports.size() << endl;
    cout << "Lines skipped: "
         << skipped << endl;

    // Task 1(b): Insert airports in file order
    AirportBST tree;

    for (int i = 0; i < (int)airports.size(); i++) {
        tree.insert(airports[i]);
    }

    tree.printCounts();

    // Task 1(c): Report height and average depth
    tree.printStats();

    return 0;
}
