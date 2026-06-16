#pragma once
#include <easyx.h>
#include <string>

typedef std::basic_string<TCHAR> tstring;

class TextInput
{
public:
    TextInput(int x, int y, int width, int height, const tstring& placeholder = _T(""));

    // 事件处理
    bool contains(int mx, int my) const;
    bool handleMouseDown(int mx, int my);
    void handleKeyDown(WPARAM vkcode);   // 键盘输入（同事来实现逻辑）

    // 绘制
    void draw() const;

    // Setter/Getter
    void setText(const tstring& text);
    const tstring& getText() const;
    bool isFocused() const;
    void setFocus(bool focus);

private:
    int m_x, m_y;
    int m_width, m_height;
    tstring m_text;
    tstring m_placeholder;
    bool m_focused;
};
