#pragma once
#include "../Core/CustomJuceHeader.h"

namespace StringUtil
{
    inline bool charIsInString(juce_wchar c, const String& s)
    {
        for(int i = 0; i < s.length(); i++)
        {
            if (s[i] == c)
                return true;
        }
        return false;
    }
    inline String removeTrailingNumbers(const String& str)
    {
        String out = str;
        int length = out.length();
        while(charIsInString(out.getLastCharacter(), "0123456789"))
        {
            --length;
            out = out.substring(0, length);
        }
        return out;
    }
}