#pragma once

#include "Modulation/ModContextComponent.h"

class ElectrumEditor : public ModContextComponent {
private:
  ElectrumState* const state;

public:
  ElectrumEditor(ElectrumState* s);
};
