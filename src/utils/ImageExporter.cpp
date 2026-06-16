#include "ImageExporter.h"
#include <cstdio>
#include <tchar.h>

// ==================== 导出图表为 PNG 图片 ====================
// 实现原理：
//   1. 创建一个离屏 IMAGE 对象（1200×800）
//   2. 将 EasyX 的工作图像切换为该离屏图像
//   3. 用白色清空背景，调用 chart.draw() 绘制图表
//   4. 恢复工作图像到屏幕，将离屏图像保存为 PNG
//   5. 通过尝试打开文件来验证导出是否成功
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
