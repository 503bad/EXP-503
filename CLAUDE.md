# CLAUDE.md

このファイルは、Claude Code (claude.ai/code) がこのリポジトリでコードを扱う際のガイダンスを提供します。

## プロジェクト概要

**EXP-503** - JUCEフレームワークを使用したVSTプラグイン
- 開発者: Igarashi Date
- 音を3系統に分割して処理し、爆発的なエフェクトを作成
- ディレイ、ピッチシフト、フィルタリングを組み合わせた空間系エフェクト

## 開発環境のセットアップ

### 1. JUCEのダウンロード
```bash
# JUCEの公式リリースから最新の安定版（7.x系）をダウンロード
# https://github.com/juce-framework/JUCE/releases
# プロジェクトルートに "JUCE" フォルダとして展開
```

### 2. ビルド環境の準備
```bash
# ビルドディレクトリを作成
mkdir builds
cd builds

# CMakeでプロジェクトを設定
cmake .. -G "Visual Studio 17 2022"  # Windows + Visual Studio
# または
cmake .. -G "Xcode"                  # macOS + Xcode

# ビルド実行
cmake --build . --config Release
```

## アーキテクチャの概要

### プロジェクト構造
```
exp503/
├── CMakeLists.txt          # CMakeビルド設定
├── source/                 # ソースコード
│   ├── PluginProcessor.h   # メイン処理クラス
│   ├── PluginProcessor.cpp
│   ├── PluginEditor.h      # UIクラス
│   └── PluginEditor.cpp
├── assets/                 # リソースファイル
│   └── igarashi_exp.png    # ロゴ画像
├── builds/                 # ビルド出力
└── JUCE/                   # JUCEフレームワーク
```

### 音声処理の流れ
1. **入力を3系統に分割**:
   - 1系統目: 左チャンネル → 5msディレイ → -5centピッチダウン
   - 2系統目: 右チャンネル → 20msディレイ → +5centピッチアップ  
   - 3系統目: センター → 2つのサブパス（3A、3B）に分割

2. **3系統目の詳細処理**:
   - 3A: 800Hzハイパス → 1オクターブダウン → 200Hzハイパス
   - 3B: 800Hzハイパス → 2オクターブダウン → 100Hzハイパス

3. **ミックス**: Dry/Wetコントロールで原音とエフェクト音をブレンド

### UIコンポーネント
- **SPARK**: 1系統目・2系統目のDry/Wetバランス (0-100%)
- **EXPLODE**: 3系統目のDry/Wetバランス (0-100%)
- 背景色はEXPLODEの値に応じて #2c2c2c → #c20000 に変化

## よく使用されるコマンド

### ビルド
```bash
# Debug ビルド
cmake --build builds --config Debug

# Release ビルド  
cmake --build builds --config Release

# プロジェクトを再生成
cd builds && cmake .. --fresh
```

### 開発時の注意点
- JUCEフレームワークは必須依存関係
- プラグインフォーマット: VST3, AU
- ターゲット: Windows, macOS
- C++17 標準を使用

## ファイルの役割

### 主要クラス
- `AudioPluginAudioProcessor`: 音声処理とパラメータ管理
- `AudioPluginAudioProcessorEditor`: UI表示とユーザーインタラクション
- `PitchShifter`: ピッチシフト処理の内部クラス

### パラメータ
- `sparkParam`: SPARK ノブの値 (0.0-100.0)
- `explodeParam`: EXPLODE ノブの値 (0.0-100.0)