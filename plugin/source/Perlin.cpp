#include "Electrum/Audio/Modulator/Perlin.h"
#include "Electrum/Audio/AudioUtil.h"

namespace Perlin {
/**
 * Permutation table. This is just a random jumble of all numbers 0-255.
 *
 * This produce a repeatable pattern of 256, but Ken Perlin stated
 * that it is not a problem for graphic texture as the noise features disappear
 * at a distance far enough to be able to see a repeatable pattern of 256.
 *
 * This needs to be exactly the same for all instances on all platforms,
 * so it's easiest to just keep it as static explicit data.
 * This also removes the need for any initialisation of this class.
 *
 * Note that making this an uint32_t[] instead of a uint8_t[] might make the
 * code run faster on platforms with a high penalty for unaligned single
 * byte addressing. Intel x86 is generally single-byte-friendly, but
 * some other CPUs are faster with 4-aligned reads.
 * However, a char[] is smaller, which avoids cache trashing, and that
 * is probably the most important aspect on most architectures.
 * This array is accessed a *lot* by the noise functions.
 * A vector-valued noise over 3D accesses it 96 times, and a
 * float-valued 4D noise 64 times. We want this to fit in the cache!
 */
static const uint8_t perm[256] = {
    151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,
    225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190,
    6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203, 117,
    35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125, 136,
    171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158,
    231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,  46,
    245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,  209,
    76,  132, 187, 208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3,   64,  52,  217, 226, 250, 124, 123, 5,
    202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,  16,
    58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248, 152, 2,   44,
    154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253,
    19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246, 97,
    228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,
    145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157, 184,
    84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205, 93,
    222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,  156,
    180};

/**
 * Helper function to hash an integer using the above permutation table
 *
 *  This inline function costs around 1ns, and is called N+1 times for a noise
 * of N dimension.
 *
 *  Using a real hash function would be better to improve the "repeatability of
 * 256" of the above permutation table, but fast integer Hash functions uses
 * more time and have bad random properties.
 *
 * @param[in] i Integer value to hash
 *
 * @return 8-bits hashed value
 */
static inline uint8_t hash(int i) {
  return perm[static_cast<uint8_t>(i)];
}

/**
 * Helper function to compute gradients-dot-residual vectors (1D)
 *
 * @note that these generate gradients of more than unit length. To make
 * a close match with the value range of classic Perlin noise, the final
 * noise values need to be rescaled to fit nicely within [-1,1].
 * (The simplex noise functions as such also have different scaling.)
 * Note also that these noise functions are the most practical and useful
 * signed version of Perlin noise.
 *
 * @param[in] hash  hash value
 * @param[in] x     distance to the corner
 *
 * @return gradient value
 */
static float grad(int hash, float x) {
  const int h = hash & 0x0F;    // Convert low 4 bits of hash code
  float grad = 1.0f + (h & 7);  // Gradient value 1.0, 2.0, ..., 8.0
  if ((h & 8) != 0)
    grad = -grad;     // Set a random sign for the gradient
  return (grad * x);  // Multiply the gradient with the distance
}

// 1D perlin function
float getNoise(float x) {
  float n0 = 0.0f;
  float n1 = 0.0f;

  int32_t i0 = AudioUtil::fastFloor32(x);
  int32_t i1 = i0 + 1;

  float x0 = x - (float)i0;
  float x1 = x0 - 1.0f;
  // add from first corner
  float t0 = 1.0f - (x0 * x0);
  t0 *= t0;
  n0 = t0 * t0 * grad(hash(i0), x0);
  // add from second corner
  float t1 = 1.0f - (x1 * x1);
  t1 *= t1;
  n1 = t1 * t1 * grad(hash(i1), x1);
  // scale by 0.395 to get into the 0,1 range
  float output = juce::jmap(n0 + n1, -2.532f, 2.532f, 0.0f, 1.0f);
  jassert(output >= 0.0f && output < 1.0f);
  return output;
}

// main fractal Perlin function

float getFractal(float x, size_t octaves, float frequency, float lacunarity) {
  float output = 0.0f;
  float denom = 0.0f;
  float currentFreq = frequency;
  float amp = 1.0f;

  for (size_t i = 0; i < octaves; i++) {
    output += (amp * getNoise(x * currentFreq));
    denom += amp;

    currentFreq *= lacunarity;
    amp *= (1.0f / lacunarity);
  }
  return output / denom;
}

}  // namespace Perlin
//===================================================
void PerlinGenerator::setParams(size_t octaves,
                                float frequency,
                                float lacunarity) {
  currentOctaves = octaves;
  currentFreq = frequency;
  xDelta = (currentFreq / ((float)SampleRate::get() * 6.0f));
  currentLacunarity = lacunarity;
}

void PerlinGenerator::tick() {
  currentX += xDelta;
  lastOutput = Perlin::getFractal(currentX, currentOctaves, currentFreq,
                                  currentLacunarity);
}
