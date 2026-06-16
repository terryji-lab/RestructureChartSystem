#pragma once
#include "chart/Chart.h"
#include <vector>

// ==================== File Data Reader Class ====================
// This class stores file path and reads chart data from CSV file.
class FileDataReader
{
private:
    tstring filePath;
    std::vector<ChartItem> data;
    tstring errorMessage;

private:
    static tstring stringToTString(const std::string& s);

public:
    FileDataReader();
    FileDataReader(const tstring& path);

    // Set CSV absolute path from GUI input box
    void setFilePath(const tstring& path);

    // Get current file path
    tstring getFilePath() const;

    // Load CSV using current filePath
    bool load();

    // Set path and load immediately
    bool loadFromPath(const tstring& path);

    // Get loaded chart data
    std::vector<ChartItem> getData() const;

    // Get error message
    tstring getErrorMessage() const;

    // Clear old data and path
    void clear();
};
