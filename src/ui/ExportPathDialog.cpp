#include "ExportPathDialog.h"
#include "utils/RenderUtils.h"
#include "chart/Chart.h"

// 构造函数：居中弹窗，初始化输入框和按钮，绑定回调
ExportPathDialog::ExportPathDialog(const ColorTheme& theme,
                                   const tstring& title,
                                   const tstring& message,
                                   const tstring& placeholder,
                                   const tstring& defaultPath)
    : m_x((1200 - 620) / 2)
    , m_y((800 - 280) / 2)
    , m_w(620)
    , m_h(280)
    , m_title(title)
    , m_message(message)
    , m_path(defaultPath)
    , m_theme(theme)
    , m_shadowColor(darkenColor(theme.bgColor, 35))
    , m_bgColor(theme.cardColor)
    , m_textColor(theme.textColor)
    , m_accentColor(theme.accentColor)
    , m_input(m_x + 42, m_y + 126, m_w - 84, 42, placeholder, 6)
    , m_okButton(m_x + m_w - 270, m_y + m_h - 66, 110, 40, _T("OK"),
          [this]() {
              m_path = m_input.getText();
              m_accepted = true;
              m_done = true;
          }, 6)
    , m_cancelButton(m_x + m_w - 146, m_y + m_h - 66, 110, 40, _T("Cancel"),
          [this]() {
              m_accepted = false;
              m_done = true;
          }, 6)
    , m_done(false)
    , m_accepted(false)
{
    m_input.setText(defaultPath);
    m_input.setFocus(true);
    applyTheme(theme);
}

// 应用主题配色到弹窗各控件
void ExportPathDialog::applyTheme(const ColorTheme& theme)
{
    m_theme = theme;
    m_shadowColor = darkenColor(theme.bgColor, 35);
    m_bgColor = theme.cardColor;
    m_textColor = theme.textColor;
    m_accentColor = theme.accentColor;

    m_input.setColors(darkenColor(theme.cardColor, 30), RGB(255, 255, 255), theme.accentColor);

    m_okButton.setColorNormal(theme.btnNormal);
    m_okButton.setColorHover(theme.btnHover);
    m_okButton.setColorPress(theme.btnPress);
    m_okButton.setColorBorder(theme.btnPress);
    m_okButton.setColorText(RGB(255, 255, 255));

    m_cancelButton.setColorNormal(lightenColor(theme.btnNormal, 30));
    m_cancelButton.setColorHover(lightenColor(theme.btnHover, 20));
    m_cancelButton.setColorPress(theme.btnPress);
    m_cancelButton.setColorBorder(theme.btnPress);
    m_cancelButton.setColorText(RGB(255, 255, 255));
}

// 弹窗绘制：阴影 → 主体背景 → 加粗边框 → 顶部 accent 色条 → 标题/说明 → 输入框 → 按钮
void ExportPathDialog::draw() const
{
    // 阴影
    setfillcolor(m_shadowColor);
    solidrectangle(m_x + 10, m_y + 10, m_x + m_w + 10, m_y + m_h + 10);

    // 主体背景
    setfillcolor(m_bgColor);
    solidrectangle(m_x, m_y, m_x + m_w, m_y + m_h);

    // accent 粗边框
    setlinecolor(m_accentColor);
    setlinestyle(PS_SOLID, 2);
    rectangle(m_x, m_y, m_x + m_w, m_y + m_h);

    // 顶部 accent 色条
    setfillcolor(m_accentColor);
    solidrectangle(m_x, m_y, m_x + m_w, m_y + 8);

    // 标题
    setbkmode(TRANSPARENT);
    settextcolor(m_accentColor);
    settextstyle(28, 0, _T("Microsoft YaHei"), 0, 0, FW_BOLD, false, false, false);
    outtextxy(m_x + 42, m_y + 36, m_title.c_str());

    // 提示消息
    settextcolor(darkenColor(m_textColor, 30));
    settextstyle(18, 0, _T("Microsoft YaHei"), 0, 0, FW_NORMAL, false, false, false);
    outtextxy(m_x + 42, m_y + 82, m_message.c_str());

    // 路径格式提示
    settextcolor(darkenColor(m_textColor, 55));
    settextstyle(15, 0, _T("Microsoft YaHei"), 0, 0, FW_NORMAL, false, false, false);
    outtextxy(m_x + 42, m_y + 176, _T("Relative paths are saved from the project running directory."));
    outtextxy(m_x + 42, m_y + 198, _T("Examples: exports\\chart.png  or  D:\\charts\\chart.png"));

    // 控件绘制
    m_input.draw();
    m_okButton.draw();
    m_cancelButton.draw();
}

// 模态事件循环：处理键盘（ESC/Enter）、字符输入、鼠标移动和点击，
// 任何状态变化后立即重绘弹窗
bool ExportPathDialog::showModal()
{
    m_done = false;
    m_accepted = false;
    m_input.setFocus(true);

    BeginBatchDraw();
    draw();
    EndBatchDraw();

    ExMessage msg;
    while (!m_done)
    {
        msg = getmessage(EX_MOUSE | EX_KEY | EX_CHAR);
        bool changed = false;

        if (msg.message == WM_KEYDOWN)
        {
            if (msg.vkcode == VK_ESCAPE)
            {
                m_accepted = false;
                m_done = true;
            }
            else if (msg.vkcode == VK_RETURN)
            {
                m_path = m_input.getText();
                m_accepted = true;
                m_done = true;
            }
            else
            {
                m_input.handleKeyDown(msg.vkcode);
                changed = true;
            }
        }
        else if (msg.message == WM_CHAR)
        {
            m_input.handleChar(msg.ch);
            changed = true;
        }
        else if (msg.message == WM_MOUSEMOVE)
        {
            changed |= m_okButton.handleMouseMove(msg.x, msg.y);
            changed |= m_cancelButton.handleMouseMove(msg.x, msg.y);
        }
        else if (msg.message == WM_LBUTTONDOWN)
        {
            changed |= m_input.handleMouseDown(msg.x, msg.y);
            changed |= m_okButton.handleMouseDown(msg.x, msg.y);
            changed |= m_cancelButton.handleMouseDown(msg.x, msg.y);
        }
        else if (msg.message == WM_LBUTTONUP)
        {
            changed |= m_okButton.handleMouseUp(msg.x, msg.y);
            changed |= m_cancelButton.handleMouseUp(msg.x, msg.y);
        }

        if (changed && !m_done)
        {
            BeginBatchDraw();
            draw();
            EndBatchDraw();
        }
    }

    return m_accepted;
}

const tstring& ExportPathDialog::getPath() const
{
    return m_path;
}
