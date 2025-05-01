#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/Common.h"
#include "FontData.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_graphics/juce_graphics.h"
static typeface_ptr _ptrForEFont(FontE id) {
  switch (id) {
    case FontE::AcierDN:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::AcierDisplayNoir_otf, FontData::AcierDisplayNoir_otfSize);
    case FontE::FuturaReg:
      return juce::Typeface::createSystemTypefaceFor(FontData::Futura_ttf,
                                                     FontData::Futura_ttfSize);
    case FontE::FuturaBO:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::FuturaBoldOblique_otf, FontData::FuturaBoldOblique_otfSize);
    case FontE::FuturaLC:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::FuturaLightCondensed_otf,
          FontData::FuturaLightCondensed_otfSize);
    case FontE::FuturaMO:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::FuturaMediumOblique_otf,
          FontData::FuturaMediumOblique_otfSize);
    case FontE::RobotoMI:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::RobotoMediumItalic_ttf,
          FontData::RobotoMediumItalic_ttfSize);
    case FontE::RobotoTI:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::RobotoThinItalic_ttf, FontData::RobotoThinItalic_ttfSize);
    case FontE::RobotoThin:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::RobotoThin_ttf, FontData::RobotoThin_ttfSize);
    case FontE::HighwayGothhicW:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::HighwayGothicWide_ttf, FontData::HighwayGothicWide_ttfSize);
    case FontE::HelveticaMed:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::HelveticaNeueMedium_otf,
          FontData::HelveticaNeueMedium_otfSize);
    case FontE::HelveticaReg:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::HelveticaNeueRegular_otf,
          FontData::HelveticaNeueRegular_otfSize);
    default:
      jassert(false);
      return juce::Typeface::createSystemTypefaceFor(
          FontData::FuturaLightCondensed_otf,
          FontData::FuturaLightCondensed_otfSize);
  }
}

static std::vector<juce::Font> _generateBinaryFonts() {
  std::vector<juce::Font> vec = {};
  for (int i = 0; i < NUM_BINARY_FONTS; ++i) {
    auto ptr = _ptrForEFont((FontE)i);
    jassert(ptr != nullptr);
    // DLog::log("Got typeface pointer");
    auto opts = juce::FontOptions(ptr);
    vec.push_back(juce::Font(opts));
    // DLog::log("Created juce::Font object");
  }
  return vec;
}

//===================================================
namespace FontData {

juce::String getFontName(int idx) {
  FontE id = (FontE)idx;
  switch (id) {
    case AcierDN:
      return "Acier Display Noir";
      break;
    case FuturaReg:
      return "Futura Regular";
      break;
    case FuturaBO:
      return "Futura Bold Oblique";
      break;
    case FuturaLC:
      return "Futura Light Condensed";
      break;
    case FuturaMO:
      return "Futura Medium Oblique";
      break;
    case HelveticaReg:
      return "Helvetica Neue Regular";
      break;
    case HelveticaMed:
      return "Helvetica Neue Medium";
      break;
    case RobotoMI:
      return "Roboto Medium Italic";
      break;
    case RobotoTI:
      return "Roboto Thin Italic";
      break;
    case RobotoThin:
      return "Roboto Thin";
      break;
    case HighwayGothhicW:
      return "Highway Gothic Wide";
      break;
  }
  return "null";
}

static std::vector<juce::Font> binFonts = _generateBinaryFonts();
juce::Font getFontWithHeight(FontE id, float height) {
  size_t idx = (size_t)id;
  if (idx < binFonts.size()) {
    return binFonts[idx].withHeight(height);
  }
  jassert(false);
  return juce::Font(juce::FontOptions());
}

typeface_ptr typefaceForFont(FontE id) {
  return _ptrForEFont(id);
}
}  // namespace FontData
