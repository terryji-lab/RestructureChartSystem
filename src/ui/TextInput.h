#pragma once
#include <easyx.h>
#include <string>
#include "Card.h"
#include "../common/Types.h"

// ==================== 文本输入框控件类 ====================
// 继承 Card，在圆角卡片上叠加文本编辑功能。
// 支持：
//   - 占位符文字（灰色，输入框为空时显示）
//   - 闪烁光标（聚焦时显示当前位置）
//   - 剪贴板操作（Ctrl+C/V/X）
//   - 全选（Ctrl+A）
//   - 方向键/Home/End 导航
//   - Backspace/Delete 删除
//
// 通过 Win32 Clipboard API 实现剪贴板，支持 Unicode。
class TextInput : public Card
{
public:
    TextInput(int x, int y, int w, int h,
              const tstring& placeholder = _T(""), int radius = 6);

    // ── 事件处理（override Widget）──
    void draw() const override;
    bool handleMouseDown(int mx, int my) override;
    void handleKeyDown(WPARAM vkcode) override;
    void handleChar(WPARAM ch) override;
    bool wantsKeyInput() const override { return m_focused; }
    bool wantsCharInput() const override { return m_focused; }

    // ── 文本操作 ──
    void setText(const tstring& text);
    const tstring& getText() const;
    bool isFocused() const;
    void setFocus(bool focus);                  // 设置/取消焦点

private:
    // 剪贴板操作（内部使用 Win32 Clipboard API）
    void copy();
    void paste();
    void cut();

    tstring m_text;                              // 用户输入的文本
    tstring m_placeholder;                       // 占位符文字
    bool m_focused;                              // 是否拥有焦点
    size_t m_cursorPos;                          // 光标位置（字符索引）
};
