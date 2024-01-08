#include "FileManager.h"
#include "Identifiers.h"

File UserFiles::getPatchesFolder()
{
  File folder = File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                    .getChildFile("ElectrumLibrary")
                    .getChildFile("Patches");
  if (!folder.exists() || !folder.isDirectory())
    folder.createDirectory();
  DLog::log("Patches folder is at: " + String(folder.getFullPathName()));
  return folder;
}

File UserFiles::getWaveformsFolder()
{
  File folder = File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                    .getChildFile("ElectrumLibrary")
                    .getChildFile("Waveforms");
  if (!folder.exists() || !folder.isDirectory())
    folder.createDirectory();
  return folder;
}

bool UserFiles::isValidPatchFile(const File &f)
{
  if (f.getFileExtension() != ".eps")
    return false;
  return true;
}

void UserFiles::writePatch(ValueTree patchTree, const File &file)
{
  String data = patchTree.toXmlString();
  if (!file.replaceWithText(data))
  {
    DLog::log("Failed to write patch at: " + String(file.getFullPathName()));
  }
}

std::vector<String> UserFiles::getAvailablePatches()
{
  std::vector<String> out;
  auto files = getPatchesFolder().findChildFiles(File::findFiles, true, "*.eps");
  for (auto &f : files)
    out.push_back(f.getFileNameWithoutExtension());
  return out;
}

bool UserFiles::isLegalPatchName(const String &name)
{
  // TODO: decide what characters should be legal and check here
  auto names = getAvailablePatches();
  for (auto &p : names)
  {
    if (p == name)
      return false;
  }
  return true;
}

ValueTree UserFiles::readPatchTree(const File &file)
{
  String xmlStr = file.loadFileAsString();
  ValueTree tree = ValueTree::fromXml(xmlStr);
  jassert(tree.hasType(IDs::ELECTRUM_PATCH));
  return tree;
}

bool UserFiles::savePatch(ValueTree pTree, const String &name)
{
  if (!isLegalPatchName(name))
  {
    DLog::log("Warning! Illegal path name: " + name);
    return false;
  }
  pTree.setProperty(IDs::patchName, name, nullptr);
  auto file = getPatchesFolder().getChildFile(name + ".eps");
  if (!file.existsAsFile())
    file.create();
  writePatch(pTree, file);
  return true;
}

ValueTree UserFiles::treeForPatch(const String &name)
{
  auto pFile = getPatchesFolder().getChildFile(name + ".eps");
  if (!pFile.existsAsFile())
  {
    DLog::log("Warning! No file found for patch " + name);
  }
  return readPatchTree(pFile);
}
