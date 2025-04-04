#include "Electrum/Shared/FileSystem.h"
#include "Electrum/Identifiers.h"
#include "juce_data_structures/juce_data_structures.h"

ValueTree patch_t::toValueTree(const patch_t& patch) {
  ValueTree vt(ID::PATCH_INFO);
  vt.setProperty(ID::patchName, patch.name, nullptr);
  vt.setProperty(ID::patchPath, patch.path, nullptr);
  vt.setProperty(ID::patchAuthor, patch.author, nullptr);
  vt.setProperty(ID::patchDesc, patch.description, nullptr);
  vt.setProperty(ID::patchCategory, patch.category, nullptr);
  return vt;
}

patch_t patch_t::fromValueTree(ValueTree& vt) {
  patch_t patch;
  patch.name = vt[ID::patchName];
  patch.path = vt[ID::patchPath];
  patch.author = vt[ID::patchAuthor];
  patch.description = vt[ID::patchDesc];
  patch.category = vt[ID::patchCategory];
  return patch;
}

//===================================================
namespace UserFiles {

File getPatchesFolder() {
  File folder =
      File::getSpecialLocation(juce::File::userApplicationDataDirectory)
          .getChildFile("ElectrumData")
          .getChildFile("Patches");
  if (!folder.exists() || !folder.isDirectory())
    folder.createDirectory();
  DLog::log("Patches folder is at: " + String(folder.getFullPathName()));
  return folder;
}

File getWavetablesFolder() {
  File folder =
      File::getSpecialLocation(juce::File::userApplicationDataDirectory)
          .getChildFile("ElectrumData")
          .getChildFile("Wavetables");
  if (!folder.exists() || !folder.isDirectory())
    folder.createDirectory();
  return folder;
}

bool isValidPatch(const File& file) {
  auto str = file.loadFileAsString();
  ValueTree parent = ValueTree::fromXml(str);
  if (!parent.isValid())
    return false;
  auto child = parent.getChildWithName(ID::PATCH_INFO);
  return child.isValid();
}

bool attemptPatchSave(ValueTree& state) {
  auto patchTree = state.getChildWithName(ID::PATCH_INFO);
  if (!patchTree.isValid())
    return false;
  auto dir = getPatchesFolder();
  auto file = dir.getNonexistentChildFile(patchTree[ID::patchName],
                                          patchFileExt, false);
  // this is also where we'll set the path property
  auto fullStr = file.getRelativePathFrom(getPatchesFolder());
  patchTree.setProperty(ID::patchPath, fullStr, nullptr);
  auto xml = state.toXmlString();
  return file.replaceWithText(xml);
}

std::vector<patch_t> getAvailiblePatches() {
  std::vector<patch_t> vec;
  auto patches = getPatchesFolder();
  auto files = patches.findChildFiles(File::findFiles, true, patchFileExt);
  for (auto& f : files) {
    auto str = f.loadFileAsString();
    auto parent = juce::ValueTree::fromXml(str);
    auto child = parent.getChildWithName(ID::PATCH_INFO);
    if (child.isValid()) {
      vec.push_back(patch_t::fromValueTree(child));
    }
  }
  return vec;
}

}  // namespace UserFiles
//===================================================
