Automate copying built DLLs and libs to a desired output directory after build.
自動化在建置後將產生的 DLL 與 LIB 複製到指定目錄。
Decide msbuild configuration and platform arguments for different build targets.
為不同建置目標決定 msbuild 的配置與平台參數。
Cross-build x86 in WSL requires proper toolchain and path adjustments.
在 WSL 中建置 x86 需適當設定工具鏈與路徑。



# 使用MSBiuld來Build Code(IDE)

使用IDE Build個別的專案, 之後把dll複製到C#專案的資料夾裡面, 這樣就可以在C#專案裡面使用這些dll了。
同時需要修改csproj檔案的PlatformTarget

# 使用MSBiuld來Build Code(CLI)

modified ```<PlatformTarget>x86</PlatformTarget>``` the CSharpTest.csproj file to specify the target platform for the C# project, which is necessary to ensure that the C# application can correctly load the native DLL (CalculatorDLL.dll) that was built for a specific architecture (x86 in this case).
move the build output to the C# execute directory
[ARM64]
```
msbuild CAPI_Test.slnx /p:Configuration=Debug /p:Platform=ARM64 /restore
.\ARM64\Debug\CPPTest.exe
dotnet run --project CSharpTest -c Debug -p:Platform=ARM64
```
[x64]
```
msbuild CAPI_Test.slnx /p:Configuration=Debug /p:Platform=x64 /restore
.\x64\Debug\CPPTest.exe
dotnet run --project CSharpTest -c Debug -p:Platform=x64
```
[x86]
```
msbuild x86（注意：MSBuild 中 x86 的 Platform 名稱是 Win32，不是 x86）
msbuild CAPI_Test.slnx /p:Configuration=Debug /p:Platform=x86 /restore
.\Win32\Debug\CPPTest.exe
dotnet run --project CSharpTest -c Debug -p:Platform=x86
```
之後把dll複製到C#專案的資料夾裡面, 這樣就可以在C#專案裡面使用這些dll了。
同時需要修改csproj檔案的PlatformTarget
```
dotnet run --project CSharpTest
```

# 在Windows環境下,使用CMAKE來Build Code -> DLL
```Developer Command Prompt
echo %VSCMD_ARG_TGT_ARCH%
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
```

```powershell
$env:VSCMD_ARG_TGT_ARCH
cmake --preset windows-x64-debug
cmake --build --preset windows-x64-debug
dotnet run --project CSharpTest -c Debug -p:Platform=x64

```
之後把dll複製到C#專案的資料夾裡面, 這樣就可以在C#專案裡面使用這些dll了。
同時需要修改csproj檔案的PlatformTarget

```Developer Command Prompt
echo %VSCMD_ARG_TGT_ARCH%
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
powershell
```

```powershell
$env:VSCMD_ARG_TGT_ARCH
cmake --preset windows-x86-debug
cmake --build --preset windows-x86-debug
dotnet run --project CSharpTest -c Debug -p:Platform=x86
```
之後把dll複製到C#專案的資料夾裡面, 這樣就可以在C#專案裡面使用這些dll了。
同時需要修改csproj檔案的PlatformTarget

```Developer Command Prompt
echo %VSCMD_ARG_TGT_ARCH%
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" ARM64
powershell
```

```powershell
$env:VSCMD_ARG_TGT_ARCH
cmake --preset windows-arm64-debug
cmake --build --preset windows-arm64-debug
dotnet run --project CSharpTest -c Debug -p:Platform=ARM64
```
之後把dll複製到C#專案的資料夾裡面, 這樣就可以在C#專案裡面使用這些dll了。
同時需要修改csproj檔案的PlatformTarget

# 在WSL環境下, 使用CMAKE來Build Code -> so
```
WSL ARM64 Debug
wsl cmake --preset wsl-arm64-debug
wsl cmake --build --preset wsl-arm64-debug
wsl dotnet run --project CSharpTest -c Debug -p:Platform=ARM64
```
之後把dll複製到C#專案的資料夾裡面, 這樣就可以在C#專案裡面使用這些dll了。
同時需要修改csproj檔案的PlatformTarget
```
wsl dotnet run --project CSharpTest -c Debug -p:Platform=ARM64

```

之後要考慮.x86 ARM64(aarch64)與x64(amd64)的建置

# 檢查
```
dumpbin /headers .\out\build\windows-x64-debug\bin\CalculatorDLL.dll | findstr /i "machine"
dumpbin /headers .\out\build\windows-ARM64-debug\bin\CalculatorDLL.dll | findstr /i "machine"
dumpbin /headers .\out\build\windows-x86-debug\bin\CalculatorDLL.dll | findstr /i "machine"
dumpbin /headers .\CalculatorDLL.dll | findstr /i "machine"

wsl readelf -h ./out/build/wsl-x64-debug/lib/libCalculatorDLL.so | grep "Machine:"
wsl readelf -h ./libCalculatorDLL.so | grep "Machine:"
```