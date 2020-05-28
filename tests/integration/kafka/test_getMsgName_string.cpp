/**
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed in accordance with the terms specified in
 *  the LICENSE file found in the root directory of this source tree.
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std::chrono;

inline std::vector<size_t> findAllOccurrences(const std::string& payload, const std::string subString) {
    // Initialize occurrences vector
    std::vector<size_t> occurrences;
    // Find first occurrence of subString in payload
    size_t idx = payload.find(subString);
    // Until end of string
    while (idx != std::string::npos) {
        // Add string index to vector
        occurrences.push_back(idx);
        // Find next occurrence of subString
        idx = payload.find(subString, idx + subString.size());
    }
    return occurrences;
}

inline std::string getMsgName(const std::string& payload) {
    // Searching for "name" key
    const std::string fieldName = "\"name\"";
    // Find all occurrences of "name" key in payload
    auto occurrences = findAllOccurrences(payload, fieldName);
    // Initialize "name" key index
    size_t fieldIndex;

    // If number of occurrences is 0, return base topic key
    if (occurrences.empty()) {
        return "";
    }
    // If number of occurrences is 1, use that index
    if (occurrences.size() == 1) {
        fieldIndex = occurrences.front();
    // Otherwise, determine result mode to choose proper "name" key
    } else {
        // If first key in payload is "diffResults", is batch mode
        const std::string batchIndicator = "{\"diffResults\"";
        // If first key in payload is "snapshot", is snapshot mode
        const std::string snapshotIndicator = "{\"snapshot\"";
        // For batch and snapshot modes, us last index in occurrences
        if (payload.compare(0, batchIndicator.length(), batchIndicator) == 0 ||
            payload.compare(0, snapshotIndicator.length(), snapshotIndicator) == 0) {
            fieldIndex = occurrences.back();
        // Otherwise is event mode, use first index in occurrences
        } else {
            fieldIndex = occurrences.front();
        }
    }

    // Parse value from "name" key
    size_t first = payload.find('"', fieldIndex + 6);
    if (first == std::string::npos) {
        return "";
    }

    size_t last = payload.find('"', first + 1);
    if (last == std::string::npos) {
        return "";
    }

    return payload.substr(first + 1, last - first - 1);
}

int main(int argc, char* argv[]) {
    std::ifstream result_file("osqueryd.combined.log");

    std::string line;
    auto start = high_resolution_clock::now();
    while(std::getline(result_file, line)) {
        auto query_name = getMsgName(line);
    }
    auto stop = high_resolution_clock::now();
    auto duration  = duration_cast<microseconds>(stop - start);
    std::cout << "Test took " << duration.count() / 1000 << "s" << std::endl;
}
