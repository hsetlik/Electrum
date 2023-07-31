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


MidiBuffer TestUtil::getRandomMidiBuffer(int length, int numNotes, int minNoteLength, int maxNoteLength)
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

MidiBuffer TestUtil::getChordBuffer(int length, int numNotes, float noteLength)
{
  const std::vector<int> possibleNotes = {60, 64, 67, 71, 75, 78, 81, 83, 86};
  numNotes = std::min(numNotes, 9);
  MidiBuffer out;
  int endIdx = (int)((float)length * noteLength);
  for(size_t i = 0; i < (size_t)numNotes; i++)
  {
    out.addEvent(MidiMessage::noteOn(1, possibleNotes[i], 0.7f), 0);
    out.addEvent(MidiMessage::noteOff(1, possibleNotes[i], 0.7f), endIdx);
  }
  return out;
}

