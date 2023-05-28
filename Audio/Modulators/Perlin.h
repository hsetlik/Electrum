#ifndef PERLIN_H
#define PERLIN_H
#pragma once
#include "../../Parameters/MathUtil.h"
#include "../../Parameters/DLog.h"
#include "../../Parameters/AudioSystem.h"

class Perlin
{
private:
    static float getNoise(float x);
public:
   static float getFractal(float x, size_t octaves, float frequency, float lacunarity);
};

class PerlinGenerator
{
private:
    size_t currentOctaves;
    float currentFreq;
    float currentLacunarity;
    float currentX;
public:
    PerlinGenerator() :
    currentOctaves(1),
    currentFreq(1.0f),
    currentLacunarity(2.0f),
    currentX(0.0f)
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
        currentX += (currentFreq / ((float)AudioSystem::getSampleRate() * 6.0f));
        return Perlin::getFractal(currentX, currentOctaves, currentFreq, currentLacunarity);
    }

};

#endif
