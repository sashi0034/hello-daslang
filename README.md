# hello-daslang

This repository hosts daScript embedding samples under `projects/`.

## 0) Fetch daScript (required)
```bash
git clone --depth 1 https://github.com/GaijinEntertainment/daScript.git vendor/daScript
```

## Projects
- `projects/hot-rload-sandbox`: hot-reload sandbox for interpreter loop.
- `projects/double3-aot-sandbox`: C++ `Double3` binding + interpreter/AOT benchmark.

## Build
```bash
cmake -S . -B build
cmake --build build --config Release --target double3_aot_sandbox
```

## Run (interpreter then AOT, with timing)
```bash
# args: <iterations_per_round> <rounds>
./build/projects/double3-aot-sandbox/double3_aot_sandbox 300000 3
```

The executable runs:
1. interpreter mode (`policies.aot = false`)
2. AOT-enabled mode (`policies.aot = true`, `fail_on_no_aot = false`)

and prints each mode's average elapsed time and speedup.
