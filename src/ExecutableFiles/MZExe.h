//
// Created by Bilawal Ahmed on 22/May/2025.
//

#ifndef MZEXE_H
#define MZEXE_H

#include <fstream>
#include <memory>
#include <optional>
#include <vector>

class MZExe {
 public:
  static std::optional<MZExe> open(const std::string_view& path);

 private:
  static bool get_mz_header(std::ifstream& file);

 public:
  uint16_t NumLastPageBytes{0};
  uint16_t NumPages{0};
  uint16_t RelocationItems{0};
  uint16_t NumHeaderParagraphs{0};
  uint16_t NumMinParagraphRequired{0};
  uint16_t NumMaxParagraphRequested{0};
  uint16_t InitialSS{0};
  uint16_t InitialSP{0};
  uint16_t Checksum{0};
  uint16_t InitialIP{0};
  uint16_t InitialCS{0};
  uint16_t RelocationTableOffset{0};
  uint16_t Overlay{0};
  uint16_t OverlayInformation{0};

  std::vector<uint8_t> buffer;

  constexpr static int PARAGRAPH_SIZE = 16;
  constexpr static int PAGE_SIZE = 512;
};

#endif  // MZEXE_H
