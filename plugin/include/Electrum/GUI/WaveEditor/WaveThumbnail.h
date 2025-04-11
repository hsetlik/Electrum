#pragma once
#include "Electrum/Identifiers.h"

#define THUMBNAIL_W 100
#define THUMBNAIL_H 75

class WaveThumbnail : public Component {
private:
  juce::Image img;
  bool drawSelected = false;
  String waveString;
  void updateImage();
  bool leftWasDown = false;

public:
  int frameIndex;
  WaveThumbnail(const String& waveStr, int i);
  void paint(juce::Graphics& g) override;
  void setSelected(bool sel) {
    if (drawSelected != sel) {
      drawSelected = sel;
      updateImage();
      repaint();
    }
  }
  bool isSelected() const { return drawSelected; }
  // mouse callbacks for selecting/ deselecting frames
  void mouseUp(const juce::MouseEvent& e) override;
  void mouseDown(const juce::MouseEvent& e) override;
  void mouseEnter(const juce::MouseEvent& e) override;
};

//==========================================================

class WaveThumbnailBar : public Component {
private:
  // this private class will be the view component in our viewport
  class ThumbRow : public Component {
  public:
    juce::OwnedArray<WaveThumbnail> thumbnails;
    ThumbRow(const String& str);
    void resized() override;
  };

  ThumbRow row;
  juce::Viewport vpt;
  int numSelected = 0;

public:
  WaveThumbnailBar(const String& fullStr);
  int numFrames() const { return row.thumbnails.size(); }
  void resized() override;
  // the Thumbnail mouse overrides can call these on their parent
  void selectOnly(int idx);
  void selectUntil(int idx);
  void addToSelection(int idx);
  void removeFromSelection(int idx);
  void clearSelection();
  // returns the indices of currently selected frames
  std::vector<int> getSelection() const;
};
