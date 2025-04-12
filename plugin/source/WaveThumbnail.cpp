#include "Electrum/GUI/WaveEditor/WaveThumbnail.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
void WaveThumbnail::updateImage() {
  // 1. parse the wave into numbers
  float temp[TABLE_SIZE];
  stringDecodeWave(waveString, temp);
  img.clear(img.getBounds(), UIColor::windowBkgnd);
  juce::Graphics g(img);
  juce::Path p;
  static const float y0 = (float)THUMBNAIL_H / 2.0f;
  p.startNewSubPath(0.0f, y0);
  float y = y0;

  for (int i = 0; i < THUMBNAIL_W; ++i) {
    float xNorm = (float)i / (float)THUMBNAIL_W;
    int idx = (int)(xNorm * (float)TABLE_SIZE);
    y = y0 + (y0 * temp[idx]);
    p.lineTo((float)i, y);
  }
  juce::PathStrokeType pst(2.5f);
  auto strokeColor =
      drawSelected ? Color::periwinkle : Color::literalOrangePale;
  g.setColour(strokeColor);
  g.strokePath(p, pst);
  auto rect = img.getBounds().toFloat();
  auto outlineStroke = drawSelected ? 3.0f : 1.0f;
  g.drawRect(rect, outlineStroke);
}

//===================================================
WaveThumbnail::WaveThumbnail(const String& waveStr, int i)
    : img(juce::Image::RGB, THUMBNAIL_W, THUMBNAIL_H, true),
      waveString(waveStr),
      frameIndex(i) {
  updateImage();
}

void WaveThumbnail::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.drawImage(img, fBounds.reduced(2.5f));
}

void WaveThumbnail::mouseDown(const juce::MouseEvent& e) {
  if (e.mods.isLeftButtonDown()) {
    leftWasDown = true;
    auto* parent = findParentComponentOfClass<WaveThumbnailBar>();
    jassert(parent != nullptr);
    if (!e.mods.isCommandDown() && !e.mods.isShiftDown() && !drawSelected) {
      parent->clearSelection();
    }
  } else {
    leftWasDown = false;
  }
}

void WaveThumbnail::mouseDrag(const juce::MouseEvent& e) {
  auto* parent = findParentComponentOfClass<WaveThumbnailBar>();
  jassert(parent != nullptr);
  auto event = e.getEventRelativeTo(parent);
  auto* c = parent->getComponentAt(event.position.toInt());
  auto* child = dynamic_cast<WaveThumbnail*>(c);
  if (child != nullptr && event.mods.isLeftButtonDown()) {
    parent->addToSelection(child->frameIndex);
  }
}

void WaveThumbnail::mouseUp(const juce::MouseEvent& e) {
  auto* group = findParentComponentOfClass<WaveThumbnailBar>();
  jassert(group != nullptr);
  if (e.mouseWasClicked() || leftWasDown) {
    leftWasDown = false;
    auto m = e.mods;
    if (m.isCommandDown()) {
      if (drawSelected) {
        group->removeFromSelection(frameIndex);
      } else {
        group->addToSelection(frameIndex);
      }
    } else if (m.isShiftDown()) {
      group->selectUntil(frameIndex);
    } else if (!e.mouseWasDraggedSinceMouseDown()) {
      group->selectOnly(frameIndex);
    }
  }
}

void WaveThumbnail::mouseEnter(const juce::MouseEvent& e) {
  auto* group = findParentComponentOfClass<WaveThumbnailBar>();
  jassert(group != nullptr);
  if (e.mods.isLeftButtonDown() && e.mouseWasDraggedSinceMouseDown()) {
    group->addToSelection(frameIndex);
  }
}

//==========================================================

WaveThumbnailBar::ThumbRow::ThumbRow(const String& fullStr) {
  auto waveStrings = splitWaveStrings(fullStr);
  DLog::log("Table split into " + String(waveStrings.size()) + " waves");
  for (int i = 0; i < waveStrings.size(); ++i) {
    auto thumb = thumbnails.add(new WaveThumbnail(waveStrings[i], i));
    addAndMakeVisible(thumb);
  }
  jassert(thumbnails.size() > 0);
}

void WaveThumbnailBar::ThumbRow::resized() {
  static const int height = 45;
  static const int width = 65;
  int x = 0;
  for (auto* t : thumbnails) {
    t->setBounds(x, 0, width, height);
    x += width;
  }
  setSize(x + width, height);
}
//------------------------------

WaveThumbnailBar::WaveThumbnailBar(const String& fullStr) : row(fullStr) {
  vpt.setViewedComponent(&row, false);
  vpt.setViewPosition(0, 0);
  vpt.setInterceptsMouseClicks(true, true);
  addAndMakeVisible(vpt);
  row.resized();
}

void WaveThumbnailBar::resized() {
  vpt.setBounds(getLocalBounds());
}

void WaveThumbnailBar::focusFrame(int idx) {
  for (auto* l : tListeners) {
    l->frameWasFocused(idx);
  }
}

std::vector<int> WaveThumbnailBar::getSelection() const {
  std::vector<int> sel = {};
  for (auto* t : row.thumbnails) {
    if (t->isSelected()) {
      sel.push_back(t->frameIndex);
    }
  }
  return sel;
}

void WaveThumbnailBar::selectOnly(int idx) {
  for (int i = 0; i < row.thumbnails.size(); ++i) {
    row.thumbnails[i]->setSelected(i == idx);
  }
  focusFrame(idx);
  numSelected = 1;
}

void WaveThumbnailBar::selectUntil(int idx) {
  int startIdx = 0;
  numSelected = 0;
  for (int i = 0; i < idx; ++i) {
    if (row.thumbnails[i]->isSelected()) {
      startIdx = i;
      ++numSelected;
    }
  }
  for (int i = startIdx + 1; i < row.thumbnails.size(); ++i) {
    if (i <= idx) {
      row.thumbnails[i]->setSelected(true);
      ++numSelected;
    } else {
      row.thumbnails[i]->setSelected(false);
    }
  }
}

void WaveThumbnailBar::addToSelection(int idx) {
  row.thumbnails[idx]->setSelected(true);
  ++numSelected;
}

void WaveThumbnailBar::removeFromSelection(int idx) {
  row.thumbnails[idx]->setSelected(false);
  --numSelected;
}

void WaveThumbnailBar::clearSelection() {
  for (int i = 0; i < row.thumbnails.size(); ++i) {
    row.thumbnails[i]->setSelected(false);
  }
  numSelected = 0;
}
