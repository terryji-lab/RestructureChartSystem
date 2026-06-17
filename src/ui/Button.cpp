#include "Button.h"
#include "../utils/RenderUtils.h"

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

    // 3. 细线边框（用 line + arc 组合绘制圆角边框）
    setlinecolor(m_colorBorder);
    if (m_radius > 0)
    {
        int r = m_radius;
        int x = m_x, y = m_y, w = m_w, h = m_h;
        // 四条直线边
        line(x + r, y,         x + w - r, y);           // 上边
        line(x + r, y + h,     x + w - r, y + h);       // 下边
        line(x,     y + r,     x,         y + h - r);   // 左边
        line(x + w, y + r,     x + w,     y + h - r);   // 右边
        const double PI = 3.141592653589793;
        // 四个圆弧角（EasyX arc 参数：左上、右下、起始角、终止角）
        arc(x, y,                 x + r * 2, y + r * 2,     PI,       PI * 1.5);  // 左上
        arc(x + w - r * 2, y,     x + w,     y + r * 2,     PI * 1.5, PI * 2.0);  // 右上
        arc(x + w - r * 2, y + h - r * 2, x + w, y + h,     0,        PI * 0.5);  // 右下
        arc(x,     y + h - r * 2, x + r * 2, y + h,         PI * 0.5, PI);        // 左下
    }
    else
    {
        // 无圆角时使用普通矩形
        rectangle(m_x, m_y, m_x + m_w, m_y + m_h);
    }

    // 4. 文字（水平和垂直居中，使用 Microsoft YaHei 字体）
    settextcolor(m_colorText);
    setbkmode(TRANSPARENT);
    settextstyle(22, 0, _T("Microsoft YaHei"), 0, 0, FW_SEMIBOLD, false, false, false);

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
