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

#include <osquery/utils/json/json.h>

using namespace osquery;
using namespace std::chrono;

inline std::string getMsgName(const std::string& payload) {
  const std::string fieldName = "name";

  // Parse payload as JSON
  auto doc = JSON::newObject();
  // If failed to parse as JSON, or JSON object doesn't have "name" top-level
  // key, return base topic
  if (!doc.fromString(payload, JSON::ParseMode::Iterative) ||
      !doc.doc().HasMember(fieldName)) {
    return "";
  }
  auto& name = doc.doc()[fieldName];
  // If value for "name" isn't a String, return base topic
  if (!name.IsString()) {
    return "";
  }
  // Otherwise, return value
  return name.GetString();
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
