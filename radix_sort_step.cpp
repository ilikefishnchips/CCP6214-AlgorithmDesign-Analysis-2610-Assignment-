// *********************************************************
// Program: radix_sort_step.cpp
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
    uint64_t key;     // the 10-digit integer we sort by
    string   str;     // the 5-letter string, carried along
};

// Read "integer,string" rows from the dataset file.
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
        if (!e.str.empty() && e.str.back() == '\r') e.str.pop_back(); // Windows CR
        data.push_back(e);
    }
    return true;
}

// Print the whole array as: [key/str, key/str, ...] label
void printSnapshot(ofstream& out, const vector<Element>& a, const string& label) {
    out << "[";
    for (size_t i = 0; i < a.size(); ++i) {
        if (i) out << ", ";
        out << a[i].key << "/" << a[i].str;
    }
    out << "] " << label << "\n";
}

// LSD radix sort, base 10, over the 10 decimal digits of the key.
// Processes the rightmost digit first (labelled d=10) up to the
// leftmost digit (labelled d=1), printing the array after each pass.
void radixSortStep(vector<Element>& data, ofstream& out) {
    size_t n = data.size();
    printSnapshot(out, data, "original");
    if (n <= 1) return;

    vector<Element> buffer(n);
    uint64_t place = 1;                       // 1, 10, 100, ... (units first)
    for (int d = 10; d >= 1; --d) {           // 10-digit keys => 10 passes
        int count[10] = {0};

        for (size_t i = 0; i < n; ++i)        // 1) tally each digit 0..9
            count[(data[i].key / place) % 10]++;

        for (int k = 1; k < 10; ++k)          // 2) prefix sums -> end positions
            count[k] += count[k - 1];

        for (size_t i = n; i-- > 0; )         // 3) place from the back => stable
            buffer[--count[(data[i].key / place) % 10]] = data[i];

        data.swap(buffer);                    // result becomes the new input
        printSnapshot(out, data, "d=" + to_string(d));
        place *= 10;
    }
}

int main() {
    // ----- Input file (one active line, the rest commented) -----
    string filename = "dataset_1000000.csv";     // <-- active

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

    // Sort only the requested rows, among themselves.
    vector<Element> sub(all.begin() + (startRow - 1), all.begin() + endRow);

    // Output name: dataset_<n>_radix_sorted_step_<start>_<end>.txt
    string stem = filename;
    size_t dot = stem.rfind(".csv");
    if (dot != string::npos) stem = stem.substr(0, dot);
    string outName = stem + "_radix_sorted_step_"
                   + to_string(startRow) + "_" + to_string(endRow) + ".txt";

    ofstream out(outName);
    radixSortStep(sub, out);
    out.close();

    cout << "Step trace written to " << outName << "\n";
    return 0;
}
