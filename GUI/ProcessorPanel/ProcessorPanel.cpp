#include "ProcessorPanel.h"
#include <memory>

ProcessorPanel::ProcessorPanel(EVT *tree)
    : TabbedComponent(TabbedButtonBar::Orientation::TabsAtLeft),
      filterPanel(std::make_unique<FilterPanel>(tree)),
      satPanel(std::make_unique<SaturationPanel>(tree))
{
  addTab("Filter", Color::darkBkgnd, filterPanel.get(), true);
  addTab("Saturation", Color::darkBkgnd, satPanel.get(), true);
}
