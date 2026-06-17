#include "ExportHelpers.h"
#include "AppHelpers.h"
#include "utils/FilePathUtils.h"
#include "utils/ImageExporter.h"

// ---- 导出当前图表 ----

void exportCurrentChart(const ColorTheme& theme,
                        ChartType currentChart,
                        const tstring& loadedTitle,
                        const std::vector<ChartItem>& loadedData)
{
    if (loadedData.empty())
    {
        showPopup(theme, _T("Export"),
                  _T("No data loaded. Please load a CSV file first."),
                  _T(""), true);
        return;
    }

    tstring inputPath;
    tstring defaultPath = FilePathUtils::safeFileName(loadedTitle)
                        + getChartSuffix(currentChart);
    if (!promptExportPath(theme,
                          _T("Export Current"),
                          _T("Enter a save path for the current chart:"),
                          _T("Relative or absolute path, e.g. exports\\chart.png or D:\\charts\\chart.png"),
                          defaultPath, inputPath))
        return;

    tstring savePath = FilePathUtils::buildSingleChartPath(
        inputPath, loadedTitle, getChartSuffix(currentChart));
    if (savePath.empty())
    {
        showPopup(theme, _T("Export"),
                  _T("Please enter a valid save path."), _T(""), true);
        return;
    }

    if (!FilePathUtils::ensureParentDirectoryExists(savePath))
    {
        showPopup(theme, _T("Export"), _T("Export failed."),
                  _T("Could not create the destination folder."), true);
        return;
    }

    auto chart = createChart(currentChart, loadedTitle, loadedData, theme);
    if (chart && ImageExporter::exportChart(*chart, savePath))
        showPopup(theme, _T("Export"), _T("Exported current chart as PNG."), savePath);
    else
        showPopup(theme, _T("Export"), _T("Export failed."), savePath, true);
}

// ---- 批量导出 ----

void exportAllCharts(const ColorTheme& theme,
                     const tstring& loadedTitle,
                     const std::vector<ChartItem>& loadedData)
{
    if (loadedData.empty())
    {
        showPopup(theme, _T("Export"),
                  _T("No data loaded. Please load a CSV file first."),
                  _T(""), true);
        return;
    }

    tstring inputPath;
    if (!promptExportPath(theme,
                          _T("Export All"),
                          _T("Enter a save path for exporting all charts:"),
                          _T("Relative or absolute path, e.g. exports\\ or D:\\charts\\all.png"),
                          FilePathUtils::safeFileName(loadedTitle), inputPath))
        return;

    ChartType allTypes[] = { CHART_BAR, CHART_PIE, CHART_LINE, CHART_AREA };
    const TCHAR* suffixes[] = { _T("_bar.png"), _T("_pie.png"), _T("_line.png"), _T("_area.png") };
    bool allOk = true;

    for (int i = 0; i < 4; i++)
    {
        tstring savePath = FilePathUtils::buildExportAllChartPath(
            inputPath, loadedTitle, suffixes[i]);
        if (savePath.empty())
            { allOk = false; continue; }
        if (!FilePathUtils::ensureParentDirectoryExists(savePath))
            { allOk = false; continue; }

        auto chart = createChart(allTypes[i], loadedTitle, loadedData, theme);
        if (!chart || !ImageExporter::exportChart(*chart, savePath))
            allOk = false;
    }

    if (allOk)
        showPopup(theme, _T("Export"),
                  _T("Exported: bar / pie / line / area PNG files."), inputPath);
    else
        showPopup(theme, _T("Export"), _T("Export failed."),
                  _T("Please check the save path and permissions."), true);
}
