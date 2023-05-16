#include "../Core/CustomJuceHeader.h"
#ifndef DLOG_H
#define DLOG_H
struct DLog
{
    static void log(const String& str)
    {
        #if JUCE_DEBUG
        std::cout << str.toStdString() << "\n";
        #endif
    }
};

#endif