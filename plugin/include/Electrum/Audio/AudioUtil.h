#pragma once

#include "../Common.h"

/* Namespace for stuff that might
 * be useful in any number of situations
 * */
namespace AudioUtil {
int fastFloor32(float fValue);
size_t fastFloor64(float fValue);
float fastSine(float phaseNorm);

}  // namespace AudioUtil
