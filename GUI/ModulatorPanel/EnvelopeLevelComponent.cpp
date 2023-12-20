#include "EnvelopeLevelComponent.h"
#define LEVEL_UPDATE_HZ 24

EnvelopeLevelComponent::EnvelopeLevelComponent(EVT *tree, int idx) : state(tree), index(idx)
{
  startTimerHz(LEVEL_UPDATE_HZ);
  lower = Color::getColorForModSource(IDs::envSource.toString() + String(idx));
  upper = lower.darker();
}

void EnvelopeLevelComponent::paint(Graphics &g)
{
  auto fBounds = getLocalBounds().toFloat();
  float level = 1.0f - state->getLeadingVoiceEnvLevel(index);
  float yMin = fBounds.getHeight() * level;
  g.setColour(lower);
  g.fillRect(fBounds);
  g.setColour(upper);
  g.fillRect(fBounds.removeFromTop(yMin));
}
