cmake_minimum_required(VERSION 3.15)

project(EXP503 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# JUCEのパスを設定（実際のパスに変更してください）
# set(JUCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/JUCE")

# JUCEが見つからない場合のエラーメッセージ
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE")
    message(FATAL_ERROR "JUCE not found. Please download JUCE and extract it to the project root directory as 'JUCE'")
endif()

# JUCEを追加
add_subdirectory(JUCE)

# プラグインを定義
juce_add_plugin(EXP503
    COMPANY_NAME "Igarashi Date"
    BUNDLE_ID com.igarashidate.exp503
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE IGDT
    PLUGIN_CODE Exp5
    FORMATS VST3 AU
    PRODUCT_NAME "EXP-503")

# ソースファイルを追加
target_sources(EXP503
    PRIVATE
        source/PluginProcessor.cpp
        source/PluginEditor.cpp
        source/PluginProcessor.h
        source/PluginEditor.h)

# コンパイル定義
target_compile_definitions(EXP503
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

# JUCEモジュールをリンク
target_link_libraries(EXP503
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# アセットを埋め込む（BinaryDataとして）
juce_add_binary_data(EXP503Assets
    SOURCES
        assets/logo.png
        assets/igarashi_exp.png)

target_link_libraries(EXP503 PRIVATE EXP503Assets)