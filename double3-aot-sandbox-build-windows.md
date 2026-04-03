# `double3-aot-sandbox` Windows ネイティブ環境向けビルド手順

Windows ネイティブ環境で `double3_aot_sandbox` をビルドして実行する手順です。

## 前提

- Visual Studio 2022 が入っていること
- CMake for Windows が入っていること
- `vendor\daScript` が存在すること

## 1. 初回 configure

まだ `build` ディレクトリを作っていない場合だけ実行します。

### `cmd.exe`

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

### PowerShell

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

## 2. Release ビルド

### `cmd.exe`

```bat
cmake --build build --config Release --target double3_aot_sandbox
```

### PowerShell

```powershell
cmake --build build --config Release --target double3_aot_sandbox
```

このターゲットをビルドすると、内部で次の処理が自動で行われます。

1. `double3_aot_tool.exe` をビルド
2. `double3_demo.das` を AOT コンパイルして C++ を生成
3. 生成された C++ を `double3_aot_sandbox.exe` にリンク

つまり、`double3_demo.das` を手で別途 AOT コンパイルする必要はありません。

## 3. 実行

### `cmd.exe`

```bat
build\projects\double3-aot-sandbox\Release\double3_aot_sandbox.exe 10000 2 1
```

### PowerShell

```powershell
.\build\projects\double3-aot-sandbox\Release\double3_aot_sandbox.exe 10000 2 1
```

引数の意味:

- 第1引数: `iterations_per_round`
- 第2引数: `rounds`
- 第3引数: `strict_aot`

`strict_aot=1` にすると、AOT に失敗した場合は interpreter へフォールバックせず、その場で失敗します。

## 4. 生成物

- 実行ファイル:
  `build\projects\double3-aot-sandbox\Release\double3_aot_sandbox.exe`
- AOT 生成 C++:
  `build\projects\double3-aot-sandbox\double3_aot_sandbox_double3_demo.das.cpp`
- AOT ツール:
  `build\projects\double3-aot-sandbox\Release\double3_aot_tool.exe`

## 5. 補足

`double3_aot_tool.exe` は `libDaScriptDyn.dll` に依存しています。
その DLL が見えないと、AOT 生成フェーズが Windows の終了コード `-1073741515` で落ちます。

このリポジトリでは、`double3_aot_tool.exe` のビルド後に `libDaScriptDyn.dll` を同じディレクトリへ自動コピーするようにしてあります。
そのため、通常は `cmake --build build --config Release --target double3_aot_sandbox` だけで通ります。

## 6. 最短手順

すでに `build` があるなら、必要なのは次の 2 コマンドだけです。

```bat
cmake --build build --config Release --target double3_aot_sandbox
build\projects\double3-aot-sandbox\Release\double3_aot_sandbox.exe 10000 2 1
```
