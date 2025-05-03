#include "Electrum/GUI/WaveEditor/WaveMenu.h"

namespace WaveCmd {

static std::vector<CommandInfo> getCommandInfoList() {
  std::vector<CommandInfo> vec = {};
  // append wave
  CommandInfo cAppend(appendWaveFrame);
  cAppend.shortName = "Append frame";
  cAppend.description =
      "Create a new wave frame at the end of the current table";
  cAppend.categoryName = "Wave";
  vec.push_back(cAppend);
  // insert frame
  CommandInfo cFrameInsert(insertWaveFrame);
  cFrameInsert.shortName = "Insert frame";
  cFrameInsert.description = "Insert a new wave frame after the selected one";
  cFrameInsert.categoryName = "Wave";
  vec.push_back(cFrameInsert);
  // delete frame
  CommandInfo cDelete(deleteWaveFrame);
  cDelete.shortName = "Delete frames";
  cDelete.description = "Delete the selected wave frames";
  cDelete.categoryName = "Wave";
  vec.push_back(cDelete);
  // lerp frames
  CommandInfo cLerpFrames(insertLerpFrames);
  cLerpFrames.shortName = "Add interpolated frames";
  cLerpFrames.description =
      "Add some number of frames to interpolate from this frame to the next";
  cLerpFrames.categoryName = "Wave";
  vec.push_back(cLerpFrames);
  // File menu-------
  // new wavetable
  CommandInfo cNewTable(newWavetable);
  cNewTable.shortName = "New wavetable";
  cNewTable.description = "Create a new wavetable file";
  cNewTable.categoryName = "File";
  cNewTable.addDefaultKeypress('n', juce::ModifierKeys::commandModifier);
  vec.push_back(cNewTable);

  CommandInfo cOpenTable(openWavetable);
  cOpenTable.shortName = "Open Wavetable";
  cOpenTable.description = "Open an existing wavetable";
  cOpenTable.categoryName = "File";

  return vec;
}

CommandInfo getInfo(CommandID id) {
  static std::vector<CommandInfo> commands = getCommandInfoList();
  return commands[(size_t)id];
}

}  // namespace WaveCmd
//===================================================
