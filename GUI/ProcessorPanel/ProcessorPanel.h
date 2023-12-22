#pragma once
#include "FilterPanel.h"
#include "SaturationPanel.h"

class ProcessorPanel : public TabbedComponent
{
public:
  ProcessorPanel(EVT *tree);

private:
  std::unique_ptr<FilterPanel> filterPanel;
  std::unique_ptr<SaturationPanel> satPanel;
};
