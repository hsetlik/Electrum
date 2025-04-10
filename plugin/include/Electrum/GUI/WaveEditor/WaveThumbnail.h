#pragma once
#include "EditValueTree.h"
#include "Electrum/Identifiers.h"

#define THUMBNAIL_W 100
#define THUMBNAIL_H 75

// keeps track of which Thumbnails are selected, all thumbnails should get
// a pointer to one of these

class WaveThumbnail : public Component {
private:
  juce::Image img;
  bool drawSelected = false;
  String waveString;
  void updateImage();

public:
  int frameIndex;
  WaveThumbnail(const String& waveStr, int i);
  void paint(juce::Graphics& g) override;
  void setSelected(bool sel) {
    drawSelected = sel;
    updateImage();
    repaint();
  }
  bool isSelected() const { return drawSelected; }
};

class WaveThumbnailGroup : public Component {
private:
  juce::OwnedArray<WaveThumbnail> thumbnails;

public:
  WaveThumbnailGroup(const String& fullStr);
  int numFrames() const { return thumbnails.size(); }
  void resized() override;
  // the Thumbnail mouse overrides can call these on their parent
  void selectOnly(int idx);
  void selectUntil(int idx);
  void addToSelection(int idx);
  void removeFromSelection(int idx);
  void clearSelection();
};
