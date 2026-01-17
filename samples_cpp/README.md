# nRF Connect SDK C++ Samples

このディレクトリには、nRF Connect SDK の C++ サンプルが含まれています。

## 概要

標準の C サンプルを C++ に移植したサンプルコードを提供します。
各サンプルは元の C 版と機能的に同等ですが、C++ の機能（名前空間、constexpr、nullptr など）を活用しています。

## ビルドとフラッシュ

### 前提条件

- nRF Connect SDK がインストールされていること
- 適切なツールチェーンがセットアップされていること
- ターゲットボードが接続されていること

### 環境変数の設定

ビルド前に、以下の環境変数を設定してください（パスはインストール環境に合わせて変更してください）:

```bash
export PATH=/opt/nordic/ncs/toolchains/322ac893fe/bin:$PATH
export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
export ZEPHYR_SDK_INSTALL_DIR=/opt/nordic/ncs/toolchains/322ac893fe/opt/zephyr-sdk
```

### ビルド手順

SDK のルートディレクトリから以下のコマンドを実行します:

```bash
west build --pristine --board <BOARD> --no-sysbuild <サンプルパス>
```

例: nrf54l15dk ボード用に peripheral_lbs をビルド

```bash
west build --pristine --board nrf54l15dk/nrf54l15/cpuapp --no-sysbuild samples_cpp/bluetooth/peripheral_lbs
```

### フラッシュ手順

ビルド後、以下のコマンドでボードにフラッシュします:

```bash
west flash
```

### ワンライナー例

ビルドとフラッシュを一度に実行する場合:

```bash
export PATH=/opt/nordic/ncs/toolchains/322ac893fe/bin:$PATH && \
export ZEPHYR_TOOLCHAIN_VARIANT=zephyr && \
export ZEPHYR_SDK_INSTALL_DIR=/opt/nordic/ncs/toolchains/322ac893fe/opt/zephyr-sdk && \
west build --pristine --board nrf54l15dk/nrf54l15/cpuapp --no-sysbuild samples_cpp/bluetooth/peripheral_lbs && \
west flash
```

## 利用可能なサンプル

### Bluetooth

- **peripheral_lbs**: LED Button Service を使用した BLE ペリフェラルのサンプル（C++ 版）
  - パス: `samples_cpp/bluetooth/peripheral_lbs`
  - 元の C 版: `samples/bluetooth/peripheral_lbs`

## サンプルの構造

各 C++ サンプルは以下の構造を持っています:

```
samples_cpp/<category>/<sample_name>/
├── CMakeLists.txt          # C++ プロジェクト設定
├── Kconfig                 # Kconfig 設定
├── Kconfig.sysbuild        # Sysbuild 設定（必要な場合）
├── prj.conf                # プロジェクト設定（C++ サポート有効化済み）
├── README.rst              # 詳細ドキュメント
├── sample.yaml             # テスト定義
├── src/
│   └── main.cpp            # C++ ソースコード
└── boards/                 # ボード固有の設定（必要な場合）
```

## C++ 固有の設定

すべての C++ サンプルは `prj.conf` で以下の設定が有効になっています:

```
CONFIG_CPP=y
CONFIG_STD_CPP17=y
CONFIG_REQUIRES_FULL_LIBCPP=y
```

## トラブルシューティング

### ビルドディレクトリのクリーンアップ

別のサンプルをビルドした後にエラーが出る場合は、`--pristine` オプションを使用してください:

```bash
west build --pristine --board <BOARD> --no-sysbuild <サンプルパス>
```

または、手動でビルドディレクトリを削除:

```bash
rm -rf build
```

### C++ コンパイラの確認

ツールチェーンに C++ コンパイラが含まれていることを確認してください:

```bash
arm-zephyr-eabi-g++ --version
```

## デバッグ実行 (VS Code)

VS Code 上で Cortex-Debug 拡張機能を使用してデバッグを行う場合の注意点です。

### 必要な拡張機能

- [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus.ko)

### 必要な Python パッケージ

ビルド環境によっては、以下の Python パッケージが不足しておりビルドエラーになる場合があります。その場合はインストールしてください。

```bash
pip install pyelftools intelhex
```

### launch.json の設定 (nRF54L15 DK の例)

nRF54L15 DK (`nrf54l15dk/nrf54l15/cpuapp`) を使用する場合、J-Link のデバイス名あツールチェーンの設定に注意が必要です。
また、Sysbuild が有効な場合、生成される ELF ファイルのパスが変更になります。

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Cortex Debug: Blinky C++",
            "cwd": "${workspaceFolder}",
            "executable": "${workspaceFolder}/build/blinky/zephyr/zephyr.elf", /* Sysbuild 使用時は build/<app_name>/zephyr/ 配下になります */
            "request": "launch",
            "type": "cortex-debug",
            "servertype": "jlink",
            "serverpath": "/usr/local/bin/JLinkGDBServer", /* パスが通っていない場合は明示的に指定 */
            "device": "nRF54L15_M33", /* nRF54L15 DK (CpuApp) の場合のデバイス名 */
            "interface": "swd",
            "armToolchainPath": "/opt/nordic/ncs/toolchains/322ac893fe/opt/zephyr-sdk/arm-zephyr-eabi/bin", /* Zephyr SDK 内の GDB を使用 */
            "toolchainPrefix": "arm-zephyr-eabi",
            "runToEntryPoint": "main",
            "preLaunchTask": "West Build Blinky C++"
        }
    ]
}
```

## 参考資料

- [nRF Connect SDK Documentation](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/index.html)
- [Zephyr C++ Support](https://docs.zephyrproject.org/latest/develop/languages/cpp/index.html)
