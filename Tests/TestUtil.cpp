#include "TestUtil.h"

std::vector<int> TestUtil::randPositiveInRange(int min, int max, int num)
{
    std::vector<int> out;
    Range<int> range(min, max);
    juce::Random r(25);
    for(int i = 0; i < num; i++)
    {
        out.push_back(r.nextInt(range));
    }
    return out;
}


MidiBuffer TestUtil::getTestMidiBuffer(int length, int numNotes, int minNoteLength, int maxNoteLength)
{
    Random r(25);
    MidiBuffer out;
    auto noteStartIndeces = randPositiveInRange(0, length - maxNoteLength, numNotes);
    std::sort(noteStartIndeces.begin(), noteStartIndeces.end());
    size_t numStartsAdded = 0;
    for(int s = 0; s < length; s++)
    {
        while (noteStartIndeces[numStartsAdded] == s)
        {
            int noteLength = r.nextInt(Range<int>(minNoteLength, maxNoteLength));
            int note = r.nextInt(Range<int>(35, 75));
            out.addEvent(MidiMessage::noteOn(1, note, 0.7f), s);
            out.addEvent(MidiMessage::noteOff(1, note, 0.7f), s + noteLength);
            ++numStartsAdded;
        }
    }
    return out;
}
