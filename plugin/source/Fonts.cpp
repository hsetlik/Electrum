#include "ELectrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/Common.h"
#include "FontData.h"
#include "juce_core/system/juce_PlatformDefs.h"
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
          FontData::FuturaMediumOblique_otfSize);
      break;
      break;
    case FontE::RobotoMI:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::RobotoMediumItalic_ttf,
          FontData::RobotoMediumItalic_ttfSize);
      break;
    case FontE::RobotoTI:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::RobotoThinItalic_ttf, FontData::RobotoThinItalic_ttfSize);
      break;
    case FontE::RobotoThin:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::RobotoThin_ttf, FontData::RobotoThin_ttfSize);
      break;
    case FontE::HighwayGothhicW:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::HighwayGothicWide_ttf, FontData::HighwayGothicWide_ttfSize);
      break;
    case FontE::HelveticaMed:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::HelveticaNeueMedium_otf,
          FontData::HelveticaNeueMedium_otfSize);
      break;
    case FontE::HelveticaReg:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::HelveticaNeueRegular_otf,
          FontData::HelveticaNeueRegular_otfSize);
      break;
    default:
      return juce::Typeface::createSystemTypefaceFor(
          FontData::FuturaLightCondensed_otf,
          FontData::FuturaLightCondensed_otfSize);
      break;
  }
}

static std::vector<juce::Font> _generateBinaryFonts() {
  std::vector<juce::Font> vec = {};
  for (int i = 0; i < NUM_BINARY_FONTS; ++i) {
    auto ptr = _ptrForEFont((FontE)i);
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

juce::Font getFontWithHeight(FontE id, float height) {
  static std::vector<juce::Font> binFonts = _generateBinaryFonts();
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
