# Dedicated CMake Configuration for Unit Tests and Benchmarking

include(CTest)

# Utility macro to register tests cleanly
macro(add_xinfer_test TEST_NAME SOURCE_FILE)
    add_executable(${TEST_NAME} ${SOURCE_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE xinfer::xinfer)
    
    # Ensure test binaries can resolve libxinfer.so from build output folder
    set_target_properties(${TEST_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$ORIGIN/.."
    )
    
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

# 1. Tensor Unit Test
add_xinfer_test(test_tensor test_tensor.cpp)

# 2. Memory Allocator Unit Test
add_xinfer_test(test_memory test_memory.cpp)

# 3. Backend Driver Unit Test
add_xinfer_test(test_backends test_backends.cpp)

# 4. Latency & FPS Benchmarker Binary
add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark PRIVATE xinfer::xinfer)
set_target_properties(benchmark PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)