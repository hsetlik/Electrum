#pragma once
#include "../Common.h"
#include "Electrum/Identifiers.h"

// the categories we'll divide the patch library into
enum patch_categ_t { Bass, Lead, Keys, Pad, Other };

// our file extensions

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

/* Namespace for handling access to patches
 * on the file system
 * */
namespace UserFiles {

const juce::StringArray PatchCategStrings = {"Bass", "Lead", "Keys", "Pad",
                                             "Other"};

const String patchFileExt = ".epf";
const String waveFileExt = ".ewf";
File getPatchesFolder();
File getWavetablesFolder();
// check whether a file is a valid Electrum patch
bool isValidPatch(const File& file);
// call this with the 'state' member of ElectrumState
// save a patch. returns success or failure
bool attemptPatchSave(ValueTree& state);

// this should run on startup to
// save the list of patches on the system
std::vector<patch_meta_t> getAvailiblePatches();
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
  bool isPatchNameLegal(const String& name) const;

public:
  ElectrumUserLib();
  // GUI should call this to check if
  // the user's entered metadata is legal
  bool validatePatchData(patch_meta_t* patch) const;
  int numPatches() const { return (int)patches.size(); }
  bool attemptPatchSave(apvts* tree, const patch_meta_t& patchData);
  patch_meta_t* getPatchAtIndex(int index);
  patch_meta_t* getPatch(const String& name);

private:
  ValueTree getMasterTreeForPatch(patch_meta_t* patch);

  //
};
