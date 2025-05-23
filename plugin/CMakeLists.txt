cmake_minimum_required(VERSION 3.22)
# Version is needed by JUCE.
project(Electrum VERSION 0.1.0)

#before we set up the plugin targets, we'll set up a static lib
# with our font files

# font files
set(FONTFILES
    "${CMAKE_CURRENT_LIST_DIR}/fonts/RobotoMediumItalic.ttf"
    "${CMAKE_CURRENT_LIST_DIR}/fonts/RobotoThinItalic.ttf"
    "${CMAKE_CURRENT_LIST_DIR}/fonts/RobotoThin.ttf"
    "${CMAKE_CURRENT_LIST_DIR}/fonts/FuturaBoldOblique.otf"
    "${CMAKE_CURRENT_LIST_DIR}/fonts/FuturaMediumOblique.otf"
    "${CMAKE_CURRENT_LIST_DIR}/fonts/FuturaLightCondensed.otf"
    "${CMAKE_CURRENT_LIST_DIR}/fonts/AcierDisplayNoir.otf"
    "${CMAKE_CURRENT_LIST_DIR}/fonts/HighwayGothicWide.ttf"
    "${CMAKE_CURRENT_LIST_DIR}/fonts/HelveticaNeueMedium.otf"
    "${CMAKE_CURRENT_LIST_DIR}/fonts/HelveticaNeueRegular.otf"
    "${CMAKE_CURRENT_LIST_DIR}/fonts/Futura.ttf")

juce_add_binary_data(
  fontBinaries
  HEADER_NAME
  "FontData.h"
  NAMESPACE
  "FontData"
  SOURCES
  "${FONTFILES}")

# and another for our component PNGs
set(IMGFILES
"${CMAKE_CURRENT_LIST_DIR}/img/Expand256.png"
"${CMAKE_CURRENT_LIST_DIR}/img/ExpandHL256.png"
"${CMAKE_CURRENT_LIST_DIR}/img/Expand128.png"
"${CMAKE_CURRENT_LIST_DIR}/img/ExpandHL128.png"
"${CMAKE_CURRENT_LIST_DIR}/img/Expand64.png"
"${CMAKE_CURRENT_LIST_DIR}/img/ExpandHL64.png"

"${CMAKE_CURRENT_LIST_DIR}/img/PowerOn256.png"
"${CMAKE_CURRENT_LIST_DIR}/img/PowerOff256.png"
"${CMAKE_CURRENT_LIST_DIR}/img/PowerOn128.png"
"${CMAKE_CURRENT_LIST_DIR}/img/PowerOff128.png"
"${CMAKE_CURRENT_LIST_DIR}/img/PowerOn64.png"
"${CMAKE_CURRENT_LIST_DIR}/img/PowerOff64.png"
)

juce_add_binary_data(imgBinaries
HEADER_NAME
"ImgData.h"
NAMESPACE
"ImgData"
SOURCES
"${IMGFILES}"
)
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
				source/AHDSR.cpp
				${INCLUDE_DIR}/Audio/Modulator/AHDSR.h
				source/KeyboardView.cpp
				${INCLUDE_DIR}/GUI/KeyboardView.h
				source/GraphingData.cpp
				${INCLUDE_DIR}/Shared/GraphingData.h
				source/OscillatorPanel.cpp
				${INCLUDE_DIR}/GUI/Wavetable/OscillatorPanel.h
				source/WavetableGraph.cpp
				${INCLUDE_DIR}/GUI/Wavetable/WavetableGraph.h
				source/DestinationSlider.cpp
				${INCLUDE_DIR}/GUI/Modulation/DestinationSlider.h
				source/WedgeButton.cpp
				${INCLUDE_DIR}/GUI/Modulation/WedgeButton.h
				source/Color.cpp
				${INCLUDE_DIR}/GUI/LookAndFeel/Color.h
				source/ElectrumLnF.cpp
				${INCLUDE_DIR}/GUI/LookAndFeel/ElectrumLnF.h
				source/DepthSliderStack.cpp
				${INCLUDE_DIR}/GUI/Modulation/DepthSliderStack.h
				source/Fonts.cpp
				${INCLUDE_DIR}/GUI/LookAndFeel/Fonts.h
				source/LayoutHelpers.cpp
				${INCLUDE_DIR}/GUI/LayoutHelpers.h
				source/MacroModSources.cpp
				${INCLUDE_DIR}/GUI/ModulatorPanel/MacroModSources.h
				source/Mat3x3.cpp
				${INCLUDE_DIR}/GUI/Wavetable/Mat3x3.h
				source/DragPoint.cpp
				${INCLUDE_DIR}/GUI/Util/DragPoint.h
				${INCLUDE_DIR}/GUI/GUITypedefs.h
				source/EnvelopeGraph.cpp
				${INCLUDE_DIR}/GUI/ModulatorPanel/EnvelopeGraph.h
				source/EnvelopeComponent.cpp
				${INCLUDE_DIR}/GUI/ModulatorPanel/EnvelopeComponent.h
				source/ModSourceButton.cpp
				${INCLUDE_DIR}/GUI/Modulation/ModSourceButton.h
				source/FileSystem.cpp
				${INCLUDE_DIR}/Shared/FileSystem.h
				source/PatchBrowser.cpp
				${INCLUDE_DIR}/GUI/PatchBrowser.h
				${INCLUDE_DIR}/GUI/Util/ClickableComponent.h
				source/MovingAverage.cpp
				${INCLUDE_DIR}/Audio/Filters/MovingAverage.h
				source/TPTFilter.cpp
				${INCLUDE_DIR}/Audio/Filters/TPTFilter.h
				source/Ladder.cpp
				${INCLUDE_DIR}/Audio/Filters/Ladder.h
				source/ModalParent.cpp
				${INCLUDE_DIR}/GUI/Util/ModalParent.h
				source/WaveEditor.cpp
				${INCLUDE_DIR}/GUI/WaveEditor/WaveEditor.h
				source/EditValueTree.cpp
				${INCLUDE_DIR}/GUI/WaveEditor/EditValueTree.h
				source/WaveThumbnail.cpp
				${INCLUDE_DIR}/GUI/WaveEditor/WaveThumbnail.h
				source/BinaryGraphics.cpp
				${INCLUDE_DIR}/GUI/LookAndFeel/BinaryGraphics.h
				source/CommonAudioData.cpp
				${INCLUDE_DIR}/Shared/CommonAudioData.h
				source/TimeView.cpp
				${INCLUDE_DIR}/GUI/WaveEditor/TimeView.h
				source/VoiceFilter.cpp
				${INCLUDE_DIR}/Audio/Filters/VoiceFilter.h
				source/FilterPanel.cpp
				${INCLUDE_DIR}/GUI/ProcessPanel/FilterPanel.h
				${INCLUDE_DIR}/GUI/Util/PowerButton.h
				source/WaveEdiorContext.cpp
				${INCLUDE_DIR}/GUI/WaveEditor/WaveEdiorContext.h
				source/FFTView.cpp
				${INCLUDE_DIR}/GUI/WaveEditor/FFTView.h
				source/FrameWarp.cpp
				${INCLUDE_DIR}/GUI/WaveEditor/FrameWarp.h
				source/LFO.cpp
				${INCLUDE_DIR}/Audio/Modulator/LFO.h
				source/LFOComponent.cpp
				${INCLUDE_DIR}/GUI/ModulatorPanel/LFOComponent.h
				source/LFOEdit.cpp
				${INCLUDE_DIR}/GUI/LFOEditor/LFOEdit.h
				source/BeatSyncToggle.cpp
				${INCLUDE_DIR}/GUI/Util/BeatSyncToggle.h
				source/PointwiseWave.cpp
				${INCLUDE_DIR}/Shared/PointwiseWave.h
				source/PointEditor.cpp
				${INCLUDE_DIR}/GUI/WaveEditor/PointEditor.h
				source/ScalableViewport.cpp
				${INCLUDE_DIR}/GUI/Util/ScalableViewport.h
				source/RollingRMS.cpp
				${INCLUDE_DIR}/Audio/Filters/RollingRMS.h
				source/LevelSourceComponent.cpp
				${INCLUDE_DIR}/GUI/ModulatorPanel/LevelSourceComponent.h
				source/Perlin.cpp
				${INCLUDE_DIR}/Audio/Modulator/Perlin.h
				source/PerlinPanel.cpp
				${INCLUDE_DIR}/GUI/ModulatorPanel/PerlinPanel.h
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
    fontBinaries
    imgBinaries
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        juce::juce_dsp
        juce::juce_opengl
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
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-unused-variable -Wno-unused-private-field)
endif()

# In Visual Studio this command provides a nice grouping of source files in "filters".
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)

