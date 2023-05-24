#pragma once
#ifndef GUIUTIL_H
#define GUIUTIL_H
#include "../Core/CustomJuceHeader.h"
namespace GUIUtil
{
    inline void strokeArc(Graphics& g, float x, float y, float w, float h, float startRads, float endRads, float lineWeight)
    {
        Path path;
        path.addArc(x, y, w, h, startRads, endRads, true);
        PathStrokeType pst(lineWeight, PathStrokeType::JointStyle::mitered, PathStrokeType::EndCapStyle::rounded);
        g.strokePath(path, pst);
    }
    inline void strokeArc(Graphics& g, Rectangle<float> area, float startRads, float endRads, float lineWeight)
    {
        strokeArc(g, area.getX(), area.getY(), area.getWidth(), area.getHeight(), startRads, endRads, lineWeight);
    }

    inline AffineTransform getTransformForSlider(Rectangle<float>& area, float startRads, float endRads, float sliderPos)
    {
        auto angle = Math::flerp(startRads, endRads, sliderPos);
        float pivotX = area.getX() + (area.getWidth() / 2.0f);
        float pivotY = area.getY() + (area.getHeight() / 2.0f);
        return AffineTransform::rotation(angle, pivotX, pivotY);
    }
}


#endif