#pragma once
#include "GUIUtil.h"

#include <Symbols.h>

namespace VectorSymbols {
enum Symbol
{
  Left,
  Right,
  Save,
  Open
};

// returns a stringified version of the SVG file
String getSymbolData(Symbol s);

std::unique_ptr<Drawable> getDrawable(Symbol s);
} // namespace VectorSymbols

//=============================================================================
class SymbolButton : public Button
{
private:
  std::unique_ptr<Drawable> drawable;

public:
  SymbolButton(VectorSymbols::Symbol s);
  void paintButton(Graphics &g, bool mouseOver, bool mouseDown) override;
};
