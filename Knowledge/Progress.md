Decide MSBuild configuration and platform arguments for different build targets.
Cross-building x86 in WSL requires proper toolchain and path adjustments.

## Build using MSBuild (IDE)

Build individual projects from your IDE, then copy the produced DLLs into the C# project's output folder so the C# application can load them.
You may need to adjust the C# project's `Platform` in the .csproj file to match native DLL architecture.

## Build using MSBuild (CLI)

- ARM64:

```powershell
msbuild CAPI_Test.slnx /p:Configuration=Debug /p:Platform=ARM64 /restore
.\ARM64\Debug\CPPTest.exe
dotnet run --project CSharpTest -c Debug -p:Platform=ARM64
```

- x64:

```powershell
msbuild CAPI_Test.slnx /p:Configuration=Debug /p:Platform=x64 /restore
.\x64\Debug\CPPTest.exe
dotnet run --project CSharpTest -c Debug -p:Platform=x64
```

- x86 (note: MSBuild platform name for x86 is 'Win32'):

```powershell
msbuild CAPI_Test.slnx /p:Configuration=Debug /p:Platform=x86 /restore
.\Win32\Debug\CPPTest.exe
dotnet run --project CSharpTest -c Debug -p:Platform=x86
```

After building, copy the produced native DLLs into the C# project's output folder or set up the C# project to reference the native build output. 
Ensure the .NET `Platform` matches the native DLL architecture.

## Build using CMake on Windows (produce DLL)

- x64

```Developer Command Prompt
echo %VSCMD_ARG_TGT_ARCH%
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
echo %VSCMD_ARG_TGT_ARCH%
cmake --preset windows-x64-debug
cmake --build --preset windows-x64-debug
dotnet run --project CSharpTest -c Debug -p:Platform=x64

```

- x86

```Developer Command Prompt
echo %VSCMD_ARG_TGT_ARCH%
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
echo %VSCMD_ARG_TGT_ARCH%
cmake --preset windows-x86-debug
cmake --build --preset windows-x86-debug
dotnet run --project CSharpTest -c Debug -p:Platform=x86
```

- ARM64

```Developer Command Prompt
echo %VSCMD_ARG_TGT_ARCH%
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" ARM64
echo %VSCMD_ARG_TGT_ARCH%
cmake --preset windows-arm64-debug
cmake --build --preset windows-arm64-debug
dotnet run --project CSharpTest -c Debug -p:Platform=ARM64
```

After building, copy the produced DLLs into the C# project's folder so the C# project can load them.
Also ensure the C# project's `PlatformTarget` matches the native DLL architecture.

## Build in WSL (produce .so)

- ARM64
- 
```shell
wsl cmake --preset wsl-arm64-debug
wsl cmake --build --preset wsl-arm64-debug
wsl dotnet run --project CSharpTest -c Debug -p:Platform=ARM64
```

After building, copy the produced shared objects into the C# project's native library search path or adjust the C# project's runtime probing settings. 
Ensure the .NET runtime matches the native architecture.

Consider building for x86, ARM64 (aarch64), and x64 (amd64) targets.
  
## Verification

```Developer Command Prompt
cd C:\Users\KCRic\Desktop\Demo
dumpbin /headers CalculatorDLL.dll | findstr /i "machine"
dumpbin /exports CalculatorDLL.dll
```

```shell
readelf -h ./libCalculatorDLL.so | grep "Machine:"
```
