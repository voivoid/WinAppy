#pragma once

#include "WinAppy/window.h"

#include <optional>
#include <unordered_map>

#include <windef.h>

namespace winappy
{
class Control;
class Layout
{
  public:
    virtual void update(int new_width, int new_height) = 0;
    virtual std::optional<LRESULT> try_reflect_notification(UINT, WPARAM, LPARAM);

    void set_padding(int padding_in_pixels);
    int get_padding() const;

  protected:
    void add_control(Control& control);
    void remove_control(Control& control);
    void clear_controls();
    const std::unordered_map<HWND, Control*>& get_controls() const
    {
        return m_controls;
    }

  private:
    int m_padding = 0;
    std::unordered_map<HWND, Control*> m_controls;
};

namespace layouts
{

class Fill : public Layout
{
  public:
    void set_control(Control& control);
    void clear_control();

  private:
    void update(int new_width, int new_height) override;
};

class Horizontal : public Layout
{
  public:
    void push_control(Control& control);
    void pop_control(Control& control);
    void pop_control();
    void clear_control();
};

class Vertical : public Layout
{
  public:
    void push_control(Control& control);
    void pop_control(Control& control);
    void pop_control();
    void clear_control();
};

class Splitter : public Layout, private Window
{
};

class HorizontalSplitter : public Splitter
{
  public:
    void set_left_control(Control& control);
    void set_right_control(Control& control);
};

class VerticalSplitter : public Splitter
{
  public:
    void set_top_control(Control& control);
    void set_bottom_control(Control& control);
};

}  // namespace layouts
}  // namespace winappy