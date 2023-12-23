#include "Color.h"
#include "ElectrumLookAndFeel.h"
#include "FontBinaries.h"
#include "Fonts.h"

ElectrumLookAndFeel::ElectrumLookAndFeel()
    : labelFont(Fonts::getTypeface(Fonts::HelveticaNeueMedium)),
      tabButtonFont(Fonts::getTypeface(Fonts::FuturaBoldOblique))
{
  setDefaultSansSerifTypeface(Fonts::getTypeface(Fonts::HelveticaNeueMedium));
  /* We set colors up here so that we can use the 'ColourIds' in the rest of the code*/
  setColour(Label::backgroundColourId, Color::darkBkgnd);
  setColour(Label::textColourId, Color::offWhite);

  setColour(Slider::backgroundColourId, Color::darkBkgnd);
  setColour(Slider::trackColourId, Color::paleOrange);
  setColour(Slider::thumbColourId, Color::brightSeafoam);
}
void ElectrumLookAndFeel::drawRotarySlider(Graphics &g, int x, int y, int width, int height,
                                           float sliderPosProportional, float rotaryStartAngle,
                                           float rotaryEndAngle, Slider &)
{
  float centerX = (float)x + ((float)width / 2.0f);
  float centerY = (float)y + ((float)height / 2.0f);
  float rOuter = (float)width / 2.0f;
  float rInner = rOuter * 0.75f;
  float thumbCenterAngle = Math::flerp(rotaryStartAngle, rotaryEndAngle, sliderPosProportional);
  Path leftTrack;
  leftTrack.addCentredArc(centerX, centerY, rInner, rInner, 0.0f, rotaryStartAngle,
                          thumbCenterAngle, true);
  leftTrack.addCentredArc(centerX, centerY, rOuter, rOuter, 0.0f, thumbCenterAngle,
                          rotaryStartAngle, false);
  leftTrack.closeSubPath();
  g.setColour(findColour(Slider::ColourIds::trackColourId).darker());
  g.fillPath(leftTrack);

  Path rightTrack;
  rightTrack.addCentredArc(centerX, centerY, rInner, rInner, 0.0f, thumbCenterAngle, rotaryEndAngle,
                           true);
  rightTrack.addCentredArc(centerX, centerY, rOuter, rOuter, 0.0f, rotaryEndAngle, thumbCenterAngle,
                           false);
  rightTrack.closeSubPath();
  g.setColour(findColour(Slider::ColourIds::trackColourId));
  g.fillPath(rightTrack);

  const float thumbAngleWidth = 0.2f;
  Path thumb;
  thumb.addCentredArc(centerX, centerY, rOuter, rOuter, 0.0f, thumbCenterAngle - thumbAngleWidth,
                      thumbCenterAngle + thumbAngleWidth, true);
  thumb.addCentredArc(centerX, centerY, rInner * 0.85f, rInner * 0.85f, 0.0f,
                      thumbCenterAngle + thumbAngleWidth, thumbCenterAngle - thumbAngleWidth,
                      false);
  thumb.closeSubPath();
  auto thumbColor = findColour(Slider::ColourIds::thumbColourId);
  g.setColour(thumbColor);
  g.fillPath(thumb);
}

Font ElectrumLookAndFeel::getLabelFont(Label &l)
{
  return l.getFont().withHeight((float)(l.getHeight() - l.getBorderSize().getTopAndBottom()));
}

void ElectrumLookAndFeel::drawLabel(Graphics &g, Label &l)
{
  auto f = getLabelFont(l);
  g.setFont(f);
  auto b = l.getBorderSize();
  // fill the background
  g.setColour(findColour(Label::ColourIds::backgroundColourId));
  g.fillRoundedRectangle(l.getLocalBounds().toFloat(), 8.0f);
  // draw the text
  auto textBounds = b.subtractedFrom(l.getLocalBounds());
  g.setColour(findColour(Label::ColourIds::textColourId));
  g.drawFittedText(l.getText(true), textBounds, l.getJustificationType(), 1);
}
//=======================================================================================
void ElectrumLookAndFeel::drawTabButton(TabBarButton &button, Graphics &g, bool isMouseOver,
                                        bool isMouseDown)
{
  Path tabShape;
  createTabButtonShape(button, tabShape, isMouseOver, isMouseDown);

  auto activeArea = button.getActiveArea();
  tabShape.applyTransform(
      AffineTransform::translation((float)activeArea.getX(), (float)activeArea.getY()));

  DropShadow(Colours::black.withAlpha(0.5f), 2, Point<int>(0, 1)).drawForPath(g, tabShape);

  fillTabButtonShape(button, g, tabShape, isMouseOver, isMouseDown);
  drawTabButtonText(button, g, isMouseOver, isMouseDown);
}

Font ElectrumLookAndFeel::getTabButtonFont(TabBarButton &, float height)
{
  return tabButtonFont.withHeight(height * 0.6f);
}
void ElectrumLookAndFeel::drawTabButtonText(TabBarButton &button, Graphics &g, bool isMouseOver,
                                            bool isMouseDown)
{
  //  DLog::log("Drawing custom tab button text");
  auto area = button.getTextArea().toFloat();

  auto length = area.getWidth();
  auto depth = area.getHeight();

  if (button.getTabbedButtonBar().isVertical())
    std::swap(length, depth);

  Font font(getTabButtonFont(button, depth));
  font.setUnderline(button.hasKeyboardFocus(false));

  AffineTransform t;

  switch (button.getTabbedButtonBar().getOrientation())
  {
  case TabbedButtonBar::TabsAtLeft:
    t = t.rotated(MathConstants<float>::pi * -0.5f).translated(area.getX(), area.getBottom());
    break;
  case TabbedButtonBar::TabsAtRight:
    t = t.rotated(MathConstants<float>::pi * 0.5f).translated(area.getRight(), area.getY());
    break;
  case TabbedButtonBar::TabsAtTop:
  case TabbedButtonBar::TabsAtBottom:
    t = t.translated(area.getX(), area.getY());
    break;
  default:
    jassertfalse;
    break;
  }
  Colour col;
  if (button.isFrontTab() && (button.isColourSpecified(TabbedButtonBar::frontTextColourId) ||
                              isColourSpecified(TabbedButtonBar::frontTextColourId)))
    col = findColour(TabbedButtonBar::frontTextColourId);
  else if (button.isColourSpecified(TabbedButtonBar::tabTextColourId) ||
           isColourSpecified(TabbedButtonBar::tabTextColourId))
    col = findColour(TabbedButtonBar::tabTextColourId);
  else
    col = button.getTabBackgroundColour().contrasting();

  auto alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;

  g.setColour(col.withMultipliedAlpha(alpha));
  g.setFont(font);
  g.addTransform(t);

  g.drawFittedText(button.getButtonText().trim(), 0, 0, (int)length, (int)depth,
                   Justification::centred, jmax(1, ((int)depth) / 12));
}
void ElectrumLookAndFeel::createTabButtonShape(TabBarButton &button, Path &p, bool /*mouseOver*/,
                                               bool /*mouseDown*/)
{
  auto activeArea = button.getActiveArea();
  auto w = (float)activeArea.getWidth();
  auto h = (float)activeArea.getHeight();

  auto length = w;
  auto depth = h;

  if (button.getTabbedButtonBar().isVertical())
    std::swap(length, depth);

  const float indent = (float)getTabButtonOverlap((int)depth);
  const float overhang = 4.0f;

  switch (button.getTabbedButtonBar().getOrientation())
  {
  case TabbedButtonBar::TabsAtLeft:
    p.startNewSubPath(w, 0.0f);
    p.lineTo(0.0f, indent);
    p.lineTo(0.0f, h - indent);
    p.lineTo(w, h);
    p.lineTo(w + overhang, h + overhang);
    p.lineTo(w + overhang, -overhang);
    break;

  case TabbedButtonBar::TabsAtRight:
    p.startNewSubPath(0.0f, 0.0f);
    p.lineTo(w, indent);
    p.lineTo(w, h - indent);
    p.lineTo(0.0f, h);
    p.lineTo(-overhang, h + overhang);
    p.lineTo(-overhang, -overhang);
    break;

  case TabbedButtonBar::TabsAtBottom:
    p.startNewSubPath(0.0f, 0.0f);
    p.lineTo(indent, h);
    p.lineTo(w - indent, h);
    p.lineTo(w, 0.0f);
    p.lineTo(w + overhang, -overhang);
    p.lineTo(-overhang, -overhang);
    break;

  case TabbedButtonBar::TabsAtTop:
  default:
    p.startNewSubPath(0.0f, h);
    p.lineTo(indent, 0.0f);
    p.lineTo(w - indent, 0.0f);
    p.lineTo(w, h);
    p.lineTo(w + overhang, h + overhang);
    p.lineTo(-overhang, h + overhang);
    break;
  }
  p.closeSubPath();
  p = p.createPathWithRoundedCorners(3.0f);
}
