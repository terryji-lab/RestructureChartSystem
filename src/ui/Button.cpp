#include "Button.h"
#include "../utils/RenderUtils.h"
#include "../utils/AntiAlias.h"

// 构造函数，默认使用蓝色系配色方案
Button::Button(int x, int y, int width, int height, const tstring& text,
               std::function<void()> onClick, int cornerRadius)
    : Widget(x, y, width, height)
    , m_radius(cornerRadius)
    , m_text(text)
    , m_isHovered(false)
    , m_isPressed(false)
    , m_visible(true)
    , m_onClick(onClick)
    , m_colorNormal(RGB(70, 130, 210))         // 默认蓝色
    , m_colorHover(RGB(100, 160, 235))          // 悬停亮蓝
    , m_colorPress(RGB(50, 100, 175))           // 按下深蓝
    , m_colorBorder(RGB(50, 110, 190))
    , m_colorText(WHITE)
    , m_colorShadow(RGB(180, 190, 210))
{
}

// 事件处理

// 命中检测：仅在可见时生效
bool Button::contains(int mx, int my) const
{
    return m_visible
        && mx >= m_x && mx <= m_x + m_w
        && my >= m_y && my <= m_y + m_h;
}

// 鼠标移动：更新悬停状态，状态变化时返回 true 触发重绘
bool Button::handleMouseMove(int mx, int my)
{
    if (!m_visible) return false;
    bool wasHovered = m_isHovered;
    m_isHovered = contains(mx, my);
    return (wasHovered != m_isHovered);         // 仅状态变化时需重绘
}

// 鼠标按下：记录按下状态
bool Button::handleMouseDown(int mx, int my)
{
    if (!m_visible) return false;
    if (contains(mx, my) && !m_isPressed)
    {
        m_isPressed = true;
        return true;                            // 需要重绘
    }
    return false;
}

// 鼠标释放：如果之前在按钮内按下，且在按钮内释放，触发 onClick 回调
bool Button::handleMouseUp(int mx, int my)
{
    if (!m_visible) return false;
    bool wasPressed = m_isPressed;
    m_isPressed = false;
    if (wasPressed && contains(mx, my))
    {
        if (m_onClick) m_onClick();             // 执行点击回调
    }
    return wasPressed;                          // 需要重绘恢复外观
}

// 绘制
void Button::draw() const
{
    if (!m_visible) return;

    // 根据状态选择填充颜色
    COLORREF fillColor = m_colorNormal;
    if (m_isPressed)
        fillColor = m_colorPress;               // 按下：深色
    else if (m_isHovered)
        fillColor = m_colorHover;               // 悬停：亮色

    // 1. 阴影（仅在普通/悬停状态绘制，按下时不绘制以模拟"按下"效果）
    if (!m_isPressed && m_radius > 0)
    {
        int so = 3;                             // 阴影偏移 3px
        drawRoundRectFill(m_x + so, m_y + so, m_w, m_h, m_radius, m_colorShadow);
    }

    // 2. 按钮主体（圆角矩形填充）
    drawRoundRectFill(m_x, m_y, m_w, m_h, m_radius, fillColor);

    // 3. 细线边框（使用 GDI+ 抗锯齿圆角边框）
    if (m_radius > 0)
    {
        AA::roundrect(m_x, m_y, m_w, m_h, m_radius,
                      fillColor, m_colorBorder, 1);
    }
    else
    {
        // 无圆角时使用普通矩形
        setlinecolor(m_colorBorder);
        rectangle(m_x, m_y, m_x + m_w, m_y + m_h);
    }

    // 4. 文字（水平和垂直居中，使用 Microsoft YaHei 字体）
    settextcolor(m_colorText);
    setbkmode(TRANSPARENT);
    AA::setTextStyleAA(22, 0, _T("Microsoft YaHei"), 0, 0, FW_SEMIBOLD);

    int tw = textwidth(m_text.c_str());
    int th = textheight(m_text.c_str());
    int tx = m_x + (m_w - tw) / 2;
    int ty = m_y + (m_h - th) / 2;
    if (tx < m_x + 4) tx = m_x + 4;            // 防止文字太靠左
    outtextxy(tx, ty, m_text.c_str());
}

// 属性设置器
void Button::setText(const tstring& text)        { m_text = text; }
void Button::setPosition(int x, int y)           { m_x = x; m_y = y; }
void Button::setVisible(bool visible)            { m_visible = visible; }
void Button::setOnClick(std::function<void()> cb){ m_onClick = cb; }
void Button::setColorNormal(COLORREF c)          { m_colorNormal = c; }
void Button::setColorHover(COLORREF c)           { m_colorHover = c; }
void Button::setColorPress(COLORREF c)           { m_colorPress = c; }
void Button::setColorBorder(COLORREF c)          { m_colorBorder = c; }
void Button::setColorText(COLORREF c)            { m_colorText = c; }
void Button::setColorShadow(COLORREF c)          { m_colorShadow = c; }
void Button::setRadius(int r)                    { m_radius = r; }

const tstring& Button::getText() const           { return m_text; }
bool Button::isVisible() const                   { return m_visible; }
