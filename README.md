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
# args: <iterations_per_round> <rounds> <strict_aot:0|1>
./build/projects/double3-aot-sandbox/double3_aot_sandbox 300000 3 0
```

The executable runs:
1. interpreter mode (`policies.aot = false`)
2. AOT-enabled mode (`policies.aot = true`)

If you pass `strict_aot=1`, the executable enforces `fail_on_no_aot=true` for AOT runs,
so it fails instead of silently falling back to interpreter.

and prints each mode's average elapsed time and speedup.
