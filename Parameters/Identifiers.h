#include "../Core/CustomJuceHeader.h"
// Every Identifier for every parameter should be in here
namespace IDs
{
#define DECLARE_ID(name) const juce::Identifier name (#name);
DECLARE_ID(BIGSYNTH_STATE)


#undef DECLARE_ID
};