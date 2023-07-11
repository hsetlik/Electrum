#pragma once
#include "FilterPanel.h"

class ProcessorPanel : public Component
{
  public:
    ProcessorPanel(EVT* tree);
    void resized() override;
  private:
    FilterPanel filterPanel;
};
