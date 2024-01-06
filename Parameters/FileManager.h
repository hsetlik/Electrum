#pragma once
#include "ElectrumValueTree.h"

// handles the finding, reading, and writing of presets and waveforms
namespace UserFiles {
File getPresetsFolder();
File getWaveformsFolder();

bool isValidPreset(const File &f);
} // namespace UserFiles
