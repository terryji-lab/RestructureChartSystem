#pragma once
#include <easyx.h>

// Widget 基类，所有 UI 控件的公共抽象
// Card 和 Button 都继承自此，统一位置、绘制、事件接口
// Page 通过 vector<Widget*> 统一管理
class Widget
{
protected:
    int m_x, m_y, m_w, m_h;

public:
    Widget(int x, int y, int w, int h) : m_x(x), m_y(y), m_w(w), m_h(h) {}
    virtual ~Widget() = default;

    // 几何
    int x()      const { return m_x; }
    int y()      const { return m_y; }
    int width()  const { return m_w; }
    int height() const { return m_h; }

    virtual bool contains(int mx, int my) const
    {
        return mx >= m_x && mx <= m_x + m_w
            && my >= m_y && my <= m_y + m_h;
    }

    // 绘制
    virtual void draw() const = 0;

    // 事件（默认空实现，子类按需覆盖）
    virtual bool handleMouseMove(int mx, int my) { return false; }
    virtual bool handleMouseDown(int mx, int my) { return false; }
    virtual bool handleMouseUp(int mx, int my)   { return false; }
    virtual void handleKeyDown(WPARAM vkcode)    {}
    virtual void handleChar(WPARAM ch)           {}
    virtual bool wantsKeyInput() const           { return false; }
    virtual bool wantsCharInput() const          { return false; }
};
