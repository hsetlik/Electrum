#include "EnvelopeTabbedComponent.h"

EnvelopeSourceComponent::EnvelopeSourceComponent(EVT *tree) : state(tree)
{
  for (int i = 0; i < NUM_ENVELOPES; i++)
  {
    String id = IDs::envSource.toString() + String(i);
    sources.add(new ModulationSourceComponent(state, id));
    addAndMakeVisible(sources.getLast());
    String tooltip = "Envelope " + String(i) + " source";
    sources.getLast()->setTooltip(tooltip);
  }
}

void EnvelopeSourceComponent::resized()
{
  auto lBounds = getLocalBounds().toFloat();
  auto dY = lBounds.getWidth();
  for (auto s : sources)
  {
    auto bounds = lBounds.removeFromTop(dY);
    s->setBounds(bounds.toNearestInt());
  }
}
//===========================================================================

EnvelopeTabbedComponent::EnvelopeTabbedComponent(EVT *tree)
    : TabbedComponent(TabbedButtonBar::Orientation::TabsAtBottom), state(tree)
{
  for (int i = 0; i < NUM_ENVELOPES; i++)
  {
    // add the envelope panels
    envs.add(new EnvelopePanel(state, i));
    addTab("Env " + String(i + 1), Color::darkBkgnd, envs.getLast(), false);
  }
  setCurrentTabIndex(0);
}

void EnvelopeTabbedComponent::resized() { TabbedComponent::resized(); }
//===========================================================================
EnvelopesComponent::EnvelopesComponent(EVT *tree) : tabs(tree), src(tree)
{
  addAndMakeVisible(&tabs);
  addAndMakeVisible(&src);
}

void EnvelopesComponent::resized()
{
  auto fBounds = getLocalBounds().toFloat();
  auto tabsArea = fBounds.removeFromLeft(fBounds.getWidth() * 0.85f);
  tabs.setBounds(tabsArea.toNearestInt());
  src.setBounds(fBounds.toNearestInt());
}
