#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

extern int total_tests;
extern int passed_tests;

inline void raportBleduSekwencji(const std::vector<double>& spodz, const std::vector<double>& fakt) {
    constexpr size_t PREC = 3;
    std::cerr << std::fixed << std::setprecision(PREC);
    std::cerr << "  Spodziewany:\t";
    for (auto& el : spodz) std::cerr << el << ", ";
    std::cerr << "\n  Faktyczny:\t";
    for (auto& el : fakt) std::cerr << el << ", ";
    std::cerr << std::endl << std::endl;
}

inline bool porownanieSekwencji(const std::vector<double>& spodz, const std::vector<double>& fakt) {
    constexpr double TOL = 1e-3;
    if (fakt.size() != spodz.size()) return false;
    for (size_t i = 0; i < fakt.size(); i++)
        if (std::abs(fakt[i] - spodz[i]) >= TOL) return false;
    return true;
}

inline void myAssert(const std::vector<double>& spodz, const std::vector<double>& fakt, std::string nazwa = "Test z Testy_UAR") {
    total_tests++;
    if (porownanieSekwencji(spodz, fakt)) {
        passed_tests++;
    } else {
        std::cerr << "[FAIL] " << nazwa << "\n";
        raportBleduSekwencji(spodz, fakt);
    }
}

