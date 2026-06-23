// *********************************************************
// Program: hash_table_search_step.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC4L
// Tutorial Class: T13L
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
#include <cstdint>
using namespace std;

struct Element {
    uint64_t key;
    string   str;
};

// ---- AVL node (pointer / linked-node representation) ----
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
    Node* x = y->left;
    Node* t = x->right;
    x->right = y;
    y->left  = t;
    updateHeight(y);
    updateHeight(x);
    return x;                              // x is the new subtree root
}

Node* rotateLeft(Node* x) {
    Node* y = x->right;
    Node* t = y->left;
    y->left  = x;
    x->right = t;
    updateHeight(x);
    updateHeight(y);
    return y;
}

// Insert into the AVL tree and rebalance. Returns the (possibly new) root.
Node* insertAVL(Node* node, uint64_t key, const string& str) {
    if (!node) return new Node(key, str);
    if (key < node->key)       node->left  = insertAVL(node->left,  key, str);
    else if (key > node->key)  node->right = insertAVL(node->right, key, str);
    else                       return node;        // duplicate (won't occur: keys unique)

    updateHeight(node);
    int bf = balanceFactor(node);
    if (bf >  1 && key < node->left->key)  return rotateRight(node);             // LL
    if (bf < -1 && key > node->right->key) return rotateLeft(node);              // RR
    if (bf >  1 && key > node->left->key)  { node->left  = rotateLeft(node->left);   return rotateRight(node); } // LR
    if (bf < -1 && key < node->right->key) { node->right = rotateRight(node->right); return rotateLeft(node);  } // RL
    return node;
}

void freeTree(Node* n) {
    if (!n) return;
    freeTree(n->left);
    freeTree(n->right);
    delete n;
}

// ---- Hash table: array of buckets, each an AVL tree ----
struct HashTable {
    vector<Node*>  buckets;
    vector<size_t> bucketSize;             // node count per bucket (for direct/tree label)
    size_t         m;
    explicit HashTable(size_t tableSize)
        : buckets(tableSize, nullptr), bucketSize(tableSize, 0), m(tableSize) {}
    size_t hash(uint64_t key) const { return key % m; }
    void insert(uint64_t key, const string& str) {
        size_t b = hash(key);
        buckets[b] = insertAVL(buckets[b], key, str);
        bucketSize[b]++;
    }
    ~HashTable() { for (Node* b : buckets) freeTree(b); }
};

bool isPrime(size_t x) {
    if (x < 2) return false;
    if (x % 2 == 0) return x == 2;
    for (size_t i = 3; i * i <= x; i += 2)
        if (x % i == 0) return false;
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

// Walk the bucket's AVL tree toward the target, logging every integer field
// compared, then write the found / not-found verdict in the required format.
void searchStep(const HashTable& ht, uint64_t target, ofstream& out) {
    size_t b      = ht.hash(target);
    size_t bsize  = ht.bucketSize[b];
    string kind   = (bsize <= 1) ? "direct" : "tree";   // singleton bucket = direct hit
    out << "Search path (" << kind << ") in bucket " << b << ":\n";

    Node* cur   = ht.buckets[b];
    Node* found = nullptr;
    if (!cur) out << "  (empty bucket)\n";
    while (cur) {
        if (target == cur->key) {
            out << "  compare " << cur->key << "/" << cur->str << ", match\n";
            found = cur;
            break;
        } else if (target < cur->key) {
            out << "  compare " << cur->key << "/" << cur->str << ", target < key, go left"
                << (cur->left ? "" : ", no child") << "\n";
            cur = cur->left;
        } else {
            out << "  compare " << cur->key << "/" << cur->str << ", target > key, go right"
                << (cur->right ? "" : ", no child") << "\n";
            cur = cur->right;
        }
    }

    if (found) out << target << " = " << found->key << "/" << found->str << "\n";
    else       out << "-1 != " << target << "\n";
}

int main() {
    // ----- Input file (one active line, the rest commented) -----
    string filename = "dataset_1000000.csv";          // <-- active

    // ----- Targets: one found, one not-found (tutor specifies in code) -----
    // The found target MUST be an integer that exists in the dataset.
    // The not-found target MUST NOT exist in the dataset.
    uint64_t foundTarget    = 2008864030ULL;        // <-- replace with a key from your csv
    uint64_t notFoundTarget = 123456789ULL;         // <-- a value not in the dataset

    vector<Element> data;
    if (!loadDataset(filename, data)) {
        cerr << "Error: cannot open " << filename << "\n";
        return 1;
    }

    // Table size = next prime >= 2n  =>  load factor ~0.5 (few collisions).
    size_t m = nextPrime(data.size() * 2 + 1);
    HashTable ht(m);
    for (const auto& e : data) ht.insert(e.key, e.str);

    // One output file per target: dataset_<n>_hash_table_search_step_<target>.txt
    string stem = filename;
    size_t dot  = stem.rfind(".csv");
    if (dot != string::npos) stem = stem.substr(0, dot);

    for (uint64_t target : { foundTarget, notFoundTarget }) {
        string outName = stem + "_hash_table_search_step_" + to_string(target) + ".txt";
        ofstream out(outName);
        searchStep(ht, target, out);
        out.close();
        cout << "Wrote " << outName << "\n";
    }
    return 0;
}
