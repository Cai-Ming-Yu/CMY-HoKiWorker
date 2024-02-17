project(hokiworker)
cmake_minimum_required(VERSION 3.6.4111459)
enable_language(ASM)

file(GLOB THIRDPARTY
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/miniz/miniz.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/zstd/lib/common/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/zstd/lib/compress/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/zstd/lib/decompress/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/zstd/lib/decompress/huf_decompress_amd64.S"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/tiny-AES-c/aes.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/cpp-base64/base64.cpp"
)

add_library(hokiworker SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../cmd_exe/jni/hokiworker.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/library.cpp
  ${THIRDPARTY}
)

target_include_directories(hokiworker PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../../
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/zlib/
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/zstd/lib/
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../../../thirdparty/)

target_compile_options(hokiworker PRIVATE -Wl,-e,main -pthread -Wl,--strip-all -Os -fPIC -pipe -g0 -flto=thin -Wl,--gc-sections -ffunction-sections -fdata-sections -falign-functions -falign-loops -Wl,--sort-section=alignment -fvisibility=hidden -ffast-math -fno-stack-protector -fno-builtin -fmerge-all-constants -fmerge-constants -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-strict-aliasing -fno-rtti -fexceptions -fomit-frame-pointer -fno-stack-check -fms-extensions -fdeclspec -fvisibility-inlines-hidden)
set_target_properties(hokiworker PROPERTIES C_STANDARD 90)
set_target_properties(hokiworker PROPERTIES CXX_STANDARD 17)