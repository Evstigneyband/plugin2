cmake_minimum_required(VERSION 3.22)
project(WaveScope VERSION 0.1.0)

# JUCE via FetchContent (no brew needed)
include(FetchContent)
FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 7.0.12 # stable tag; change if needed
)
FetchContent_MakeAvailable(juce)

# Enable AU + VST3
juce_add_plugin(WaveScope
    COMPANY_NAME "EVST"
    COMPANY_WEBSITE "https://example.com"
    COMPANY_EMAIL "dev@example.com"
    PLUGIN_MANUFACTURER_CODE EVST
    PLUGIN_CODE Wscp
    FORMATS AU VST3
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PRODUCT_NAME "WaveScope"
)

target_sources(WaveScope PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Oscilloscope.h
    Source/Oscilloscope.cpp
)

target_compile_features(WaveScope PRIVATE cxx_std_17)

target_link_libraries(WaveScope PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
