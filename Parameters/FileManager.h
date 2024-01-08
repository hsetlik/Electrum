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

// the above are mostly helpers, other code should use mostly these three:
// returns a list of the unique name of each patch installed
std::vector<String> getAvailablePatches();
bool savePatch(ValueTree pTree, const String &name);
ValueTree treeForPatch(const String &name);

} // namespace UserFiles
