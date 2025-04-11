#include "Electrum/GUI/WaveEditor/EditValueTree.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/FileSystem.h"
namespace WaveEdit {

ValueTree getWavetableTree(const String& name) {
  auto file = UserFiles::getWavetablesFolder().getChildFile(
      name + UserFiles::waveFileExt);
  if (!file.existsAsFile())
    return ValueTree();
  auto xml = file.loadFileAsString();
  auto metaTree = ValueTree::fromXml(xml);
  ValueTree vt(WAVETABLE);
  vt.setProperty(waveName, name, nullptr);
  String fullStr = metaTree[ID::waveStringData];
  auto waves = splitWaveStrings(fullStr);
  for (int i = 0; i < waves.size(); ++i) {
    ValueTree frame(WAVE_FRAME);
    frame.setProperty(frameStringData, waves[i], nullptr);
    frame.setProperty(frameIndex, i, nullptr);
    vt.appendChild(frame, nullptr);
  }
  return vt;
}

String getFullWavetableString(const ValueTree& tree) {
  jassert(tree.hasType(WaveEdit::WAVETABLE));
  String fullStr = "";
  for (auto it = tree.begin(); it != tree.end(); ++it) {
    auto child = *it;
    if (child.hasType(WaveEdit::WAVE_FRAME)) {
      String str = child[WaveEdit::frameStringData];
      fullStr += str;
    }
  }
  return fullStr;
}

}  // namespace WaveEdit
//===================================================
