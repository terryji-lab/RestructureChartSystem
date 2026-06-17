#pragma once
#include <easyx.h>
#include <functional>
#include "Pages.h"
#include "Button.h"
#include "TextInput.h"
#include "Card.h"

// 主页面
class MainPage : public Page
{
public:
    using Callback = std::function<void()>;
    using ChartSelectCallback = std::function<void(ChartType)>;
    using LoadCSVCallback = std::function<void(const tstring& path)>;

    MainPage(
        const ColorTheme& theme,
        ChartSelectCallback onChartSelect,
        LoadCSVCallback onLoadCSV,
        Callback onExport,
        Callback onThemeSwitch
    );

    void draw() override;

    const tstring& getCSVPath() const;
    void setThemeButtonText(const tstring& text);
    void applyTheme(const ColorTheme& theme);

private:
    void drawSidePanel();

    ChartSelectCallback m_onChartSelect;
    LoadCSVCallback      m_onLoadCSV;
    Callback             m_onExport;
    Callback             m_onThemeSwitch;
    ColorTheme           m_theme;

    Card      m_cardData;
    Card      m_cardChart;
    Card      m_cardActions;
    Card      m_cardInstr;
    Card      m_cardTips;

    TextInput txtCSV;
    Button    btnReadCSV;
    Button    btnBarChart;
    Button    btnPieChart;
    Button    btnLineChart;
    Button    btnAreaChart;
    Button    btnExport;
    Button    btnTheme;
};
