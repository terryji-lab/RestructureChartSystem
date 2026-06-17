# ChartSystem

基于 **C++14** 和 **EasyX** 图形库的 Windows 桌面图表可视化工具。加载 CSV 数据，以柱状图、饼图、折线图、面积图四种形式呈现，支持数据排序、统计分析和 PNG 导出。项目引入了 **GDI+** 抗锯齿技术，提供高质量的图形和字体渲染。

## 编译配置

### 环境要求

| 组件 | 说明 |
| ---- | ---- |
| **MinGW-w64** | g++ ≥ 5.0，支持 C++14。推荐 [WinLibs](https://winlibs.com/) 或 [MSYS2](https://www.msys2.org/) 发行版 |
| **EasyX for MinGW** | Windows 轻量图形库，需手动安装到 MinGW 目录（见下方） |
| **Windows 7+** | EasyX 和 GDI+ 依赖 Win32 API，仅支持 Windows |

### 安装 EasyX for MinGW

EasyX 官方提供 [EasyX_2022xxxx_mingw.zip](https://easyx.cn/download/easyx4mingw/) 离线包：

1. 下载并解压 `EasyX4MinGW_xxxxxxxx.zip`
2. 将 `include/` 下的 `easyx.h`、`graphics.h` 复制到 `<mingw>\x86_64-w64-mingw32\include\`
3. 将 `lib/` 下的 `libeasyx.a` 复制到 `<mingw>\x86_64-w64-mingw32\lib\`

> **验证**：`g++ -leasyx -dM -E - < nul 2>&1 | findstr easyx` 无报错即成功。

### 命令行编译

项目根目录提供了 `build.bat` 一键编译：

```bat
build.bat
```

或手动编译（将所有文件列为一行，或用 `^` 换行）：

```bash
<mingw>\bin\g++ -std=c++14 -g -Isrc \
    src/main.cpp \
    src/app/Themes.cpp \
    src/app/AppHelpers.cpp \
    src/app/ExportHelpers.cpp \
    src/chart/Chart.cpp \
    src/chart/BarChart.cpp \
    src/chart/LineChart.cpp \
    src/chart/PieChart.cpp \
    src/chart/AreaChart.cpp \
    src/chart/ChartCoordinates.cpp \
    src/chart/ChartGridRenderer.cpp \
    src/ui/Button.cpp \
    src/ui/Card.cpp \
    src/ui/PopupCard.cpp \
    src/ui/TextInput.cpp \
    src/ui/Pages.cpp \
    src/ui/MainPage.cpp \
    src/ui/ChartPage.cpp \
    src/ui/ExportPathDialog.cpp \
    src/utils/FileDataReader.cpp \
    src/utils/FilePathUtils.cpp \
    src/utils/ImageExporter.cpp \
    src/utils/StringUtils.cpp \
    src/utils/ClipboardUtils.cpp \
    -o main.exe \
    -leasyx -lgdi32 -lole32 -loleaut32 -luuid -lwinmm -lmsimg32 -lgdiplus
```

Release 模式追加 `-O2 -s -mwindows`。

#### 链接库说明

| 库 | 用途 |
| -- | ---- |
| `-leasyx` | EasyX 图形库（窗口、绘图、双缓冲） |
| `-lgdi32` | GDI32 — 底层图形设备接口 |
| `-lole32` | COM 基础组件 |
| `-loleaut32` | OLE Automation |
| `-luuid` | UUID 生成 |
| `-lwinmm` | Windows 多媒体 API |
| `-lmsimg32` | 透明位图 AlphaBlend |
| `-lgdiplus` | **GDI+** 高级图形接口（抗锯齿与高质量渲染） |

### Code::Blocks / VS Code 项目

项目包含 `ChartSystem.cbp`，在 Code::Blocks 中打开即可编译。**首次使用请修改** `-LD:\easyx4mingw_25.9.10\lib64` 和 `-ID:\easyx4mingw_25.9.10\include` 为你的 EasyX 路径。

项目中也包含了 VS Code 配置(`.vscode`)，可实现开箱即用的开发体验。

## 功能

| 功能 | 说明 |
| ---- | ---- |
| **高质量渲染** | 基于 **GDI+** 实现平滑抗锯齿绘图（折线、圆角、饼图等）与 ClearType 字体渲染 |
| **CSV 加载** | 支持 UTF-8 BOM / ANSI，自动跳过标题行 |
| **四种图表** | Bar（柱状图）、Pie（饼图）、Line（折线图）、Area（面积图） |
| **六套主题** | Classic / Ocean / Warm / Forest / Purple / Dark，一键切换 |
| **数据统计** | 实时显示 Count / Max / Min / Average / Median |
| **多维排序** | 按值升/降序、按名称升/降序，Reset 恢复原始数据 |
| **批量导出** | Export All 一次性导出四种图表 PNG；Export Current 导出当前图表 |
| **路径对话框** | 独立输入弹窗，支持相对/绝对路径 |
| **模态弹窗** | CSV 加载成功/失败/路径为空均有弹窗反馈 |
| **无数据提示** | 未加载数据时图表区居中显示 "No Data" |
| **自适应标签** | 数据量 >10 条缩小字体，>25 条自动跳过标签 |
| **双缓冲渲染** | `BeginBatchDraw()` / `EndBatchDraw()` 消除闪烁 |

## 使用

### CSV 格式

两列，首行为标题行（自动跳过）：

```csv
City,Revenue
Beijing,850
Shanghai,720
Shenzhen,680
```

### 操作

1. 输入 CSV 文件路径 → **Load CSV**
2. 选择图表类型：Bar / Pie / Line / Area
3. 图表页面内排序/导出/Reset
4. 主页 **Export All** — 批量导出四种图表
5. **Theme** — 循环切换六套主题

### 快捷键

| 按键 | 场景 | 功能 |
| ---- | ---- | ---- |
| `ESC` | 全局 | 退出程序 |
| `Enter` / `Space` / `ESC` | 弹窗 | 关闭弹窗 |
| `Backspace` / `Delete` | 输入框 | 删除光标前/后字符 |
| `←` `→` | 输入框 | 移动光标 |
| `Home` / `End` | 输入框 | 光标跳至开头/末尾 |
| `Ctrl+A` | 输入框 | 全选 |
| `Ctrl+C/V/X` | 输入框 | 复制/粘贴/剪切 |

## 项目结构

```text
ChartSystem/
├── src/
│   ├── main.cpp                       # 入口：状态管理 + GDI+生命周期 + 事件循环
│   ├── common/
│   │   └── Types.h                    # 公共类型：tstring / tstringstream
│   ├── app/
│   │   ├── Themes.h / .cpp            # 6 套预设颜色主题
│   │   ├── AppHelpers.h / .cpp        # 弹窗、路径对话框、CSV 加载
│   │   └── ExportHelpers.h / .cpp     # 单图 / 批量导出
│   ├── chart/
│   │   ├── Chart.h / .cpp             # 抽象基类 Chart + ChartItem + ColorTheme + 颜色工具
│   │   ├── BarChart.h / .cpp          # 柱状图
│   │   ├── PieChart.h / .cpp          # 饼图
│   │   ├── LineChart.h / .cpp         # 折线图
│   │   ├── AreaChart.h / .cpp         # 面积图（继承 LineChart）
│   │   ├── ChartCoordinates.h / .cpp  # PlotCoords 结构体 + 坐标计算
│   │   └── ChartGridRenderer.h / .cpp # Y 轴网格线 + 刻度绘制
│   ├── ui/
│   │   ├── Widget.h                   # 控件抽象基类
│   │   ├── Button.h / .cpp            # 圆角按钮（三态 + 回调）
│   │   ├── Card.h / .cpp              # Card 基类 + DisplayBox
│   │   ├── PopupCard.h / .cpp         # 模态弹窗（继承 Card）
│   │   ├── TextInput.h / .cpp         # 文本输入框（继承 Card）
│   │   ├── ExportPathDialog.h / .cpp  # 导出路径输入对话框
│   │   ├── Pages.h / .cpp             # Page 基类 + ChartType 枚举 + createChart()
│   │   ├── MainPage.h / .cpp          # 主页面
│   │   └── ChartPage.h / .cpp         # 图表页面
│   └── utils/
│       ├── AntiAlias.h                # GDI+ 抗锯齿绘图核心封装
│       ├── RenderUtils.h              # drawRoundRectFill() 圆角矩形填充
│       ├── DataAnalyzer.h             # 统计 + 排序（全部内联）
│       ├── FileDataReader.h / .cpp    # CSV 读取 / 解析
│       ├── FilePathUtils.h / .cpp     # 路径处理（修剪/拼接/创建目录）
│       ├── ImageExporter.h / .cpp     # PNG 导出
│       ├── StringUtils.h / .cpp       # 字符串编码转换（UTF-8/ANSI → TCHAR）
│       └── ClipboardUtils.h / .cpp    # Win32 剪贴板操作
├── data/                              # 示例 CSV 数据
├── .vscode/                           # VS Code 配置文件
├── ChartSystem.cbp                    # Code::Blocks 项目文件
├── build.bat                          # 一键编译脚本
└── README.md
```

### 文件行数约束

除 `main.cpp` 外，所有源文件均 **≤ 200 行**。超过阈值的类/函数已拆分为独立文件，每个文件职责单一。

## 类设计

### 图表继承树

```text
Chart（抽象基类 — draw() / applyTheme() 纯虚）
├── BarChart      柱状图：坐标轴 + Y轴网格刻度 + 多色柱子 + 数值标签
├── PieChart      饼图：抗锯齿扇形 + 图例 + 百分比标签
└── LineChart     折线图：坐标轴 + 网格 + 平滑连线 + 数据点 + Y轴刻度
    └── AreaChart    面积图：复用坐标计算，折线下抗锯齿填充
```

坐标计算（`PlotCoords` / `computePlotCoords()`）提取到 `ChartCoordinates.h/.cpp`，`LineChart` 和 `AreaChart` 共用。Y 轴网格刻度提取到 `ChartGridRenderer.h/.cpp`，`BarChart`、`LineChart`、`AreaChart` 共用。

### UI 继承树

```text
Widget（抽象基类 — 位置/尺寸/事件虚函数）
├── Button       独立按钮：抗锯齿圆角 + 三态颜色 + 投影 + onClick 回调
└── Card         圆角背景框：阴影 + 标题
    ├── TextInput    输入框：叠加文字 + 光标 + 键盘处理
    ├── DisplayBox   只读展示框
    └── PopupCard    模态弹窗：accent 色条 + 图标 + 消息 + OK 按钮

Page（页面基类 — vector<Widget*> 统一管理事件分发与绘制）
├── MainPage     主页：卡片 + 输入框 + 按钮
└── ChartPage    图表页：顶栏 + 统计 + 图表区 + 按钮

ExportPathDialog（独立对话框 — 组合 TextInput + Button）
```

### 工厂函数

```cpp
// Pages.h — 根据 ChartType 枚举创建对应图表子类
std::unique_ptr<Chart> createChart(ChartType, title, data, theme);
```

### 应用层模块

`src/app/` 目录包含与具体应用程序相关的代码：

| 模块 | 内容 |
| ---- | ---- |
| `Themes` | 6 套 `ColorTheme` 预设数据 |
| `AppHelpers` | `showPopup()` 弹窗、`promptExportPath()` 路径对话框、`tryLoadCSV()` 加载 |
| `ExportHelpers` | `exportCurrentChart()`、`exportAllCharts()` |

### 设计要点

- **渲染引擎升级**：通过自研的 `AntiAlias.h` 桥接层无缝集成了 **GDI+**，使原本基于 GDI 的粗糙边缘获得了平滑的抗锯齿效果和 ClearType 字体增强。
- **容器化事件分发**：`Page` 持有 `vector<Widget*>`，事件遍历分发，新增控件只需 `push_back`
- **Card 继承复用**：`TextInput`、`PopupCard` 继承 `Card`，背景和阴影由基类绘制
- **多态主题**：`Chart::applyTheme()` 和 `Page::applyTheme()` 各自实现，图表和 UI 同步变色
- **tstring 统一定义**：`common/Types.h` 中 `typedef std::basic_string<TCHAR> tstring`

## 颜色主题

| 主题 | 色系 | 风格 |
| ---- | ---- | ---- |
| Classic | 多彩 | 通用演示 |
| Ocean | 蓝色 | 科技/商务 |
| Warm | 橙红 | 数据热度 |
| Forest | 绿色 | 自然/环境 |
| Purple | 紫色 | 创意/科技 |
| Dark | 深色 | 暗色背景 |

每套主题定义 10 个颜色字段（`barPalette[8]`、`axisColor`、`gridColor`、`textColor`、`bgColor`、`cardColor`、`btnNormal/Hover/Press`、`accentColor`）。`darkenColor()` / `lightenColor()` 为内联颜色工具函数（`Chart.h`）。

## 依赖

- **MinGW-w64 (g++ ≥ 5.0)** — C++14 支持
- **[EasyX](https://easyx.cn/)** for MinGW — Windows 轻量图形库
- **Windows 7+** — Win32 API
- **GDI32 / OLE32 / OLEAut32** — 系统图形与 COM 库
- **Gdiplus** — GDI+ 图形渲染核心模块

## 示例数据

| 文件 | 条数 | 内容 |
| ---- | ---- | ---- |
| `sales.csv` | 8 | 城市销售额 |
| `products.csv` | 15 | 产品销量 |
| `budget.csv` | 8 | 预算分类 |
| `monthly.csv` | 12 | 月度收入 |
| `large.csv` | ~110 | 区域数据（压力测试） |

## License

MIT
