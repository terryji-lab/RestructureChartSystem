# ChartSystem — 面向对象的图表可视化系统

基于 C++ 和 EasyX 图形库的 Windows 桌面图表可视化应用。支持从 CSV 加载数据，以柱状图、饼图、折线图、面积图四种形式展示，提供数据排序、统计分析和 PNG 批量导出功能。

## 功能特性

| 功能 | 说明 |
| ---- | ---- |
| **CSV 数据加载** | 自动跳过标题行，兼容 UTF-8 BOM 和 ANSI 编码 |
| **四种图表类型** | 柱状图（Bar）、饼图（Pie）、折线图（Line）、面积图（Area） |
| **六套颜色主题** | Classic / Ocean / Warm / Forest / Purple / Dark，一键切换，图表和 UI 同步变色 |
| **圆角 UI** | 按钮、卡片、输入框统一样式：圆角矩形 + 投影 + 悬停高亮 |
| **卡片式布局** | 主页面三张卡片（数据源 / 图表类型 / 操作）+ 右侧说明面板 |
| **数据统计** | 实时显示 Count / Max / Min / Average / Median |
| **数据排序** | Sort ↑ 升序 / Sort ↓ 降序，两个独立按钮；Reset 恢复原始数据 |
| **批量导出** | 四种图表一键导出为 PNG 图片 |
| **自适应标签** | 数据量 > 25 条自动增大标签步长，> 10 条自动缩小字体 |
| **双页面架构** | 主页面（加载数据 / 选择图表）和图表页面（展示 / 操作数据），通过多态 Page 管理 |
| **双缓冲渲染** | `BeginBatchDraw()` / `EndBatchDraw()` 消除画面闪烁 |

## 项目结构

```text
ChartSystem/
├── src/
│   ├── main.cpp                      # 应用入口：窗口初始化、创建页面、事件循环
│   ├── chart/                        # 图表模块
│   │   ├── Chart.h                   #   抽象基类 + ColorTheme + 颜色工具函数
│   │   ├── BarChart.h / BarChart.cpp #   柱状图
│   │   ├── PieChart.h / PieChart.cpp #   饼图
│   │   ├── LineChart.h / LineChart.cpp   #   折线图（含 PlotCoords 坐标计算）
│   │   └── AreaChart.h / AreaChart.cpp   #   面积图（继承 LineChart）
│   ├── ui/                           # UI 组件模块
│   │   ├── Card.h / Card.cpp         #   圆角卡片基类 + DisplayBox
│   │   ├── Button.h / Button.cpp     #   圆角按钮（悬停/按下/投影/回调）
│   │   ├── TextInput.h / TextInput.cpp   #   文本输入框（继承 Card）
│   │   └── Pages.h / Pages.cpp       #   Page 基类 + MainPage + ChartPage
│   └── utils/                        # 工具模块
│       ├── RenderUtils.h             #   drawRoundRectFill() 共享绘制函数
│       ├── FileDataReader.h / .cpp   #   CSV 读取器
│       ├── DataAnalyzer.h            #   数据分析器（统计 + 排序）
│       └── ImageExporter.h / .cpp    #   PNG 导出
├── data/                             # 示例 CSV 数据
│   ├── sales.csv                     #   城市销售额（8 条）
│   ├── products.csv                  #   产品销量（15 条）
│   ├── budget.csv                    #   预算分类（8 条）
│   ├── monthly.csv                   #   月度收入（12 条）
│   └── large.csv                     #   区域数据（约 110 条，压力测试）
├── .vscode/
│   ├── tasks.json                    #   编译任务配置
│   └── c_cpp_properties.json         #   C++ IntelliSense 配置
├── .gitignore
└── README.md
```

## 类继承关系

### 图表体系

```text
Chart（抽象基类 — 定义 draw() / applyTheme() 纯虚接口）
├── BarChart     柱状图：坐标轴 + 网格 + 多色柱子 + 数值标签
├── PieChart     饼图：扇形 + 图例 + 百分比标签
└── LineChart    折线图：坐标轴 + 网格 + 数据点连线 + PlotCoords
    └── AreaChart   面积图：复用 LineChart::computePlotCoords()，折线下填充面积

所有子类实现 applyTheme(theme) 统一接收主题色 → 一键切换图表配色
```

### UI 体系

```text
Card（圆角背景框 — 阴影 + 圆角矩形 + 标题）
├── TextInput    输入框：继承 Card 背景，叠加文字输入 + 光标 + 键盘处理
└── DisplayBox   只读展示框：继承 Card 背景，叠加可变内容文字

Page（页面基类 — 统一管理 m_cards / m_buttons 容器）
├── MainPage     主页：三张操作卡片 + 右侧说明面板 + 7 个按钮 + 输入框
└── ChartPage    图表页：顶栏 + 图表区域 + 4 个按钮

Button（圆角按钮 — 独立类，不继承 Card）
```

`Page` 基类持有 `std::vector<Card*>` 和 `std::vector<Button*>`，事件分发（MouseMove/Down/Up/KeyDown）和绘制均在基类默认实现中遍历容器完成。子类只需在构造函数中往容器注册控件，只覆盖 `draw()` 添加自定义绘制逻辑。加新控件只需一行 push。

## 系统架构

```text
┌────────────────────────────────────────────────────┐
│                     main.cpp                        │
│      窗口管理 / 事件循环 / 页面切换 / 主题状态        │
└──────────┬───────────────────────┬──────────────────┘
           │                       │
    ┌──────▼──────┐         ┌──────▼──────────┐
    │ Page 基类    │         │   UI 组件        │
    │ ├ MainPage  │         │  Card（基类）    │
    │ └ ChartPage │         │  ├ TextInput    │
    │  容器遍历    │         │  └ DisplayBox  │
    │  事件分发    │         │  Button         │
    └──────┬──────┘         └─────────────────┘
           │
    ┌──────▼──────────────────────────────────┐
    │           Chart（抽象基类）               │
    │   applyTheme() — 统一主题色接口          │
    │  BarChart / PieChart / LineChart         │
    │  AreaChart（继承 LineChart）              │
    └──────┬───────────────────────────────────┘
           │
    ┌──────▼───────────┬──────────────────────┐
    │  FileDataReader  │  DataAnalyzer        │
    │  CSV 读取/解析    │  统计 / 排序          │
    └──────────────────┴──────────────────────┘
```

## 依赖项

### 编译工具链

| 依赖 | 要求 | 说明 |
| ---- | ---- | ---- |
| **g++ (MinGW-w64)** | ≥ 8.0 | 需支持 C++17 |
| **Windows SDK** | Windows 7+ | Win32 API |

### 第三方库

| 库 | 链接参数 | 说明 |
| ---- | ---- | ---- |
| **EasyX** | `-leasyxw`（Unicode）或 `-leasyx`（ANSI） | Windows 轻量图形库，封装 GDI/GDI+ |

### 系统库

| 库 | 链接参数 | 说明 |
| ---- | ---- | ---- |
| **GDI32** | `-lgdi32` | 图形设备接口 |
| **OLE32** | `-lole32` | COM/OLE 支持 |

## 编译与运行

### 前置条件

1. 安装 [MinGW-w64](https://www.mingw-w64.org/)（推荐通过 [MSYS2](https://www.msys2.org/)）：

   ```bash
   pacman -S mingw-w64-x86_64-gcc
   ```

2. 安装 [EasyX](https://easyx.cn/)，将头文件和库文件放入 MinGW 对应目录。

### 编译

```bash
# Unicode 版本（推荐）
g++ -g -DUNICODE -D_UNICODE -Isrc src/*.cpp src/chart/*.cpp src/ui/*.cpp src/utils/*.cpp -o main.exe -leasyxw -lgdi32 -lole32

# ANSI 版本
g++ -g -Isrc src/*.cpp src/chart/*.cpp src/ui/*.cpp src/utils/*.cpp -o main.exe -leasyx -lgdi32 -lole32
```

### Code::Blocks 一键打开

项目根目录提供了 `ChartSystem.cbp` 工程文件，**双击即可在 Code::Blocks 中打开整个项目**，无需手动配置源文件和编译选项。

1. 打开 Code::Blocks → **File → Open**，选择 `ChartSystem.cbp`
2. 上方工具栏选择 **Debug** 或 **Release** 目标
3. 按 **F9** 编译并运行（或点工具栏黄色齿轮+绿三角）

> **提示**：如果链接时报 `-leasyxw` 找不到，请确保 EasyX 库已放入 MinGW 的 `lib/` 目录。

### VS Code 一键编译

按 `Ctrl+Shift+B` 选择编译任务：

- **EasyX - 编译 (Unicode)** — 默认任务
- **EasyX - 编译 (ANSI)**
- **EasyX - 编译运行** — 编译后自动启动

### 运行

```bash
./main.exe
```

## 使用指南

### CSV 数据格式

两列数据，第一行为标题行（自动跳过）：

```csv
City,Revenue
New York,850
London,720
Tokyo,680
```

### 操作步骤

1. 在输入框中输入 CSV 文件路径（支持相对路径和绝对路径）
2. 点击 **Load CSV** 加载数据
3. 选择图表类型：
   - **Bar** — 柱状图
   - **Pie** — 饼图
   - **Line** — 折线图
   - **Area** — 面积图
4. 在图表页面：
   - 点击 **Sort ↑** 按值升序排列
   - 点击 **Sort ↓** 按值降序排列
   - 点击 **Reset** 恢复原始数据顺序
   - 点击 **Back** 返回主页面
5. 点击 **Theme** 循环切换六套颜色主题（图表 + UI 同步变色）
6. 点击 **Export All** 一键导出四种图表为 PNG

### 快捷键

| 按键 | 场景 | 功能 |
| ---- | ---- | ---- |
| `ESC` | 全局 | 退出程序 |
| `Backspace` / `Delete` | 输入框聚焦 | 删除光标前 / 后一个字符 |
| `←` `→` | 输入框聚焦 | 移动光标 |
| `Home` / `End` | 输入框聚焦 | 光标跳到开头 / 末尾 |
| `Ctrl+C` | 输入框聚焦 | 复制全部文本 |
| `Ctrl+V` | 输入框聚焦 | 粘贴剪贴板文本 |
| `Ctrl+X` | 输入框聚焦 | 剪切全部文本 |
| `Ctrl+A` | 输入框聚焦 | 光标移至末尾 |

## 颜色主题

内置六套预设主题，通过 **Theme** 按钮循环切换：

| 主题 | 风格 | 适用场景 |
| ---- | ---- | ---- |
| **Classic** | 红橙黄绿青蓝紫 | 默认主题，通用演示 |
| **Ocean** | 深浅蓝色系 | 海洋 / 科技 / 商务数据 |
| **Warm** | 暖橙红色系 | 强调数据热度 |
| **Forest** | 绿色系 | 环境 / 自然类数据 |
| **Purple** | 紫色系 | 创意 / 科技类数据 |
| **Dark** | 深色系 | 暗色背景偏好 |

每个主题定义 10 个颜色字段：

| 字段 | 用途 |
| ---- | ---- |
| `barPalette[8]` | 柱状图柱子色 / 饼图扇区色 / 折线图线色 / 面积图填充色 |
| `axisColor` | 坐标轴颜色 |
| `gridColor` | 网格线颜色 |
| `textColor` | 图表文字色 |
| `bgColor` | 窗口背景色 |
| `cardColor` | 卡片背景色 |
| `btnNormal / btnHover / btnPress` | 按钮三态颜色 |
| `accentColor` | 标题 / 强调色 |

## 技术要点

- **多态主题系统**：`Chart::applyTheme()` 纯虚接口，四个图表子类各自实现。`Page::applyTheme()` 遍历 `m_cards` / `m_buttons` 容器统一设置 UI 控件颜色。主题切换时图表和界面同步变色。
- **容器化控件管理**：`Page` 基类持有 `std::vector<Card*>` 和 `std::vector<Button*>`，事件分发（MouseMove/Down/Up/KeyDown）和绘制均在基类默认实现中遍历完成。子类只覆盖 `draw()`，不加新控件无需写任何事件处理代码。
- **Card 系继承**：`TextInput` 继承 `Card`（圆角框 + 投影），背景绘制委托基类，自身只处理文字和光标。`Card` 基类提供 `handleMouseDown` / `handleKeyDown` / `handleChar` / `wantsKeyInput` / `wantsCharInput` 虚方法，默认空操作，子类按需覆盖。按键走 WM_KEYDOWN（Backspace / 方向键 / 剪贴板快捷键），字符走 WM_CHAR（支持任意字符和 IME 输入）。
- **双缓冲渲染**：`BeginBatchDraw()` / `EndBatchDraw()` 消除画面闪烁。
- **事件驱动**：主循环通过 `getmessage()` 获取鼠标 / 键盘事件，经 `currentPage->handle*()` 多态分发，基类遍历容器路由到具体控件。
- **Unicode / ANSI 双模式**：通过 `TCHAR`、`tstring`、`_T()` 宏支持编译时切换。
- **UTF-8 BOM 兼容**：`FileDataReader` 自动检测并跳过 BOM 头。
- **自适应标签**：根据数据量动态调整标签间距和字体大小。
