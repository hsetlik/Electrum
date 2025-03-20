cmake_minimum_required(VERSION 3.22)
# Version is needed by JUCE.
project(Electrum VERSION 0.1.0)
# Adding a directory with the library/application name as a subfolder of the
# include folder is a good practice. It helps avoid name clashes later on.
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/Electrum")
# Adds a plugin target (that's basically what the Projucer does).

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME Setlik # change this
    IS_SYNTH TRUE # may change this
    NEEDS_MIDI_INPUT TRUE # may change this
    NEEDS_MIDI_OUTPUT FALSE # may change this
    PLUGIN_MANUFACTURER_CODE STLK # change this
    PLUGIN_CODE ELRT # change this
    FORMATS Standalone VST3  AU # may change this
    PRODUCT_NAME "Electrum" # change this
    COPY_PLUGIN_AFTER_BUILD TRUE 
)

# Sets the source files of the plugin project.
target_sources(${PROJECT_NAME}
    PRIVATE
        source/Identifiers.cpp
        source/Common.cpp
        source/PluginEditor.cpp
        source/PluginProcessor.cpp
        ${INCLUDE_DIR}/Identifiers.h
        ${INCLUDE_DIR}/Common.h
        ${INCLUDE_DIR}/PluginEditor.h
				source/ModContextComponent.cpp
				${INCLUDE_DIR}/GUI/Modulation/ModContextComponent.h
				source/ElectrumEditor.cpp
				${INCLUDE_DIR}/GUI/ElectrumEditor.h
				source/Wavetable.cpp
				${INCLUDE_DIR}/Audio/Wavetable.h
				source/AudioUtil.cpp
				${INCLUDE_DIR}/Audio/AudioUtil.h
				source/HarmonicSeries.cpp
				${INCLUDE_DIR}/Audio/HarmonicSeries.h
				source/Engine.cpp
				${INCLUDE_DIR}/Audio/Synth/Engine.h
				source/Oscillator.cpp
				${INCLUDE_DIR}/Audio/Generator/Oscillator.h
				source/Voice.cpp
				${INCLUDE_DIR}/Audio/Synth/Voice.h
        ${INCLUDE_DIR}/PluginProcessor.h
        ${INCLUDE_DIR}/GUI/Modulation/ModSourceComponent.h
        ${INCLUDE_DIR}/Shared/ElectrumState.h
        source/ElectrumState.cpp
        source/ModSourceComponent.cpp
)
# Sets the include directories of the plugin project.
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# Links to all necessary dependencies. The present ones are recommended by JUCE.
# If you use one of the additional modules, like the DSP module, you need to specify it here.
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        juce::juce_dsp
)


# These definitions are recommended by JUCE.
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)
# Enables all warnings and treats warnings as errors.
# This needs to be set up only for your projects, not 3rd party
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# In Visual Studio this command provides a nice grouping of source files in "filters".
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)

