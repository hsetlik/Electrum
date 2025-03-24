#include "Electrum/GUI/LookAndFeel/ElectrumLnF.h"
#include "juce_gui_basics/juce_gui_basics.h"

static juce::LookAndFeel_V4::ColourScheme _getColorScheme() {
  juce::LookAndFeel_V4::ColourScheme scheme =
      juce::LookAndFeel_V4::getDarkColourScheme();

  return scheme;
}

ElectrumLnF::ElectrumLnF() : juce::LookAndFeel_V4(_getColorScheme()) {}
//===================================================
