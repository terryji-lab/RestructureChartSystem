#pragma once
#include <easyx.h>
#include <string>
#include "../utils/RenderUtils.h"
#include "Widget.h"
#include "Button.h"
#include "../common/Types.h"

// ==================== Card —— 圆角背景框 ====================
// 继承 Widget，提供统一的阴影 + 背景 + 标题绘制。
// 子类可复用 drawShadow() / drawBody() / drawTitle() 组合自己的 draw()。
class Card : public Widget
{
public:
    Card(int x, int y, int w, int h,
         const tstring& title = _T(""), int radius = 10);

    void draw() const override;

    // ── 主题配色 ──
    void setColors(COLORREF shadow, COLORREF bg, COLORREF title);
    void setTitle(const tstring& t);

protected:
    // ── 可拆分的绘制步骤，供子类（如 PopupCard）按需组合 ──
    void drawShadow() const;
    void drawBody() const;
    void drawTitle() const;

    int      m_radius;
    tstring  m_title;
    COLORREF m_shadowColor;
    COLORREF m_bgColor;
    COLORREF m_titleColor;
};

// ==================== DisplayBox —— 只读文本展示框 ====================
class DisplayBox : public Card
{
public:
    DisplayBox(int x, int y, int w, int h,
               const tstring& title = _T(""), int radius = 6);

    void setText(const tstring& t);
    void draw() const override;

private:
    tstring m_text;
};

// ==================== PopupCard —— 模态弹窗 ====================
// 继承 Card，复用 drawBody() 画背景，叠加 accent 色条、图标、文字和 OK 按钮。
class PopupCard : public Card
{
public:
    PopupCard(int x, int y, int w, int h, const tstring& title);

    void setMessage(const tstring& msg);
    void setDetail(const tstring& detail);
    void setAccentColor(COLORREF c);
    void setButtonColors(COLORREF normal, COLORREF hover, COLORREF press);
    void setError(bool isError);

    void draw() const override;
    void showModal();

private:
    tstring  m_message;
    tstring  m_detail;
    COLORREF m_accentColor;
    Button   m_okBtn;
    bool     m_shouldClose;
    bool     m_isError;
};
