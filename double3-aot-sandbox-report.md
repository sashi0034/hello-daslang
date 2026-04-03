# `double3-aot-sandbox` Execution Time Report

Date: 2026-04-03
Workspace: `/home/sashi/ws/hello-daslang`
Target binary: `build/projects/double3-aot-sandbox/double3_aot_sandbox`

## Summary

`double3-aot-sandbox` was measured in both interpreter mode and true AOT mode with `strict_aot=1`.

- At `1,000,000` iterations and `5` rounds per run, interpreter average was `86.8 ms`
- At the same setting, AOT average was `7.47 ms`
- Overall speedup was about `11.6x`

Both modes returned the same final value:

- `Double3(x=2.01224e+08, y=6.70757e+07, z=1.34149e+08)`

## Measurement Conditions

Command used:

```bash
./build/projects/double3-aot-sandbox/double3_aot_sandbox 1000000 5 1
```

Argument meaning:

- `1000000`: loop iterations inside `run_entry`
- `5`: rounds per mode
- `1`: `strict_aot=1`, so AOT fallback is not allowed

Important note:

- The executable measures only the time spent in `run_entry`
- daScript compile time and `simulate()` time are outside this timing window

## Results

### Sanity Check Run

Command:

```bash
./build/projects/double3-aot-sandbox/double3_aot_sandbox 300000 3 1
```

Result:

| Mode | Average |
|---|---:|
| Interpreter | 25.33 ms |
| AOT | 2.00 ms |
| Speedup | 12.67x |

### Main Runs

Three benchmark runs were executed with `1000000` iterations and `5` rounds each.

| Run | Interpreter avg (ms) | AOT avg (ms) | Speedup |
|---|---:|---:|---:|
| 1 | 90.8 | 7.6 | 11.95x |
| 2 | 85.0 | 7.6 | 11.18x |
| 3 | 84.6 | 7.2 | 11.75x |
| Mean | 86.8 | 7.47 | 11.63x |

Observed per-round range across the three main runs:

- Interpreter: `82 ms` to `117 ms`
- AOT: `7 ms` to `10 ms`

## Interpretation

Within this sandbox, AOT execution consistently reduced `run_entry` time by roughly one order of magnitude.

- Interpreter mode stayed around `85-91 ms` on average
- AOT mode stayed around `7-8 ms` on average
- The measured gain was stable at roughly `11x-12x`

One interpreter round reached `117 ms`, so some scheduler or system noise was present, but the AOT advantage remained clear even with that variance.

## Repro Steps

```bash
cmake -S . -B build
cmake --build build --config Release --target double3_aot_sandbox -j4
./build/projects/double3-aot-sandbox/double3_aot_sandbox 1000000 5 1
```

## Implementation Note

To make this a true AOT comparison, the project was updated so that:

- `double3_demo.das` is AOT-generated into C++
- the generated C++ is compiled into `double3_aot_sandbox`
- a project-specific AOT tool can resolve the custom `double3_bindings` module during generation

Without that setup, `strict_aot=1` failed and the benchmark could not compare real AOT execution.
