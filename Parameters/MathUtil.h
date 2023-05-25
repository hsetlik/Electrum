#pragma once
#include "../Core/CustomJuceHeader.h"
#define SEMITONE_RATIO 1.05946309436f
#ifndef MATHUTIL_H
#define MATHUTIL_H
namespace Math
{
    inline float flerp (float a, float b, float t)
    {
        return a + ((b - a) * t);
    }

    inline double dlerp (double a, double b, double t)
    {
        return a + ((b - a) * t);
    }
    inline Colour clerp(const Colour& a, const Colour& b, float t)
    {
        return Colour::fromHSV(
        flerp(a.getHue(), b.getHue(), t),
        flerp(a.getSaturation(), b.getSaturation(), t),
        flerp(a.getBrightness(), b.getBrightness(), t),
        flerp(a.getFloatAlpha(), b.getFloatAlpha(), t));
    }
    // get the fundamenta frequency in hertz for a midi note
    inline double midiToHz(int midiNum)
    {
        return 440.0f * std::pow (SEMITONE_RATIO, (float)midiNum - 69);
    }
    inline double midiToHzTunedUp(int midiNum, int cents)
    {
        double dCents = (double)cents / 100.0f;
        return dlerp(midiToHz(midiNum), midiToHz(midiNum + 1), dCents);
    }
    inline double midiToHzTunedDown(int midiNum, int cents)
    {
        double dCents = (double)cents / 100.0f;
        return dlerp(midiToHz(midiNum - 1), midiToHz(midiNum), 1.0f - dCents);
    }
    inline float fconstrain(float min, float max, float value)
    {
        return std::min(std::max(value, min), max);
    }
    //convert an int in the range 0, 16383 into a float in the range -1.0, 1.0
    inline float toPitchBendValue(int val)
    {
        //make sure we're getting a valid value
        jassert(val < 16384);
        return jmap((float)val, 0.0f, 16383.0f, 0.0f, 1.0f);
    }
}

struct FFT
{
    static void runFloat(int N, float *ar, float *ai)
    /*
     in-place complex fft
     
     After Cooley, Lewis, and Welch; from Rabiner & Gold (1975)
     
     program adapted from FORTRAN
     by K. Steiglitz  (ken@princeton.edu)
     Computer Science Dept.
     Princeton University 08544          */
    {
        int i, j, k, L;            /* indexes */
        int M, TEMP, LE, LE1, ip;  /* M = log N */
        int NV2, NM1;
        double t;               /* temp */
        float Ur, Ui, Wr, Wi, Tr, Ti;
        float Ur_old;
        // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2
        NV2 = N >> 1;
        NM1 = N - 1;
        TEMP = N; /* get M = log N */
        M = 0;
        while (TEMP >>= 1)
            ++M;
        /* shuffle */
        j = 1;
        for (i = 1; i <= NM1; i++)
        {
            if(i<j)
            {             /* swap a[i] and a[j] */
                t = ar[j-1];
                ar[j-1] = ar[i-1];
                ar[i-1] = t;
                t = ai[j-1];
                ai[j-1] = ai[i-1];
                ai[i-1] = t;
            }
            k = NV2;             /* bit-reversed counter */
            while(k < j)
            {
                j -= k;
                k /= 2;
            }
            j += k;
        }
        LE = 1.0f;
        for (L = 1; L <= M; L++) {            // stage L
            LE1 = LE;                         // (LE1 = LE/2)
            LE *= 2;                          // (LE = 2^L)
            Ur = 1.0f;
            Ui = 0.0f;
            Wr = cos(M_PI/(float)LE1);
            Wi = -sin(M_PI/(float)LE1); // Cooley, Lewis, and Welch have "+" here
            for (j = 1; j <= LE1; j++)
            {
                for (i = j; i <= N; i += LE)
                { // butterfly
                    ip = i+LE1;
                    Tr = ar[ip-1] * Ur - ai[ip-1] * Ui;
                    Ti = ar[ip-1] * Ui + ai[ip-1] * Ur;
                    ar[ip-1] = ar[i-1] - Tr;
                    ai[ip-1] = ai[i-1] - Ti;
                    ar[i-1]  = ar[i-1] + Tr;
                    ai[i-1]  = ai[i-1] + Ti;
                }
                Ur_old = Ur;
                Ur = Ur_old * Wr - Ui * Wi;
                Ui = Ur_old * Wi + Ui * Wr;
            }
        }
    }
};

#endif