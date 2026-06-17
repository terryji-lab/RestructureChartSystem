#pragma once
#include <easyx.h>
#include <vector>
#include <memory>
#include <functional>
#include "Widget.h"
#include "chart/Chart.h"

enum ChartType { CHART_BAR, CHART_PIE, CHART_LINE, CHART_AREA };

// 图表工厂函数
std::unique_ptr<Chart> createChart(
    ChartType type,
    const tstring& title,
    const std::vector<ChartItem>& data,
    const ColorTheme& theme
);

// Page 基类，统一管理所有 Widget
class Page
{
protected:
    std::vector<Widget*> m_widgets;

public:
    virtual ~Page() = default;

    virtual void draw()
    {
        for (auto* w : m_widgets) w->draw();
    }

    virtual bool handleMouseMove(int mx, int my)
    {
        bool changed = false;
        for (auto* w : m_widgets) changed |= w->handleMouseMove(mx, my);
        return changed;
    }

    virtual bool handleMouseDown(int mx, int my)
    {
        bool changed = false;
        for (auto* w : m_widgets) changed |= w->handleMouseDown(mx, my);
        return changed;
    }

    virtual bool handleMouseUp(int mx, int my)
    {
        bool changed = false;
        for (auto* w : m_widgets) changed |= w->handleMouseUp(mx, my);
        return changed;
    }

    virtual bool handleKeyDown(WPARAM vkcode)
    {
        for (auto* w : m_widgets)
        {
            if (w->wantsKeyInput()) { w->handleKeyDown(vkcode); return true; }
        }
        return false;
    }

    virtual bool handleChar(WPARAM ch)
    {
        for (auto* w : m_widgets)
        {
            if (w->wantsCharInput()) { w->handleChar(ch); return true; }
        }
        return false;
    }
};
