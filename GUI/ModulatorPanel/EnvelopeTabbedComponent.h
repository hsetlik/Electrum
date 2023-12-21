#pragma once
#include "../Modulation/ModulationSourceComponent.h"
#include "EnvelopePanel.h"

class EnvelopeSourceComponent : public Component
{
public:
  EnvelopeSourceComponent(EVT *tree);
  void resized() override;

private:
  EVT *const state;
  OwnedArray<ModulationSourceComponent> sources;
};
//==================================================
class EnvelopeTabbedComponent : public TabbedComponent
{
private:
  OwnedArray<EnvelopePanel> envs;

public:
  EVT *const state;
  EnvelopeTabbedComponent(EVT *tree);
  void resized() override;
};

class EnvelopesComponent : public Component
{
private:
  EnvelopeTabbedComponent tabs;
  EnvelopeSourceComponent src;

public:
  EnvelopesComponent(EVT *tree);
  void resized() override;
};
