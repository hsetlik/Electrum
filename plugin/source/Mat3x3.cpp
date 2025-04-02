#include "Electrum/GUI/Wavetable/Mat3x3.h"

Mat3x3 Mat3x3::getRotationMatrix(float x, float y, float z) {
  Mat3x3 m1;

  m1.data[0][0] = 1.0f;
  m1.data[0][1] = 0.0f;
  m1.data[0][2] = 0.0f;

  m1.data[1][0] = 0.0f;
  m1.data[1][1] = std::cosf(x);
  m1.data[1][2] = std::sinf(x);

  m1.data[2][0] = 0.0f;
  m1.data[2][1] = std::sinf(x) * -1.0f;
  m1.data[2][2] = std::cosf(x);

  Mat3x3 m2;

  m2.data[0][0] = std::cosf(y);
  m2.data[0][1] = 0.0f;
  m2.data[0][2] = std::sinf(y) * -1.0f;

  m2.data[1][0] = 0.0f;
  m2.data[1][1] = 1.0f;
  m2.data[1][2] = 0.0f;

  m2.data[2][0] = 0.0f;
  m2.data[2][1] = std::sinf(x) * -1.0f;
  m2.data[2][2] = std::cosf(x);

  Mat3x3 m3;

  m3.data[0][0] = std::cosf(z);
  m3.data[0][1] = std::sinf(z);
  m3.data[0][2] = 0.0f;

  m3.data[1][0] = std::sinf(z) * -1.0f;
  m3.data[1][1] = std::cosf(z);
  m3.data[1][2] = 0.0f;

  m3.data[2][0] = 0.0f;
  m3.data[2][1] = 0.0f;
  m3.data[2][2] = 1.0f;

  return (m1 * m2) * m3;
}
//===================================================
