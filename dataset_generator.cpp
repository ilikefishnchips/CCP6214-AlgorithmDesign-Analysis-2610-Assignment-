// *********************************************************
// Program: dataset_generator.cpp
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
#include <string>
#include <random>
#include <vector>
#include <cstdint>
using namespace std;

// ---------------------------------------------------------
// Convert a student ID into the seed number (section E.5).
// Letter map (wraps every 10 letters, A=1):
//   A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
//   1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6
// Digits already in the ID are kept as-is.
// Example: "243UC247CT" -> "2431324730" -> seed 2431324730
// ---------------------------------------------------------
unsigned long long studentIdToSeed(const string& id) {
    string digits;
    for (char c : id) {
        if (c >= '0' && c <= '9') {
            digits += c;                       // keep digit unchanged
        } else if (c >= 'A' && c <= 'Z') {
            int v = (c - 'A' + 1) % 10;        // A=1 ... I=9, J=0, K=1 ...
            digits += char('0' + v);
        } else if (c >= 'a' && c <= 'z') {
            int v = (c - 'a' + 1) % 10;        // accept lowercase IDs too
            digits += char('0' + v);
        }
        // any other character (spaces, dashes) is ignored
    }
    return stoull(digits);
}

int main() {
    // ----- Input: dataset size n -----
    // Uncomment exactly ONE line, recompile, run. Run once per size you need.
    // The first sizes up to 200M are the radix/heap sweep; the smaller extras
    // also feed the hash-table experiment so it has 10 sizes too.
    // long long n = 1000;
    // long long n = 5000;
    // long long n = 10000;
    // long long n = 50000;
    // long long n = 100000;
     long long n = 1000000;
    // long long n = 5000000;
    // long long n = 10000000;
    // long long n = 100000000;
   // long long n = 200000000;          // <-- active size; change per experiment

    // ----- Seed: GROUP LEADER's student ID -----
    const string LEADER_ID = "243UC247CT";   // <-- REPLACE with your leader's ID
    unsigned long long seed = studentIdToSeed(LEADER_ID);

    // 64-bit Mersenne Twister, seeded as required so each group differs.
    mt19937_64 rng(seed);
    uniform_int_distribution<long long> intDist(1000000000LL, 9999999999LL);
    uniform_int_distribution<int>      letterDist(0, 25);

    // ----- Uniqueness via a bit array over the whole value range -----
    // The integers live in [1,000,000,000 .. 9,999,999,999] -> exactly
    // 9,000,000,000 possible values. We keep ONE bit per possible value to
    // mark "already used". For each new record we draw a random integer; if
    // its bit is already set we draw again (rejection). This guarantees
    // uniqueness using a FIXED ~1.05 GB no matter how large n is. (An
    // unordered_set would need 10+ GB at n = 200 million and would not fit.)
    // It also needs no shuffle: values come straight off the RNG, so the
    // file is already in random order.
    const long long RANGE_LO   = 1000000000LL;            // smallest value
    const long long RANGE_SIZE = 9000000000LL;            // 1e9 .. 1e10-1 inclusive
    const size_t    WORDS      = static_cast<size_t>((RANGE_SIZE + 63) / 64);
    vector<uint64_t> used(WORDS, 0ULL);                   // ~1.05 GB, zero-filled

    // Mark value as used; return true only if it was not used before.
    auto testAndSet = [&](long long value) -> bool {
        size_t   idx  = static_cast<size_t>(value - RANGE_LO);
        size_t   word = idx >> 6;                          // idx / 64
        uint64_t mask = 1ULL << (idx & 63);                // idx % 64
        if (used[word] & mask) return false;               // already used
        used[word] |= mask;                                // mark and accept
        return true;
    };

    string filename = "dataset_" + to_string(n) + ".csv";
    ofstream out(filename);
    if (!out) {
        cerr << "Error: cannot open " << filename << " for writing\n";
        return 1;
    }

    long long count = 0;
    while (count < n) {
        long long value = intDist(rng);
        if (testAndSet(value)) {                   // accept only brand-new values
            char s[6];
            for (int i = 0; i < 5; ++i)
                s[i] = char('a' + letterDist(rng));
            s[5] = '\0';
            out << value << ',' << s << '\n';      // format: integer,string
            ++count;
        }
    }
    out.close();

    cout << "Generated " << n << " unique records into " << filename << '\n';
    return 0;
}
