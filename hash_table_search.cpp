// *********************************************************
// Program: hash_table_search.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC3L
// Tutorial Class: T10L
// Trimester: 2610
// Member_1: 252UC254QT | CHIN WEI LUN | CHIN.WEI.LUN@student.mmu.edu.my | 011-39676291
// Member_2: 252UC254QS | NG ZE JIAN | NG.ZE.JIAN@student.mmu.edu.my | 019-3589262
// Member_3: 261uc2614r | NICHOLAS LIM KIM HONG | NICHOLAS.LIM.KIM.HONG@student.mmu.edu.my | 011-16233853
// Member_4: 252UC254RX | THAM YU XIANG | THAM.YU.XIANG@student.mmu.edu.my | 011-10681380
// *********************************************************
// Task Distribution
// Member_1:
// Member_2:
// Member_3:
// Member_4:
// *********************************************************

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <cstdint>
using namespace std;

struct Element {
    uint64_t key;
    string   str;
};

struct Node {
    uint64_t key;
    string   str;
    Node*    left;
    Node*    right;
    int      height;
    Node(uint64_t k, const string& s)
        : key(k), str(s), left(nullptr), right(nullptr), height(1) {}
};

int  height(Node* n)        { return n ? n->height : 0; }
int  balanceFactor(Node* n) { return n ? height(n->left) - height(n->right) : 0; }
void updateHeight(Node* n) {
    int hl = height(n->left), hr = height(n->right);
    n->height = 1 + (hl > hr ? hl : hr);
}

Node* rotateRight(Node* y) {
    Node* x = y->left;  Node* t = x->right;
    x->right = y;  y->left = t;
    updateHeight(y);  updateHeight(x);
    return x;
}
Node* rotateLeft(Node* x) {
    Node* y = x->right; Node* t = y->left;
    y->left = x;  x->right = t;
    updateHeight(x);  updateHeight(y);
    return y;
}
Node* insertAVL(Node* node, uint64_t key, const string& str) {
    if (!node) return new Node(key, str);
    if (key < node->key)       node->left  = insertAVL(node->left,  key, str);
    else if (key > node->key)  node->right = insertAVL(node->right, key, str);
    else                       return node;
    updateHeight(node);
    int bf = balanceFactor(node);
    if (bf >  1 && key < node->left->key)  return rotateRight(node);
    if (bf < -1 && key > node->right->key) return rotateLeft(node);
    if (bf >  1 && key > node->left->key)  { node->left  = rotateLeft(node->left);   return rotateRight(node); }
    if (bf < -1 && key < node->right->key) { node->right = rotateRight(node->right); return rotateLeft(node);  }
    return node;
}
void freeTree(Node* n) { if (!n) return; freeTree(n->left); freeTree(n->right); delete n; }

struct HashTable {
    vector<Node*> buckets;
    size_t        m;
    explicit HashTable(size_t tableSize) : buckets(tableSize, nullptr), m(tableSize) {}
    size_t hash(uint64_t key) const { return key % m; }
    void insert(uint64_t key, const string& str) {
        size_t b = hash(key);
        buckets[b] = insertAVL(buckets[b], key, str);
    }
    ~HashTable() { for (Node* b : buckets) freeTree(b); }
};

bool isPrime(size_t x) {
    if (x < 2) return false;
    if (x % 2 == 0) return x == 2;
    for (size_t i = 3; i * i <= x; i += 2) if (x % i == 0) return false;
    return true;
}
size_t nextPrime(size_t x) { while (!isPrime(x)) ++x; return x; }

bool loadDataset(const string& filename, vector<Element>& data) {
    ifstream in(filename);
    if (!in) return false;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        size_t comma = line.find(',');
        if (comma == string::npos) continue;
        Element e;
        e.key = stoull(line.substr(0, comma));
        e.str = line.substr(comma + 1);
        if (!e.str.empty() && e.str.back() == '\r') e.str.pop_back();
        data.push_back(e);
    }
    return true;
}

// Plain search (no logging). Returns the matching node or nullptr.
Node* search(const HashTable& ht, uint64_t target) {
    Node* cur = ht.buckets[ht.hash(target)];
    while (cur) {
        if (target == cur->key) return cur;
        cur = (target < cur->key) ? cur->left : cur->right;
    }
    return nullptr;
}

// Find the deepest key in a tree (longest search path) and its depth.
void deepestKey(Node* node, int depth, int& maxDepth, uint64_t& key) {
    if (!node) return;
    if (depth > maxDepth) { maxDepth = depth; key = node->key; }
    deepestKey(node->left,  depth + 1, maxDepth, key);
    deepestKey(node->right, depth + 1, maxDepth, key);
}

// Return a key located at (about) the given target depth - used to pick a
// representative "average depth" element. Falls back to the root key.
bool keyAtDepth(Node* node, int depth, int target, uint64_t& out) {
    if (!node) return false;
    if (depth == target) { out = node->key; return true; }
    if (keyAtDepth(node->left,  depth + 1, target, out)) return true;
    return keyAtDepth(node->right, depth + 1, target, out);
}

int main() {
    // ----- Input file: 10 sizes. Uncomment exactly ONE, recompile, run. -----
    // Capped at 10,000,000 ON PURPOSE for the hash-table experiment:
    //   - Each element becomes one AVL Node allocated with `new`, plus a
    //     std::string field, plus we run n searches. At 100M+ this needs
    //     ~15-30 GB live, which forces the OS to swap to disk and the timing
    //     becomes meaningless (the first timed case eats the page-in cost).
    //   - The >=60-second separation rule applies to the two SORTS
    //     (radix vs heap), NOT to this search experiment.
    //   - 50,000,000 is the largest size here. With the fixed code (load
    //     factor ~100, one-target-per-bucket) it needs ~6-7 GB live, so it
    //     fits in RAM on a 16 GB machine. Do NOT run 50M on an 8 GB machine:
    //     it will swap to disk and the timing becomes meaningless.
     //string filename = "dataset_1000.csv";
    // string filename = "dataset_5000.csv";
    // string filename = "dataset_10000.csv";
    // string filename = "dataset_50000.csv";
    // string filename = "dataset_100000.csv";
    // string filename = "dataset_1000000.csv";
    // string filename = "dataset_5000000.csv";
    // string filename = "dataset_10000000.csv";
    //string filename = "dataset_50000000.csv";
     string filename = "dataset_100000000.csv"; // avoid: swaps even on 16 GB

    vector<Element> data;
    if (!loadDataset(filename, data)) {
        cerr << "Error: cannot open " << filename << "\n";
        return 1;
    }
    size_t n = data.size();

    // ---- Table size: deliberately high load factor (~100) ----
    // With m = nextPrime(n + 1) the load factor is ~1.0, so almost every
    // bucket holds a SINGLE element. Then the bucket root IS the deepest node,
    // so best == worst and the three cases are indistinguishable.
    // Using m = nextPrime(n/100 + 1) packs ~100 elements per bucket, giving AVL
    // trees of height ~log2(100) ~= 7. This is what makes the three cases
    // measurably different. (Explain this experimental choice in the report.)
    size_t m = nextPrime(n / 100 + 1);
    HashTable ht(m);
    for (const auto& e : data) ht.insert(e.key, e.str);

    // ---- Build ONE target per non-empty bucket for EACH case ----
    // All three target sets are the SAME SIZE and touch the SAME buckets, so
    // they have an identical cache footprint. The ONLY difference between the
    // cases is the depth of the searched node:
    //   BEST   : bucket root            -> depth 0          (1 comparison, O(1))
    //   AVERAGE: node at ~half height    -> typical depth    (O(log k))
    //   WORST  : deepest node in bucket  -> maximum depth     (O(log k), max const)
    vector<uint64_t> bestTargets, avgTargets, worstTargets;
    for (size_t b = 0; b < ht.m; ++b) {
        if (!ht.buckets[b]) continue;
        Node* root = ht.buckets[b];

        bestTargets.push_back(root->key);                 // depth 0

        int md = -1; uint64_t dk = 0;
        deepestKey(root, 0, md, dk);
        worstTargets.push_back(dk);                       // deepest node

        uint64_t mid = root->key;
        keyAtDepth(root, 0, md / 2, mid);                 // ~half the height
        avgTargets.push_back(mid);
    }

    // Perform n searches for a case and return elapsed seconds (search only).
    // I/O is excluded as required. Measured in nanoseconds for precision.
    auto timeCase = [&](const vector<uint64_t>& targets) -> double {
        if (targets.empty()) return 0.0;
        volatile uint64_t sink = 0;                       // stops dead-code elimination
        auto t0 = chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; ++i) {
            Node* r = search(ht, targets[i % targets.size()]);
            if (r) sink += r->key;
        }
        auto t1 = chrono::high_resolution_clock::now();
        (void)sink;
        return chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count() / 1e9;
    };

    // ---- Warm-up: one untimed sweep so the first TIMED case does not pay the
    // first-touch / page-in cost. Makes the three timings independent of order.
    {
        volatile uint64_t s = 0;
        for (const auto& e : data) { Node* r = search(ht, e.key); if (r) s += r->key; }
        (void)s;
    }

    double bestTime  = timeCase(bestTargets);
    double avgTime   = timeCase(avgTargets);
    double worstTime = timeCase(worstTargets);

    // ---- Output: hash_table_search_dataset_n.txt ----
    string outName = "hash_table_search_" + filename;
    size_t dot = outName.rfind(".csv");
    if (dot != string::npos) outName = outName.substr(0, dot) + ".txt";

    ofstream out(outName);
    out << "Dataset size (n): " << n << "\n";
    out << "Table size (m):   " << m << "\n";
    out << "Load factor:      " << (double)n / (double)m << "\n";
    out << "Searches per case: " << n << "\n\n";
    out << fixed << setprecision(9)
        << "Best case time: "    << bestTime  << " seconds\n"
        << "Average case time: " << avgTime   << " seconds\n"
        << "Worst case time: "   << worstTime << " seconds\n";
    out.close();

    cout << fixed << setprecision(9)
         << "Best case time: "    << bestTime  << " seconds\n"
         << "Average case time: " << avgTime   << " seconds\n"
         << "Worst case time: "   << worstTime << " seconds\n";
    cout << "Written to " << outName << "\n";
    return 0;
}
