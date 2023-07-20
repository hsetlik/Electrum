#pragma once
#include "../Core/CustomJuceHeader.h"
#define SEMITONE_RATIO 1.05946309436f
#ifndef MATHUTIL_H
#define MATHUTIL_H
namespace Math
{
    // interpolate between a and b where t is in the range 0, 1
    inline float flerp (float a, float b, float t)
    {
        return a + ((b - a) * t);
    }
    inline float bipolarFlerp(float min, float max, float current, float t)
    {
      if(t > 0.0f)
        return flerp(current, max, t); 
      return flerp(min, current, 1.0f - std::fabs(t)); 
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
    // get the fundamental frequency in hertz for a midi note
    inline double midiToHz(int midiNum)
    {
        return 440.0f * std::pow (SEMITONE_RATIO, (float)midiNum - 69);
    }
    inline double midiToHzTunedUp(int midiNum, float amt)
    {
        return dlerp(midiToHz(midiNum), midiToHz(midiNum + 1), (double)amt);
    }
    inline double midiToHzTunedDown(int midiNum, float amt)
    {
        return dlerp(midiToHz(midiNum), midiToHz(midiNum - 1), (double)amt);
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
        return jmap((float)val, 0.0f, 16383.0f, -1.0f, 1.0f);
    }
    //the Y value of a basic quadratic bezier curve at point t
    // inline float bezierValueFor(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float t)
    // {
    //     // find the first three points
    //     float t1x = flerp(x1, x2, t);
    //     float t1y = flerp(y1, y2, t);
    //     float t2x = flerp(x2, x3, t);
    //     float t2y = flerp(y2, y3, t);
    //     float t3x = flerp(x3, x4, t);
    //     float t3y = flerp(y3, y4, t);
    //     // the next two
    //     float t4x = flerp(t1x, t2x, t);
    //     float t4y = flerp(t1y, t2y, t);
    //     float t5x = flerp(t2x, t3x, t);
    //     float t5y = flerp(t2y, t3y, t);

    // }
    /**
     * @brief Find the minimum distance betwen a line (defined by two points) and some third point
     * 
     * @param x1 First line point X
     * @param y1 First line point Y
     * @param x2 Second line point X
     * @param y2 Second line point Y
     * @param pX Distance point X 
     * @param pY Distance point Y
     * @return the minimum distance
     */
    inline float perpindicularDistance(float x1, float y1, float x2, float y2, float pX, float pY)
    {
        float m = (y2 - y1) / (x2 - x1);
        // the original line:
        // point slope: y - y1 = m(x - x1)
        // slope-intercept: y = mx - (mx1 + y1)
        // yInt = -mx1 + y1
        // y + yInt = mx
        // mx - y = yInt
        float yInt = -1.0f * ((m * x1) + y1);
        return std::fabs((m * pX) + (-1.0f * pY) + yInt) / std::sqrt((m * m) + 1.0f);
    }

    inline size_t fastFloor(float fp)
    {
        size_t i = static_cast<size_t>(fp);
        return (fp < i) ? (i - 1) : (i);
    }

    inline float onEasingCurve(float y0, float y1, float y2, float x)
    {
      float yM = jmap(y1, y0, y2, 0.0f, 1.0f); // normalize y1 between 0 and 1 to use the exponential parent function
      // this is a basic exponential function where we know that:
  //     f(x) = x^t 
  //     f(0.5) = yM
  //     0.5^t = yM
  //     log<0.5>(yM) = t
      float t = std::log(yM) / std::log(0.5f);
      float val = std::pow(x, t);
      return flerp(y0, y2, val); // de-normalize from 0-1 range back to full range (undo the jmap in other words)
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
