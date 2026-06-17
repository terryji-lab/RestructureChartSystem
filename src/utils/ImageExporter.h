#pragma once
#include "chart/Chart.h"
#include <easyx.h>

// 图片导出类，将图表渲染到离屏图像并以 PNG 格式保存到磁盘
// 默认导出尺寸为 1200×800 像素，与窗口大小一致
class ImageExporter
{
private:
    // 默认导出图片尺寸
    static const int DEFAULT_IMAGE_WIDTH = 1200;
    static const int DEFAULT_IMAGE_HEIGHT = 800;

public:
    // 将指定图表导出为 PNG 文件
    // @param chart    要导出的图表对象引用
    // @param filePath 目标文件路径（含 .png 后缀）
    // @return 导出成功返回 true，失败返回 false
    static bool exportChart(
        Chart& chart,
        const tstring& filePath
    );
};
