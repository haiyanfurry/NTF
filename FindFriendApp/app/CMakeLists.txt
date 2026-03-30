cmake_minimum_required(VERSION 3.22.1)
project(findfriend)

add_library(
        native-lib
        SHARED
        src/main/java/com/yourname/findfriend/jni/native-lib.cpp
)

find_library(
        log-lib
        log
)

target_link_libraries(
        native-lib
        ${log-lib}
        android
)

target_compile_features(native-lib PRIVATE cxx_std_17)
