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
  for (int i = 0; i < waves; ++i) {
    ValueTree frame(WAVE_FRAME);
    frame.setProperty(frameStringData, waves[i], nullptr);
    frame.setProperty(frameIndex, i, nullptr);
    vt.appendChild(frame, nullptr);
  }
  return vt;
}

}  // namespace WaveEdit
//===================================================
