#pragma once
#include "../AudioUtil.h"
#include "Electrum/Common.h"
#include "Electrum/Identifiers.h"
#include "juce_events/juce_events.h"

#define LFO_SIZE 2048

typedef std::array<float, LFO_SIZE> lfo_table_t;

// represents one editable point on the LFO
// a vector of these can be used to compute the table
struct lfo_handle_t {
  size_t tableIdx;
  float level;
};

namespace LFO {
// converts the `lfo_handle_t` vector into a string to be saved/loaded
String stringEncode(std::vector<lfo_handle_t>& handles);
// parses the given string into a list of LFO handles
void stringDecode(const String& str, std::vector<lfo_handle_t>& dest);
// parses a list of handles into an actual LUT
void parseHandlesToTable(const std::vector<lfo_handle_t>& handles,
                         lfo_table_t& dest);
}  // namespace LFO

