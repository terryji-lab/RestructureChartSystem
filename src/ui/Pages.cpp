#include "Pages.h"
#include "chart/BarChart.h"
#include "chart/PieChart.h"
#include "chart/LineChart.h"
#include "chart/AreaChart.h"
#include "utils/DataAnalyzer.h"

// ==================== 图表工厂函数 ====================
// 根据 ChartType 枚举创建对应的图表子类实例，并应用主题配色。
// 所有图表使用统一的绘图区域：x=80, y=60, w=1040, h=680
//
// @param type  图表类型（BAR/PIE/LINE/AREA）
// @param title 图表标题
// @param data  图表数据
// @param theme 颜色主题
// @return 图表对象的 unique_ptr，未知类型返回 nullptr
std::unique_ptr<Chart> createChart(
    ChartType type,
    const tstring& title,
    const std::vector<ChartItem>& data,
    const ColorTheme& theme
)
{
    std::unique_ptr<Chart> chart;
    switch(type)
    {
    case CHART_BAR:
        chart = std::make_unique<BarChart>(title, data, 80, 60, 1040, 680);
        break;
    case CHART_PIE:
        chart = std::make_unique<PieChart>(title, data, 80, 60, 1040, 680, 280);
        break;
    case CHART_LINE:
        chart = std::make_unique<LineChart>(title, data, 80, 60, 1040, 680);
        break;
    case CHART_AREA:
        chart = std::make_unique<AreaChart>(title, data, 80, 60, 1040, 680);
        break;
    default:
        return nullptr;
    }
    // 应用主题配色
    chart->applyTheme(theme);
    return chart;
}

// ==================== MainPage 主页面 ====================

MainPage::MainPage(
    const ColorTheme& theme,
    ChartSelectCallback onChartSelect,
    LoadCSVCallback onLoadCSV,
    Callback onExport,
    Callback onThemeSwitch
)
    : m_onChartSelect(onChartSelect)
    , m_onLoadCSV(onLoadCSV)
    , m_onExport(onExport)
    , m_onThemeSwitch(onThemeSwitch)
    , m_theme(theme)
    // ── 左侧信息卡片 ──
    , m_cardData(35, 82, 290, 152, _T("Data Source"), 10)
    , m_cardChart(35, 250, 290, 264, _T("Chart Type"), 10)
    , m_cardActions(35, 530, 290, 144, _T("Actions"), 10)
    // ── 右侧说明卡片 ──
    , m_cardInstr(380, 82, 800, 460, _T("Getting Started"), 10)
    , m_cardTips(380, 558, 800, 216, _T("Tips"), 10)
    // ── 控件 ──
    , txtCSV(50, 112, 260, 36, _T("Enter CSV file path..."))
    , btnReadCSV(50, 158, 260, 48, _T("Load CSV"),
          [this]{ m_onLoadCSV(txtCSV.getText()); }, 8)
    , btnBarChart(50, 278, 124, 100, _T("Bar"),
          [this]{ m_onChartSelect(CHART_BAR); }, 8)
    , btnPieChart(186, 278, 124, 100, _T("Pie"),
          [this]{ m_onChartSelect(CHART_PIE); }, 8)
    , btnLineChart(50, 390, 124, 100, _T("Line"),
          [this]{ m_onChartSelect(CHART_LINE); }, 8)
    , btnAreaChart(186, 390, 124, 100, _T("Area"),
          [this]{ m_onChartSelect(CHART_AREA); }, 8)
    , btnExport(50, 558, 260, 48, _T("Export All"),
          [this]{ m_onExport(); }, 8)
    , btnTheme(50, 616, 260, 48, _T("Theme: Classic"),
          [this]{ m_onThemeSwitch(); }, 8)
{
    // 将所有控件注册到基类 m_widgets 容器（Page 通过指针统一管理事件分发和绘制）
    m_widgets = {&m_cardData, &m_cardChart, &m_cardActions, &m_cardInstr, &m_cardTips,
                 &txtCSV,
                 &btnReadCSV, &btnBarChart, &btnPieChart, &btnAreaChart,
                 &btnLineChart, &btnExport, &btnTheme};

    applyTheme(theme);
}

// 应用主题配色到所有控件
// 通过 dynamic_cast 区分 Button 和 Card，分别设置各自的颜色属性
void MainPage::applyTheme(const ColorTheme& theme)
{
    m_theme = theme;

    // 阴影颜色：基于背景色加深
    COLORREF shadow = darkenColor(theme.bgColor, 40);

    for (auto* w : m_widgets)
    {
        // Button → 设置按钮三态配色
        if (auto* btn = dynamic_cast<Button*>(w))
        {
            btn->setColorNormal(theme.btnNormal);
            btn->setColorHover(theme.btnHover);
            btn->setColorPress(theme.btnPress);
        }
        // Card（含 TextInput / DisplayBox）→ 设置卡片配色
        if (auto* card = dynamic_cast<Card*>(w))
        {
            card->setColors(shadow, theme.cardColor, theme.accentColor);
        }
    }
}

// 主页面绘制
void MainPage::draw()
{
    // ── 窗口背景（纯色填充）──
    setfillcolor(m_theme.bgColor);
    solidrectangle(0, 0, 1200, 800);

    // ── 顶部标题 "Chart System" ──
    settextstyle(34, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    settextcolor(m_theme.accentColor);
    outtextxy(50, 20, _T("Chart System"));
    // 标题下方分隔线
    setlinecolor(darkenColor(m_theme.bgColor, 30));
    line(50, 66, 1190, 66);

    // ── 卡片 + 控件（基类 Page::draw() 遍历所有 Widget 绘制）──
    Page::draw();

    // ── 右侧面板使用说明文字 ──
    drawSidePanel();
}

// 绘制右侧卡片中的使用说明和提示文字
void MainPage::drawSidePanel()
{
    // ── Getting Started 卡片内的说明 ──
    setlinecolor(darkenColor(m_theme.cardColor, 40));
    line(400, 82 + 46, 400 + 800 - 40, 82 + 46);  // 标题下方分隔线

    settextstyle(22, 0, _T("Microsoft YaHei"), 0, 0, FW_SEMIBOLD, false, false, false);
    settextcolor(m_theme.textColor);

    int ty = 82 + 66, lineH = 42;
    tstring lines[] = {
        _T("1. Enter the CSV file path in the input box"),
        _T("2. Click [Load CSV] to load the data"),
        _T("3. Choose a chart type: Bar / Pie / Line / Area"),
        _T("4. Use [Sort] and [Reset] on the chart page"),
        _T("5. Click [Export All] to save all charts as PNG"),
        _T("6. Click [Theme] to switch color themes"),
        _T(""),
        _T("CSV format:  name, value  (one per line)"),
        _T("The first row is automatically skipped as header"),
    };
    for (int i = 0; i < 9; i++)
    {
        if (lines[i].empty()) { ty += lineH; continue; }
        outtextxy(416, ty, lines[i].c_str());
        ty += lineH;
    }

    // ── Tips 卡片内的提示 ──
    setlinecolor(darkenColor(m_theme.cardColor, 40));
    line(400, 558 + 46, 400 + 800 - 40, 558 + 46);  // 标题下方分隔线

    settextstyle(20, 0, _T("Microsoft YaHei"), 0, 0, FW_SEMIBOLD, false, false, false);
    settextcolor(darkenColor(m_theme.textColor, 50));

    tstring tips[] = {
        _T("- Supported file encoding: UTF-8"),
        _T("- Export generates four PNG files at once"),
        _T("- Six built-in color themes available"),
        _T("- Click chart type buttons to explore"),
        _T("- Press ESC to exit the application"),
    };
    ty = 558 + 62;
    for (int i = 0; i < 5; i++)
    {
        outtextxy(416, ty, tips[i].c_str());
        ty += 30;
    }
}

const tstring& MainPage::getCSVPath() const  { return txtCSV.getText(); }
void MainPage::setThemeButtonText(const tstring& text) { btnTheme.setText(text); }

// ==================== ChartPage 图表页面 ====================

ChartPage::ChartPage(const ColorTheme& theme,
                     Callback onBack,
                     Callback onExport,
                     Callback onSortAsc, Callback onSortDesc,
                     Callback onSortNameAsc, Callback onSortNameDesc,
                     Callback onReset)
    : m_onBack(onBack)
    , m_onExport(onExport)
    , m_onSortAsc(onSortAsc)
    , m_onSortDesc(onSortDesc)
    , m_onSortNameAsc(onSortNameAsc)
    , m_onSortNameDesc(onSortNameDesc)
    , m_onReset(onReset)
    , m_theme(theme)
    // ── 顶部栏卡片（标题行 + 统计行）──
    , m_cardTopBar(48, 6, 1104, 38, _T(""), 6)     // Row 1：标题
    , m_cardStats(48, 48, 1104, 22, _T(""), 5)     // Row 2：统计信息
    // ── 功能按钮（位于顶栏右侧）──
    , btnExport(568, 8, 80, 30, _T("Export"),
          [this]{ m_onExport(); }, 5)
    , btnReset(652, 8, 80, 30, _T("Reset"),
          [this]{ m_onReset(); }, 5)
    , btnSortNameAsc(736, 8, 80, 30, _T("A-Z"),
          [this]{ m_onSortNameAsc(); }, 5)
    , btnSortNameDesc(820, 8, 80, 30, _T("Z-A"),
          [this]{ m_onSortNameDesc(); }, 5)
    , btnSortAsc(904, 8, 80, 30, _T("0-9"),
          [this]{ m_onSortAsc(); }, 5)
    , btnSortDesc(988, 8, 80, 30, _T("9-0"),
          [this]{ m_onSortDesc(); }, 5)
    , btnBack(1072, 8, 80, 30, _T("Back"),
          [this]{ m_onBack(); }, 5)
    , m_chartType(CHART_BAR)
    , m_chartTheme(theme)
    , m_hasData(false)
{
    // 注册控件到基类容器
    m_widgets = {&m_cardTopBar, &m_cardStats,
                 &btnExport, &btnReset, &btnSortNameAsc, &btnSortNameDesc,
                 &btnSortAsc, &btnSortDesc, &btnBack};

    applyTheme(theme);
}

// 应用主题配色
void ChartPage::applyTheme(const ColorTheme& theme)
{
    m_theme = theme;
    m_chartTheme = theme;

    // 设置各按钮的三态颜色
    for (auto* w : m_widgets)
    {
        if (auto* btn = dynamic_cast<Button*>(w))
        {
            btn->setColorNormal(theme.btnNormal);
            btn->setColorHover(theme.btnHover);
            btn->setColorPress(theme.btnPress);
        }
    }

    // 设置两张顶栏卡片的配色
    m_cardTopBar.setColors(darkenColor(theme.bgColor, 30), theme.cardColor, theme.accentColor);
    m_cardStats.setColors(darkenColor(theme.bgColor, 20), theme.cardColor, theme.accentColor);
}

// 图表页面绘制
void ChartPage::draw()
{
    // ── 窗口背景 ──
    setfillcolor(m_theme.bgColor);
    solidrectangle(0, 0, 1200, 800);

    // ── 图表绘制（使用工厂函数创建，绘制后自动销毁）──
    if (m_hasData)
    {
        auto chart = createChart(m_chartType, m_title, m_data, m_chartTheme);
        if (chart) chart->draw();
    }

    // ── 卡片 + 按钮（基类默认绘制）──
    Page::draw();

    // ── Row 1：标题文字（显示在顶栏卡片上）──
    settextstyle(22, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    settextcolor(m_theme.accentColor);
    outtextxy(66, 13, m_title.c_str());

    // ── Row 2：统计信息（Count、Max、Min、Avg、Median）──
    if (!m_data.empty())
    {
        DataAnalyzer da(m_data);
        TCHAR buf[128];
        _stprintf_s(buf, _T("Count: %d   Max: %.0f   Min: %.0f   Avg: %.1f   Med: %.1f"),
            da.count(), da.max(), da.min(), da.average(), da.median());

        settextstyle(17, 0, _T("Microsoft YaHei"), 0, 0, FW_SEMIBOLD, false, false, false);
        settextcolor(m_theme.textColor);
        int tw = textwidth(buf);
        outtextxy(600 - tw / 2, 48 + 3, buf);    // 水平居中
    }
}

// 设置图表数据（由 main.cpp 中的回调调用）
void ChartPage::setChartData(ChartType type, const std::vector<ChartItem>& data,
                             const tstring& title, const ColorTheme& theme)
{
    m_chartType = type;
    m_data = data;
    m_title = title;
    m_chartTheme = theme;
    m_hasData = true;
}
