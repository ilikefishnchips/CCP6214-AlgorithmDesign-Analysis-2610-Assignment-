// *********************************************************
// Program: heap_sort.cpp
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
#include <chrono>
#include <cstdint>
using namespace std;

struct Element {
    uint64_t key;
    string   str;
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

void siftDown(vector<Element>& a, size_t i, size_t heapSize) {
    while (true) {
        size_t left    = 2 * i + 1;
        size_t right    = 2 * i + 2;
        size_t largest = i;
        if (left  < heapSize && a[left].key  > a[largest].key) largest = left;
        if (right < heapSize && a[right].key > a[largest].key) largest = right;
        if (largest == i) break;
        swap(a[i], a[largest]);
        i = largest;
    }
}

void buildMaxHeap(vector<Element>& a) {
    size_t n = a.size();
    if (n < 2) return;
    for (size_t i = n / 2; i-- > 0; )
        siftDown(a, i, n);
}

// In-place heap sort: build a max-heap, then repeatedly move the root
// (current maximum) to the end and re-heapify the shrinking front.
void heapSort(vector<Element>& a) {
    size_t n = a.size();
    if (n < 2) return;
    buildMaxHeap(a);
    for (size_t i = n - 1; i >= 1; --i) {
        swap(a[0], a[i]);
        siftDown(a, 0, i);
    }
}

int main() {
    // ----- Input file (one active line, the rest commented) -----
    // string filename = "dataset_1000.csv";
    // string filename = "dataset_100000.csv";
    string filename = "dataset_1000000.csv";  // <-- active

    vector<Element> data;
    if (!loadDataset(filename, data)) {
        cerr << "Error: cannot open " << filename << "\n";
        return 1;
    }

    // Time ONLY the sort (file read above and write below are excluded).
    auto t0 = chrono::high_resolution_clock::now();
    heapSort(data);
    auto t1 = chrono::high_resolution_clock::now();
    double seconds = chrono::duration<double>(t1 - t0).count();

    // Output: heap_sorted_<input filename> (e.g. heap_sorted_dataset_1000000.csv)
    string outName = "heap_sorted_" + filename;
    ofstream out(outName);
    for (const auto& e : data)
        out << e.key << ',' << e.str << '\n';
    // Running time in the file too; delete this line if a data-only csv is wanted.
    out << "Running time: " << seconds << " seconds\n";
    out.close();

    cout << "Heap sort of " << data.size() << " elements: "
         << seconds << " seconds\n";
    cout << "Sorted output written to " << outName << "\n";
    return 0;
}
