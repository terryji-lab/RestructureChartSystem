#include "Pages.h"
#include "chart/BarChart.h"
#include "chart/PieChart.h"
#include "chart/LineChart.h"
#include "chart/AreaChart.h"
#include "utils/DataAnalyzer.h"

// ==================== 工厂函数 ====================
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
    chart->applyTheme(theme);
    return chart;
}

// ==================== MainPage ====================

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
    // 卡片
    , m_cardData(35, 82, 290, 152, _T("Data Source"), 10)
    , m_cardChart(35, 250, 290, 264, _T("Chart Type"), 10)
    , m_cardActions(35, 530, 290, 144, _T("Actions"), 10)
    , m_cardInstr(380, 82, 800, 460, _T("Getting Started"), 10)
    , m_cardTips(380, 558, 800, 216, _T("Tips"), 10)
    // 控件
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
    // 注册到基类容器（TextInput 是 Card 子类，放入 m_cards）
    m_cards   = {&m_cardData, &m_cardChart, &m_cardActions, &m_cardInstr, &m_cardTips, &txtCSV};
    m_buttons = {&btnReadCSV, &btnBarChart, &btnPieChart, &btnAreaChart,
                 &btnLineChart, &btnExport, &btnTheme};

    applyTheme(theme);
}

void MainPage::applyTheme(const ColorTheme& theme)
{
    m_theme = theme;

    for (auto* btn : m_buttons)
    {
        btn->setColorNormal(theme.btnNormal);
        btn->setColorHover(theme.btnHover);
        btn->setColorPress(theme.btnPress);
    }

    COLORREF shadow = darkenColor(theme.bgColor, 40);
    for (auto* c : m_cards) c->setColors(shadow, theme.cardColor, theme.accentColor);
}

void MainPage::draw()
{
    // 窗口背景
    setfillcolor(m_theme.bgColor);
    solidrectangle(0, 0, 1200, 800);

    // 标题
    settextstyle(34, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    settextcolor(m_theme.accentColor);
    outtextxy(50, 20, _T("Chart System"));
    setlinecolor(darkenColor(m_theme.bgColor, 30));
    line(50, 66, 1190, 66);

    // 卡片 + 控件（基类默认绘制）
    Page::draw();

    // 右侧面板文字
    drawSidePanel();
}

void MainPage::drawSidePanel()
{
    // ── 说明卡片文字 ──
    setlinecolor(darkenColor(m_theme.cardColor, 40));
    line(400, 82 + 46, 400 + 800 - 40, 82 + 46);

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

    // ── 提示卡片文字 ──
    setlinecolor(darkenColor(m_theme.cardColor, 40));
    line(400, 558 + 46, 400 + 800 - 40, 558 + 46);

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

// ==================== ChartPage ====================

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
    // 卡片
    , m_cardTopBar(48, 6, 1104, 38, _T(""), 6)
    , m_cardStats(48, 48, 1104, 22, _T(""), 5)
    // 按钮
    , btnExport(568, 8, 80, 30, _T("Export"),
          [this]{ m_onExport(); }, 5)
    , btnReset(652, 8, 80, 30, _T("Reset"),
          [this]{ m_onReset(); }, 5)
    , btnSortNameAsc(736, 8, 80, 30, _T("Name ↑"),
          [this]{ m_onSortNameAsc(); }, 5)
    , btnSortNameDesc(820, 8, 80, 30, _T("Name ↓"),
          [this]{ m_onSortNameDesc(); }, 5)
    , btnSortAsc(904, 8, 80, 30, _T("Sort ↑"),
          [this]{ m_onSortAsc(); }, 5)
    , btnSortDesc(988, 8, 80, 30, _T("Sort ↓"),
          [this]{ m_onSortDesc(); }, 5)
    , btnBack(1072, 8, 80, 30, _T("Back"),
          [this]{ m_onBack(); }, 5)
    , m_chartType(CHART_BAR)
    , m_chartTheme(theme)
    , m_hasData(false)
{
    m_cards   = {&m_cardTopBar, &m_cardStats};
    m_buttons = {&btnExport, &btnReset, &btnSortNameAsc, &btnSortNameDesc, &btnSortAsc, &btnSortDesc, &btnBack};

    applyTheme(theme);
}

void ChartPage::applyTheme(const ColorTheme& theme)
{
    m_theme = theme;
    m_chartTheme = theme;

    for (auto* btn : m_buttons)
    {
        btn->setColorNormal(theme.btnNormal);
        btn->setColorHover(theme.btnHover);
        btn->setColorPress(theme.btnPress);
    }

    m_cardTopBar.setColors(darkenColor(theme.bgColor, 30), theme.cardColor, theme.accentColor);
    m_cardStats.setColors(darkenColor(theme.bgColor, 20), theme.cardColor, theme.accentColor);
}

void ChartPage::draw()
{
    // 背景
    setfillcolor(m_theme.bgColor);
    solidrectangle(0, 0, 1200, 800);

    // 图表
    if (m_hasData)
    {
        auto chart = createChart(m_chartType, m_title, m_data, m_chartTheme);
        if (chart) chart->draw();
    }

    // 卡片 + 按钮（基类默认绘制）
    Page::draw();

    // Row 1 标题文字
    settextstyle(22, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    settextcolor(m_theme.accentColor);
    outtextxy(66, 13, m_title.c_str());

    // Row 2 统计文字
    if (!m_data.empty())
    {
        DataAnalyzer da(m_data);
        TCHAR buf[128];
        _stprintf_s(buf, _T("Count: %d   Max: %.0f   Min: %.0f   Avg: %.1f   Med: %.1f"),
            da.count(), da.max(), da.min(), da.average(), da.median());

        settextstyle(17, 0, _T("Microsoft YaHei"), 0, 0, FW_SEMIBOLD, false, false, false);
        settextcolor(m_theme.textColor);
        int tw = textwidth(buf);
        outtextxy(600 - tw / 2, 48 + 3, buf);
    }
}

void ChartPage::setChartData(ChartType type, const std::vector<ChartItem>& data,
                             const tstring& title, const ColorTheme& theme)
{
    m_chartType = type;
    m_data = data;
    m_title = title;
    m_chartTheme = theme;
    m_hasData = true;
}
