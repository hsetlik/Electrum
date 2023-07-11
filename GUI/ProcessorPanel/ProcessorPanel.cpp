#include "ProcessorPanel.h"

ProcessorPanel::ProcessorPanel(EVT* tree) : filterPanel(tree)
{
  addAndMakeVisible(&filterPanel);
}

void ProcessorPanel::resized()
{
  filterPanel.setBounds(getLocalBounds());
}
