#pragma once
#include <catch2/catch_test_macros.hpp>
#include "catch2/benchmark/catch_benchmark_all.hpp"
#include "PluginEditor.h"

struct TestUtil
{
    static std::vector<int> randPositiveInRange(int min, int max, int num);
    static MidiBuffer getRandomMidiBuffer(int length, int numNotes, int minNoteLength, int maxNoteLength);
  
    static MidiBuffer getChordBuffer(int length, int numNotes, float noteLength);
};
