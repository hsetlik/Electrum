#pragma once
#include "../Color.h"
#include "../../Parameters/MathUtil.h"
#include "../../Parameters/ElectrumValueTree.h"


class DragPoint
{
protected:
    float fX; // X position relative to the parent's width
    float fY; // Y position relative to the parent's width
    Range<float> xRange; // allowed range for fX
    Range<float> yRange; // allowed range for fY
public:
    float getX() { return fX; }
    float getY() { return fY; }
    void setXRange(float min, float max);
    void setYRange(float min, float max);
    Range<float> getXRange() { return xRange; }
    Range<float> getYRange() { return yRange; }

    float getXNormalized()
    {
        return (fX - xRange.getStart()) / (xRange.getEnd() - xRange.getStart());
    }
    float getYNormalized()
    {
        return (fX - yRange.getStart()) / (yRange.getEnd() - yRange.getStart());
    }
    // normalized setters
    void setXNormalized(float fVal)
    {
        fX = Math::flerp(xRange.getStart(), xRange.getEnd(), fVal);
    }
   void setYNormalized(float fVal)
    {
        fY = Math::flerp(yRange.getStart(), yRange.getEnd(), fVal);
    }
    // this is the function that calls the listeners
    void setPoint(float x, float y, bool updateListeners=true);
    // and these two
    void startMovement()
    {
        for (auto l : listeners)
        {
            l->moveStarted(this);
        }
    }
    void endMovement()
    {
        for (auto l : listeners)
        {
            l->moveEnded(this);
        }
    }
    
    DragPoint(float x=0.0f, float y=0.0f, Range<float> xR=Range<float>(0.0f, 0.1f), Range<float>yR=Range<float>(0.0f, 1.0f));
    DragPoint(const DragPoint& other);
    struct Listener
    {
        Listener() {}
        virtual ~Listener() {}
        virtual void moveStarted(DragPoint* point)=0;
        virtual void moveEnded(DragPoint* point)=0;
        virtual void pointMoved(DragPoint* point)=0; 
    };
    void addListener(Listener* l)
    {
        listeners.push_back(l);
    }
    void removeListener(Listener* l)
    {
        for(auto it = listeners.begin(); it != listeners.end(); it++)
        {
            if (*it == l)
            {
                listeners.erase(it);
                return;
            }
        }
    }
    void operator=(const DragPoint& other)
    {
        fX = other.fX;
        fY = other.fY;
        xRange = other.xRange;
        yRange = other.yRange;
    }
    //========================================================
    Point<int> getIPointWithin(Component* parent);
    Point<float> getFPointWithin(Component* parent, float cushion=0.0f);
    bool eventIsWithin(const MouseEvent& e, float radius);
    void moveTo(const MouseEvent& e);
private:
    std::vector<DragPoint::Listener*> listeners;
};
//========================================================================================
class DragPointParameterAttachment : public DragPoint::Listener
{
private:
    EVT* const state;
    DragPoint* const point;
    const String paramID;
    std::unique_ptr<ParameterAttachment> attach; 
    frange paramRange;
    std::function<void(float)> changeCallback;
    std::function<float(float,float)> posToValue;
public:
    /**
     * This handles all communication between the APVTS and the DragPoint objects on the UI thread
     * 
     * @param tree The EVT instance
     * @param pt The DragPoint to attach to
     * @param id ID of the parameter to be controlled
     * @param onParamChange this needs to set the position of the DragPoint based on this parameter's value
     * @param positionToValue this needs to convert the x and y values of the point to the single value of this parameter
     * 
     */
    DragPointParameterAttachment(EVT* tree, DragPoint* pt, const String& id, std::function<void(float)> onParamChange, std::function<float(float,float)> positionToValue);
    ~DragPointParameterAttachment() override
    {
        point->removeListener(this);
    }
    void pointMoved(DragPoint* point) override;
    void moveStarted(DragPoint* point) override;
    void moveEnded(DragPoint* point) override;
};