#include "FundamentalFinder.h"

FundamentalFinder::FundamentalFinder() :
samplesSinceLastCrossing(0),
crossingIntervals(MAX_FINDER_INTERVALS)
{

}

void FundamentalFinder::push(float value)
{
    bool prevAbove = prevValue > 0.0f;
    bool currentAbove = value > 0.0f;
    if (prevAbove != currentAbove)
    {
        crossingIntervals.push(samplesSinceLastCrossing);
        samplesSinceLastCrossing = 0;
    }
    else
    {
        ++samplesSinceLastCrossing;
    }
    prevValue = value;
}