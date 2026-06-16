#include "TextInput.h"

// ==================== 构造 ====================
TextInput::TextInput(int x, int y, int w, int h,
                     const tstring& placeholder, int radius)
    : Card(x, y, w, h, _T(""), radius)
    , m_text(_T(""))
    , m_placeholder(placeholder)
    , m_focused(false)
    , m_cursorPos(0)
{
}

// ==================== 绘制 ====================
void TextInput::draw() const
{
    // Card 基类绘制阴影 + 圆角背景
    Card::draw();

    setbkmode(TRANSPARENT);

    if (m_text.empty())
    {
        // 占位文字（灰色，提示用户输入）
        settextcolor(RGB(0xAA, 0xAA, 0xAA));
        settextstyle(20, 0, _T("Segoe UI"), 0, 0, FW_SEMIBOLD, false, false, false);
        outtextxy(m_x + 10, m_y + (m_h - 18) / 2, m_placeholder.c_str());
    }
    else
    {
        // 用户输入的文字（黑色）
        settextcolor(BLACK);
        settextstyle(20, 0, _T("Segoe UI"), 0, 0, FW_SEMIBOLD, false, false, false);
        outtextxy(m_x + 10, m_y + (m_h - 18) / 2, m_text.c_str());
    }

    // 闪烁光标（仅在焦点状态下绘制）
    if (m_focused)
    {
        // 计算光标 X 位置：从左边距 + 光标前文本宽度
        int cursorX = m_x + 10;
        if (m_cursorPos > 0)
            cursorX += (int)textwidth(m_text.substr(0, m_cursorPos).c_str());
        cursorX += 1;
        setlinecolor(BLACK);
        line(cursorX, m_y + 7, cursorX, m_y + m_h - 7);
    }
}

// ==================== 鼠标事件 ====================
// 点击时获取焦点，光标移到文本末尾
bool TextInput::handleMouseDown(int mx, int my)
{
    bool clicked = Card::contains(mx, my);
    m_focused = clicked;
    if (m_focused) m_cursorPos = m_text.length();
    return clicked;
}

// ==================== 键盘事件 ====================
// 处理剪贴板快捷键和文本编辑键
void TextInput::handleKeyDown(WPARAM vkcode)
{
    // 检测 Ctrl 键是否按下
    bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;

    if (ctrl)
    {
        switch (vkcode)
        {
        case 'C': copy();            break;     // Ctrl+C 复制
        case 'V': paste();           break;     // Ctrl+V 粘贴
        case 'X': cut();             break;     // Ctrl+X 剪切
        case 'A': m_cursorPos = m_text.length(); break; // Ctrl+A 全选（将光标移到末尾）
        }
        return;
    }

    // 非 Ctrl 组合键：文本编辑
    switch (vkcode)
    {
    case VK_BACK:                                // 退格：删除光标前一个字符
        if (m_cursorPos > 0) {
            m_text.erase(m_cursorPos - 1, 1);
            m_cursorPos--;
        }
        break;

    case VK_DELETE:                              // Delete：删除光标后一个字符
        if (m_cursorPos < m_text.length()) {
            m_text.erase(m_cursorPos, 1);
        }
        break;

    case VK_LEFT:                                // 左箭头：光标左移
        if (m_cursorPos > 0) m_cursorPos--;
        break;

    case VK_RIGHT:                               // 右箭头：光标右移
        if (m_cursorPos < m_text.length()) m_cursorPos++;
        break;

    case VK_HOME:                                // Home：光标移到开头
        m_cursorPos = 0;
        break;

    case VK_END:                                 // End：光标移到末尾
        m_cursorPos = m_text.length();
        break;
    }
}

// ==================== 字符输入 ====================
// 过滤控制字符，只在光标位置插入可打印字符
void TextInput::handleChar(WPARAM ch)
{
    TCHAR c = (TCHAR)ch;
    // 过滤控制字符（保留空格及以上可打印字符，排除 DEL(127)）
    if (c >= 32 && c != 127)
    {
        m_text.insert(m_cursorPos, 1, c);
        m_cursorPos++;
    }
}

// ==================== 剪贴板操作 ====================

// 复制：将当前文本写入系统剪贴板
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
        SetClipboardData(CF_UNICODETEXT, hMem);    // Unicode 文本格式
#else
        SetClipboardData(CF_TEXT, hMem);            // ANSI 文本格式
#endif
    }

    CloseClipboard();
}

// 粘贴：从系统剪贴板读取文本并插入到光标位置
void TextInput::paste()
{
    // 检查剪贴板是否有可用格式
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
            m_cursorPos += _tcslen(pData);         // 光标移到粘贴内容末尾
            GlobalUnlock(hData);
        }
    }

    CloseClipboard();
}

// 剪切：复制当前文本到剪贴板，然后清空文本框
void TextInput::cut()
{
    if (m_text.empty()) return;
    copy();
    m_text.clear();
    m_cursorPos = 0;
}

// ==================== 属性设置/获取 ====================
void TextInput::setText(const tstring& text) { m_text = text; m_cursorPos = m_text.length(); }
const tstring& TextInput::getText() const     { return m_text; }
bool TextInput::isFocused() const             { return m_focused; }
void TextInput::setFocus(bool focus)          { m_focused = focus; }
