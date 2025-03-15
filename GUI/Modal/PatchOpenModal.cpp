#include "PatchOpenModal.h"
#include "juce_gui_basics/juce_gui_basics.h"

static int getBrowserFlags()
{
  return (int)(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles |
               FileBrowserComponent::canSelectDirectories);
}

PatchOpenModal::PatchOpenModal(EVT *tree)
    : state(tree), browser(getBrowserFlags(), UserFiles::getPatchesFolder(), &filter, nullptr)
{
  addAndMakeVisible(&browser);
  okButton.setButtonText("Load");
  addAndMakeVisible(&okButton);
  cancelButton.setButtonText("Cancel");
  addAndMakeVisible(&cancelButton);

  okButton.addListener(this);
  cancelButton.addListener(this);
}

void PatchOpenModal::buttonClicked(Button *b)
{
  auto f = browser.getSelectedFile(0);
  if (b == &okButton && UserFiles::isValidPatchFile(f))
  {
    // load the patch here
    auto vt = UserFiles::readPatchTree(f);
    state->loadPatch(vt);
    this->setVisible(false);
    this->setEnabled(false);
  } else if (b == &cancelButton)
  {
    this->setVisible(false);
    this->setEnabled(false);
  }
  getParentComponent()->resized();
}

void PatchOpenModal::resized()
{
  auto iBounds = getLocalBounds();
  constexpr int buttonHeight = 30;
  auto bBounds = iBounds.removeFromBottom(buttonHeight);
  int cWidth = cancelButton.getBestWidthForHeight(buttonHeight);
  cancelButton.setBounds(bBounds.removeFromLeft(cWidth));
  int oWidth = okButton.getBestWidthForHeight(buttonHeight);
  okButton.setBounds(bBounds.removeFromRight(oWidth));
  browser.setBounds(iBounds);
}
