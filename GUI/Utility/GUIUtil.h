#pragma once
#ifndef GUIUTIL_H
#define GUIUTIL_H
#include "../../Parameters/MathUtil.h"
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

    inline Path ringSegment(float centerX, float centerY, float r1, float r2, float startRads, float endRads)
    {
        Path out;
        Point<float> center(centerX, centerY);
        auto innerArcStart = center.getPointOnCircumference(r1, startRads);
        auto outerArcEnd = center.getPointOnCircumference(r2, endRads);
        out.addCentredArc(centerX, centerY, r1, r1, 0.0f, startRads, endRads);
        out.lineTo(outerArcEnd);
        out.addCentredArc(centerX, centerY, r2, r2, 0.0f, endRads, startRads);
        out.lineTo(innerArcStart);
        
        return out;
    }

    inline void addExpCurveToPath(Path& path, Point<float> p0, Point<float> p1, Point<float> p2, int numSegments=50)
    {
        path.lineTo(p0);
    }
}


#endif