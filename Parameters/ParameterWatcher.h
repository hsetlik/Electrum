#pragma once
#include "MathUtil.h"
#include "Identifiers.h"

class ParameterWatcher : public ValueTree::Listener
{
public:
    void valueTreePropertyChanged(ValueTree &tree, const Identifier &property) override
    {
        if(property.toString().contains(IDs::attackCurve.toString()))
        {
            float value = tree[property];
            DLog::log("Curve value is: " + String(value));
        }
    }
};