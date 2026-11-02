#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

class Airport {
public:
    string code, name, city, country, icao;
    double latitude, longitude;
};

// Parse CSV while preserving commas inside quoted airport names.
vector<string> parseCSV(const string& line) {
    vector<string> fields;
    string field;
    bool quoted = false;
    for (int i = 0; i < (int)line.size(); i++) {
        char c = line[i];
        if (c == '"') {
            if (quoted && i + 1 < (int)line.size() && line[i + 1] == '"') {
                field += '"';
                i++;
            } else {
                quoted = !quoted;
            }
        } else if (c == ',' && !quoted) {
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(field);
    return fields;
}

class AirportBST {
private:
    struct Node {
        Airport airport;
        Node* left;
        Node* right;
        Node(const Airport& a) : airport(a), left(nullptr), right(nullptr) {}
    };
    Node* root = nullptr;
    int count = 0;

public:
    // Iterative insertion avoids stack overflow for tall trees.
    bool insert(const Airport& airport) {
        if (root == nullptr) {
            root = new Node(airport);
            count++;
            return true;
        }
        Node* current = root;
        while (true) {
            if (airport.code == current->airport.code) return false;
            if (airport.code < current->airport.code) {
                if (current->left == nullptr) {
                    current->left = new Node(airport);
                    break;
                }
                current = current->left;
            } else {
                if (current->right == nullptr) {
                    current->right = new Node(airport);
                    break;
                }
                current = current->right;
            }
        }
        count++;
        return true;
    }

    // Depth of the root is 0; height is the largest depth.
    void getStats(int& height, double& averageDepth) const {
        if (root == nullptr) {
            height = -1;
            averageDepth = 0;
            return;
        }
        vector<pair<Node*, int>> pending;
        pending.push_back({root, 0});
        long long totalDepth = 0;
        height = 0;
        int visited = 0;
        while (!pending.empty()) {
            Node* node = pending.back().first;
            int depth = pending.back().second;
            pending.pop_back();
            totalDepth += depth;
            visited++;
            height = max(height, depth);
            if (node->left != nullptr) pending.push_back({node->left, depth + 1});
            if (node->right != nullptr) pending.push_back({node->right, depth + 1});
        }
        averageDepth = (double)totalDepth / visited;
    }

    // Task 2(a): Search and print every comparison.
    bool find(const string& code) const {
        Node* current = root;
        int comparisons = 0;
        cout << "Search for " << code << "\nPath: ";
        while (current != nullptr) {
            if (comparisons > 0) cout << " -> ";
            cout << current->airport.code;
            comparisons++;
            if (code == current->airport.code) {
                cout << "\nComparisons: " << comparisons << "\n";
                const Airport& a = current->airport;
                cout << "Found: " << a.code << " | " << a.name
                     << " | " << a.city << ", " << a.country
                     << " | ICAO: " << a.icao
                     << " | Coordinates: " << a.latitude << ", " << a.longitude << "\n";
                return true;
            }
            current = code < current->airport.code ? current->left : current->right;
        }
        if (comparisons > 0) cout << " -> ";
        cout << "NULL\nComparisons: " << comparisons << "\nAirport not found\n";
        return false;
    }

    string minimumCode() const {
        Node* current = root;
        if (current == nullptr) return "";
        while (current->left != nullptr) current = current->left;
        return current->airport.code;
    }

    string maximumCode() const {
        Node* current = root;
        if (current == nullptr) return "";
        while (current->right != nullptr) current = current->right;
        return current->airport.code;
    }

    string rootCode() const {
        return root == nullptr ? "" : root->airport.code;
    }

    // Select real examples from the loaded tree for Task 2(b).
    void exampleCodes(string& leaf, string& deep) const {
        leaf = "";
        deep = "";
        int deepest = -1;
        if (root == nullptr) return;
        vector<pair<Node*, int>> pending = {{root, 0}};
        while (!pending.empty()) {
            Node* node = pending.back().first;
            int depth = pending.back().second;
            pending.pop_back();
            if (node->left == nullptr && node->right == nullptr) {
                if (leaf.empty()) leaf = node->airport.code;
                if (depth > deepest) {
                    deepest = depth;
                    deep = node->airport.code;
                }
            }
            if (node->left != nullptr) pending.push_back({node->left, depth + 1});
            if (node->right != nullptr) pending.push_back({node->right, depth + 1});
        }
    }


    // Task 3(a): Remove a code using D1, D2, or D3.
    bool remove(const string& code) {
        Node* parent = nullptr;
        Node* current = root;
        while (current != nullptr && current->airport.code != code) {
            parent = current;
            current = code < current->airport.code ? current->left : current->right;
        }
        if (current == nullptr) {
            cout << "Not found for removal: " << code << "\n";
            return false;
        }
        string deletionCase;
        if (current->left != nullptr && current->right != nullptr) {
            deletionCase = "D3 (two children)";
            Node* successorParent = current;
            Node* successor = current->right;
            while (successor->left != nullptr) {
                successorParent = successor;
                successor = successor->left;
            }
            current->airport = successor->airport;
            parent = successorParent;
            current = successor;
        } else if (current->left == nullptr && current->right == nullptr) {
            deletionCase = "D1 (leaf)";
        } else {
            deletionCase = "D2 (one child)";
        }
        Node* child = current->left != nullptr ? current->left : current->right;
        if (parent == nullptr) root = child;
        else if (parent->left == current) parent->left = child;
        else parent->right = child;
        delete current;
        count--;
        cout << "Deleted " << code << " | " << deletionCase << "\n";
        return true;
    }

    // Task 3(b): Show only levels 0, 1 and 2 in breadth-first order.
    void printTopThreeLevels() const {
        if (root == nullptr) { cout << "Tree empty\n"; return; }
        vector<pair<Node*, int>> queue = {{root, 0}};
        int position = 0;
        for (int i = 0; i < (int)queue.size(); i++) {
            Node* node = queue[i].first;
            int level = queue[i].second;
            if (level > 2) break;
            if (level != position) { cout << "\n"; position = level; }
            cout << node->airport.code << " ";
            if (level < 2) {
                if (node->left != nullptr) queue.push_back({node->left, level + 1});
                if (node->right != nullptr) queue.push_back({node->right, level + 1});
            }
        }
        cout << "\n";
    }

    // Check the BST ordering rule after every change.
    bool validate() const {
        struct Check { Node* node; string low, high; bool hasLow, hasHigh; };
        vector<Check> pending = {{root, "", "", false, false}};
        while (!pending.empty()) {
            Check c = pending.back(); pending.pop_back();
            if (c.node == nullptr) continue;
            string key = c.node->airport.code;
            if ((c.hasLow && key <= c.low) || (c.hasHigh && key >= c.high)) return false;
            pending.push_back({c.node->left, c.low, key, c.hasLow, true});
            pending.push_back({c.node->right, key, c.high, true, c.hasHigh});
        }
        return true;
    }

    // Pick an existing code matching a requested child configuration.
    // kind: 0 leaf, 1 left-only, 2 right-only, 3 two children, 4 root.
    string codeForCase(int kind) const {
        if (root == nullptr) return "";
        if (kind == 4) return root->airport.code;
        vector<Node*> pending = {root};
        while (!pending.empty()) {
            Node* node = pending.back(); pending.pop_back();
            bool left = node->left != nullptr;
            bool right = node->right != nullptr;
            if ((kind == 0 && !left && !right) ||
                (kind == 1 && left && !right) ||
                (kind == 2 && !left && right) ||
                (kind == 3 && left && right)) return node->airport.code;
            if (left) pending.push_back(node->left);
            if (right) pending.push_back(node->right);
        }
        return "";
    }

    void testDeletionCases() {
        const string names[5] = {"Leaf D1", "Left child only D2", "Right child only D2",
                                 "Two children D3", "Root"};
        for (int i = 0; i < 5; i++) {
            string code = codeForCase(i);
            cout << "\n--- " << names[i] << " ---\n";
            if (code.empty()) { cout << "No matching node\n"; continue; }
            cout << "Code: " << code << "\nBefore (levels 0-2):\n";
            printTopThreeLevels();
            remove(code);
            cout << "After (levels 0-2):\n";
            printTopThreeLevels();
            cout << "Validate: " << (validate() ? "OK" : "FAILED") << "\n";
        }
    }

    // Task 3(c): Add, find and remove a fictional airport.
    void testFictionalAirport() {
        Airport a;
        a.code = "QQQ";
        a.name = "Student Test Airport";
        a.city = "Boston";
        a.country = "United States";
        a.icao = "TEST";
        a.latitude = 0;
        a.longitude = 0;
        cout << "\n--- Task 3(c): QQQ ---\n";
        cout << "Inserted QQQ: " << (insert(a) ? "YES" : "NO (duplicate)") << "\n";
        cout << "Validate after insert: " << (validate() ? "OK" : "FAILED") << "\n";
        find("QQQ");
        remove("QQQ");
        cout << "Validate after delete: " << (validate() ? "OK" : "FAILED") << "\n";
        find("QQQ");
    }

    ~AirportBST() {
        if (root == nullptr) return;
        vector<Node*> pending = {root};
        while (!pending.empty()) {
            Node* node = pending.back();
            pending.pop_back();
            if (node->left != nullptr) pending.push_back(node->left);
            if (node->right != nullptr) pending.push_back(node->right);
            delete node;
        }
    }
};

int main(int argc, char* argv[]) {
    string filename = argc > 1 ? argv[1] : "airports.dat";
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open " << filename << endl;
        return 1;
    }

    vector<Airport> airports;
    string line;
    int skipped = 0;
    while (getline(file, line)) {
        vector<string> fields = parseCSV(line);
        if (fields.size() < 8 || fields[4].size() != 3 || fields[4] == "\\N") {
            skipped++;
            continue;
        }
        Airport airport;
        airport.name = fields[1];
        airport.city = fields[2];
        airport.country = fields[3];
        airport.code = fields[4];
        airport.icao = fields[5];
        airport.latitude = stod(fields[6]);
        airport.longitude = stod(fields[7]);
        airports.push_back(airport);
    }
    file.close();

    AirportBST tree;
    int inserted = 0;
    int duplicates = 0;
    for (int i = 0; i < (int)airports.size(); i++) {
        if (tree.insert(airports[i])) {
            inserted++;
        } else {
            duplicates++;
            cout << "Duplicate IATA code: " << airports[i].code << endl;
        }
    }

    int height;
    double averageDepth;
    tree.getStats(height, averageDepth);
    cout << "Airports inserted: " << inserted << endl;
    cout << "Lines skipped: " << skipped << endl;
    cout << "Duplicates: " << duplicates << endl;
    cout << "Tree height: " << height << endl;
    cout << "Average depth: " << fixed << setprecision(1) << averageDepth << endl;
    // Task 2(b): Demonstrate all required search cases.
    string leaf, deep;
    tree.exampleCodes(leaf, deep);
    cout << "\nMinimum code: " << tree.minimumCode() << "\n";
    cout << "Maximum code: " << tree.maximumCode() << "\n";
    cout << "\n--- Root search ---\n";
    tree.find(tree.rootCode());
    cout << "\n--- Leaf search ---\n";
    tree.find(leaf);
    cout << "\n--- Deep-node search ---\n";
    tree.find(deep);
    cout << "\n--- Missing-code search ---\n";
    tree.find("XYZ");
    cout << "\n--- Below minimum ---\n";
    tree.find("000");
    cout << "\n--- Above maximum ---\n";
    tree.find("ZZZZ");
    cout << "\n=== TASK 3(b): Deletion tests ===\n";
    tree.testDeletionCases();
    cout << "\n=== TASK 3(c): Fictional airport ===\n";
    tree.testFictionalAirport();
    return 0;
}
