#pragma once
#include "../Parameters/Identifiers.h"
#include "Utility/GUIUtil.h"
#include <FontBinaries.h>

namespace Fonts {
enum Name
{
  RobotoMediumItalic,
  Helvetica,
  Futura,
  FuturaBoldOblique
};

inline Typeface::Ptr getTypeface(Name n)
{
  switch (n)
  {
  case RobotoMediumItalic:
    return Typeface::createSystemTypefaceFor(FontBinaries::RobotoMediumItalic_ttf,
                                             FontBinaries::RobotoMediumItalic_ttfSize);
  case Helvetica:
    return Typeface::createSystemTypefaceFor(FontBinaries::HelveticaNeueRegular_otf,
                                             FontBinaries::HelveticaNeueRegular_otfSize);
  case Futura:
    return Typeface::createSystemTypefaceFor(FontBinaries::Futura_ttf,
                                             FontBinaries::Futura_ttfSize);
  case FuturaBoldOblique:
    return Typeface::createSystemTypefaceFor(FontBinaries::FuturaBoldOblique_otf,
                                             FontBinaries::FuturaBoldOblique_otfSize);
  default:
    return Typeface::createSystemTypefaceFor(FontBinaries::Futura_ttf,
                                             FontBinaries::Futura_ttfSize);
  }
}
} // namespace Fonts
