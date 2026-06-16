#pragma once
#include <easyx.h>
#include <string>
#include <functional>
#include "Widget.h"
#include "../common/Types.h"

// ==================== 按钮控件类 ====================
// 继承 Widget，实现圆角按钮。支持三态视觉反馈：
//   - Normal（正常）: 基础颜色
//   - Hover（悬停）: 鼠标移入时变亮
//   - Press（按下）: 鼠标按下时变暗
//
// 包含阴影效果、圆角边框、居中文字，文字使用 Microsoft YaHei 字体。
// 通过 std::function 回调实现点击事件。
class Button : public Widget
{
public:
    Button(int x, int y, int width, int height, const tstring& text,
           std::function<void()> onClick = nullptr,
           int cornerRadius = 8);

    // ── 事件处理（override Widget）──
    bool contains(int mx, int my) const override;
    bool handleMouseMove(int mx, int my) override;
    bool handleMouseDown(int mx, int my) override;
    bool handleMouseUp(int mx, int my)   override;
    void draw() const override;

    // ── 属性设置 ──
    void setText(const tstring& text);
    void setPosition(int x, int y);
    void setVisible(bool visible);              // 控制按钮可见性
    void setOnClick(std::function<void()> onClick);

    // 三态颜色设置
    void setColorNormal(COLORREF color);
    void setColorHover(COLORREF color);
    void setColorPress(COLORREF color);
    void setColorBorder(COLORREF color);
    void setColorText(COLORREF color);
    void setColorShadow(COLORREF color);
    void setRadius(int r);                      // 圆角半径

    // ── 属性获取 ──
    const tstring& getText() const;
    bool isVisible() const;

private:
    int m_radius;                               // 圆角半径
    tstring m_text;                             // 按钮文字
    bool m_isHovered;                           // 鼠标悬停状态
    bool m_isPressed;                           // 鼠标按下状态
    bool m_visible;                             // 是否可见
    std::function<void()> m_onClick;           // 点击回调

    // 三态颜色 + 边框/文字/阴影颜色
    COLORREF m_colorNormal;
    COLORREF m_colorHover;
    COLORREF m_colorPress;
    COLORREF m_colorBorder;
    COLORREF m_colorText;
    COLORREF m_colorShadow;
};
