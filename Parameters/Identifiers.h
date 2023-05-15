#include "../Core/CustomJuceHeader.h"
// Every Identifier for every parameter should be in here
namespace IDs
{
#define DECLARE_ID(name) const juce::Identifier name (#name);
DECLARE_ID(ELECTRUM_STATE)

DECLARE_ID(WAVETABLE_DATA)
DECLARE_ID(wavetableName)
DECLARE_ID(wavetableSize)
DECLARE_ID(wavetableStringData)


#undef DECLARE_ID
}