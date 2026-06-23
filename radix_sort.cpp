// *********************************************************
// Program: radix_sort.cpp
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
#include <chrono>
#include <cstdint>
using namespace std;

struct Element {
    uint64_t key;        // 10-digit integer we sort by
    char     str[6];     // 5-letter field + '\0'. Fixed buffer (not std::string)
                         // so each Element is ~16 bytes instead of ~40, which
                         // roughly thirds the RAM needed at 100M-200M elements.
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
        string s = line.substr(comma + 1);
        if (!s.empty() && s.back() == '\r') s.pop_back();   // strip Windows CR
        size_t len = s.size() < 5 ? s.size() : 5;           // keep up to 5 letters
        for (size_t i = 0; i < len; ++i) e.str[i] = s[i];
        e.str[len] = '\0';                                  // null-terminate
        data.push_back(e);
    }
    return true;
}

// LSD radix sort, base 10, 10 passes over the 10-digit key. Stable.
void radixSort(vector<Element>& data) {
    size_t n = data.size();
    if (n <= 1) return;
    vector<Element> buffer(n);
    uint64_t place = 1;
    for (int d = 0; d < 10; ++d) {            // 10 digit positions, units first
        int count[10] = {0};
        for (size_t i = 0; i < n; ++i)
            count[(data[i].key / place) % 10]++;
        for (int k = 1; k < 10; ++k)
            count[k] += count[k - 1];
        for (size_t i = n; i-- > 0; )         // back-to-front keeps it stable
            buffer[--count[(data[i].key / place) % 10]] = data[i];
        data.swap(buffer);
        place *= 10;
    }
}

int main() {
    // ----- Input file: 10 sizes. Uncomment exactly ONE, recompile, run. -----
    // string filename = "dataset_1000.csv";
    // string filename = "dataset_10000.csv";
    // string filename = "dataset_100000.csv";
     string filename = "dataset_1000000.csv";
    // string filename = "dataset_5000000.csv";
    // string filename = "dataset_10000000.csv";
    // string filename = "dataset_25000000.csv";
    // string filename = "dataset_50000000.csv";
    // string filename = "dataset_100000000.csv";
    // string filename = "dataset_200000000.csv";   // <-- largest; this is where you
                                                  //     need radix vs heap to differ
                                                  //     by >= 60 seconds (rubric Q7)

    vector<Element> data;
    if (!loadDataset(filename, data)) {
        cerr << "Error: cannot open " << filename << "\n";
        return 1;
    }

    // Time ONLY the sort. File read above and write below are excluded
    // from the measurement, as required by the spec (no I/O in timing).
    auto t0 = chrono::high_resolution_clock::now();
    radixSort(data);
    auto t1 = chrono::high_resolution_clock::now();
    double seconds = chrono::duration<double>(t1 - t0).count();

    // Output: radix_sorted_<input filename>  (e.g. radix_sorted_dataset_1000000.csv)
    string outName = "radix_sorted_" + filename;
    ofstream out(outName);
    for (const auto& e : data)
        out << e.key << ',' << e.str << '\n';
    // Running time recorded in the file too. If your tutor wants a pure
    // data-only csv, delete the next line and rely on the screenshot.
    out << "Running time: " << seconds << " seconds\n";
    out.close();

    cout << "Radix sort of " << data.size() << " elements: "
         << seconds << " seconds\n";
    cout << "Sorted output written to " << outName << "\n";
    return 0;
}
