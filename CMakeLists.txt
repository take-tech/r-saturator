cmake_minimum_required(VERSION 3.22)

project(RSaturator VERSION 0.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external/juce)
add_subdirectory(external/ranze-plugin-ui)

set(RSAT_PLUGIN_FORMATS VST3)

if(APPLE)
    list(APPEND RSAT_PLUGIN_FORMATS AU)
endif()

juce_add_plugin(RSaturator
    COMPANY_NAME "Ranze"
    BUNDLE_ID "com.ranze.rsaturator"
    PLUGIN_MANUFACTURER_CODE Rnze
    PLUGIN_CODE RSat
    FORMATS ${RSAT_PLUGIN_FORMATS}
    PRODUCT_NAME "R-Saturator"
)

target_sources(RSaturator
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
        src/Parameters.cpp
        src/UserSettings.cpp
        src/dsp/Saturator.cpp
)

target_include_directories(RSaturator
    PRIVATE
        src
)

target_compile_definitions(RSaturator
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(RSaturator
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        ranze::plugin_ui
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
