mkdir -p build-debug-nogui
cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" -S . -B ./build-debug-nogui
cmake --build ./build-debug-nogui