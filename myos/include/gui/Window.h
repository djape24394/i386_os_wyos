#ifndef MYOS_GUI_WINDOW_H
#define MYOS_GUI_WINDOW_H

#include <gui/Widget.h>
#include <drivers/MouseDriver.h>
#include <common/types.h>

namespace myos::gui
{
    class Window : public CompositeWidget
    {
    protected:
        bool dragged;

    public:
        Window(Widget *parent, common::int32_t x, common::int32_t y, common::int32_t width, common::int32_t height, common::uint8_t red, common::uint8_t green, common::uint8_t blue);
        ~Window();

        void onMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button);
        void onMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button);
        void onMouseMove(common::int32_t old_x, common::int32_t old_y, common::int32_t new_x, common::int32_t new_y);
    };
} // namespace myos::gui

#endif // MYOS_GUI_WINDOW_H
