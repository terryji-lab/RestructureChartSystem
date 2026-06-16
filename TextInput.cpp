#include "TextInput.h"

TextInput::TextInput(int x, int y, int width, int height, const tstring& placeholder)
    : m_x(x), m_y(y)
    , m_width(width), m_height(height)
    , m_text(_T(""))
    , m_placeholder(placeholder)
    , m_focused(false)
{}

// 检查坐标是否在输入框范围内
bool TextInput::contains(int mx, int my) const
{
    return mx >= m_x && mx <= m_x + m_width
        && my >= m_y && my <= m_y + m_height;
}

// 鼠标按下 - 切换焦点
bool TextInput::handleMouseDown(int mx, int my)
{
    bool clicked = contains(mx, my);
    m_focused = clicked;
    return clicked;
}

// 键盘输入
void TextInput::handleKeyDown(WPARAM vkcode)
{
    // 退格键：删除最后一个字符
    if(vkcode == VK_BACK && !m_text.empty())m_text.pop_back();

    // 字母键 A~Z（检测Shift状态切换大小写）
    if(vkcode >= 'A' && vkcode <= 'Z')
    {
        TCHAR ch = (TCHAR)vkcode;
        if(!(GetAsyncKeyState(VK_SHIFT) & 0x8000))ch = ch - 'A' + 'a';
        m_text.push_back(ch);
    }

    // 数字键 0~9
    if(vkcode >= '0' && vkcode <= '9')m_text.push_back((TCHAR)vkcode);

    // 空格键
    if(vkcode == VK_SPACE)m_text.push_back(_T(' '));

    // 句点键（用于输入 .csv 等扩展名）
    if(vkcode == VK_OEM_PERIOD)m_text.push_back(_T('.'));

    // 斜杠 / 和反斜杠 \
    if(vkcode == VK_OEM_2)m_text.push_back(_T('/'));
    if(vkcode == VK_OEM_5)m_text.push_back(_T('\\'));

    // 减号/下划线（用于文件名）
    if(vkcode == VK_OEM_MINUS)
    {
        if(GetAsyncKeyState(VK_SHIFT) & 0x8000)m_text.push_back(_T('_'));
        else m_text.push_back(_T('-'));
    }
}

// 绘制输入框
void TextInput::draw() const
{
    // 背景填充
    setfillcolor(WHITE);
    setlinecolor(m_focused ? RGB(0, 120, 215) : RGB(0xAA, 0xAA, 0xAA));
    fillrectangle(m_x, m_y, m_x + m_width, m_y + m_height);

    setbkmode(TRANSPARENT);

    if (m_text.empty())
    {
        // 显示占位文字（灰色）
        settextcolor(RGB(0xAA, 0xAA, 0xAA));
        settextstyle(16, 0, _T("Arial"));
        outtextxy(m_x + 8, m_y + (m_height - 18) / 2, m_placeholder.c_str());
    }
    else
    {
        // 显示用户输入的文字（黑色）
        settextcolor(BLACK);
        settextstyle(16, 0, _T("Arial"));
        outtextxy(m_x + 8, m_y + (m_height - 18) / 2, m_text.c_str());
    }

    // 聚焦时绘制光标
    if (m_focused)
    {
        int textW = m_text.empty() ? 0
            : (int)m_text.length() * textwidth(m_text[0]);
        int cursorX = m_x + 8 + textW + 1;
        setlinecolor(BLACK);
        line(cursorX, m_y + 6, cursorX, m_y + m_height - 6);
    }
}

// Setter/Getter
void TextInput::setText(const tstring& text)
{
    m_text = text;
}

const tstring& TextInput::getText() const
{
    return m_text;
}

bool TextInput::isFocused() const
{
    return m_focused;
}

void TextInput::setFocus(bool focus)
{
    m_focused = focus;
}
