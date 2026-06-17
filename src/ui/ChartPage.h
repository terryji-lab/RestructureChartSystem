#pragma once
#include <easyx.h>
#include <functional>
#include "Pages.h"
#include "Button.h"
#include "Card.h"

// 图表页面
class ChartPage : public Page
{
public:
    using Callback = std::function<void()>;

    ChartPage(const ColorTheme& theme,
              Callback onBack,
              Callback onExport,
              Callback onSortAsc, Callback onSortDesc,
              Callback onSortNameAsc, Callback onSortNameDesc,
              Callback onReset);

    void draw() override;

    void setChartData(ChartType type, const std::vector<ChartItem>& data,
                      const tstring& title, const ColorTheme& theme);
    void applyTheme(const ColorTheme& theme);

private:
    Callback   m_onBack;
    Callback   m_onExport;
    Callback   m_onSortAsc;
    Callback   m_onSortDesc;
    Callback   m_onSortNameAsc;
    Callback   m_onSortNameDesc;
    Callback   m_onReset;
    ColorTheme m_theme;

    Card      m_cardTopBar;
    Card      m_cardStats;

    Button    btnExport;
    Button    btnReset;
    Button    btnSortNameAsc;
    Button    btnSortNameDesc;
    Button    btnSortAsc;
    Button    btnSortDesc;
    Button    btnBack;

    ChartType              m_chartType;
    std::vector<ChartItem> m_data;
    tstring                m_title;
    ColorTheme             m_chartTheme;
    bool                   m_hasData;
};
