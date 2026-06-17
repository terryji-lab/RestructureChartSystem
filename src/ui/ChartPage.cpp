#include "ChartPage.h"
#include "utils/DataAnalyzer.h"

// ChartPage 图表页面

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
    , m_cardTopBar(48, 6, 1104, 38, _T(""), 6)
    , m_cardStats(48, 48, 1104, 22, _T(""), 5)
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

    for (auto* w : m_widgets)
    {
        if (auto* btn = dynamic_cast<Button*>(w))
        {
            btn->setColorNormal(theme.btnNormal);
            btn->setColorHover(theme.btnHover);
            btn->setColorPress(theme.btnPress);
        }
    }

    m_cardTopBar.setColors(darkenColor(theme.bgColor, 30), theme.cardColor, theme.accentColor);
    m_cardStats.setColors(darkenColor(theme.bgColor, 20), theme.cardColor, theme.accentColor);
}

// 图表页面绘制
void ChartPage::draw()
{
    setfillcolor(m_theme.bgColor);
    solidrectangle(0, 0, 1200, 800);

    if (m_hasData)
    {
        auto chart = createChart(m_chartType, m_title, m_data, m_chartTheme);
        if (chart) chart->draw();
    }

    Page::draw();

    settextstyle(22, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    settextcolor(m_theme.accentColor);
    outtextxy(66, 13, m_title.c_str());

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

// 设置图表数据
void ChartPage::setChartData(ChartType type, const std::vector<ChartItem>& data,
                             const tstring& title, const ColorTheme& theme)
{
    m_chartType = type;
    m_data = data;
    m_title = title;
    m_chartTheme = theme;
    m_hasData = true;
}
