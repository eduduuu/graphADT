cd build
ctest --output-on-failure
ctest -T MemCheck
cat build/Testing/Temporary/MemoryChecker.1.log
