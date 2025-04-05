#pragma once
#include "../GUITypedefs.h"
#include "juce_gui_basics/juce_gui_basics.h"

/*Inheriting from this is a quick and simple way
 * to attach a left click callback to a component*/
class ClickableComponent : public Component {
private:
  uint32_t lastMouseDown = 0;

public:
  std::function<void()> onClick;
  ClickableComponent() = default;
  void mouseDown(const juce::MouseEvent&) override {
    lastMouseDown = juce::Time::getApproximateMillisecondCounter();
  }
  void mouseUp(const juce::MouseEvent&) override {
    if (isMouseOver()) {
      uint32_t now = juce::Time::getApproximateMillisecondCounter();
      if (now - lastMouseDown < 1200) {
        onClick();
      }
    }
  }
};
