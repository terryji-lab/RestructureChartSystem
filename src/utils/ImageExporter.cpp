#include "ImageExporter.h"
#include <cstdio>
#include <tchar.h>

bool ImageExporter::exportChart(
    Chart& chart,
    const tstring& filePath
)
{
    if (filePath.empty())
    {
        return false;
    }

    IMAGE img(DEFAULT_IMAGE_WIDTH, DEFAULT_IMAGE_HEIGHT);

    SetWorkingImage(&img);

    setbkcolor(WHITE);
    cleardevice();

    chart.draw();

    SetWorkingImage(NULL);

    saveimage(filePath.c_str(), &img);

    FILE* fp = _tfopen(filePath.c_str(), _T("rb"));

    if (fp == NULL)
    {
        return false;
    }

    fclose(fp);
    return true;
}
