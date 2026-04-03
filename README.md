# hello-daslang

This repository shares `vendor/daScript` and can host multiple projects under `projects/`.

The current project migrated from the old top-level `src` layout is `projects/hot-rload-sandbox`.

## Build
```powershell
cmake -S . -B build
cmake --build build --config Release
```

## Run
```powershell
.\build\projects\hot-rload-sandbox\Release\hot_rload_sandbox.exe
```

If you use a single-config generator, run `.\build\projects\hot-rload-sandbox\hot_rload_sandbox.exe`.
