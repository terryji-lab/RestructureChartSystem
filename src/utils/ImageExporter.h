#pragma once
#include "chart/Chart.h"
#include <easyx.h>

class ImageExporter
{
private:
    static const int DEFAULT_IMAGE_WIDTH = 1200;
    static const int DEFAULT_IMAGE_HEIGHT = 800;

public:
    static bool exportChart(
        Chart& chart,
        const tstring& filePath
    );
};
