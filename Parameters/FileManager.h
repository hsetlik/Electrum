#pragma once
#include "ElectrumValueTree.h"

// handles the finding, reading, and writing of presets and waveforms
namespace UserFiles {
File getPatchesFolder();
File getWaveformsFolder();

bool isValidPatchFile(const File &f);
void writePatch(ValueTree patchTree, const File &file);
ValueTree readPatchTree(const File &file);
bool isLegalPatchName(const String &name);

// the above are mostly helpers, other code should use these three:
//
// returns a list of the unique name of each patch installed
std::vector<String> getAvailablePatches();
// attempts to save a patch with the specified name from the given ValueTree.
// returns success or failure
bool savePatch(ValueTree pTree, const String &name);
// returns the top level ValueTree for the patch with the given name
ValueTree treeForPatch(const String &name);
} // namespace UserFiles

class PatchFileFilter : public FileFilter
{
public:
  PatchFileFilter() : FileFilter("Valid patch files only") {}
  bool isFileSuitable(const File &f) const override { return UserFiles::isValidPatchFile(f); }
  bool isDirectorySuitable(const File &f) const override { return f.isDirectory(); }
};
