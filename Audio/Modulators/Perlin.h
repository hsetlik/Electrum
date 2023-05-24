#ifndef PERLIN_H
#define PERLIN_H
#pragma once
#include "../../Parameters/MathUtil.h"

class Perlin
{
private:
    static float getNoise(float x);
    float currentX;
    float getFractalPriv(size_t octaves, float frequency,  float lacunarity);
   Perlin();
   ~Perlin();
public:
   static float getFractal(size_t octaves, float frequency, float lacunarity);
   JUCE_DECLARE_SINGLETON(Perlin, false)
};

class PerlinGenerator
{
private:
    size_t currentOctaves;
    float currentFreq;
    float currentLacunarity;
public:
    PerlinGenerator() :
    currentOctaves(1),
    currentFreq(1.0f),
    currentLacunarity(2.0f)
    {

    }
    void setParams(size_t octaves, float frequency, float lacunarity)
    {
        currentOctaves = octaves;
        currentFreq = frequency;
        currentLacunarity = lacunarity;
    }
    float getNextValue()
    {
        return Perlin::getFractal(currentOctaves, currentFreq, currentLacunarity);
    }

};

#endif
