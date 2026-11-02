
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <utility>
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


    // Task 2(a): Search for an airport
    void find(string code) {
        Node* current = root;
        int comparisons = 0;

        cout << "Path: ";

        while (current != nullptr) {
            cout << current->airport.code;
            comparisons++;

            if (code == current->airport.code) {
                cout << endl;
                cout << "Comparisons: " << comparisons << endl;
                cout << "Airport: " << current->airport.name << endl;
                cout << "City: " << current->airport.city << endl;
                cout << "Country: " << current->airport.country << endl;
                cout << "ICAO: " << current->airport.icao << endl;
                cout << "Latitude: " << current->airport.latitude << endl;
                cout << "Longitude: " << current->airport.longitude << endl;
                return;
            }

            cout << " -> ";

            if (code < current->airport.code) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        cout << "NULL" << endl;
        cout << "Airport not found" << endl;
        cout << "Comparisons: " << comparisons << endl;
    }


    // Task 2(b): Find minimum airport code
    string findMinimum() {
        if (root == nullptr) {
            return "";
        }

        Node* current = root;

        while (current->left != nullptr) {
            current = current->left;
        }

        return current->airport.code;
    }

    // Task 2(b): Find maximum airport code
    string findMaximum() {
        if (root == nullptr) {
            return "";
        }

        Node* current = root;

        while (current->right != nullptr) {
            current = current->right;
        }

        return current->airport.code;
    }

    // Find a leaf and a deepest node for testing
    void findTestCodes(string& leafCode, string& deepCode) {
        if (root == nullptr) {
            return;
        }

        vector<pair<Node*, int>> nodes;
        nodes.push_back({root, 0});
        int deepest = -1;

        for (int i = 0; i < (int)nodes.size(); i++) {
            Node* current = nodes[i].first;
            int depth = nodes[i].second;

            if (current->left == nullptr &&
                current->right == nullptr) {
                leafCode = current->airport.code;
            }

            if (depth > deepest) {
                deepest = depth;
                deepCode = current->airport.code;
            }

            if (current->left != nullptr) {
                nodes.push_back({current->left, depth + 1});
            }

            if (current->right != nullptr) {
                nodes.push_back({current->right, depth + 1});
            }
        }
    }

    // Task 2(b): Run the required search tests
    void testSearchCases() {
        if (root == nullptr) {
            cout << "Tree is empty" << endl;
            return;
        }

        string leafCode = "";
        string deepCode = "";

        findTestCodes(leafCode, deepCode);

        cout << "\n1. Search root\n";
        find(root->airport.code);

        cout << "\n2. Search leaf\n";
        find(leafCode);

        cout << "\n3. Search deep node\n";
        find(deepCode);

        cout << "\n4. Search nonexistent code\n";
        find("XYZ");

        string minimum = findMinimum();
        string maximum = findMaximum();

        cout << "\nMinimum code: " << minimum << endl;
        cout << "Maximum code: " << maximum << endl;

        cout << "\n5. Search below minimum\n";
        find("000");

        cout << "\n6. Search above maximum\n";
        find("ZZZZ");
    }


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

    // Task 2(a): Example airport search
    cout << "\nTask 2(a): Search BOS\n";
    tree.find("BOS");

    // Task 2(b): Test all required search cases
    cout << "\nTask 2(b): Search Tests\n";
    tree.testSearchCases();

    return 0;
}
