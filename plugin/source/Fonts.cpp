#include "ELectrum/GUI/LookAndFeel/Fonts.h"
#include "FontData.h"
#include "juce_graphics/juce_graphics.h"
static typeface_ptr _ptrForEFont(FontE id) {
  switch (id) {
    case FontE::AcierDN:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::AcierDisplayNoir_otf, FontData::AcierDisplayNoir_otfSize);
      break;
    case FontE::FuturaReg:
      return juce::Typeface::createSystemTypefaceFor(FontData::Futura_ttf,
                                                     FontData::Futura_ttfSize);
      break;
    case FontE::FuturaBO:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::FuturaBoldOblique_otf, FontData::FuturaBoldOblique_otfSize);
      break;
    case FontE::FuturaLC:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::FuturaLightCondensed_otf,
          FontData::FuturaLightCondensed_otfSize);
      break;
    case FontE::FuturaMO:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::FuturaMediumOblique_otf,
          FontData::FuturaLightCondensed_otfSize);
      break;
    case FontE::HelveticaReg:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::HelveticaNeueRegular_otf,
          FontData::HelveticaNeueRegular_otfSize);
      break;
    case FontE::HelveticaMed:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::HelveticaNeueMedium_otf,
          FontData::HelveticaNeueMedium_otfSize);
      break;
    case FontE::RobotoMI:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::RobotoMediumItalic_ttf,
          FontData::RobotoMediumItalic_ttfSize);
      break;
    default:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::HelveticaNeueRegular_otf,
          FontData::HelveticaNeueRegular_otfSize);
      break;
  }
}

static std::array<juce::FontOptions, NUM_BINARY_FONTS> _genFontOptions() {
  std::array<juce::FontOptions, NUM_BINARY_FONTS> arr;
  for (size_t i = 0; i < NUM_BINARY_FONTS; ++i) {
    arr[i] = juce::FontOptions(_ptrForEFont((FontE)i));
  }
  return arr;
}
//===================================================
namespace FontData {

static std::array<juce::FontOptions, NUM_BINARY_FONTS> opts = _genFontOptions();
juce::Font getFontWithHeight(FontE id, float height) {
  auto opt = opts[(size_t)id].withHeight(height);
  return juce::Font(opt);
}
}  // namespace FontData

