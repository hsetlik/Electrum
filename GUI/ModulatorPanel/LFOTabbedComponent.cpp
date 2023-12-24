#include "LFOTabbedComponent.h"

LFOTabbedComponent::LFOTabbedComponent(EVT *tree)
    : TabbedComponent(TabbedButtonBar::Orientation::TabsAtBottom)
{
  for (int i = 0; i < NUM_LFOS; i++)
  {
    lfos.add(new LFOPanel(tree, i));
    String name = "LFO " + String(i + 1);
    addTab(name, Color::darkBkgnd, lfos.getLast(), false);
  }
}
