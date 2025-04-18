#include "Electrum/Shared/FileSystem.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/Identifiers.h"
#include "juce_data_structures/juce_data_structures.h"

ValueTree patch_meta_t::toValueTree(const patch_meta_t& patch) {
  ValueTree vt(ID::PATCH_INFO);
  vt.setProperty(ID::patchName, patch.name, nullptr);
  vt.setProperty(ID::patchPath, patch.path, nullptr);
  vt.setProperty(ID::patchAuthor, patch.author, nullptr);
  vt.setProperty(ID::patchDesc, patch.description, nullptr);
  vt.setProperty(ID::patchCategory, patch.category, nullptr);
  return vt;
}

patch_meta_t patch_meta_t::fromValueTree(ValueTree& vt) {
  patch_meta_t patch;
  patch.name = vt[ID::patchName];
  patch.path = vt[ID::patchPath];
  patch.author = vt[ID::patchAuthor];
  patch.description = vt[ID::patchDesc];
  patch.category = vt[ID::patchCategory];
  return patch;
}

//--------------------------------------------------

ValueTree wave_meta_t::toValueTree(const wave_meta_t& wave) {
  ValueTree vt(ID::WAVE_INFO);
  vt.setProperty(ID::waveName, wave.name, nullptr);
  vt.setProperty(ID::waveAuthor, wave.author, nullptr);
  vt.setProperty(ID::waveCategory, wave.category, nullptr);

  return vt;
}

wave_meta_t wave_meta_t::fromValueTree(ValueTree& vt) {
  wave_meta_t wave;
  wave.name = vt[ID::waveName];
  wave.author = vt[ID::waveAuthor];
  wave.category = vt[ID::waveCategory];
  return wave;
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
  return folder;
}

static void createDefaultWaveFile(const File& waveFolder) {
  wave_meta_t meta;
  meta.name = "Default";
  meta.author = "Hayden";
  meta.category = 0;
  auto vt = wave_meta_t::toValueTree(meta);
  auto waveStr = Wavetable::getDefaultWavesetString();
  vt.setProperty(ID::waveStringData, waveStr, nullptr);
  auto file = waveFolder.getNonexistentChildFile(meta.name, waveFileExt, false);
  auto xml = vt.toXmlString();
  file.replaceWithText(xml);
}

File getWavetablesFolder() {
  File folder =
      File::getSpecialLocation(juce::File::userApplicationDataDirectory)
          .getChildFile("ElectrumData")
          .getChildFile("Wavetables");
  if (!folder.exists() || !folder.isDirectory()) {
    folder.createDirectory();
    createDefaultWaveFile(folder);
  }
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

bool isValidWave(const File& file) {
  auto str = file.loadFileAsString();
  ValueTree parent = ValueTree::fromXml(str);
  if (!parent.isValid())
    return false;
  return parent.hasType(ID::PATCH_INFO);
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

bool attemptWaveSave(const wave_meta_t& waveData, const String& waveString) {
  wave_meta_t wd = waveData;
  auto dir = getWavetablesFolder();
  auto file = dir.getChildFile(wd.name + waveFileExt);
  if (!file.existsAsFile()) {
    if (!file.create().wasOk()) {
      return false;
    }
  }
  auto vt = wave_meta_t::toValueTree(wd);
  vt.setProperty(ID::waveStringData, waveString, nullptr);
  auto xml = vt.toXmlString();
  return file.replaceWithText(xml);
}

std::vector<patch_meta_t> getAvailiblePatches() {
  std::vector<patch_meta_t> vec;
  auto patches = getPatchesFolder();
  auto files = patches.findChildFiles(File::findFiles, true);
  for (auto& f : files) {
    auto str = f.loadFileAsString();
    auto parent = juce::ValueTree::fromXml(str);
    auto child = parent.getChildWithName(ID::PATCH_INFO);
    if (child.isValid()) {
      vec.push_back(patch_meta_t::fromValueTree(child));
    }
  }
  return vec;
}

std::vector<wave_meta_t> getAvailableWaves() {
  std::vector<wave_meta_t> vec;
  auto folder = getWavetablesFolder();
  auto waves = folder.findChildFiles(File::findFiles, true);
  for (auto& wave : waves) {
    auto str = wave.loadFileAsString();
    auto parent = ValueTree::fromXml(str);
    if (parent.isValid()) {
      vec.push_back(wave_meta_t::fromValueTree(parent));
    }
  }
  return vec;
}

}  // namespace UserFiles
//===================================================

ElectrumUserLib::ElectrumUserLib()
    : patches(UserFiles::getAvailiblePatches()),
      waves(UserFiles::getAvailableWaves()) {}

bool ElectrumUserLib::isPatchNameLegal(const String& name) const {
  if (name.length() < 4 || name.length() > 20)
    return false;
  for (auto& p : patches) {
    if (name.compareIgnoreCase(p.name) == 0)
      return false;
  }
  return true;
}

bool ElectrumUserLib::isWaveNameLegal(const String& name) const {
  if (name.length() < 4 || name.length() > 20)
    return false;
  return true;
}

bool ElectrumUserLib::validatePatchData(patch_meta_t* patch) const {
  if (!isPatchNameLegal(patch->name)) {
    DLog::log("Illegal patch name: " + patch->name);
    return false;
  }
  if (patch->category > (int)patch_categ_t::Other || patch->category < 0) {
    DLog::log("Illegal patch category: " + String(patch->category));
    return false;
  }
  if (patch->author.length() > 18 || patch->description.length() > 200) {
    DLog::log("Illegal author/description");
    return false;
  }
  return true;
}

bool ElectrumUserLib::validateWaveData(wave_meta_t* patch) const {
  if (!isWaveNameLegal(patch->name))
    return false;
  auto wFile = UserFiles::getWavetablesFolder().getChildFile(
      patch->name + UserFiles::waveFileExt);
  String legalPath = juce::File::createLegalPathName(wFile.getFullPathName());
  if (legalPath != wFile.getFullPathName())
    return false;
  return true;
}
patch_meta_t* ElectrumUserLib::getPatchAtIndex(int index) {
  if (index >= numPatches())
    return nullptr;
  return &patches[(size_t)index];
}

patch_meta_t* ElectrumUserLib::getPatch(const String& name) {
  for (auto& p : patches) {
    if (p.name == name)
      return &p;
  }
  return nullptr;
}

wave_meta_t* ElectrumUserLib::getWavetableData(const String& name) {
  for (auto& w : waves) {
    if (w.name == name)
      return &w;
  }
  jassert(false);
  return nullptr;
}

bool ElectrumUserLib::attemptPatchSave(apvts* tree,
                                       const patch_meta_t& patchData) {
  auto state = tree->copyState();
  patches.push_back(patchData);
  // 1. remove the existing PATCH_INFO child and
  // replace it with a new one
  auto oldPI = state.getChildWithName(ID::PATCH_INFO);
  if (oldPI.isValid()) {
    state.removeChild(oldPI, nullptr);
  }
  auto newPI = patch_meta_t::toValueTree(patchData);
  state.appendChild(newPI, nullptr);
  bool success = UserFiles::attemptPatchSave(state);
  if (success) {
    // notify the listeners
    for (auto* l : listeners) {
      l->patchWasSaved(&patches[patches.size() - 1]);
    }
    return true;
  }
  return false;
}

juce::StringArray ElectrumUserLib::getAvailableWaveNames() const {
  juce::StringArray names;
  for (size_t i = 0; i < waves.size(); ++i) {
    names.add(waves[i].name);
  }
  DLog::log("Found " + String(waves.size()) + " wavetable files");
  return names;
}

bool ElectrumUserLib::attemptWaveSave(const wave_meta_t& waveData,
                                      const String& waveString) {
  if (!isWaveNameLegal(waveData.name))
    return false;

  bool success = UserFiles::attemptWaveSave(waveData, waveString);
  if (success) {
    waves.push_back(waveData);
    auto* ptr = &waves[waves.size() - 1];
    for (auto* l : listeners) {
      l->waveWasSaved(ptr);
    }
  }
  return success;
}

ValueTree ElectrumUserLib::getMasterTreeForPatch(patch_meta_t* patch) {
  File patchFile = UserFiles::getPatchesFolder().getChildFile(patch->path);
  if (!patchFile.existsAsFile() || !UserFiles::isValidPatch(patchFile)) {
    jassert(false);
    return ValueTree();
  }
  auto str = patchFile.loadFileAsString();
  return ValueTree::fromXml(str);
}
