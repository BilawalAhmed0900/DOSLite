//
// Created by Bilawal Ahmed on 17/Jun/2025.
//

#ifndef COM_H
#define COM_H

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

class COM {
 public:
  static std::optional<COM> open(const std::string_view& path);

 public:
  std::vector<uint8_t> buffer;
};

#endif  // COM_H
