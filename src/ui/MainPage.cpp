#include "MainPage.h"
#include "../utils/AntiAlias.h"

// MainPage 主页面

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
    // 左侧信息卡片
    , m_cardData(35, 82, 290, 152, _T("Data Source"), 10)
    , m_cardChart(35, 250, 290, 264, _T("Chart Type"), 10)
    , m_cardActions(35, 530, 290, 144, _T("Actions"), 10)
    // 右侧说明卡片
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
    // 将所有控件注册到基类 m_widgets 容器
    m_widgets = {&m_cardData, &m_cardChart, &m_cardActions, &m_cardInstr, &m_cardTips,
                 &txtCSV,
                 &btnReadCSV, &btnBarChart, &btnPieChart, &btnAreaChart,
                 &btnLineChart, &btnExport, &btnTheme};

    applyTheme(theme);
}

// 应用主题配色到所有控件，通过 dynamic_cast 区分 Button 和 Card
void MainPage::applyTheme(const ColorTheme& theme)
{
    m_theme = theme;

    COLORREF shadow = darkenColor(theme.bgColor, 40);

    for (auto* w : m_widgets)
    {
        if (auto* btn = dynamic_cast<Button*>(w))
        {
            btn->setColorNormal(theme.btnNormal);
            btn->setColorHover(theme.btnHover);
            btn->setColorPress(theme.btnPress);
        }
        if (auto* card = dynamic_cast<Card*>(w))
        {
            card->setColors(shadow, theme.cardColor, theme.accentColor);
        }
    }
}

// 主页面绘制
void MainPage::draw()
{
    setfillcolor(m_theme.bgColor);
    solidrectangle(0, 0, 1200, 800);

    AA::setTextStyleAA(34, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD);
    settextcolor(m_theme.accentColor);
    outtextxy(50, 20, _T("Chart System"));
    setlinecolor(darkenColor(m_theme.bgColor, 30));
    line(50, 66, 1190, 66);

    Page::draw();
    drawSidePanel();
}

// 绘制右侧卡片中的使用说明和提示文字
void MainPage::drawSidePanel()
{
    setlinecolor(darkenColor(m_theme.cardColor, 40));
    line(400, 82 + 46, 400 + 800 - 40, 82 + 46);

    AA::setTextStyleAA(22, 0, _T("Microsoft YaHei"), 0, 0, FW_SEMIBOLD);
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

    setlinecolor(darkenColor(m_theme.cardColor, 40));
    line(400, 558 + 46, 400 + 800 - 40, 558 + 46);

    AA::setTextStyleAA(20, 0, _T("Microsoft YaHei"), 0, 0, FW_SEMIBOLD);
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
