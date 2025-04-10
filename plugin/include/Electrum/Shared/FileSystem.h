#pragma once
#include "../Common.h"
#include "Electrum/Identifiers.h"

// the categories we'll divide the patch library into
enum patch_categ_t { Bass, Lead, Keys, Pad, Other };

enum wave_categ_t { Analog, Digital, User };

// what the library/patch browser system needs to know
// about a patch
struct patch_meta_t {
  String name;
  String path;
  String author;
  String description;
  int category;
  //------------
  static ValueTree toValueTree(const patch_meta_t& patch);
  static patch_meta_t fromValueTree(ValueTree& vt);
};

// same idea for wavetables
struct wave_meta_t {
  String name = "untitled";
  String path;
  String author;
  int category;
  //------------
  // NOTE- the wave's serial data still needs to be attached
  // to this ValueTree before saving the fine
  static ValueTree toValueTree(const wave_meta_t& wave);
  static wave_meta_t fromValueTree(ValueTree& vt);
};

/* Namespace for handling access to patches
 * on the file system
 * */
namespace UserFiles {

const juce::StringArray PatchCategStrings = {"Bass", "Lead", "Keys", "Pad",
                                             "Other"};
const juce::StringArray WaveCategStrings = {"Analog", "Digital", "User"};
#define NUM_PATCH_CATEGORIES 5
#define NUM_WAVE_CATEGORIES 3

const String patchFileExt = ".epf";
const String waveFileExt = ".ewf";
File getPatchesFolder();
File getWavetablesFolder();
// check whether a file is a valid Electrum patch
bool isValidPatch(const File& file);
// check for a valid wavetable
bool isValidWave(const File& file);
// call this with the 'state' member of ElectrumState
// save a patch. returns success or failure
bool attemptPatchSave(ValueTree& state);
bool attemptWaveSave(const wave_meta_t& wave, const String& waveString);

// this should run on startup to
// save the list of patches on the system
std::vector<patch_meta_t> getAvailiblePatches();
std::vector<wave_meta_t> getAvailableWaves();
}  // namespace UserFiles

//====================================================
/* Object for managing and editing the
 * patch and wavetable files. the master
 * ElectrumState should own one of these
 * and let components access it via pointer
 * */
class ElectrumUserLib {
private:
  std::vector<patch_meta_t> patches;
  std::vector<wave_meta_t> waves;
  bool isPatchNameLegal(const String& name) const;
  bool isWaveNameLegal(const String& name) const;

public:
  // components can inherit this to update
  // when patches are loaded or saved
  struct Listener {
    Listener() {}
    virtual ~Listener() {}
    virtual void patchWasSaved(patch_meta_t*) {}
    virtual void waveWasSaved(wave_meta_t*) {}
  };
  ElectrumUserLib();
  // GUI should call this to check if
  // the user's entered metadata is legal
  bool validatePatchData(patch_meta_t* patch) const;
  int numPatches() const { return (int)patches.size(); }
  int numWavetables() const { return (int)waves.size(); }
  bool attemptPatchSave(apvts* tree, const patch_meta_t& patchData);
  bool attemptWaveSave(const wave_meta_t& waveData, const String& waveString);
  patch_meta_t* getPatchAtIndex(int index);
  patch_meta_t* getPatch(const String& name);

  // listener stuff
  void addListener(Listener* l) { listeners.push_back(l); }
  void removeListener(Listener* l) {
    for (auto it = listeners.begin(); it != listeners.end(); ++it) {
      if (*it == l) {
        listeners.erase(it);
        return;
      }
    }
  }

  ValueTree getMasterTreeForPatch(patch_meta_t* patch);

private:
  std::vector<Listener*> listeners;
  //
};
