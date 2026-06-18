// *********************************************************
// Program: heap_sort_step.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC4L
// Tutorial Class: T13L
// Trimester: 2610
// Member_1: ID | NAME | EMAIL | PHONE
// Member_2: ID | NAME | EMAIL | PHONE
// Member_3: ID | NAME | EMAIL | PHONE
// Member_4: ID | NAME | EMAIL | PHONE
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
#include <utility>
#include <cstdint>
using namespace std;

struct Element {
    uint64_t key;     // the 10-digit integer we sort by
    string   str;     // the 5-letter string, carried along
};

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

void printSnapshot(ofstream& out, const vector<Element>& a, const string& label) {
    out << "[";
    for (size_t i = 0; i < a.size(); ++i) {
        if (i) out << ", ";
        out << a[i].key << "/" << a[i].str;
    }
    out << "] " << label << "\n";
}

// Push the element at index i down until the max-heap property holds,
// considering only positions [0, heapSize).
void siftDown(vector<Element>& a, size_t i, size_t heapSize) {
    while (true) {
        size_t left    = 2 * i + 1;
        size_t right    = 2 * i + 2;
        size_t largest = i;
        if (left  < heapSize && a[left].key  > a[largest].key) largest = left;
        if (right < heapSize && a[right].key > a[largest].key) largest = right;
        if (largest == i) break;          // parent already largest -> stop
        swap(a[i], a[largest]);
        i = largest;                      // follow the element down
    }
}

// Floyd's bottom-up build: heapify every internal node from the last
// parent up to the root. Runs in O(n).
void buildMaxHeap(vector<Element>& a) {
    size_t n = a.size();
    if (n < 2) return;
    for (size_t i = n / 2; i-- > 0; )     // i = n/2 - 1 down to 0
        siftDown(a, i, n);
}

void heapSortStep(vector<Element>& a, ofstream& out) {
    size_t n = a.size();
    buildMaxHeap(a);
    printSnapshot(out, a, "initial");     // array right after building the heap
    if (n < 2) return;
    for (size_t i = n - 1; i >= 1; --i) {
        swap(a[0], a[i]);                 // largest goes to the end
        siftDown(a, 0, i);                // restore heap over the shrunk range
        printSnapshot(out, a, "i = " + to_string(i));
    }
}

int main() {
    // ----- Input file (one active line, the rest commented) -----
    string filename = "dataset_1000.csv";     // <-- active

    // ----- Row range, 1-indexed and inclusive -----
    // long long startRow = 1, endRow = 7;
    long long startRow = 1;                    // <-- tutor may change
    long long endRow   = 7;                    // <-- tutor may change

    vector<Element> all;
    if (!loadDataset(filename, all)) {
        cerr << "Error: cannot open " << filename << "\n";
        return 1;
    }

    if (startRow < 1) startRow = 1;
    if (endRow > (long long)all.size()) endRow = (long long)all.size();
    if (startRow > endRow) {
        cerr << "Error: invalid row range\n";
        return 1;
    }

    vector<Element> sub(all.begin() + (startRow - 1), all.begin() + endRow);

    // Output name: dataset_<n>_heap_sorted_step_<start>_<end>.txt
    string stem = filename;
    size_t dot = stem.rfind(".csv");
    if (dot != string::npos) stem = stem.substr(0, dot);
    string outName = stem + "_heap_sorted_step_"
                   + to_string(startRow) + "_" + to_string(endRow) + ".txt";

    ofstream out(outName);
    heapSortStep(sub, out);
    out.close();

    cout << "Step trace written to " << outName << "\n";
    return 0;
}
