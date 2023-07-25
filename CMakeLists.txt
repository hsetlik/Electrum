cmake_minimum_required(VERSION 3.4)

project("Electrum" VERSION 1.0.0)

message("Using Generator: ${CMAKE_GENERATOR}")
# Juce
add_subdirectory(JUCE)
add_subdirectory(JUCE/extras/AudioPluginHost)
enable_testing()

include(FetchContent)

# this sets whether the oscillators use integer indeces or linear interpolation
add_compile_definitions(INT_INDEX)

# Juce module wrapper for Perfetto profiler
FetchContent_Declare (
  melatonin_perfetto
  GIT_REPOSITORY https://github.com/sudara/melatonin_perfetto.git
  GIT_TAG origin/main
)
FetchContent_MakeAvailable (melatonin_perfetto)

#Catch2- C++ Unit testing framework
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    GIT_TAG v3.3.2) 
FetchContent_MakeAvailable(Catch2)

set(SOURCES
# /Core
"${CMAKE_CURRENT_LIST_DIR}/Core/PluginEditor.h"
"${CMAKE_CURRENT_LIST_DIR}/Core/PluginEditor.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Core/PluginProcessor.h"
"${CMAKE_CURRENT_LIST_DIR}/Core/PluginProcessor.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Core/CustomJuceHeader.h"

# /Parameters
"${CMAKE_CURRENT_LIST_DIR}/Parameters/Identifiers.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/ElectrumVoicesState.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/ElectrumVoicesState.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/ParameterWatcher.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/ElectrumValueTree.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/ElectrumValueTree.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/MathUtil.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/StringUtil.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/DLog.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/CircularBuffer.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/AudioSystem.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/AudioSystem.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/ElectrumAudioData.h"
"${CMAKE_CURRENT_LIST_DIR}/Parameters/ElectrumAudioData.cpp"

#/Audio/Modulators
"${CMAKE_CURRENT_LIST_DIR}/Audio/Modulators/Oscillator.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Modulators/Perlin.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Modulators/Perlin.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Modulators/AHDSR.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Modulators/AHDSR.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Modulators/AHDSREnvelope.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Modulators/AHDSREnvelope.cpp"

#/Audio
"${CMAKE_CURRENT_LIST_DIR}/Audio/AudioUtil.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/ElectrumEngine.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/ElectrumEngine.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Audio/ElectrumVoice.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/ElectrumVoice.cpp"

#/Audio/Generators
"${CMAKE_CURRENT_LIST_DIR}/Audio/Generators/Wavetable.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Generators/Wavetable.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Generators/WavetableOscillator.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Generators/WavetableOscillator.cpp"

#/Audio/Processors
"${CMAKE_CURRENT_LIST_DIR}/Audio/Processors/ProcessorBase.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Processors/ProcessorBase.cpp"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Processors/FilterProcessor.h"
"${CMAKE_CURRENT_LIST_DIR}/Audio/Processors/FilterProcessor.cpp"

#/GUI
"${CMAKE_CURRENT_LIST_DIR}/GUI/ElectrumEditor.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ElectrumEditor.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ElectrumLookAndFeel.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ElectrumLookAndFeel.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Color.h"

#/GUI/Utility
"${CMAKE_CURRENT_LIST_DIR}/GUI/Utility/GUIUtil.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Utility/GLUtil.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Utility/DragPoint.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Utility/DragPoint.cpp"

#/GUI/Modulation
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/ModulationDestSlider.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/ModulationDestSlider.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/ModulationSourceComponent.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/ModulationSourceComponent.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/DepthSlider.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/DepthSlider.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/DepthSliderStack.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/DepthSliderStack.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/WedgeButton.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/WedgeButton.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/SourceParamSlider.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/Modulation/SourceParamSlider.cpp"

#/GUI/ModulatorPanel
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/ModWheelSource.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/PitchWheelSource.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/PerlinPanel.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/PerlinPanel.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/PerlinGraph.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/PerlinGraph.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/EnvelopePanel.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/EnvelopePanel.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/EnvelopeTabbedComponent.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/EnvelopeTabbedComponent.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/EnvelopeGraph.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/EnvelopeGraph.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/EnvelopeLevelComponent.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ModulatorPanel/EnvelopeLevelComponent.cpp"
#/GUI/GeneratorPanel
"${CMAKE_CURRENT_LIST_DIR}/GUI/GeneratorPanel/OscillatorEditor.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/GeneratorPanel/OscillatorEditor.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/GeneratorPanel/WavetableGraph.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/GeneratorPanel/WavetableGraph.cpp"


"${CMAKE_CURRENT_LIST_DIR}/GUI/ProcessorPanel/ProcessorPanel.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ProcessorPanel/ProcessorPanel.cpp"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ProcessorPanel/FilterPanel.h"
"${CMAKE_CURRENT_LIST_DIR}/GUI/ProcessorPanel/FilterPanel.cpp"
)

# make a binary target to grip the shader files
juce_add_binary_data(shaderLib
  HEADER_NAME "Shaders.h"
  NAMESPACE "Shaders"
  SOURCES
  "${CMAKE_CURRENT_LIST_DIR}/Shaders/WavetableGraphFragment.glsl"
  "${CMAKE_CURRENT_LIST_DIR}/Shaders/WavetableGraphVertex.glsl"

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
    PLUGIN_MANUFACTURER_CODE Elrm               # This has to be one uppercase, rest lower for AU formats
    PLUGIN_CODE Elrm                    # A unique four-character plugin id with at least one upper-case character
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
    juce::juce_opengl
    juce::juce_cryptography
    juce::juce_audio_processors
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
    Melatonin::Perfetto
    shaderLib
  )
# setting up benchamrks
if(BUILD_BENCHMARKS)
message("Building benchmarks")
add_compile_definitions("BUILD_BENCHMARKS=1")
else()
message("Not building benchmarks")
add_compile_definitions("BUILD_BENCHMARKS=1")
endif()
# Setting up tests target and linking Catch2
file(GLOB_RECURSE TestFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Tests/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Tests/*.h")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Tests PREFIX "" FILES ${TestFiles})

add_executable(Tests ${TestFiles})
target_compile_features(Tests PRIVATE cxx_std_20)

target_compile_definitions(Tests INTERFACE $<TARGET_PROPERTY:${PROJECT_NAME},INTERFACE_COMPILE_DEFINITIONS>)
target_include_directories(Tests INTERFACE $<TARGET_PROPERTY:${PROJECT_NAME},INCLUDE_DIRECTORIES>)

target_include_directories(Tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Core)
target_link_libraries(Tests PRIVATE Catch2::Catch2WithMain "${PROJECT_NAME}")

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
catch_discover_tests(Tests)

# create the compile_commands.json file for xcode
if(${CMAKE_GENERATOR} STREQUAL "Xcode" AND ${CMAKE_EXPORT_COMPILE_COMMANDS})
 set(XC_PROJ_PATH "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.xcodeproj")
 set(CC_OUTPUT_PATH "${CMAKE_BINARY_DIR}/compile_commands.json")
 execute_process(COMMAND "xcodebuild -project ${XC_PROJ_PATH} | xcpretty -r json-compilation-database --output ${CC_OUTPUT_PATH}")
endif()

