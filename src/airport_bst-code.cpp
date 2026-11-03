#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <random>
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

    // Task 4(a): left, root, right prints codes in order.
    void inorder(Node* node, vector<Airport>& result) const {
        if (node == nullptr) return;
        inorder(node->left, result);
        result.push_back(node->airport);
        inorder(node->right, result);
    }

    // Task 4(b): root, left, right saves the shape of the tree.
    void saveNodes(Node* node, ofstream& file) const {
        if (node == nullptr) return;
        file << node->airport.code << "\n";
        saveNodes(node->left, file);
        saveNodes(node->right, file);
    }

    // Task 5: only explore branches that can contain a match.
    void rangeNodes(Node* node, string from, string to,
                    int& matches, int& visited) const {
        if (node == nullptr) return;
        visited++;
        string code = node->airport.code;
        if (code > from) rangeNodes(node->left, from, to, matches, visited);
        if (code >= from && code <= to) {
            cout << code << " | " << node->airport.name << " | "
                 << node->airport.city << ", " << node->airport.country << "\n";
            matches++;
        }
        if (code < to) rangeNodes(node->right, from, to, matches, visited);
    }

    // Task 6: choose the middle airport as root of each part.
    Node* buildBalanced(const vector<Airport>& airports, int low, int high) {
        if (low > high) return nullptr;
        int mid = (low + high) / 2;
        Node* node = new Node(airports[mid]);
        node->left = buildBalanced(airports, low, mid - 1);
        node->right = buildBalanced(airports, mid + 1, high);
        count++;
        return node;
    }

    // Search without printing a long path, used for timing.
    bool searchQuiet(string code) const {
        Node* current = root;
        while (current != nullptr) {
            if (code == current->airport.code) return true;
            if (code < current->airport.code) current = current->left;
            else current = current->right;
        }
        return false;
    }


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


    // Task 4(a): print airports in code order and check the order.
    void list() const {
        vector<Airport> sorted;
        inorder(root, sorted);
        bool correct = true;
        for (int i = 0; i < (int)sorted.size(); i++) {
            cout << sorted[i].code << " | " << sorted[i].name << " | "
                 << sorted[i].city << ", " << sorted[i].country << "\n";
            if (i > 0 && sorted[i - 1].code >= sorted[i].code) correct = false;
        }
        cout << "Airports listed: " << sorted.size() << "\n";
        cout << "Sorted: " << (correct ? "YES" : "NO") << "\n";
    }

    // Get level order as a list so we can compare two tree shapes.
    vector<string> levelCodes() const {
        vector<string> codes;
        if (root == nullptr) return codes;
        vector<Node*> queue = {root};
        for (int i = 0; i < (int)queue.size(); i++) {
            Node* node = queue[i];
            codes.push_back(node->airport.code);
            if (node->left != nullptr) queue.push_back(node->left);
            if (node->right != nullptr) queue.push_back(node->right);
        }
        return codes;
    }

    // Task 4(b): save codes in preorder.
    void save(string filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Could not save file\n";
            return;
        }
        saveNodes(root, file);
        cout << "Saved preorder codes to " << filename << "\n";
    }

    // Task 4(b): put saved airports back in the same insertion order.
    // Original data is used to get airport names and coordinates.
    void load(string filename, const vector<Airport>& airports) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Could not open " << filename << "\n";
            return;
        }
        vector<Airport> sorted = airports;
        sort(sorted.begin(), sorted.end(), [](Airport a, Airport b) {
            return a.code < b.code;
        });
        vector<Airport> records;
        string code;
        while (file >> code) {
            // Simple binary search in the sorted array.
            int left = 0;
            int right = (int)sorted.size() - 1;
            while (left <= right) {
                int mid = (left + right) / 2;
                if (sorted[mid].code == code) {
                    records.push_back(sorted[mid]);
                    break;
                }
                if (code < sorted[mid].code) right = mid - 1;
                else left = mid + 1;
            }
        }
        if (records.empty()) {
            cout << "No valid codes in file\n";
            return;
        }
        vector<string> before = levelCodes();
        int oldHeight;
        double oldAverage;
        getStats(oldHeight, oldAverage);
        clear();
        for (int i = 0; i < (int)records.size(); i++) insert(records[i]);
        int newHeight;
        double newAverage;
        getStats(newHeight, newAverage);
        cout << "Loaded " << count << " airports from " << filename << "\n";
        cout << "Height before: " << oldHeight << ", after: " << newHeight << "\n";
        cout << "Same level order: " << (before == levelCodes() ? "YES" : "NO") << "\n";
        cout << "BST valid: " << (validate() ? "YES" : "NO") << "\n";
    }

    // Task 4(b): level order (needed to compare saved tree shape).
    void levelOrder() const {
        if (root == nullptr) return;
        vector<Node*> queue = {root};
        for (int i = 0; i < (int)queue.size(); i++) {
            Node* node = queue[i];
            cout << node->airport.code << " ";
            if (node->left != nullptr) queue.push_back(node->left);
            if (node->right != nullptr) queue.push_back(node->right);
        }
        cout << "\n";
    }

    // Task 5: inclusive range search.
    void range(string from, string to) const {
        if (from > to) {
            cout << "Starting code must be smaller than ending code\n";
            return;
        }
        int matches = 0;
        int visited = 0;
        rangeNodes(root, from, to, matches, visited);
        cout << matches << " airports, " << visited << " nodes visited\n";
    }

    // Task 6: clear the old tree before making another tree.
    void clear() {
        if (root == nullptr) return;
        vector<Node*> nodes = {root};
        while (!nodes.empty()) {
            Node* node = nodes.back();
            nodes.pop_back();
            if (node->left != nullptr) nodes.push_back(node->left);
            if (node->right != nullptr) nodes.push_back(node->right);
            delete node;
        }
        root = nullptr;
        count = 0;
    }

    void rebalance() {
        vector<Airport> sorted;
        inorder(root, sorted);
        clear();
        root = buildBalanced(sorted, 0, (int)sorted.size() - 1);
        cout << "Balanced tree created\n";
    }

    void makeBalanced(const vector<Airport>& sorted) {
        clear();
        root = buildBalanced(sorted, 0, (int)sorted.size() - 1);
    }

    // Measure 10,000 searches without printing each search.
    double timeSearches(const vector<string>& codes) const {
        volatile int found = 0;
        auto start = chrono::steady_clock::now();
        for (int i = 0; i < (int)codes.size(); i++) {
            if (searchQuiet(codes[i])) found++;
        }
        auto stop = chrono::steady_clock::now();
        chrono::duration<double, milli> elapsed = stop - start;
        return elapsed.count();
    }

    // Task 6: file order, sorted order, balanced, random order.
    static void experiment(const vector<Airport>& airports) {
        vector<Airport> sorted = airports;
        sort(sorted.begin(), sorted.end(), [](Airport a, Airport b) {
            return a.code < b.code;
        });
        mt19937 generator(42);
        uniform_int_distribution<int> choose(0, (int)airports.size() - 1);
        vector<string> searches;
        for (int i = 0; i < 10000; i++) searches.push_back(airports[choose(generator)].code);

        cout << "Build order | Height | Average depth | Time for 10000 searches (ms)\n";
        for (int method = 0; method < 4; method++) {
            AirportBST tree;
            if (method == 0) {
                for (int i = 0; i < (int)airports.size(); i++) tree.insert(airports[i]);
            } else if (method == 1) {
                for (int i = 0; i < (int)sorted.size(); i++) tree.insert(sorted[i]);
            } else if (method == 2) {
                tree.makeBalanced(sorted);
            } else {
                vector<Airport> randomAirports = airports;
                shuffle(randomAirports.begin(), randomAirports.end(), generator);
                for (int i = 0; i < (int)randomAirports.size(); i++) tree.insert(randomAirports[i]);
            }
            int height;
            double avg;
            tree.getStats(height, avg);
            string name = "File order";
            if (method == 1) name = "Sorted order";
            if (method == 2) name = "Balanced";
            if (method == 3) name = "Random shuffle";
            cout << name << " | " << height << " | "
                 << fixed << setprecision(2) << avg << " | "
                 << fixed << setprecision(3) << tree.timeSearches(searches) << "\n";
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
        clear();
    }

};

int main(int argc, char* argv[]) {
    string filename = argc > 1 ? argv[1] : "data/airports.dat";
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Could not open " << filename << "\n";
        cout << "Try: ./airports data/airports.dat\n";
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
        Airport a;
        a.name = fields[1];
        a.city = fields[2];
        a.country = fields[3];
        a.code = fields[4];
        a.icao = fields[5];
        a.latitude = stod(fields[6]);
        a.longitude = stod(fields[7]);
        airports.push_back(a);
    }

    AirportBST tree;
    int duplicates = 0;
    for (int i = 0; i < (int)airports.size(); i++) {
        if (!tree.insert(airports[i])) duplicates++;
    }
    int height;
    double avg;
    tree.getStats(height, avg);
    cout << "Airports loaded: " << airports.size() << "\n";
    cout << "Lines skipped: " << skipped << "\n";
    cout << "Duplicates: " << duplicates << "\n";
    cout << "Height: " << height << " | Average depth: " << fixed << setprecision(2) << avg << "\n";
    cout << "Commands: find CODE, remove CODE, list, save FILE, load FILE, "
         << "validate, range FROM TO, rebalance, stats, level, experiment, test2, test3, quit\n";

    string command;
    while (true) {
        cout << "\n> ";
        if (!(cin >> command)) break;
        if (command == "quit") break;
        else if (command == "find") {
            string code; cin >> code; tree.find(code);
        } else if (command == "remove") {
            string code; cin >> code;
            tree.remove(code);
            cout << "Validate: " << (tree.validate() ? "OK" : "FAILED") << "\n";
        } else if (command == "list") tree.list();
        else if (command == "save") {
            string name; cin >> name; tree.save(name);
        } else if (command == "load") {
            string name; cin >> name; tree.load(name, airports);
        } else if (command == "validate") {
            cout << (tree.validate() ? "BST valid: YES" : "BST valid: NO") << "\n";
        } else if (command == "range") {
            string from, to; cin >> from >> to; tree.range(from, to);
        } else if (command == "rebalance") {
            tree.rebalance();
            tree.getStats(height, avg);
            cout << "Height: " << height << " | Average depth: " << avg << "\n";
        } else if (command == "level") tree.levelOrder();
        else if (command == "stats") {
            tree.getStats(height, avg);
            cout << "Height: " << height << " | Average depth: " << avg << "\n";
        } else if (command == "experiment") AirportBST::experiment(airports);
        else if (command == "test2") {
            string leaf, deep;
            tree.exampleCodes(leaf, deep);
            cout << "Minimum code: " << tree.minimumCode() << "\n";
            cout << "Maximum code: " << tree.maximumCode() << "\n";
            tree.find(tree.rootCode());
            tree.find(leaf);
            tree.find(deep);
            tree.find("XYZ");
            tree.find("000");
            tree.find("ZZZZ");
        }
        else if (command == "test3") {
            // Use a separate tree, so the main tree stays unchanged.
            AirportBST testTree;
            for (int i = 0; i < (int)airports.size(); i++) testTree.insert(airports[i]);
            testTree.testDeletionCases();
            testTree.testFictionalAirport();
        } else cout << "Unknown command\n";
    }
    return 0;
}
