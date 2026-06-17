#include <easyx.h>
#include <iostream>
#include <memory>
#include "app/Themes.h"
#include "app/AppHelpers.h"
#include "app/ExportHelpers.h"
#include "ui/MainPage.h"
#include "ui/ChartPage.h"
#include "utils/DataAnalyzer.h"
#include "utils/AntiAlias.h"

int main()
{
    // ---- 应用状态 ----
    std::vector<ChartItem> loadedData;
    std::vector<ChartItem> originalData;
    tstring loadedTitle = _T("Chart");
    int  themeIdx = 0;
    ChartType currentChart = CHART_BAR;
    bool needRedraw = true;

    // ---- 初始化窗口 ----
    initgraph(1200, 800);
    AA::Init();  // 初始化 GDI+ 抗锯齿引擎
    setbkcolor(PRESET_THEMES[themeIdx].bgColor);
    HWND hwnd = GetHWnd();

    // ---- 页面指针（提前声明，供回调 lambda 相互引用）----
    Page* currentPage = nullptr;
    std::unique_ptr<MainPage>  mainPage;
    std::unique_ptr<ChartPage> chartPage;

    // ---- 辅助：刷新图表页面 ----
    auto refreshChart = [&]() {
        chartPage->setChartData(currentChart, loadedData, loadedTitle,
                                PRESET_THEMES[themeIdx]);
        needRedraw = true;
    };

    // ---- 辅助：排序并刷新 ----
    auto applySort = [&](std::vector<ChartItem> sorted) {
        loadedData = std::move(sorted);
        refreshChart();
    };

    // ---- 创建主页面 ----
    const auto& initialTheme = PRESET_THEMES[themeIdx];

    mainPage = std::make_unique<MainPage>(
        initialTheme,
        // 选择图表类型 → 切换到 ChartPage
        [&](ChartType t) {
            currentChart = t;
            refreshChart();
            currentPage = chartPage.get();
        },
        // 加载 CSV
        [&](const tstring& path) {
            if (tryLoadCSV(path, loadedData, loadedTitle, initialTheme))
            {
                originalData = loadedData;
                needRedraw = true;
            }
        },
        // 批量导出
        [&]() { exportAllCharts(PRESET_THEMES[themeIdx], loadedTitle, loadedData); },
        // 切换主题
        [&]() {
            themeIdx = (themeIdx + 1) % int(PRESET_THEMES.size());
            const auto& t = PRESET_THEMES[themeIdx];
            mainPage->setThemeButtonText(_T("Theme: ") + t.name);
            mainPage->applyTheme(t);
            chartPage->applyTheme(t);
            setbkcolor(t.bgColor);
            needRedraw = true;
        }
    );
    mainPage->setThemeButtonText(_T("Theme: Classic"));

    // ---- 创建图表页面 ----
    chartPage = std::make_unique<ChartPage>(
        initialTheme,
        [&]() { currentPage = mainPage.get(); needRedraw = true; },          // Back
        [&]() { exportCurrentChart(PRESET_THEMES[themeIdx], currentChart,
                                   loadedTitle, loadedData); },               // Export
        [&]() { if (!loadedData.empty()) applySort(                          // Sort 0-9
                   DataAnalyzer(loadedData).sortByValue(true)); },
        [&]() { if (!loadedData.empty()) applySort(                          // Sort 9-0
                   DataAnalyzer(loadedData).sortByValue(false)); },
        [&]() { if (!loadedData.empty()) applySort(                          // Sort A-Z
                   DataAnalyzer(loadedData).sortByName(true)); },
        [&]() { if (!loadedData.empty()) applySort(                          // Sort Z-A
                   DataAnalyzer(loadedData).sortByName(false)); },
        [&]() { if (!originalData.empty()) {                                 // Reset
                   loadedData = originalData; refreshChart(); } }
    );

    currentPage = mainPage.get();

    // ---- 首次绘制 ----
    BeginBatchDraw();
    currentPage->draw();
    EndBatchDraw();

    // ---- 主事件循环 ----
    ExMessage msg;
    bool running = true;

    while (running && IsWindow(hwnd))
    {
        msg = getmessage(EX_MOUSE | EX_KEY | EX_CHAR | EX_WINDOW);

        switch (msg.message)
        {
        case WM_CLOSE:
            running = false;
            break;
        case WM_KEYDOWN:
            if (msg.vkcode == VK_ESCAPE) running = false;
            needRedraw |= currentPage->handleKeyDown(msg.vkcode);
            break;
        case WM_CHAR:
            needRedraw |= currentPage->handleChar(msg.ch);
            break;
        case WM_MOUSEMOVE:
            needRedraw |= currentPage->handleMouseMove(msg.x, msg.y);
            break;
        case WM_LBUTTONDOWN:
            needRedraw |= currentPage->handleMouseDown(msg.x, msg.y);
            break;
        case WM_LBUTTONUP:
            needRedraw |= currentPage->handleMouseUp(msg.x, msg.y);
            break;
        }

        if (needRedraw)
        {
            needRedraw = false;
            BeginBatchDraw();
            cleardevice();
            currentPage->draw();
            EndBatchDraw();
        }
    }

    std::cout << "Program exited";
    AA::Shutdown();  // 释放 GDI+ 资源
    closegraph();
    return 0;
}
