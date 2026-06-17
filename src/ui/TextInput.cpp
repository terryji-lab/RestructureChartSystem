#include "TextInput.h"
#include "../utils/ClipboardUtils.h"
#include "../utils/AntiAlias.h"

// 构造函数
TextInput::TextInput(int x, int y, int w, int h,
                     const tstring& placeholder, int radius)
    : Card(x, y, w, h, _T(""), radius)
    , m_text(_T(""))
    , m_placeholder(placeholder)
    , m_focused(false)
    , m_cursorPos(0)
{
}

// 绘制
void TextInput::draw() const
{
    Card::draw();

    setbkmode(TRANSPARENT);

    if (m_text.empty())
    {
        settextcolor(RGB(0xAA, 0xAA, 0xAA));
        AA::setTextStyleAA(20, 0, _T("Segoe UI"), 0, 0, FW_SEMIBOLD);
        outtextxy(m_x + 10, m_y + (m_h - 18) / 2, m_placeholder.c_str());
    }
    else
    {
        settextcolor(BLACK);
        AA::setTextStyleAA(20, 0, _T("Segoe UI"), 0, 0, FW_SEMIBOLD);
        outtextxy(m_x + 10, m_y + (m_h - 18) / 2, m_text.c_str());
    }

    // 闪烁光标
    if (m_focused)
    {
        int cursorX = m_x + 10;
        if (m_cursorPos > 0)
            cursorX += (int)textwidth(m_text.substr(0, m_cursorPos).c_str());
        cursorX += 1;
        setlinecolor(BLACK);
        line(cursorX, m_y + 7, cursorX, m_y + m_h - 7);
    }
}

// 鼠标事件：点击获取焦点
bool TextInput::handleMouseDown(int mx, int my)
{
    bool clicked = Card::contains(mx, my);
    m_focused = clicked;
    if (m_focused) m_cursorPos = m_text.length();
    return clicked;
}

// 键盘事件
void TextInput::handleKeyDown(WPARAM vkcode)
{
    bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;

    if (ctrl)
    {
        switch (vkcode)
        {
        case 'C': copy();            break;
        case 'V': paste();           break;
        case 'X': cut();             break;
        case 'A': m_cursorPos = m_text.length(); break;
        }
        return;
    }

    switch (vkcode)
    {
    case VK_BACK:
        if (m_cursorPos > 0) {
            m_text.erase(m_cursorPos - 1, 1);
            m_cursorPos--;
        }
        break;
    case VK_DELETE:
        if (m_cursorPos < m_text.length())
            m_text.erase(m_cursorPos, 1);
        break;
    case VK_LEFT:
        if (m_cursorPos > 0) m_cursorPos--;
        break;
    case VK_RIGHT:
        if (m_cursorPos < m_text.length()) m_cursorPos++;
        break;
    case VK_HOME:
        m_cursorPos = 0;
        break;
    case VK_END:
        m_cursorPos = m_text.length();
        break;
    }
}

// 字符输入
void TextInput::handleChar(WPARAM ch)
{
    TCHAR c = (TCHAR)ch;
    if (c >= 32 && c != 127)
    {
        m_text.insert(m_cursorPos, 1, c);
        m_cursorPos++;
    }
}

// --- 剪贴板操作（委托给 ClipboardUtils）---

void TextInput::copy()
{
    ClipboardUtils::copyToClipboard(m_text, GetHWnd());
}

void TextInput::paste()
{
    tstring clip = ClipboardUtils::pasteFromClipboard(GetHWnd());
    if (!clip.empty())
    {
        m_text.insert(m_cursorPos, clip);
        m_cursorPos += clip.length();
    }
}

void TextInput::cut()
{
    if (m_text.empty()) return;
    ClipboardUtils::copyToClipboard(m_text, GetHWnd());
    m_text.clear();
    m_cursorPos = 0;
}

// 属性设置/获取
void TextInput::setText(const tstring& text) { m_text = text; m_cursorPos = m_text.length(); }
const tstring& TextInput::getText() const     { return m_text; }
bool TextInput::isFocused() const             { return m_focused; }
void TextInput::setFocus(bool focus)          { m_focused = focus; }
