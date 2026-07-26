#!/bin/sh

# Generate the cmake files. We're building from source,
# since I couldn't get the pre-compiled option to work.
emcmake cmake -B build/web -DWEBGPU_BACKEND=EMDAWNWEBGPU -DWEBGPU_BUILD_FROM_SOURCE=ON

# Clean the build directory and then build the project
cmake --build build/web --clean-first

# Run the app
# Will be available at http://localhost:8000/App.html
python -m http.server -d build/web/dist/Emscripten