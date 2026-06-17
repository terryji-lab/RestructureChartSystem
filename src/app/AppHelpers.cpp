#include "AppHelpers.h"
#include "ui/PopupCard.h"
#include "ui/ExportPathDialog.h"
#include "utils/FileDataReader.h"
#include "utils/FilePathUtils.h"

// ---- 工具函数 ----

const TCHAR* getChartSuffix(ChartType type)
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

tstring shortenForPopup(const tstring& text, size_t maxLen)
{
    if (text.length() <= maxLen) return text;
    if (maxLen <= 3) return text.substr(0, maxLen);
    return text.substr(0, maxLen - 3) + _T("...");
}

// ---- 弹窗 ----

void showPopup(const ColorTheme& theme,
               const tstring& title,
               const tstring& message,
               const tstring& detail,
               bool isError)
{
    PopupCard popup((1200 - 500) / 2, (800 - 220) / 2, 500, 220, title);
    popup.setColors(darkenColor(theme.bgColor, 35),
                    theme.cardColor, theme.textColor);

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

// ---- 导出路径对话框 ----

bool promptExportPath(const ColorTheme& theme,
                      const tstring& title,
                      const tstring& message,
                      const tstring& placeholder,
                      const tstring& defaultPath,
                      tstring& outPath)
{
    ExportPathDialog dialog(theme, title, message, placeholder, defaultPath);
    if (!dialog.showModal()) return false;
    outPath = FilePathUtils::trim(dialog.getPath());
    return true;
}

// ---- CSV 加载 ----

bool tryLoadCSV(const tstring& path,
                std::vector<ChartItem>& outData,
                tstring& outTitle,
                const ColorTheme& theme)
{
    if (path.empty())
    {
        showPopup(theme, _T("Load CSV"),
                  _T("Please enter a CSV file path."), _T(""), true);
        return false;
    }

    FileDataReader reader;
    if (!reader.loadFromPath(path))
    {
        tstring errMsg = reader.getErrorMessage();
        if (errMsg.length() > 55) errMsg = errMsg.substr(0, 52) + _T("...");
        showPopup(theme, _T("Load Failed"),
                  _T("Could not read the file:"), errMsg, true);
        return false;
    }

    outData = reader.getData();

    // 从路径提取文件名作为标题
    size_t slash = path.find_last_of(_T("\\/"));
    size_t dot   = path.find_last_of(_T('.'));
    if (slash == tstring::npos) slash = -1;
    if (dot == tstring::npos || dot <= slash) dot = path.length();
    outTitle = path.substr(slash + 1, dot - slash - 1);

    // 成功弹窗
    tstring displayName = outTitle.empty() ? _T("CSV file") : outTitle;
    if (displayName.length() > 28)
        displayName = displayName.substr(0, 25) + _T("...");

    showPopup(theme, _T("File Imported"),
              _T("Data file loaded successfully:"), displayName);

    return true;
}
