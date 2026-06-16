# VS Code 编译说明

本项目已调整为使用 Code::Blocks 自带 MinGW + EasyX4MinGW 编译。

## 环境路径

- 编译器：`D:\codeblocks-20.03mingw-nosetup\MinGW\bin\g++.exe`
- EasyX 头文件：`D:\easyx4mingw_25.9.10\include`
- EasyX 库：`D:\easyx4mingw_25.9.10\lib64`

## 编译方法

在 VS Code 中打开项目根目录 `ChartSystem-main`，按 `Ctrl + Shift + B`，选择或直接执行 `build ChartSystem`。

输出文件：`ChartSystem.exe`。

## 本次修复

1. 原配置只编译当前文件，误把 `.h` 头文件当主文件编译，已改为统一编译整个项目。
2. 添加 `${workspaceFolder}\src` 头文件搜索路径，解决 `chart/Chart.h` 找不到的问题。
3. 将编译标准从 `-std=c++11` 改成 `-std=c++14`，解决 `std::make_unique` 在 C++11 中不存在的问题。
4. 为 `main.cpp` 和 `Pages.cpp` 补充 `<memory>` 头文件，保证 `std::unique_ptr` / `std::make_unique` 可用。
