#include "TextInput.h"

TextInput::TextInput(int x, int y, int w, int h,
                     const tstring& placeholder, int radius)
    : Card(x, y, w, h, _T(""), radius)
    , m_text(_T(""))
    , m_placeholder(placeholder)
    , m_focused(false)
    , m_cursorPos(0)
{
}

void TextInput::draw() const
{
    // Card 基类绘制阴影 + 圆角背景
    Card::draw();

    setbkmode(TRANSPARENT);

    if (m_text.empty())
    {
        // 占位文字（灰色）
        settextcolor(RGB(0xAA, 0xAA, 0xAA));
        settextstyle(20, 0, _T("Segoe UI"), 0, 0, FW_SEMIBOLD, false, false, false);
        outtextxy(m_x + 10, m_y + (m_h - 18) / 2, m_placeholder.c_str());
    }
    else
    {
        // 用户输入（黑色）
        settextcolor(BLACK);
        settextstyle(20, 0, _T("Segoe UI"), 0, 0, FW_SEMIBOLD, false, false, false);
        outtextxy(m_x + 10, m_y + (m_h - 18) / 2, m_text.c_str());
    }

    // 焦点光标
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

bool TextInput::handleMouseDown(int mx, int my)
{
    bool clicked = Card::contains(mx, my);
    m_focused = clicked;
    if (m_focused) m_cursorPos = m_text.length();
    return clicked;
}

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
        case 'A': m_cursorPos = m_text.length(); break; // move cursor to end (acts as select-all+end)
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
        if (m_cursorPos < m_text.length()) {
            m_text.erase(m_cursorPos, 1);
        }
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

void TextInput::handleChar(WPARAM ch)
{
    TCHAR c = (TCHAR)ch;
    // 过滤控制字符（保留空格及以上可打印字符，排除 DEL）
    if (c >= 32 && c != 127)
    {
        m_text.insert(m_cursorPos, 1, c);
        m_cursorPos++;
    }
}

// ── 剪贴板操作 ──

void TextInput::copy()
{
    if (m_text.empty()) return;
    if (!OpenClipboard(GetHWnd())) return;

    EmptyClipboard();

    size_t size = (m_text.length() + 1) * sizeof(TCHAR);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
    if (hMem)
    {
        TCHAR* pMem = (TCHAR*)GlobalLock(hMem);
        if (pMem)
        {
            _tcscpy_s(pMem, m_text.length() + 1, m_text.c_str());
            GlobalUnlock(hMem);
        }
#ifdef UNICODE
        SetClipboardData(CF_UNICODETEXT, hMem);
#else
        SetClipboardData(CF_TEXT, hMem);
#endif
    }

    CloseClipboard();
}

void TextInput::paste()
{
    if (!IsClipboardFormatAvailable(
#ifdef UNICODE
        CF_UNICODETEXT
#else
        CF_TEXT
#endif
    )) return;

    if (!OpenClipboard(GetHWnd())) return;

#ifdef UNICODE
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
#else
    HANDLE hData = GetClipboardData(CF_TEXT);
#endif

    if (hData)
    {
        TCHAR* pData = (TCHAR*)GlobalLock(hData);
        if (pData)
        {
            m_text.insert(m_cursorPos, pData);
            m_cursorPos += _tcslen(pData);
            GlobalUnlock(hData);
        }
    }

    CloseClipboard();
}

void TextInput::cut()
{
    if (m_text.empty()) return;
    copy();
    m_text.clear();
    m_cursorPos = 0;
}

void TextInput::setText(const tstring& text) { m_text = text; m_cursorPos = m_text.length(); }
const tstring& TextInput::getText() const     { return m_text; }
bool TextInput::isFocused() const             { return m_focused; }
void TextInput::setFocus(bool focus)          { m_focused = focus; }
