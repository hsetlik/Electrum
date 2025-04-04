#pragma once
#include "../Common.h"
#include "Electrum/Identifiers.h"

// the categories we'll divide the patch library into
enum patch_categ_t { Bass, Lead, Keys, Pad, Other };

// our file extensions

// what the library/patch browser system needs to know
// about a patch
struct patch_t {
  String name;
  String path;
  String author;
  String description;
  int category;
  //------------
  static ValueTree toValueTree(const patch_t& patch);
  static patch_t fromValueTree(ValueTree& vt);
};

/* Namespace for handling access to patches
 * on the file system
 * */
namespace UserFiles {

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
std::vector<patch_t> getAvailiblePatches();
}  // namespace UserFiles
