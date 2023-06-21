#pragma once 
#include "../AudioUtil.h"
#include "../../Parameters/CircularBuffer.h"
#define MAX_FINDER_INTERVALS 1024

class FundamentalFinder
{
private:
    size_t samplesSinceLastCrossing;
    CircularBuffer<size_t> crossingIntervals;
    float prevValue;
public:
    FundamentalFinder();
    void push(float value);
};
