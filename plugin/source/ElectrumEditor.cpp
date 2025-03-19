#include "Electrum/GUI/ElectrumEditor.h"
#include "Electrum/GUI/Modulation/ModContextComponent.h"

ElectrumEditor::ElectrumEditor(ElectrumState* s)
    : ModContextComponent(s), state(s) {}

//===================================================
