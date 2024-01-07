#include "Symbols.h"
#include "VectorSymbols.h"

String VectorSymbols::getSymbolData(Symbol s)
{
  switch (s)
  {
  case Left:
    return String(Symbols::Left_svg, Symbols::Left_svgSize);
  case Right:
    return String(Symbols::Right_svg, Symbols::Right_svgSize);
  case Open:
    return String(Symbols::Open_svg, Symbols::Open_svgSize);
  case Save:
    return String(Symbols::Save_svg, Symbols::Save_svgSize);
  default:
    return String(Symbols::Left_svg, Symbols::Left_svgSize);
  }
}

std::unique_ptr<Drawable> VectorSymbols::getDrawable(Symbol s)
{
  switch (s)
  {
  case Left:
    return Drawable::createFromImageData(Symbols::Left_svg, Symbols::Left_svgSize);
  case Right:
    return Drawable::createFromImageData(Symbols::Right_svg, Symbols::Right_svgSize);
  case Open:
    return Drawable::createFromImageData(Symbols::Open_svg, Symbols::Open_svgSize);
  case Save:
    return Drawable::createFromImageData(Symbols::Save_svg, Symbols::Save_svgSize);
  default:
    return Drawable::createFromImageData(Symbols::Left_svg, Symbols::Left_svgSize);
  }
}

SymbolButton::SymbolButton(VectorSymbols::Symbol s)
    : Button(""), drawable(VectorSymbols::getDrawable(s))
{
}

void SymbolButton::paintButton(Graphics &g, bool, bool)
{
  auto fBounds = getLocalBounds().toFloat();
  drawable->drawWithin(g, fBounds, RectanglePlacement::stretchToFit, 1.0f);
}
