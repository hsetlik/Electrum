#pragma once
#include "../Parameters/Identifiers.h"
#include "Utility/GUIUtil.h"
#include <FontBinaries.h>

namespace Fonts {
enum Name
{
  RobotoMediumItalic,
  HelveticaNeueRegular,
  HelveticaNeueMedium,
  FuturaRegular,
  FuturaBoldOblique,
  FuturaLightCondensed,
  FuturaMediumOblique,
  AcierDisplayNoir
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
  case HelveticaNeueMedium:
    return Typeface::createSystemTypefaceFor(FontBinaries::HelveticaNeueMedium_otf,
                                             FontBinaries::HelveticaNeueMedium_otfSize);
  case FuturaRegular:
    return Typeface::createSystemTypefaceFor(FontBinaries::Futura_ttf,
                                             FontBinaries::Futura_ttfSize);
  case FuturaBoldOblique:
    return Typeface::createSystemTypefaceFor(FontBinaries::FuturaBoldOblique_otf,
                                             FontBinaries::FuturaBoldOblique_otfSize);
  case FuturaLightCondensed:
    return Typeface::createSystemTypefaceFor(FontBinaries::FuturaLightCondensed_otf,
                                             FontBinaries::FuturaLightCondensed_otfSize);
  case FuturaMediumOblique:
    return Typeface::createSystemTypefaceFor(FontBinaries::FuturaMediumOblique_otf,
                                             FontBinaries::FuturaMediumOblique_otfSize);
  case AcierDisplayNoir:
    return Typeface::createSystemTypefaceFor(FontBinaries::AcierDisplayNoir_otf,
                                             FontBinaries::AcierDisplayNoir_otfSize);
  default:
    return Typeface::createSystemTypefaceFor(FontBinaries::Futura_ttf,
                                             FontBinaries::Futura_ttfSize);
  }
}
} // namespace Fonts
