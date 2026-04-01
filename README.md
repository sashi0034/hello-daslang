# hello-daslang

`daScript` を `vendor/daScript` に同梱し、`src/main.cpp` から `scripts/hello.das` を実行する最小サンプルです。
現在は `libDaScript` への静的リンク構成です。

## ビルド

```powershell
cmake -S . -B build
cmake --build build --config Release
```

## 実行

```powershell
.\build\Release\hello_daslang.exe
```

シングルコンフィグ生成器を使っている場合は `.\build\hello_daslang.exe` です。

`hello_daslang.exe` は `daScript` ランタイム DLL なしで起動できます。
