# WaveScope (AU + VST3)

Сборка без Homebrew: JUCE подтягивается автоматически через CMake FetchContent.

## Сборка
```bash
cmake -B build -G Xcode -D CMAKE_OSX_ARCHITECTURES="arm64;x86_64" .
cmake --build build --config Release
```
