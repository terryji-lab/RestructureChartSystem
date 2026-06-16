#pragma once
#include <easyx.h>
#include <string>
#include <functional>

// 根据编译选项自动选择窄/宽字符
typedef std::basic_string<TCHAR> tstring;

class Button
{
public:
    Button(int x, int y, int width, int height, const tstring& text,
           std::function<void()> onClick = nullptr,
           int cornerRadius = 8);

    // 事件处理
    bool contains(int mx, int my) const;
    bool handleMouseMove(int mx, int my);
    bool handleMouseDown(int mx, int my);
    bool handleMouseUp(int mx, int my);
    void draw() const;

    // Setter方法
    void setText(const tstring& text);
    void setPosition(int x, int y);
    void setVisible(bool visible);
    void setOnClick(std::function<void()> onClick);

    void setColorNormal(COLORREF color);
    void setColorHover(COLORREF color);
    void setColorPress(COLORREF color);
    void setColorBorder(COLORREF color);
    void setColorText(COLORREF color);
    void setColorShadow(COLORREF color);
    void setRadius(int r);

    // Getter方法
    const tstring& getText() const;
    bool isVisible() const;

private:
    int m_x, m_y;
    int m_width, m_height;
    int m_radius;
    tstring m_text;
    bool m_isHovered;
    bool m_isPressed;
    bool m_visible;
    std::function<void()> m_onClick;

    COLORREF m_colorNormal;
    COLORREF m_colorHover;
    COLORREF m_colorPress;
    COLORREF m_colorBorder;
    COLORREF m_colorText;
    COLORREF m_colorShadow;
};
