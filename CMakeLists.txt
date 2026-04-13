# cmake_minimum_required: Specifies the minimum version of CMake required to build the project.
cmake_minimum_required(VERSION 3.16)

# project: Defines the name of the project and the programming languages used.
# CAPI_Calculator: is the name of the project. You can choose any name you like, 
# but it should be descriptive of what the project does.
# CXX: indicates that the project uses C++ as the programming language.
project(CAPI_Test LANGUAGES CXX)

# set: Configures various settings for the project.
# CMAKE_CXX_STANDARD: Specifies the C++ standard to use when compiling the project.
set(CMAKE_CXX_STANDARD 20)
# CMAKE_CXX_STANDARD_REQUIRED: Ensures that the specified C++ standard is used.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default visibility: hidden (only CALCULATORDLL_API symbols are exported)
# CMAKE_CXX_VISIBILITY_PRESET: Sets the default visibility for symbols in the project.
# that means only symbols marked with CALCULATORDLL_API (which expands to __attribute__((visibility("default"))) on Linux) 
# will be exported from the shared library.
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
# CMAKE_VISIBILITY_INLINES_HIDDEN: Hides inline functions by default, which is a common practice to reduce symbol exports.
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# --- stb (single-header image libraries, used by SobelDLL) ---
# Headers live in thirdparty/stb (committed to repo, platform-independent).
set(STB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb)

# --- Shared library (.dll on Windows, .so on Linux) ---
# LIB_SOURCES: A variable that holds the list of source files for the shared library target.
# if we have a lot of source files, we can use file(GLOB ...) to automatically collect them, but for a small project it's often clearer to list them explicitly.
# assuming we have five cpp cal1,cal2, cal3, cal4, cal5, we can do:
# set(LIB_SOURCES
#     CalculatorDLL/cal1.cpp
#     CalculatorDLL/cal2.cpp
#     CalculatorDLL/cal3.cpp
#     CalculatorDLL/cal4.cpp
#     CalculatorDLL/cal5.cpp
# )
# and it will be packaged into a shared library named CalculatorDLL.dll on Windows or CalculatorDLL.so on Linux, 
# which can be loaded by the C# application at runtime.
set(LIB_SOURCES
    CalculatorDLL/calculator.cpp
)

# add_library: Defines a new library target named CalculatorDLL
# SHARED: Indicates that this target is a shared library (as opposed to STATIC or OBJECT).
# STATIC: would create a static library (e.g., .lib on Windows or .a on Linux) instead of a shared library.
# OBJECT: would create an object library(e.g., .o files or .obj files) that can be linked into other targets but does not produce a standalone library file.
# ${LIB_SOURCES}: Specifies the source files that will be compiled to create the shared library.
# so this command tells CMake to create a shared library named CalculatorDLL using the source files listed in LIB_SOURCES.
add_library(CalculatorDLL SHARED ${LIB_SOURCES})


# target_include_directories: Specifies the include directories for the CalculatorDLL target.
# PUBLIC: means that the include directories are needed both when building the library and when using it 
# (i.e., when compiling code that links against the library).
# ${CMAKE_CURRENT_SOURCE_DIR}: is a CMake variable that expands to the directory where the current CMakeLists.txt file is located.
# target_include_directories(_TARGET_ <INTERFACE|PUBLIC|PRIVATE> [items...])
# ${CMAKE_CURRENT_SOURCE_DIR}/CalculatorDLL: is the path to the directory containing the header files for the CalculatorDLL library.
# 所以這是將header files所在的目錄添加到CalculatorDLL target的include path中，這樣在編譯CalculatorDLL和使用它的其他target（如CPPTest）時都能找到這些header files。
target_include_directories(CalculatorDLL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CalculatorDLL)

# target_compile_definitions: Defines preprocessor definitions for the CalculatorDLL target.
# Define CALCULATORDLL_EXPORTS so __attribute__((visibility("default"))) is used
# PRIVATE: means that the definition is only needed when building the CalculatorDLL library itself, not when using it.
# CALCULATORDLL_EXPORTS: is a preprocessor definition that we will use in our code to control symbol visibility.
target_compile_definitions(CalculatorDLL PRIVATE CALCULATORDLL_EXPORTS)

# static lib how to use?




# On Linux, pch.h includes framework.h which is just #pragma once — harmless.
# No precompiled header needed for CMake/gcc.

# --- C++ test executable ---
# add_executable: Defines a new executable target;
# CPPTest: is the name of the executable target.
# CPPTest/CPPTest.cpp: is the source file that contains the main function for the test executable.
add_executable(CPPTest CPPTest/CPPTest.cpp)

# target_link_libraries: Specifies that the CPPTest executable depends on the CalculatorDLL library.
# PRIVATE: means that the dependency is only needed when building the CPPTest executable, not when using it.
# CalculatorDLL: is the name of the library target that we defined earlier. This tells CMake to link the CPPTest executable against the CalculatorDLL library,
target_link_libraries(CPPTest PRIVATE CalculatorDLL)
target_link_libraries(CPPTest PRIVATE SobelDLL)

# Output directories
# On Windows, both .dll and .exe go to bin/ so they can find each other.
# On Linux, .so goes to lib/ (loaded via LD_LIBRARY_PATH or rpath).
# set_target_properties: Configures properties for the specified target.
# LIBRARY_OUTPUT_DIRECTORY: Specifies the directory where the shared library (.so on Linux) will be placed after building.
# RUNTIME_OUTPUT_DIRECTORY: Specifies the directory where the executable (.exe on Windows) will be placed after building.
# ARCHIVE_OUTPUT_DIRECTORY: Specifies the directory where the static library (.lib on Windows) will be placed after building.
set_target_properties(CalculatorDLL PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib      # Linux .so
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin      # Windows .dll → same dir as .exe
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib      # Windows .lib
)

# --- Sobel DLL (image loading + Sobel edge detection) ---
set(SOBEL_SOURCES
    SobelDLL/sobel.cpp
)
add_library(SobelDLL SHARED ${SOBEL_SOURCES})
target_include_directories(SobelDLL PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/SobelDLL)
target_include_directories(SobelDLL PRIVATE ${STB_INCLUDE_DIR})
target_compile_definitions(SobelDLL PRIVATE SOBELDLL_EXPORTS)
set_target_properties(SobelDLL PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
# set_target_properties: Configures properties for the CPPTest target.
# CPPTest: is the name of the executable target for which we are setting properties.
# ${CMAKE_BINIARY_DIR}: is a CMake variable that expands to the directory where the build files are generated (often called the build directory).
# where does the ${CMAKE_BINARY_DIR} come from? It is automatically defined by CMake when you run the cmake command to configure the project.
# what is the value? It is the path to the directory where CMake generates the build files (e.g., Makefiles, Visual Studio project files, etc.).
# how can i look this value? You can look at the value of ${CMAKE_BINARY_DIR} by adding a message command in your CMakeLists.txt file, like this:
# message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
set_target_properties(CPPTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy image.jpg next to CPPTest so the Sobel test can find it at runtime
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/image.jpg")
    add_custom_command(TARGET CPPTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/image.jpg"
            "$<TARGET_FILE_DIR:CPPTest>/image.jpg"
        COMMENT "Copying image.jpg to output directory"
    )
endif()

# On Linux, set RPATH so the executable can find the .so at runtime
# message: it is used to display a message during the CMake configuration process.
message(STATUS "WIN32: ${WIN32}")
if(NOT WIN32)
    # RPATH: is a mechanism used on Unix-like operating systems to specify the runtime library search path for executables and shared libraries.
    set_target_properties(CPPTest PROPERTIES
        BUILD_RPATH "${CMAKE_BINARY_DIR}/lib"
    )
endif()


# windows compile command: 
# cmake -S . -B build_msvc && cmake --build build_msvc --config Debug
# linux compile command: wsl cmake -S . -B build_linux -G Ninja && wsl cmake --build build_linux


# dll -> this use MSVC on Windows
# cd C:\Users\KCRic\Desktop\Code\CAPI_Calculator
# cmake -S . -B build_msvc
# --config is MSVC-specific. G++ on Linux uses CMAKE_BUILD_TYPE instead.
# cmake --build build_msvc --config Debug
# .\.build_msvc\bin\Debug\CPPTest.exe


# .so
# wsl cmake -S . -B build_linux -G Ninja
# wsl cmake --build build_linux
# wsl ./build_linux/bin/CPPTest


# C# 

# wsl sudo apt-get update
# wsl sudo apt-get install -y dotnet-sdk-10.0

# wsl bash -c "cd /mnt/c/Users/KCRic/Desktop/Code/CAPI_Calculator && dotnet run --project CSharpTest"
# wsl 
# cd /mnt/c/Users/KCRic/Desktop/Code/CAPI_Calculator
# dotnet run --project CSharpTest


# CMake Presets

# windows

# x64 Debug
# cmd /c "`"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat`" x64"
#cmake --preset windows-x64-debug
#cmake --build --preset windows-x64-debug

# x86 Debug
# cmd /c "`"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat`" x86"
#cmake --preset windows-x86-debug
#cmake --build --preset windows-x86-debug

# ARM64 Debug（你目前 csproj 預設用的）
# cmd /c "`"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat`" ARM64"
# cmake --preset windows-arm64-debug
# cmake --build --preset windows-arm64-debug

# wsl ARM64 Debug（直接在 Linux 環境中）
#wsl cmake --preset wsl-arm64-debug
#wsl cmake --build --preset wsl-arm64-debug
# dotnet run --project CSharpTest -p:PlatformTarget=x86

# wsl x86 Debug（直接在 Linux 環境中）
#wsl cmake --preset wsl-x86-debug
#wsl cmake --build --preset wsl-x86-debug
# dotnet run --project CSharpTest -p:PlatformTarget=ARM64


# CMakeLists.txt vs CMakePresets.json
# CMakeLists.txt: Defines the build targets, source files, and dependencies. 
# It is the core configuration file for CMake.
# CMakePresets.json: Provides predefined build configurations (presets) 
# that can be easily invoked from the command line or IDE. 
# It allows you to specify different build options, such as build type (Debug/Release), 
# generator, and environment variables, without modifying the CMakeLists.txt file.
# the command line with json will be like: cmake --preset linux-debug
# otherwise you would have to specify all the options manually each time you want to build,
# like: cmake -S . -B build_linux -G Ninja -DCMAKE_BUILD_TYPE=Debug
# DCMAKE_BUILD_TYPE: Specifies the build type (e.g., Debug, Release, RelWithDebInfo, MinSizeRel).



# CMAKE
# ⚠️  vcvarsall.bat 必須與 cmake 在同一個 shell session 中執行！
#     用 cmd /c "..." && cmake ... 串接，否則環境變數在 cmd 結束後就消失，
#     導致連結器找到錯誤架構的 libraries (如 x86 lib → ARM64 target = LNK1120)。

# ARM64 Debug (PowerShell 中執行)
# cmd /c "`"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat`" arm64 && cmake --preset windows-arm64-debug && cmake --build --preset windows-arm64-debug"
# .\out\build\windows-arm64-debug\bin\CPPTest.exe
# dotnet run --project CSharpTest

# x64 Debug (PowerShell 中執行)
# cmd /c "`"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat`" x64 && cmake --preset windows-x64-debug && cmake --build --preset windows-x64-debug"
# .\out\build\windows-x64-debug\bin\CPPTest.exe
# dotnet run --project CSharpTest -p:PlatformTarget=x64

# x86 Debug (PowerShell 中執行)
# ⚠️  "strategy": "external" 代表 CMake 不會自己切換編譯器架構，
#     必須先用 vcvarsall.bat x86 設定好 x86 的 cl.exe，否則會 build 出 x64 的 DLL。
# cmd /c "`"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat`" x86 && cmake --preset windows-x86-debug && cmake --build --preset windows-x86-debug"
# .\out\build\windows-x86-debug\bin\CPPTest.exe
# dotnet run --project CSharpTest -p:PlatformTarget=x86
# 驗證架構: dumpbin /headers .\out\build\windows-x86-debug\bin\CalculatorDLL.dll | findstr /i "machine"
#           → 應顯示 14C machine (x86)

# x86 Release (PowerShell 中執行)
# cmd /c "`"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat`" x86 && cmake --preset windows-x86-release && cmake --build --preset windows-x86-release"

