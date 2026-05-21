# R-Saturator

R-Saturator は、軽いサチュレーションを加えるためのシンプルなオーディオプラグインです。
JUCE + CMake で開発しており、macOS / Windows の DAW で使える VST3 と、macOS 用の AU に対応します。

現在は v0.2.0 のプロトタイプです。

## 主な機能

- 軽めのソフトクリップサチュレーション
- 5つの基本パラメータ
  - Input Gain
  - Drive
  - Tone
  - Output Gain
  - Mix
- Dry/Wet Mix による原音とのブレンド
- アナログ感を意識した Tone コントロール
- プラグイン処理を一時的に切り替える Effect ON/OFF
- Ember / Amber / Ruby / Graphite / Blush のUIテーマ切替
- DAWオートメーション対応
- 黒基調のシンプルな5ノブGUI

## 対応フォーマット

- VST3: macOS / Windows
- AU: macOS

## インストール方法

GitHub Releases から、自分のOSに合ったインストーラーをダウンロードしてください。

- macOS: `R-Saturator-<version>-macOS.pkg`
- Windows: `R-Saturator-<version>-Windows-x64-Setup.exe`

### macOS

1. `R-Saturator-<version>-macOS.pkg` をダウンロードします。
2. ダウンロードした `.pkg` を開き、画面の案内に沿ってインストールします。
3. インストール後、DAWを起動し直します。
4. DAWのプラグイン一覧で `R-Saturator` を探します。

macOS版では、VST3とAUがインストールされます。

インストール先:

```text
/Library/Audio/Plug-Ins/VST3/R-Saturator.vst3
/Library/Audio/Plug-Ins/Components/R-Saturator.component
```

未署名の開発版では、macOSのセキュリティ警告が出ることがあります。その場合は「システム設定」内の「プライバシーとセキュリティ」から許可してください。

### Windows

1. `R-Saturator-<version>-Windows-x64-Setup.exe` をダウンロードします。
2. ダウンロードした `.exe` を開き、画面の案内に沿ってインストールします。
3. インストール後、DAWを起動し直します。
4. DAWのプラグイン一覧で `R-Saturator` を探します。

Windows版では、VST3がインストールされます。

インストール先:

```text
C:\Program Files\Common Files\VST3\R-Saturator.vst3
```

Windows SmartScreen の警告が出ることがあります。これは未署名の開発版で起こる可能性があります。

### DAWに表示されない場合

- DAWを再起動してください。
- DAWのプラグイン再スキャンを実行してください。
- Windowsの場合、DAWがVST3プラグインを読み込む設定になっているか確認してください。
- macOSの場合、AU対応DAWではAU、VST3対応DAWではVST3として表示されます。

## DAWでの使い方

1. DAWを起動します。
2. プラグイン一覧を再スキャンします。
3. オーディオトラック、インストゥルメントトラック、またはバス/ミックスバスに `R-Saturator` を挿します。
4. 音を再生しながら `Drive` と `Mix` を調整します。
5. 効果を一時的に外したい場合は、右上の `Effect` をOFFにします。

DAWによって表示場所は少し異なります。

- Logic Pro: Audio Units のエフェクトとして表示されます。
- Ableton Live / Cubase / Studio One / REAPER など: VST3 エフェクトとして表示されます。

## パラメータ

### Input

プラグインに入る音量を調整します。

- 小さすぎる音を少し持ち上げたいときに上げます。
- 歪みすぎるときは下げます。

### Drive

サチュレーションの強さを調整します。

- 低め: ほぼクリーン
- 中くらい: 軽い厚み
- 高め: ほどよく飽和した音

まずは `0.2` から `0.5` くらいで試すのがおすすめです。

### Tone

音の明るさを調整します。

- 左に回す: 低中域がふくよかになり、こもりすぎない温かい質感になります。
- 中央: ニュートラル
- 右に回す: 音像がブライトになり、アナログ機材のような軽いキラキラ感が加わります。

v0.2.0では、Toneは単なる明るさ調整ではなく、左側でWarmth、右側でAir/Sparkleが加わるように調整しています。
効きはやや分かりやすめにしているため、強すぎる場合は中央付近へ戻して調整してください。

Toneは厳密なEQだけではなく、フィルターと軽い倍音付加を組み合わせています。周波数ごとの効き方はおおよそ以下のイメージです。

```text
Tone 左側: Warm

Gain
 +4 dB |       ______
 +2 dB |  ____/      \__
  0 dB |_/              \________
 -2 dB |                          \___
       +--------------------------------
        100   300   700   2k   6k  12k Hz

主な効果:
- 150 Hz から 700 Hz あたりを少し持ち上げて、音像を太くします。
- 4 kHz から 8 kHz 以上を軽く丸めて、硬さを抑えます。
- 高域を完全には落とさないので、こもりすぎないようにしています。
```

```text
Tone 右側: Air / Sparkle

Gain
 +4 dB |                         ______
 +2 dB |                    ____/
  0 dB |___________________/
 -2 dB |
       +--------------------------------
        100   300   700   2k   6k  12k Hz

主な効果:
- 4 kHz から 12 kHz あたりを持ち上げて、明るさと抜けを足します。
- 5 kHz 以上の帯域に軽いサチュレーションを加えて、キラッとした倍音感を出します。
- 上げすぎても耳に痛くなりすぎないよう、ソフトクリップで丸めています。
```

### Output

処理後の音量を調整します。

Driveを上げると音量感が変わるので、最終的な大きさをここで整えます。

### Mix

原音と処理音の混ざり具合を調整します。

- `0%`: 原音のみ
- `50%`: 原音と処理音を半分ずつ
- `100%`: 処理音のみ

強めにDriveをかけて、Mixを少し下げると自然に馴染ませやすいです。

### Effect

プラグイン全体の処理をON/OFFします。

- ON: Input / Drive / Tone / Output / Mix の処理が有効になります。
- OFF: 入力音をそのまま通します。

処理前後の音を素早く比較したいときに使います。

## おすすめの始め方

まずは以下のような設定から試してください。

- Input: `0 dB`
- Drive: `0.2` から `0.4`
- Tone: `0.5`
- Output: `0 dB`
- Mix: `100%`

音が歪みすぎる場合は `Drive` または `Input` を下げてください。
効果が強すぎる場合は `Mix` を下げると自然になります。

## UIテーマ

画面右上の小さなカラーアイコンから、UIの色味を選択できます。

- Ember: 黒 + 赤橙。標準テーマです。
- Amber: 黒 + 琥珀色。少し暖かい印象です。
- Ruby: 黒 + 深い赤。濃いサチュレーション感に合うテーマです。
- Graphite: 黒 + グレー。落ち着いたスタジオ系の見た目です。
- Blush: 白背景 + ピンクアクセント。明るくかわいい見た目です。

テーマ設定はDAWセッションに保存されます。

## 開発者向けビルド方法

JUCE は `external/juce` に配置されている必要があります。

### macOS

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Windows

必要なもの:

- Visual Studio 2022
- Desktop development with C++ ワークロード
- CMake
- `external/juce` に配置された JUCE

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

## インストーラーの作成

インストーラーは `build/packages/` に生成されます。

macOS:

```sh
bash scripts/package_macos.sh
```

Windows:

WindowsインストーラーはGitHub Actions上で Inno Setup を使って作成します。
ローカルで作る場合は、Windowsに Inno Setup をインストールし、`installer/windows/R-Saturator.iss` を `ISCC.exe` でビルドしてください。

GitHub Actions では、`v0.2.0` のような Git タグをpushすると、インストーラーをGitHub Releaseへアップロードします。

## 開発状況

現在のバージョン: v0.2.0 prototype

このバージョンでは、まずDAWで読み込めて軽いサチュレーションができることを優先しています。
v0.2.0ではToneのキャラクター改善とUIテーマ切替を追加しました。
オーバーサンプリング、プリセット管理、コード署名/Notarizationは今後追加予定です。
