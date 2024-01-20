#pragma once
#include "../../Parameters/FileManager.h"
#include "../../Parameters/ElectrumValueTree.h"


class PatchOpenModal : public Component
{
private:
  EVT* const state;
  TextButton oButton;
  TextButton closeButton;
public:
  PatchOpenModal(EVT* tree);
};
