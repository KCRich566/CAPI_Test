# cmake_minimum_required: Specifies the minimum version of CMake required to build the project.
cmake_minimum_required(VERSION 3.16)

# --- Read shared version from version.txt (single source of truth) ---
# Read the version number from version.txt. This is the single source of truth
# for native versioning; the C# Directory.Build.props reads the same file.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" _VERSION_LINE LIMIT_COUNT 1)
string(STRIP "${_VERSION_LINE}" _VERSION_LINE)

# Project definition: name and languages.
# Project name: CAPI_Test. You can rename it, but it should describe the project.
# LANGUAGES CXX indicates the project uses C++.
project(CAPI_Test VERSION ${_VERSION_LINE} LANGUAGES CXX)
message(STATUS "Project version: ${PROJECT_VERSION}")

# Configure project-wide C++ settings.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default symbol visibility: hidden. Only symbols explicitly exported with
# CALCULATORDLL_API will be visible from the shared library.
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# --- stb (single-header image libraries, used by SobelDLL) ---
# Headers live in thirdparty/stb (committed to repo, platform-independent).
set(STB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb)

# --- Version: generate version.h (shared by native DLLs) ---
# Generate version.h from cmake/version.h.in so native code can include it.
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/version.h
    @ONLY
)

# --- Version: generate Windows .rc files per DLL ---
# On Windows, generate .rc resource files embedding version information.
if(WIN32)
    set(RC_FILE_DESCRIPTION "Calculator C API DLL")
    set(RC_INTERNAL_NAME "CalculatorDLL")
    set(RC_ORIGINAL_FILENAME "CalculatorDLL.dll")
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/generated/CalculatorDLL_version.rc
        @ONLY
    )

    set(RC_FILE_DESCRIPTION "Sobel Edge Detection DLL")
    set(RC_INTERNAL_NAME "SobelDLL")
    set(RC_ORIGINAL_FILENAME "SobelDLL.dll")
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/generated/SobelDLL_version.rc
        @ONLY
    )
endif()

# --- Shared library (.dll on Windows, .so on Linux) ---
# List of sources for the CalculatorDLL shared library.
set(LIB_SOURCES
    CalculatorDLL/calculator.cpp
)

# Create the CalculatorDLL shared library target.
add_library(CalculatorDLL SHARED ${LIB_SOURCES})

# Include directories for CalculatorDLL. PUBLIC means consumers of the library
# will also use these include paths when compiling.
target_include_directories(CalculatorDLL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CalculatorDLL)
# Generated headers (version.h) so #include "version.h" works in both DLLs.
target_include_directories(CalculatorDLL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/generated)

# Define CALCULATORDLL_EXPORTS when building the DLL so exported symbols
# are emitted properly.
target_compile_definitions(CalculatorDLL PRIVATE CALCULATORDLL_EXPORTS)

# No precompiled headers are required for this project with CMake and GCC/Clang.

# --- C++ test executable ---
add_executable(CPPTest CPPTest/CPPTest.cpp)

# Link the test executable against both native libraries so it can call into them.
target_link_libraries(CPPTest PRIVATE CalculatorDLL)
target_link_libraries(CPPTest PRIVATE SobelDLL)

# Output directories
# On Windows, both .dll and .exe are placed in bin/ so they can find each other.
# On Linux, .so files are placed into lib/ (rpath or LD_LIBRARY_PATH is used at runtime).
set_target_properties(CalculatorDLL PROPERTIES
    VERSION              ${PROJECT_VERSION}
    SOVERSION            ${PROJECT_VERSION_MAJOR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib      # Linux .so
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin      # Windows .dll → same dir as .exe
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib      # Windows .lib
)
if(WIN32)
    target_sources(CalculatorDLL PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated/CalculatorDLL_version.rc)
endif()

# --- Sobel DLL (image loading + Sobel edge detection) ---
set(SOBEL_SOURCES
    SobelDLL/sobel.cpp
)
add_library(SobelDLL SHARED ${SOBEL_SOURCES})
target_include_directories(SobelDLL PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/SobelDLL)
target_include_directories(SobelDLL PRIVATE ${STB_INCLUDE_DIR})
# Generated headers (version.h)
target_include_directories(SobelDLL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/generated)
target_compile_definitions(SobelDLL PRIVATE SOBELDLL_EXPORTS)
set_target_properties(SobelDLL PROPERTIES
    VERSION              ${PROJECT_VERSION}
    SOVERSION            ${PROJECT_VERSION_MAJOR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
if(WIN32)
    target_sources(SobelDLL PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated/SobelDLL_version.rc)
endif()

# Set the runtime output directory for the CPPTest executable.
set_target_properties(CPPTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy image.jpg next to CPPTest so the Sobel test can find it at runtime.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/image.jpg")
    add_custom_command(TARGET CPPTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/image.jpg"
            "$<TARGET_FILE_DIR:CPPTest>/image.jpg"
        COMMENT "Copying image.jpg to output directory"
    )
endif()

# On non-Windows platforms, set BUILD_RPATH so the executable can find the shared
# libraries at runtime without needing to set LD_LIBRARY_PATH.
message(STATUS "WIN32: ${WIN32}")
if(NOT WIN32)
    set_target_properties(CPPTest PROPERTIES
        BUILD_RPATH "\$ORIGIN;\$ORIGIN/../lib"
        INSTALL_RPATH "\$ORIGIN/../lib"
    )
endif()