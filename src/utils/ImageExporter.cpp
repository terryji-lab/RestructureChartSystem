#include "ImageExporter.h"
#include <cstdio>
#include <tchar.h>

// 导出图表为 PNG 图片
// 实现原理：创建离屏图像 → 切换工作图像 → 绘制图表 → 保存 PNG → 验证文件
bool ImageExporter::exportChart(
    Chart& chart,
    const tstring& filePath
)
{
    if (filePath.empty())
    {
        return false;
    }

    // 创建离屏绘制图像
    IMAGE img(DEFAULT_IMAGE_WIDTH, DEFAULT_IMAGE_HEIGHT);

    // 切换工作图像到离屏
    SetWorkingImage(&img);

    // 白色背景
    setbkcolor(WHITE);
    cleardevice();

    // 绘制图表到离屏图像
    chart.draw();

    // 恢复工作图像到屏幕
    SetWorkingImage(NULL);

    // 保存为 PNG 文件（EasyX 根据后缀名自动选择格式）
    saveimage(filePath.c_str(), &img);

    // 验证文件是否成功写出
    FILE* fp = _tfopen(filePath.c_str(), _T("rb"));

    if (fp == NULL)
    {
        return false;
    }

    fclose(fp);
    return true;
}
