#include "Card.h"
#include "../chart/Chart.h"   // darkenColor, lightenColor

// ==================== Card 基类 ====================

Card::Card(int x, int y, int w, int h, const tstring& title, int radius)
    : Widget(x, y, w, h)
    , m_radius(radius)
    , m_title(title)
    , m_shadowColor(RGB(200, 200, 210))         // 默认浅灰阴影
    , m_bgColor(WHITE)                          // 默认白色背景
    , m_titleColor(RGB(50, 50, 80))             // 默认深色标题
{
}

// 绘制阴影（在主体右下方偏移 2-3px 的圆角矩形）
void Card::drawShadow() const
{
    if (m_radius > 0)
        drawRoundRectFill(m_x + 2, m_y + 3, m_w, m_h, m_radius, m_shadowColor);
}

// 绘制主体背景（圆角矩形填充）
void Card::drawBody() const
{
    drawRoundRectFill(m_x, m_y, m_w, m_h, m_radius, m_bgColor);
}

// 绘制标题文字（粗体，左上角）
void Card::drawTitle() const
{
    if (!m_title.empty())
    {
        settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
        settextcolor(m_titleColor);
        outtextxy(m_x + 14, m_y + 10, m_title.c_str());
    }
}

// 默认绘制：阴影 → 主体 → 标题
void Card::draw() const
{
    drawShadow();
    drawBody();
    drawTitle();
}

// 设置卡片主题配色（阴影、背景、标题）
void Card::setColors(COLORREF shadow, COLORREF bg, COLORREF title)
{
    m_shadowColor = shadow;
    m_bgColor = bg;
    m_titleColor = title;
}

void Card::setTitle(const tstring& t)
{
    m_title = t;
}

// ==================== DisplayBox —— 只读文本展示框 ====================

DisplayBox::DisplayBox(int x, int y, int w, int h, const tstring& title, int radius)
    : Card(x, y, w, h, title, radius)
{
}

void DisplayBox::setText(const tstring& t)
{
    m_text = t;
}

void DisplayBox::draw() const
{
    Card::draw();  // 复用 Card 的阴影+背景+标题

    // 在标题下方绘制内容文本
    if (!m_text.empty())
    {
        settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_SEMIBOLD, false, false, false);
        settextcolor(m_titleColor);
        outtextxy(m_x + 14, m_y + 36, m_text.c_str());
    }
}

// ==================== PopupCard —— 模态弹窗 ====================

PopupCard::PopupCard(int x, int y, int w, int h, const tstring& title)
    : Card(x, y, w, h, title, 0)       // radius=0：直角矩形（弹窗风格）
    , m_accentColor(RGB(70, 130, 210)) // 默认强调色蓝色
    , m_okBtn(x + (w - 150) / 2, y + h - 68, 150, 42, _T("OK"),
              [this]() { m_shouldClose = true; })  // OK 按钮点击后关闭
    , m_shouldClose(false)
    , m_isError(false)
{
    m_okBtn.setColorText(RGB(255, 255, 255));
}

// 设置弹窗属性
void PopupCard::setMessage(const tstring& msg)  { m_message = msg; }
void PopupCard::setDetail(const tstring& detail) { m_detail = detail; }
void PopupCard::setAccentColor(COLORREF c)       { m_accentColor = c; }
void PopupCard::setError(bool isError)           { m_isError = isError; }

// 设置 OK 按钮的三态颜色
void PopupCard::setButtonColors(COLORREF normal, COLORREF hover, COLORREF press)
{
    m_okBtn.setColorNormal(normal);
    m_okBtn.setColorHover(hover);
    m_okBtn.setColorPress(press);
    m_okBtn.setColorBorder(press);
}

// ==================== 弹窗绘制 ====================
void PopupCard::draw() const
{
    // 1. 阴影（偏移 10px，比普通 Card 更重，突出弹窗层次感）
    setfillcolor(m_shadowColor);
    solidrectangle(m_x + 10, m_y + 10, m_x + m_w + 10, m_y + m_h + 10);

    // 2. 主体背景 —— 复用 Card::drawBody() 绘制圆角矩形
    Card::drawBody();

    // 3. 描边（2px 的强调色边框）
    setlinecolor(m_accentColor);
    setlinestyle(PS_SOLID, 2);
    rectangle(m_x, m_y, m_x + m_w, m_y + m_h);

    // 4. 顶部 accent 色条（8px 高，装饰性元素）
    setfillcolor(m_accentColor);
    solidrectangle(m_x, m_y, m_x + m_w, m_y + 8);

    // 5. 图标（成功 ✓ / 失败 ✕）
    int cx = m_x + 70;                         // 图标圆心 X
    int cy = m_y + 78;                         // 图标圆心 Y
    setlinecolor(m_accentColor);
    setfillcolor(lightenColor(m_accentColor, 95));  // 浅色圆形背景
    fillcircle(cx, cy, 28);

    setlinecolor(m_accentColor);
    setlinestyle(PS_SOLID, 4);
    if (m_isError)
    {
        // 绘制 X 号（错误图标）
        line(cx - 12, cy - 12, cx + 12, cy + 12);
        line(cx + 12, cy - 12, cx - 12, cy + 12);
    }
    else
    {
        // 绘制 ✓ 号（成功图标）
        line(cx - 13, cy, cx - 3, cy + 10);
        line(cx - 3, cy + 10, cx + 16, cy - 12);
    }
    setlinestyle(PS_SOLID, 1);

    // 6. 标题（大号粗体，位于图标右侧）
    setbkmode(TRANSPARENT);
    settextcolor(m_titleColor);
    settextstyle(28, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    outtextxy(m_x + 118, m_y + 48, m_title.c_str());

    // 7. 副标题/消息（较小字体，浅色）
    settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_NORMAL, false, false, false);
    settextcolor(darkenColor(m_titleColor, 35));
    outtextxy(m_x + 118, m_y + 92, m_message.c_str());

    // 8. 详情文字（强调色，例如文件名）
    settextcolor(m_accentColor);
    settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    outtextxy(m_x + 118, m_y + 122, m_detail.c_str());

    // 9. OK 按钮
    m_okBtn.draw();
}

// ==================== 模态事件循环 ====================
// 进入本地事件循环，阻塞直到用户点击 OK 或按下 Enter/Space/Escape。
// 独立处理鼠标移动、按下、释放事件以更新按钮状态。
void PopupCard::showModal()
{
    m_shouldClose = false;

    // 首次绘制
    BeginBatchDraw();
    draw();
    EndBatchDraw();

    ExMessage msg;
    while (!m_shouldClose)
    {
        msg = getmessage(EX_MOUSE | EX_KEY);

        if (msg.message == WM_KEYDOWN)
        {
            // Enter / Space / Escape 关闭弹窗
            if (msg.vkcode == VK_RETURN || msg.vkcode == VK_SPACE || msg.vkcode == VK_ESCAPE)
                break;
        }
        else if (msg.message == WM_MOUSEMOVE)
        {
            // 更新按钮悬停状态
            if (m_okBtn.handleMouseMove(msg.x, msg.y))
            {
                BeginBatchDraw();
                draw();
                EndBatchDraw();
            }
        }
        else if (msg.message == WM_LBUTTONDOWN)
        {
            // 更新按钮按下状态
            if (m_okBtn.handleMouseDown(msg.x, msg.y))
            {
                BeginBatchDraw();
                draw();
                EndBatchDraw();
            }
        }
        else if (msg.message == WM_LBUTTONUP)
        {
            // 触发按钮点击回调（可能设置 m_shouldClose = true）
            if (m_okBtn.handleMouseUp(msg.x, msg.y))
            {
                BeginBatchDraw();
                draw();
                EndBatchDraw();
            }
        }
    }
}
