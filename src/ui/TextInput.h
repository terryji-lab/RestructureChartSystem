#pragma once
#include <easyx.h>
#include <string>
#include "Card.h"

typedef std::basic_string<TCHAR> tstring;

class TextInput : public Card
{
public:
    TextInput(int x, int y, int w, int h,
              const tstring& placeholder = _T(""), int radius = 6);

    void draw() const override;
    bool handleMouseDown(int mx, int my) override;
    void handleKeyDown(WPARAM vkcode) override;
    void handleChar(WPARAM ch) override;
    bool wantsKeyInput() const override { return m_focused; }
    bool wantsCharInput() const override { return m_focused; }

    void setText(const tstring& text);
    const tstring& getText() const;
    bool isFocused() const;
    void setFocus(bool focus);

private:
    void copy();
    void paste();
    void cut();

    tstring m_text;
    tstring m_placeholder;
    bool m_focused;
    size_t m_cursorPos;
};
