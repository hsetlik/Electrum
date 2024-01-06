#include "FileManager.h"

File UserFiles::getPresetsFolder()
{
  File folder = File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                    .getChildFile("ElectrumLibrary")
                    .getChildFile("Presets");
  if (!folder.exists() || !folder.isDirectory())
    folder.createDirectory();
  DLog::log("Presets folder is at: " + String(folder.getFullPathName()));
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

bool UserFiles::isValidPreset(const File &f)
{
  if (f.getFileExtension() != ".eps")
    return false;

  return true;
}
