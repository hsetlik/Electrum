cmake_minimum_required(VERSION 3.4)

project("Electrum" VERSION 1.0.0)

# Juce
add_subdirectory(JUCE)
add_subdirectory(JUCE/extras/AudioPluginHost)

include(FetchContent)

# Juce module wrapper for Perfetto profiler
FetchContent_Declare (
  melatonin_perfetto
  GIT_REPOSITORY https://github.com/sudara/melatonin_perfetto.git
  GIT_TAG origin/main
)

FetchContent_MakeAvailable (melatonin_perfetto)

set(SOURCES
# /Core
"${CMAKE_CURRENT_LIST_DIR}/Core/PluginEditor.h"
"${CMAKE_CURRENT_LIST_DIR}/Core/PluginEditor.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Core/PluginProcessor.h"
"${CMAKE_CURRENT_LIST_DIR}/Core/PluginProcessor.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Core/CustomJuceHeader.h"
# /Parameters
"${CMAKE_CURRENT_LIST_DIR}/Parameters/Identifiers.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/ElectrumValueTree.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/MathUtil.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/DLog.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/ElectrumAudioData.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/ElectrumAudioData.cpp"
#/Audio
"${CMAKE_CURRENT_LIST_DIR}/Audio/Modulators/Envelope.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Modulators/Oscillator.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/AudioBasics.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/ElectrumEngine.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/ElectrumEngine.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Audio/ElectrumVoice.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/ElectrumVoice.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Generators/Wavetable.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Generators/Wavetable.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Generators/WavetableOscillator.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Generators/WavetableOscillator.cpp"
#/GUI
"${CMAKE_CURRENT_LIST_DIR}/GUI/ElectrumEditor.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ElectrumEditor.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/GeneratorPanel/OscillatorEditor.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/GeneratorPanel/OscillatorEditor.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/EnvelopeEditor.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/EnvelopeEditor.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/ModWheelSource.h"
#/GUI/Modulation
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/ModulationDestSlider.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/ModulationDestSlider.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/ModulationSourceComponent.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/ModulationSourceComponent.cpp"
)


set(FORMATS AU VST3)
# By default we don't want Xcode schemes to be made for modules, etc
set(CMAKE_XCODE_GENERATE_SCHEME OFF)

juce_add_plugin("${PROJECT_NAME}"
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME "Hayden Setlik"
    BUNDLE_ID "com.HaydenSetlik.${PROJECT_NAME}"
    IS_SYNTH TRUE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT TRUE              # Does the plugin need midi input?
    # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE # On MacOS, plugin will be copied to /Users/you/Library/Audio/Plug-Ins/
    PLUGIN_MANUFACTURER_CODE Bgst               # This has to be one uppercase, rest lower for AU formats
    PLUGIN_CODE Bgst                    # A unique four-character plugin id with at least one upper-case character
    FORMATS "${FORMATS}"
    PRODUCT_NAME "${PROJECT_NAME}")

juce_enable_copy_plugin_step("${PROJECT_NAME}")


target_sources("${PROJECT_NAME}" PRIVATE "${SOURCES}")

target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_20)

target_compile_definitions("${PROJECT_NAME}"
    PUBLIC
    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries("${PROJECT_NAME}"
    PUBLIC
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_cryptography
    juce::juce_audio_processors
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
    PRIVATE
    Melatonin::Perfetto
  )
