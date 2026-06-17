#pragma once
#include <easyx.h>
#include <functional>
#include "Pages.h"
#include "Button.h"
#include "Card.h"

// 图表页面：显示图表渲染结果 + 顶部工具栏（排序、导出、返回按钮）
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

    // 设置当前图表的数据、标题和主题
    void setChartData(ChartType type, const std::vector<ChartItem>& data,
                      const tstring& title, const ColorTheme& theme);
    void applyTheme(const ColorTheme& theme);

private:
    // 回调函数
    Callback   m_onBack;
    Callback   m_onExport;
    Callback   m_onSortAsc;       // 0-9 升序
    Callback   m_onSortDesc;      // 9-0 降序
    Callback   m_onSortNameAsc;   // A-Z
    Callback   m_onSortNameDesc;  // Z-A
    Callback   m_onReset;         // 恢复原始顺序
    ColorTheme m_theme;

    // 顶部卡片
    Card      m_cardTopBar;       // 标题栏背景
    Card      m_cardStats;        // 统计信息栏

    // 工具栏按钮
    Button    btnExport;
    Button    btnReset;
    Button    btnSortNameAsc;
    Button    btnSortNameDesc;
    Button    btnSortAsc;
    Button    btnSortDesc;
    Button    btnBack;            // 返回主页

    // 图表数据
    ChartType              m_chartType;
    std::vector<ChartItem> m_data;
    tstring                m_title;
    ColorTheme             m_chartTheme;
    bool                   m_hasData;       // 是否已有加载的数据
};
