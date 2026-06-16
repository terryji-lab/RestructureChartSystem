#include <easyx.h>
#include <iostream>
#include <memory>
#include "ui/Button.h"
#include "ui/Pages.h"
#include "ui/TextInput.h"
#include "utils/FileDataReader.h"
#include "utils/ImageExporter.h"
#include "utils/DataAnalyzer.h"

// 鈹€鈹€ 棰勮棰滆壊涓婚锛堟墿灞?UI 閰嶈壊瀛楁锛夆攢鈹€
static const std::vector<ColorTheme> PRESET_THEMES = {
    // name       barPalette鈥?                                        axis          grid          text          bg              card        btnNormal          btnHover           btnPress           accent
    { _T("Classic"), { RGB(228,87,86), RGB(250,176,58), RGB(245,210,67), RGB(99,190,123), RGB(78,160,212), RGB(148,108,189), RGB(237,125,158), RGB(100,181,180) }, RGB(80,80,80), RGB(220,220,220), RGB(40,40,40), RGB(242,244,248), RGB(255,255,255), RGB(70,130,210), RGB(100,160,235), RGB(50,100,175), RGB(45,95,170) },
    { _T("Ocean"),   { RGB(13,71,119), RGB(25,118,180), RGB(52,152,219), RGB(93,173,226), RGB(26,145,163), RGB(72,202,228), RGB(36,82,134), RGB(160,210,235) }, RGB(30,60,100), RGB(200,220,235), RGB(20,40,70), RGB(235,242,250), RGB(255,255,255), RGB(25,118,180), RGB(52,152,219), RGB(13,71,119), RGB(13,71,119) },
    { _T("Warm"),    { RGB(220,100,60), RGB(230,150,40), RGB(210,60,60), RGB(240,190,60), RGB(190,90,45), RGB(225,140,70), RGB(175,60,45), RGB(240,120,90) }, RGB(80,60,40), RGB(230,210,190), RGB(50,35,25), RGB(252,245,240), RGB(255,255,255), RGB(220,100,60), RGB(240,135,80), RGB(180,70,40), RGB(180,70,40) },
    { _T("Forest"),  { RGB(55,139,55), RGB(92,173,92), RGB(46,125,50), RGB(107,191,110), RGB(74,158,69), RGB(53,122,48), RGB(129,212,138), RGB(80,141,74) }, RGB(50,70,50), RGB(200,220,200), RGB(30,50,30), RGB(240,248,240), RGB(255,255,255), RGB(55,139,55), RGB(92,173,92), RGB(46,125,50), RGB(46,125,50) },
    { _T("Purple"),  { RGB(123,79,191), RGB(156,107,222), RGB(94,53,168), RGB(139,95,207), RGB(109,68,184), RGB(168,126,224), RGB(82,48,160), RGB(147,112,216) }, RGB(60,50,80), RGB(220,210,230), RGB(40,30,55), RGB(248,242,252), RGB(255,255,255), RGB(123,79,191), RGB(156,107,222), RGB(94,53,168), RGB(94,53,168) },
    { _T("Dark"),    { RGB(80,88,112), RGB(101,112,144), RGB(69,80,104), RGB(112,128,160), RGB(88,96,128), RGB(107,120,152), RGB(74,85,120), RGB(92,104,136) }, RGB(180,180,190), RGB(60,60,70), RGB(220,220,225), RGB(42,44,52), RGB(58,60,70), RGB(80,88,112), RGB(112,128,160), RGB(69,80,104), RGB(140,160,200) },
};


// Theme-aware popup for successful CSV import.
static void showImportSuccessPopup(const ColorTheme& theme, const tstring& fileTitle)
{
    const int boxW = 500;
    const int boxH = 220;
    const int boxX = (1200 - boxW) / 2;
    const int boxY = (800 - boxH) / 2;

    const int btnW = 150;
    const int btnH = 42;
    const int btnX = boxX + (boxW - btnW) / 2;
    const int btnY = boxY + boxH - 68;

    BeginBatchDraw();

    // Shadow
    setfillcolor(darkenColor(theme.bgColor, 35));
    solidrectangle(boxX + 10, boxY + 10, boxX + boxW + 10, boxY + boxH + 10);

    // Body
    setfillcolor(theme.cardColor);
    solidrectangle(boxX, boxY, boxX + boxW, boxY + boxH);

    setlinecolor(theme.accentColor);
    setlinestyle(PS_SOLID, 2);
    rectangle(boxX, boxY, boxX + boxW, boxY + boxH);

    // Accent bar
    setfillcolor(theme.accentColor);
    solidrectangle(boxX, boxY, boxX + boxW, boxY + 8);

    // Success icon
    int cx = boxX + 70;
    int cy = boxY + 78;
    setlinecolor(theme.accentColor);
    setfillcolor(lightenColor(theme.accentColor, 95));
    fillcircle(cx, cy, 28);

    setlinecolor(theme.accentColor);
    setlinestyle(PS_SOLID, 4);
    line(cx - 13, cy, cx - 3, cy + 10);
    line(cx - 3, cy + 10, cx + 16, cy - 12);
    setlinestyle(PS_SOLID, 1);

    // Title and content
    setbkmode(TRANSPARENT);
    settextcolor(theme.textColor);
    settextstyle(28, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    outtextxy(boxX + 118, boxY + 48, _T("File Imported"));

    settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_NORMAL, false, false, false);
    settextcolor(darkenColor(theme.textColor, 35));
    outtextxy(boxX + 118, boxY + 92, _T("Data file loaded successfully:"));

    tstring displayName = fileTitle.empty() ? _T("CSV file") : fileTitle;
    if (displayName.length() > 28)
        displayName = displayName.substr(0, 25) + _T("...");

    settextcolor(theme.accentColor);
    settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    outtextxy(boxX + 118, boxY + 122, displayName.c_str());

    // OK button
    setfillcolor(theme.btnNormal);
    solidrectangle(btnX, btnY, btnX + btnW, btnY + btnH);
    setlinecolor(theme.btnPress);
    rectangle(btnX, btnY, btnX + btnW, btnY + btnH);

    settextcolor(RGB(255, 255, 255));
    settextstyle(20, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    outtextxy(btnX + 58, btnY + 9, _T("OK"));

    EndBatchDraw();

    // Wait for confirmation: OK click, Enter, Space, or Esc closes the popup.
    ExMessage popMsg;
    while (true)
    {
        popMsg = getmessage(EX_MOUSE | EX_KEY);
        if (popMsg.message == WM_KEYDOWN)
        {
            if (popMsg.vkcode == VK_RETURN || popMsg.vkcode == VK_SPACE || popMsg.vkcode == VK_ESCAPE)
                break;
        }
        else if (popMsg.message == WM_LBUTTONDOWN)
        {
            if (popMsg.x >= btnX && popMsg.x <= btnX + btnW &&
                popMsg.y >= btnY && popMsg.y <= btnY + btnH)
                break;
        }
    }
}

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


int main()
{
    // 鈹€鈹€ 鍏变韩鐘舵€?鈹€鈹€
    std::vector<ChartItem> loadedData;
    std::vector<ChartItem> originalData;
    tstring loadedTitle = _T("Chart");
    int  themeIdx = 0;
    ChartType currentChart = CHART_BAR;
    bool needRedraw = true;

    // Initialize EasyX graphic window.
    initgraph(1200, 800);
    setbkcolor(PRESET_THEMES[themeIdx].bgColor);

    // Get EasyX window handle for MessageBox callbacks.
    HWND hwnd = GetHWnd();

    // Page pointers used by UI callbacks.
    Page* currentPage = nullptr;
    std::unique_ptr<MainPage>  mainPage;
    std::unique_ptr<ChartPage> chartPage;

    // 鈹€鈹€ 鍒涘缓涓婚〉闈?鈹€鈹€
    const auto& initialTheme = PRESET_THEMES[themeIdx];
    mainPage = std::make_unique<MainPage>(
        initialTheme,
        // onChartSelect
        [&](ChartType t){
            currentChart = t;
            chartPage->setChartData(t, loadedData, loadedTitle, PRESET_THEMES[themeIdx]);
            currentPage = chartPage.get();
            needRedraw = true;
        },
        // onLoadCSV
        [&](const tstring& path){
            if (path.empty())
            {
                MessageBox(hwnd, _T("Please enter a CSV file path."), _T("Load CSV"), MB_OK);
                return;
            }
            FileDataReader reader;
            if (reader.loadFromPath(path))
            {
                loadedData = reader.getData();
                originalData = loadedData;
                size_t slash = path.find_last_of(_T("\\/"));
                size_t dot   = path.find_last_of(_T('.'));
                if (slash == tstring::npos) slash = -1;
                if (dot == tstring::npos || dot <= slash) dot = path.length();
                loadedTitle = path.substr(slash + 1, dot - slash - 1);
                showImportSuccessPopup(PRESET_THEMES[themeIdx], loadedTitle);
                needRedraw = true;
            }
            else
            {
                MessageBox(hwnd, reader.getErrorMessage().c_str(), _T("Load CSV"), MB_OK);
            }
        },
        // onExport
        [&](){
            if (loadedData.empty())
            {
                MessageBox(hwnd, _T("No data loaded. Please load a CSV file first."), _T("Export"), MB_OK);
                return;
            }
            ChartType allTypes[] = { CHART_BAR, CHART_PIE, CHART_LINE, CHART_AREA };
            const TCHAR* suffixes[] = { _T("_bar.png"), _T("_pie.png"), _T("_line.png"), _T("_area.png") };
            bool allOk = true;
            for (int i = 0; i < 4; i++)
            {
                auto chart = createChart(allTypes[i], loadedTitle, loadedData, PRESET_THEMES[themeIdx]);
                if (!chart || !ImageExporter::exportChart(*chart, loadedTitle + suffixes[i]))
                    allOk = false;
            }
            if (allOk)
                MessageBox(hwnd, _T("Exported: bar / pie / line / area PNG files."), _T("Export"), MB_OK);
            else
                MessageBox(hwnd, _T("Export failed."), _T("Export"), MB_OK);
        },
        // onThemeSwitch
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

    // 鈹€鈹€ 鍒涘缓鍥捐〃椤甸潰 鈹€鈹€
    chartPage = std::make_unique<ChartPage>(
        initialTheme,
        // onBack
        [&](){
            currentPage = mainPage.get();
            needRedraw = true;
        },
        [&](){
            if (loadedData.empty())
            {
                MessageBox(hwnd, _T("No data loaded. Please load a CSV file first."), _T("Export"), MB_OK);
                return;
            }
            auto chart = createChart(currentChart, loadedTitle, loadedData, PRESET_THEMES[themeIdx]);
            if (chart && ImageExporter::exportChart(*chart, loadedTitle + getChartSuffix(currentChart)))
                MessageBox(hwnd, _T("Exported current chart as PNG."), _T("Export"), MB_OK);
            else
                MessageBox(hwnd, _T("Export failed."), _T("Export"), MB_OK);
        },
        // onSortAsc 鈥?鍗囧簭
        [&](){
            if (loadedData.empty()) return;
            DataAnalyzer da(loadedData);
            loadedData = da.sortByValue(true);
            chartPage->setChartData(currentChart, loadedData, loadedTitle, PRESET_THEMES[themeIdx]);
            needRedraw = true;
        },
        // onSortDesc 鈥?闄嶅簭
        [&](){
            if (loadedData.empty()) return;
            DataAnalyzer da(loadedData);
            loadedData = da.sortByValue(false);
            chartPage->setChartData(currentChart, loadedData, loadedTitle, PRESET_THEMES[themeIdx]);
            needRedraw = true;
        },
        // onSortNameAsc 鈥?鎸夊悕绉板崌搴?
        [&](){
            if (loadedData.empty()) return;
            DataAnalyzer da(loadedData);
            loadedData = da.sortByName(true);
            chartPage->setChartData(currentChart, loadedData, loadedTitle, PRESET_THEMES[themeIdx]);
            needRedraw = true;
        },
        // onSortNameDesc 鈥?鎸夊悕绉伴檷搴?
        [&](){
            if (loadedData.empty()) return;
            DataAnalyzer da(loadedData);
            loadedData = da.sortByName(false);
            chartPage->setChartData(currentChart, loadedData, loadedTitle, PRESET_THEMES[themeIdx]);
            needRedraw = true;
        },
        // onReset
        [&](){
            if (originalData.empty()) return;
            loadedData = originalData;
            chartPage->setChartData(currentChart, loadedData, loadedTitle, PRESET_THEMES[themeIdx]);
            needRedraw = true;
        }
    );

    currentPage = mainPage.get();

    // 棣栨缁樺埗
    BeginBatchDraw();
    currentPage->draw();
    EndBatchDraw();

    // 鈹€鈹€ 浜嬩欢寰幆 鈹€鈹€
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
    closegraph();
    return 0;
}
