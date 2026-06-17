#pragma once
#include <easyx.h>
#include <functional>
#include "Pages.h"
#include "Button.h"
#include "TextInput.h"
#include "Card.h"

// 主页面：包含侧边栏（数据源加载、图表类型选择、操作按钮）和右侧说明区域
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
    void drawSidePanel();       // 绘制右侧使用说明

    // 回调函数
    ChartSelectCallback m_onChartSelect;
    LoadCSVCallback      m_onLoadCSV;
    Callback             m_onExport;
    Callback             m_onThemeSwitch;
    ColorTheme           m_theme;

    // 左侧卡片容器
    Card      m_cardData;       // 数据源区域
    Card      m_cardChart;      // 图表类型区域
    Card      m_cardActions;    // 操作按钮区域
    // 右侧卡片容器
    Card      m_cardInstr;      // 使用说明
    Card      m_cardTips;       // 提示信息

    // 控件
    TextInput txtCSV;           // CSV 路径输入框
    Button    btnReadCSV;       // 加载按钮
    Button    btnBarChart;      // 柱状图
    Button    btnPieChart;      // 饼图
    Button    btnLineChart;     // 折线图
    Button    btnAreaChart;     // 面积图
    Button    btnExport;        // 批量导出
    Button    btnTheme;         // 主题切换
};
