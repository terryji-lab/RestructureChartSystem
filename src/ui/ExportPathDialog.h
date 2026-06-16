#pragma once
#include <easyx.h>
#include <string>
#include "TextInput.h"
#include "Button.h"
#include "chart/Chart.h"

class ExportPathDialog
{
public:
    ExportPathDialog(const ColorTheme& theme,
                     const tstring& title,
                     const tstring& message,
                     const tstring& placeholder,
                     const tstring& defaultPath = _T(""));

    bool showModal();
    const tstring& getPath() const;

private:
    void applyTheme(const ColorTheme& theme);
    void draw() const;

    int m_x;
    int m_y;
    int m_w;
    int m_h;

    tstring m_title;
    tstring m_message;
    tstring m_path;

    ColorTheme m_theme;
    COLORREF m_shadowColor;
    COLORREF m_bgColor;
    COLORREF m_textColor;
    COLORREF m_accentColor;

    TextInput m_input;
    Button m_okButton;
    Button m_cancelButton;

    bool m_done;
    bool m_accepted;
};
