#include "Electrum/GUI/LookAndFeel/BinaryGraphics.h"
#include "ImgData.h"
#include "juce_graphics/juce_graphics.h"

#define NUM_IMG_IDS 4
#define NUM_IMG_WIDTHS 3

namespace ImgData {

static size_t s_idxFor(ImgID id, ImgWidth width) {
  size_t x = (size_t)id;
  size_t y = (size_t)width;
  return (x * NUM_IMG_WIDTHS) + y;
}

static constexpr size_t NUM_IMAGES = NUM_IMG_WIDTHS * NUM_IMG_IDS;
static std::array<juce::Image, NUM_IMAGES> s_loadImages() {
  std::array<juce::Image, NUM_IMAGES> arr;
  auto& er256 = arr[0];
  er256 = juce::ImageFileFormat::loadFrom(Expand256_png, Expand256_pngSize);
  jassert(er256.isValid());
  auto& er128 = arr[1];
  er128 = juce::ImageFileFormat::loadFrom(Expand128_png, Expand128_pngSize);
  jassert(er128.isValid());
  auto& er64 = arr[2];
  er64 = juce::ImageFileFormat::loadFrom(Expand64_png, Expand64_pngSize);
  jassert(er64.isValid());
  auto& eh256 = arr[3];
  eh256 = juce::ImageFileFormat::loadFrom(ExpandHL256_png, ExpandHL256_pngSize);
  jassert(eh256.isValid());
  auto& eh128 = arr[4];
  eh128 = juce::ImageFileFormat::loadFrom(ExpandHL128_png, ExpandHL128_pngSize);
  jassert(er128.isValid());
  auto& eh64 = arr[5];
  eh64 = juce::ImageFileFormat::loadFrom(ExpandHL64_png, ExpandHL64_pngSize);
  jassert(er64.isValid());

  auto& pOn256 = arr[6];
  pOn256 = juce::ImageFileFormat::loadFrom(PowerOn256_png, PowerOn256_pngSize);
  jassert(pOn256.isValid());
  auto& pOn128 = arr[7];
  pOn128 = juce::ImageFileFormat::loadFrom(PowerOn128_png, PowerOn128_pngSize);
  jassert(pOn128.isValid());
  auto& pOn64 = arr[8];
  pOn64 = juce::ImageFileFormat::loadFrom(PowerOn64_png, PowerOn64_pngSize);
  jassert(pOn64.isValid());
  auto& pOff256 = arr[9];
  pOff256 =
      juce::ImageFileFormat::loadFrom(PowerOff256_png, PowerOff256_pngSize);
  jassert(pOff256.isValid());
  auto& pOff128 = arr[10];
  pOff128 =
      juce::ImageFileFormat::loadFrom(PowerOff128_png, PowerOff128_pngSize);
  jassert(pOff128.isValid());
  auto& pOff64 = arr[11];
  pOff64 = juce::ImageFileFormat::loadFrom(PowerOff64_png, PowerOff64_pngSize);
  jassert(pOff64.isValid());
  return arr;
}

static std::array<juce::Image, NUM_IMAGES> binImages = s_loadImages();

juce::Image& getBinaryImage(ImgID id, ImgWidth width) {
  return binImages[s_idxFor(id, width)];
}

ImgWidth widthForBounds(const juce::Rectangle<float>& bounds) {
  const float shortSide = std::min(bounds.getWidth(), bounds.getHeight());
  if (shortSide <= 64.0f) {
    return ImgWidth::p64;
  } else if (shortSide <= 128.0f) {
    return ImgWidth::p128;
  }
  return ImgWidth::p256;
}

}  // namespace ImgData

//===================================================
ImgButton::ImgButton(button_image_t i)
    : juce::Button("imgButton"), imgType(i) {}

void ImgButton::paintButton(juce::Graphics& g, bool highlighted, bool down) {
  ImgData::ImgID imgID = (highlighted || down)
                             ? (ImgData::ImgID)(imgType * NUM_IMG_IDS + 1)
                             : (ImgData::ImgID)(imgType * NUM_IMG_IDS);
  auto fBounds = getLocalBounds().toFloat();
  auto imgW = ImgData::widthForBounds(fBounds);
  auto& img = ImgData::getBinaryImage(imgID, imgW);
  g.drawImage(img, fBounds);
}
