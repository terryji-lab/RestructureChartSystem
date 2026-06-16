#include <easyx.h>
#include <iostream>
#include <memory>
#include "ui/Button.h"
#include "ui/ExportPathDialog.h"
#include "ui/Pages.h"
#include "ui/TextInput.h"
#include "utils/FileDataReader.h"
#include "utils/FilePathUtils.h"
#include "utils/ImageExporter.h"
#include "utils/DataAnalyzer.h"

// ==================== 预设颜色主题（共 6 套） ====================
// 每套主题包含：
//   barPalette  - 8 色调色板（图表柱子/扇区颜色）
//   axisColor   - 坐标轴颜色
//   gridColor   - 网格线颜色
//   textColor   - 文字颜色
//   bgColor     - 窗口背景色
//   cardColor   - 卡片背景色
//   btnNormal   - 按钮正常态颜色
//   btnHover    - 按钮悬停态颜色
//   btnPress    - 按钮按下态颜色
//   accentColor - 强调色（标题、图标等）
static const std::vector<ColorTheme> PRESET_THEMES = {
    // name       barPalette...                                 axis          grid          text          bg              card        btnNormal          btnHover           btnPress           accent
    { _T("Classic"), { RGB(228,87,86), RGB(250,176,58), RGB(245,210,67), RGB(99,190,123), RGB(78,160,212), RGB(148,108,189), RGB(237,125,158), RGB(100,181,180) }, RGB(80,80,80), RGB(220,220,220), RGB(40,40,40), RGB(242,244,248), RGB(255,255,255), RGB(70,130,210), RGB(100,160,235), RGB(50,100,175), RGB(45,95,170) },
    { _T("Ocean"),   { RGB(13,71,119), RGB(25,118,180), RGB(52,152,219), RGB(93,173,226), RGB(26,145,163), RGB(72,202,228), RGB(36,82,134), RGB(160,210,235) }, RGB(30,60,100), RGB(200,220,235), RGB(20,40,70), RGB(235,242,250), RGB(255,255,255), RGB(25,118,180), RGB(52,152,219), RGB(13,71,119), RGB(13,71,119) },
    { _T("Warm"),    { RGB(220,100,60), RGB(230,150,40), RGB(210,60,60), RGB(240,190,60), RGB(190,90,45), RGB(225,140,70), RGB(175,60,45), RGB(240,120,90) }, RGB(80,60,40), RGB(230,210,190), RGB(50,35,25), RGB(252,245,240), RGB(255,255,255), RGB(220,100,60), RGB(240,135,80), RGB(180,70,40), RGB(180,70,40) },
    { _T("Forest"),  { RGB(55,139,55), RGB(92,173,92), RGB(46,125,50), RGB(107,191,110), RGB(74,158,69), RGB(53,122,48), RGB(129,212,138), RGB(80,141,74) }, RGB(50,70,50), RGB(200,220,200), RGB(30,50,30), RGB(240,248,240), RGB(255,255,255), RGB(55,139,55), RGB(92,173,92), RGB(46,125,50), RGB(46,125,50) },
    { _T("Purple"),  { RGB(123,79,191), RGB(156,107,222), RGB(94,53,168), RGB(139,95,207), RGB(109,68,184), RGB(168,126,224), RGB(82,48,160), RGB(147,112,216) }, RGB(60,50,80), RGB(220,210,230), RGB(40,30,55), RGB(248,242,252), RGB(255,255,255), RGB(123,79,191), RGB(156,107,222), RGB(94,53,168), RGB(94,53,168) },
    { _T("Dark"),    { RGB(80,88,112), RGB(101,112,144), RGB(69,80,104), RGB(112,128,160), RGB(88,96,128), RGB(107,120,152), RGB(74,85,120), RGB(92,104,136) }, RGB(180,180,190), RGB(60,60,70), RGB(220,220,225), RGB(42,44,52), RGB(58,60,70), RGB(80,88,112), RGB(112,128,160), RGB(69,80,104), RGB(140,160,200) },
};

// ==================== 获取图表类型对应的 PNG 文件后缀 ====================
// 例如 CHART_BAR 返回 "_bar.png"
static const TCHAR* getChartSuffix(ChartType type)
{
    switch (type)
    {
    case CHART_BAR:  return _T("_bar.png");
    case CHART_PIE:  return _T("_pie.png");
    case CHART_LINE: return _T("_line.png");
    case CHART_AREA: return _T("_area.png");
    default:         return _T("_chart.png");
    }
}

static tstring shortenForPopup(const tstring& text, size_t maxLen)
{
    if (text.length() <= maxLen)
        return text;

    if (maxLen <= 3)
        return text.substr(0, maxLen);

    return text.substr(0, maxLen - 3) + _T("...");
}

static void showThemePopup(const ColorTheme& theme,
                           const tstring& title,
                           const tstring& message,
                           const tstring& detail = _T(""),
                           bool isError = false)
{
    PopupCard popup(
        (1200 - 500) / 2, (800 - 220) / 2, 500, 220,
        title
    );
    popup.setColors(darkenColor(theme.bgColor, 35),
                    theme.cardColor,
                    theme.textColor);

    if (isError)
    {
        popup.setAccentColor(RGB(220, 60, 60));
        popup.setButtonColors(RGB(200, 55, 55), RGB(235, 75, 75), RGB(160, 35, 35));
    }
    else
    {
        popup.setAccentColor(theme.accentColor);
        popup.setButtonColors(theme.btnNormal, theme.btnHover, theme.btnPress);
    }

    popup.setMessage(message);
    if (!detail.empty())
        popup.setDetail(shortenForPopup(detail, 56));
    popup.setError(isError);
    popup.showModal();
}

static bool promptExportPath(const ColorTheme& theme,
                             const tstring& title,
                             const tstring& message,
                             const tstring& placeholder,
                             const tstring& defaultPath,
                             tstring& outPath)
{
    ExportPathDialog dialog(theme, title, message, placeholder, defaultPath);
    if (!dialog.showModal())
        return false;

    outPath = FilePathUtils::trim(dialog.getPath());
    return true;
}

static void exportCurrentChartWithPrompt(const ColorTheme& theme,
                                         ChartType currentChart,
                                         const tstring& loadedTitle,
                                         const std::vector<ChartItem>& loadedData)
{
    if (loadedData.empty())
    {
        showThemePopup(theme,
                       _T("Export"),
                       _T("No data loaded. Please load a CSV file first."),
                       _T(""),
                       true);
        return;
    }

    tstring inputPath;
    tstring defaultPath = FilePathUtils::safeFileName(loadedTitle) + getChartSuffix(currentChart);
    if (!promptExportPath(theme,
                          _T("Export Current"),
                          _T("Enter a save path for the current chart:"),
                          _T("Relative or absolute path, e.g. exports\\chart.png or D:\\charts\\chart.png"),
                          defaultPath,
                          inputPath))
    {
        return;
    }

    tstring savePath = FilePathUtils::buildSingleChartPath(inputPath, loadedTitle, getChartSuffix(currentChart));
    if (savePath.empty())
    {
        showThemePopup(theme,
                       _T("Export"),
                       _T("Please enter a valid save path."),
                       _T(""),
                       true);
        return;
    }

    if (!FilePathUtils::ensureParentDirectoryExists(savePath))
    {
        showThemePopup(theme,
                       _T("Export"),
                       _T("Export failed."),
                       _T("Could not create the destination folder."),
                       true);
        return;
    }

    auto chart = createChart(currentChart, loadedTitle, loadedData, theme);
    if (chart && ImageExporter::exportChart(*chart, savePath))
    {
        showThemePopup(theme,
                       _T("Export"),
                       _T("Exported current chart as PNG."),
                       savePath);
    }
    else
    {
        showThemePopup(theme,
                       _T("Export"),
                       _T("Export failed."),
                       savePath,
                       true);
    }
}

static void exportAllChartsWithPrompt(const ColorTheme& theme,
                                      const tstring& loadedTitle,
                                      const std::vector<ChartItem>& loadedData)
{
    if (loadedData.empty())
    {
        showThemePopup(theme,
                       _T("Export"),
                       _T("No data loaded. Please load a CSV file first."),
                       _T(""),
                       true);
        return;
    }

    tstring inputPath;
    tstring defaultPath = FilePathUtils::safeFileName(loadedTitle);
    if (!promptExportPath(theme,
                          _T("Export All"),
                          _T("Enter a save path for exporting all charts:"),
                          _T("Relative or absolute path, e.g. exports\\ or D:\\charts\\all.png"),
                          defaultPath,
                          inputPath))
    {
        return;
    }

    ChartType allTypes[] = { CHART_BAR, CHART_PIE, CHART_LINE, CHART_AREA };
    const TCHAR* suffixes[] = { _T("_bar.png"), _T("_pie.png"), _T("_line.png"), _T("_area.png") };
    bool allOk = true;

    for (int i = 0; i < 4; i++)
    {
        tstring savePath = FilePathUtils::buildExportAllChartPath(inputPath, loadedTitle, suffixes[i]);
        if (savePath.empty())
        {
            allOk = false;
            continue;
        }

        if (!FilePathUtils::ensureParentDirectoryExists(savePath))
        {
            allOk = false;
            continue;
        }

        auto chart = createChart(allTypes[i], loadedTitle, loadedData, theme);
        if (!chart || !ImageExporter::exportChart(*chart, savePath))
            allOk = false;
    }

    if (allOk)
    {
        showThemePopup(theme,
                       _T("Export"),
                       _T("Exported: bar / pie / line / area PNG files."),
                       inputPath);
    }
    else
    {
        showThemePopup(theme,
                       _T("Export"),
                       _T("Export failed."),
                       _T("Please check the save path and permissions."),
                       true);
    }
}


int main()
{
    // ==================== 共享状态（在 main 中集中管理） ====================
    std::vector<ChartItem> loadedData;       // 当前显示的图表数据
    std::vector<ChartItem> originalData;     // 原始数据备份（用于 Reset）
    tstring loadedTitle = _T("Chart");       // 当前图表标题（取自 CSV 文件名）
    int  themeIdx = 0;                       // 当前主题索引（0-5 循环）
    ChartType currentChart = CHART_BAR;      // 当前图表类型
    bool needRedraw = true;                  // 是否需要重绘标志

    // ── 初始化 EasyX 图形窗口（1200×800）──
    initgraph(1200, 800);
    setbkcolor(PRESET_THEMES[themeIdx].bgColor);

    // 获取窗口句柄，用于 MessageBox 和 Clipboard 等 Win32 API 调用
    HWND hwnd = GetHWnd();

    // 当前活动页面指针（用于事件分发和绘制）
    Page* currentPage = nullptr;
    std::unique_ptr<MainPage>  mainPage;
    std::unique_ptr<ChartPage> chartPage;

    // ==================== 创建主页面 ====================
    const auto& initialTheme = PRESET_THEMES[themeIdx];
    mainPage = std::make_unique<MainPage>(
        initialTheme,
        // ── onChartSelect：选择了图表类型，切换到 ChartPage ──
        [&](ChartType t){
            currentChart = t;
            chartPage->setChartData(t, loadedData, loadedTitle, PRESET_THEMES[themeIdx]);
            currentPage = chartPage.get();
            needRedraw = true;
        },
        // ── onLoadCSV：加载 CSV 文件 ──
        [&](const tstring& path){
            if (path.empty())
            {
                // 路径为空时弹出错误提示
                const auto& popupTheme = PRESET_THEMES[themeIdx];
                PopupCard popup(
                    (1200 - 500) / 2, (800 - 220) / 2, 500, 220,
                    _T("Load CSV")
                );
                popup.setColors(darkenColor(popupTheme.bgColor, 35),
                                popupTheme.cardColor, popupTheme.textColor);
                popup.setAccentColor(RGB(220, 60, 60));   // 错误红色
                popup.setButtonColors(RGB(200, 55, 55), RGB(235, 75, 75), RGB(160, 35, 35));
                popup.setMessage(_T("Please enter a CSV file path."));
                popup.setError(true);
                popup.showModal();
                return;
            }
            FileDataReader reader;
            if (reader.loadFromPath(path))
            {
                // 加载成功
                loadedData = reader.getData();
                originalData = loadedData;               // 保存原始数据备份
                // 从文件路径中提取文件名作为图表标题
                size_t slash = path.find_last_of(_T("\\/"));
                size_t dot   = path.find_last_of(_T('.'));
                if (slash == tstring::npos) slash = -1;
                if (dot == tstring::npos || dot <= slash) dot = path.length();
                loadedTitle = path.substr(slash + 1, dot - slash - 1);

                // 文件名截断（超过 28 字符时用 "..." 省略）
                tstring displayName = loadedTitle.empty() ? _T("CSV file") : loadedTitle;
                if (displayName.length() > 28)
                    displayName = displayName.substr(0, 25) + _T("...");

                // 使用 PopupCard 弹窗显示成功信息
                const auto& popupTheme = PRESET_THEMES[themeIdx];
                PopupCard popup(
                    (1200 - 500) / 2, (800 - 220) / 2, 500, 220,
                    _T("File Imported")
                );
                popup.setColors(darkenColor(popupTheme.bgColor, 35),
                                popupTheme.cardColor, popupTheme.textColor);
                popup.setAccentColor(popupTheme.accentColor);
                popup.setButtonColors(popupTheme.btnNormal, popupTheme.btnHover, popupTheme.btnPress);
                popup.setMessage(_T("Data file loaded successfully:"));
                popup.setDetail(displayName);
                popup.showModal();

                needRedraw = true;
            }
            else
            {
                // 加载失败，弹出错误提示
                tstring errMsg = reader.getErrorMessage();
                if (errMsg.length() > 55)
                    errMsg = errMsg.substr(0, 52) + _T("...");

                const auto& popupTheme = PRESET_THEMES[themeIdx];
                PopupCard popup(
                    (1200 - 500) / 2, (800 - 220) / 2, 500, 220,
                    _T("Load Failed")
                );
                popup.setColors(darkenColor(popupTheme.bgColor, 35),
                                popupTheme.cardColor, popupTheme.textColor);
                popup.setAccentColor(RGB(220, 60, 60));   // 错误红色
                popup.setButtonColors(RGB(200, 55, 55), RGB(235, 75, 75), RGB(160, 35, 35));
                popup.setMessage(_T("Could not read the file:"));
                popup.setDetail(errMsg);
                popup.setError(true);
                popup.showModal();
            }
        },
        // ── onExport：批量导出四种图表的 PNG ──
        [&](){
            exportAllChartsWithPrompt(PRESET_THEMES[themeIdx], loadedTitle, loadedData);
        },
        // ── onThemeSwitch：循环切换颜色主题 ──
        [&](){
            themeIdx = (themeIdx + 1) % int(PRESET_THEMES.size());
            const auto& newTheme = PRESET_THEMES[themeIdx];
            mainPage->setThemeButtonText(_T("Theme: ") + newTheme.name);
            mainPage->applyTheme(newTheme);
            chartPage->applyTheme(newTheme);
            setbkcolor(newTheme.bgColor);
            needRedraw = true;
        }
    );
    mainPage->setThemeButtonText(_T("Theme: Classic"));

    // ==================== 创建图表页面 ====================
    chartPage = std::make_unique<ChartPage>(
        initialTheme,
        // ── onBack：返回主页面 ──
        [&](){
            currentPage = mainPage.get();
            needRedraw = true;
        },
        // ── onExport：导出当前图表为 PNG ──
        [&](){
            exportCurrentChartWithPrompt(PRESET_THEMES[themeIdx], currentChart, loadedTitle, loadedData);
        },
        // ── onSortAsc：按数值升序排序 ──
        [&](){
            if (loadedData.empty()) return;
            DataAnalyzer da(loadedData);
            loadedData = da.sortByValue(true);
            chartPage->setChartData(currentChart, loadedData, loadedTitle, PRESET_THEMES[themeIdx]);
            needRedraw = true;
        },
        // ── onSortDesc：按数值降序排序 ──
        [&](){
            if (loadedData.empty()) return;
            DataAnalyzer da(loadedData);
            loadedData = da.sortByValue(false);
            chartPage->setChartData(currentChart, loadedData, loadedTitle, PRESET_THEMES[themeIdx]);
            needRedraw = true;
        },
        // ── onSortNameAsc：按名称升序排序 ──
        [&](){
            if (loadedData.empty()) return;
            DataAnalyzer da(loadedData);
            loadedData = da.sortByName(true);
            chartPage->setChartData(currentChart, loadedData, loadedTitle, PRESET_THEMES[themeIdx]);
            needRedraw = true;
        },
        // ── onSortNameDesc：按名称降序排序 ──
        [&](){
            if (loadedData.empty()) return;
            DataAnalyzer da(loadedData);
            loadedData = da.sortByName(false);
            chartPage->setChartData(currentChart, loadedData, loadedTitle, PRESET_THEMES[themeIdx]);
            needRedraw = true;
        },
        // ── onReset：恢复为原始数据 ──
        [&](){
            if (originalData.empty()) return;
            loadedData = originalData;
            chartPage->setChartData(currentChart, loadedData, loadedTitle, PRESET_THEMES[themeIdx]);
            needRedraw = true;
        }
    );

    // 默认显示主页面
    currentPage = mainPage.get();

    // ── 首次绘制（使用双缓冲避免闪烁）──
    BeginBatchDraw();
    currentPage->draw();
    EndBatchDraw();

    // ==================== 主事件循环 ====================
    // 监听鼠标、键盘、窗口消息，按需分发到当前页面并触发重绘。
    // 使用 needRedraw 标志避免不必要的重绘，提升性能。
    ExMessage msg;
    bool running = true;

    while (running && IsWindow(hwnd))
    {
        // 获取下一条消息（同时监听鼠标、键盘、字符输入和窗口事件）
        msg = getmessage(EX_MOUSE | EX_KEY | EX_CHAR | EX_WINDOW);

        switch (msg.message)
        {
        case WM_CLOSE:                            // 窗口关闭按钮
            running = false;
            break;

        case WM_KEYDOWN:                          // 键盘按下
            if (msg.vkcode == VK_ESCAPE) running = false;  // ESC 退出
            needRedraw |= currentPage->handleKeyDown(msg.vkcode);
            break;

        case WM_CHAR:                             // 字符输入（用于 TextInput）
            needRedraw |= currentPage->handleChar(msg.ch);
            break;

        case WM_MOUSEMOVE:                        // 鼠标移动
            needRedraw |= currentPage->handleMouseMove(msg.x, msg.y);
            break;

        case WM_LBUTTONDOWN:                      // 鼠标左键按下
            needRedraw |= currentPage->handleMouseDown(msg.x, msg.y);
            break;

        case WM_LBUTTONUP:                        // 鼠标左键释放
            needRedraw |= currentPage->handleMouseUp(msg.x, msg.y);
            break;
        }

        // 仅在需要时重绘（双缓冲）
        if (needRedraw)
        {
            needRedraw = false;
            BeginBatchDraw();
            cleardevice();                        // 清空屏幕
            currentPage->draw();                  // 重新绘制当前页面
            EndBatchDraw();
        }
    }

    // ── 程序退出 ──
    std::cout << "Program exited";
    closegraph();                                 // 关闭图形窗口
    return 0;
}
