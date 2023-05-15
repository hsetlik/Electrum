#include "../../Parameters/ElectrumValueTree.h"

class WavetableOscillator
{
private:
    EVT* const state;
    const int index;
    float phase;
public:
    WavetableOscillator(EVT* t, int idx);
    float getNextSample(double freq, double sampleRate, float position);

};