#pragma once
#include <easyx.h>
#include <string>
#include "../utils/RenderUtils.h"
#include "Button.h"

typedef std::basic_string<TCHAR> tstring;

// ==================== Card 基类 —— 圆角背景框 ====================
class Card
{
protected:
    int m_x, m_y, m_w, m_h, m_radius;
    tstring m_title;
    COLORREF m_shadowColor;
    COLORREF m_bgColor;
    COLORREF m_titleColor;

public:
    Card(int x, int y, int w, int h,
         const tstring& title = _T(""), int radius = 10);
    virtual ~Card() = default;

    virtual void draw() const;
    bool contains(int mx, int my) const;

    // 输入相关（默认空操作，TextInput 覆盖）
    virtual bool handleMouseDown(int mx, int my) { return false; }
    virtual void handleKeyDown(WPARAM vkcode)    {}
    virtual void handleChar(WPARAM ch)           {}
    virtual bool wantsKeyInput() const           { return false; }
    virtual bool wantsCharInput() const          { return false; }

    void setColors(COLORREF shadow, COLORREF bg, COLORREF title);
    void setTitle(const tstring& t);

    int x()      const { return m_x; }
    int y()      const { return m_y; }
    int width()  const { return m_w; }
    int height() const { return m_h; }
};

// ==================== DisplayBox —— 只读文本展示框 ====================
class DisplayBox : public Card
{
private:
    tstring m_text;

public:
    DisplayBox(int x, int y, int w, int h,
               const tstring& title = _T(""), int radius = 6);

    void setText(const tstring& t);
    void draw() const override;
};

// ==================== PopupCard —— 模态弹窗 ====================
// 继承 Card，封装弹窗的绘制与阻塞交互逻辑。
// 复用 Button 作为确认按钮，支持 hover 高亮。
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
    tstring m_message;
    tstring m_detail;
    COLORREF m_accentColor;
    Button m_okBtn;
    bool m_shouldClose;
    bool m_isError;
};
