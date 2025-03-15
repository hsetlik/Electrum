#pragma once
#include "../Parameters/ElectrumValueTree.h"
#include "ElectrumLookAndFeel.h"
#include "GeneratorPanel/OscillatorEditor.h"
#include "HeaderPanel/HeaderPanel.h"
#include "Modal/PatchOpenModal.h"
#include "ModulatorPanel/EnvelopeTabbedComponent.h"
#include "ModulatorPanel/ModMacroPanel.h"
#include "ProcessorPanel/ProcessorPanel.h"

class ElectrumEditor : public Component, public DragAndDropContainer
{
private:
  EVT *const state;
  ElectrumLookAndFeel lnf;

  PatchOpenModal openDialog;
  OwnedArray<OscillatorEditor> oscEditors;
  ModMacroPanel macro;
  EnvelopesComponent envPanel;
  ProcessorPanel procPanel;
  HeaderPanel headerPanel;

public:
  ElectrumEditor(EVT *tree);
  ~ElectrumEditor() override;
  void paint(Graphics &g) override;
  void resized() override;
};
