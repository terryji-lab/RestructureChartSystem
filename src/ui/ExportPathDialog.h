#pragma once
#include <easyx.h>
#include <string>
#include "TextInput.h"
#include "Button.h"
#include "chart/Chart.h"

// 导出路径对话框，模态弹窗，让用户输入文件保存路径
// 包含文本输入框、OK 和 Cancel 按钮，支持主题配色
class ExportPathDialog
{
public:
    ExportPathDialog(const ColorTheme& theme,
                     const tstring& title,
                     const tstring& message,
                     const tstring& placeholder,
                     const tstring& defaultPath = _T(""));

    // 显示模态对话框，返回 true 表示用户确认
    bool showModal();
    // 获取用户输入的路径
    const tstring& getPath() const;

private:
    void applyTheme(const ColorTheme& theme);
    void draw() const;

    // 弹窗几何位置和尺寸
    int m_x;
    int m_y;
    int m_w;
    int m_h;

    // 弹窗文案
    tstring m_title;
    tstring m_message;
    tstring m_path;

    // 主题配色
    ColorTheme m_theme;
    COLORREF m_shadowColor;
    COLORREF m_bgColor;
    COLORREF m_textColor;
    COLORREF m_accentColor;

    // 控件
    TextInput m_input;
    Button m_okButton;
    Button m_cancelButton;

    // 状态标记
    bool m_done;       // 对话框是否已完成
    bool m_accepted;   // 用户是否点击了确认
};
