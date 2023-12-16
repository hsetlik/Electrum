#pragma once
#include "../Parameters/Identifiers.h"
#include "Utility/GUIUtil.h"
#include <FontBinaries.h>

namespace Fonts {
enum Name
{
  RobotoMediumItalic,
  HelveticaNeueRegular,
  FuturaRegular,
  FuturaBoldOblique,
  FuturaLightCondensed
};

inline Typeface::Ptr getTypeface(Name n)
{
  switch (n)
  {
  case RobotoMediumItalic:
    return Typeface::createSystemTypefaceFor(FontBinaries::RobotoMediumItalic_ttf,
                                             FontBinaries::RobotoMediumItalic_ttfSize);
  case HelveticaNeueRegular:
    return Typeface::createSystemTypefaceFor(FontBinaries::HelveticaNeueRegular_otf,
                                             FontBinaries::HelveticaNeueRegular_otfSize);
  case FuturaRegular:
    return Typeface::createSystemTypefaceFor(FontBinaries::Futura_ttf,
                                             FontBinaries::Futura_ttfSize);
  case FuturaBoldOblique:
    return Typeface::createSystemTypefaceFor(FontBinaries::FuturaBoldOblique_otf,
                                             FontBinaries::FuturaBoldOblique_otfSize);
  case FuturaLightCondensed:
    return Typeface::createSystemTypefaceFor(FontBinaries::FuturaLightCondensed_otf,
                                             FontBinaries::FuturaLightCondensed_otfSize);
  default:
    return Typeface::createSystemTypefaceFor(FontBinaries::Futura_ttf,
                                             FontBinaries::Futura_ttfSize);
  }
}
} // namespace Fonts
