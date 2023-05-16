#include "../../Parameters/ElectrumValueTree.h"

class WavetableOscillator
{
private:
    EVT* const state;
    const int index;
    float phase;
    float baseWavetablePos;
public:
    WavetableOscillator(EVT* t, int idx);
    float getNextSample(double freq, double sampleRate, float position);
    void setBasePosition(float value) { baseWavetablePos = value; }
    void updateBasePos();

};