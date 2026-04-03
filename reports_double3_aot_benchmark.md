# double3-aot-sandbox 検証レポート（2026-04-03）

## 目的
- `double3-aot-sandbox` が daScript を C++ AOT で実行できているかを確認する。
- インタプリタ版とAOT版の実行時間を比較する。

## 実行環境
- 実行日: 2026-04-03
- 実行バイナリ: `./build/projects/double3-aot-sandbox/double3_aot_sandbox`
- 引数:
  - ベンチマーク: `300000 5 0`（strict_aot=false）
  - 厳密検証: `300000 1 1`（strict_aot=true）

## 結果
### 1) ベンチマーク（strict_aot=false）
- Interpreter 平均: **77.4 ms**
- AOTモード平均: **76.6 ms**
- Speedup(interpreter/aot): **1.01044x**

→ 速度差はほぼなく、約1%のみAOTモードが速い結果。

### 2) 厳密AOT検証（strict_aot=true）
- AOTフェーズで `error[50101]: AOT link failed on run_entry Ci` が発生。
- 実行は `AOT simulate failed` で終了。

## 解釈
- 現状の `double3_demo.das` は **厳密AOT（fail_on_no_aot=true）では通らない**。
- つまり「完全にAOT化されている」とは言えず、`strict_aot=false` のときはAOT失敗時にフォールバックして動いている可能性がある。
- そのため、今回のAOTモード計測値は「純粋なAOT実行性能」を表していない可能性が高い。

## 補足
- 本リポジトリでは、`strict_aot` 引数を追加し、AOT検証を明示的に切り替え可能にした。
- さらに、daScriptのAOT生成ディレクトリ不足でビルド失敗しないよう、CMake側でディレクトリを事前作成するようにした。
