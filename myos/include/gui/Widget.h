#ifndef MYOS_GUI_WIDGET_H
#define MYOS_GUI_WIDGET_H

#include <common/types.h>
#include <common/GraphicsContext.h>
#include <drivers/KeyboardDriver.h>

namespace myos::gui
{
    class Widget : public drivers::KeyboardEventHandler
    {
    protected:
        Widget *parent;
        common::int32_t x;
        common::int32_t y;
        common::int32_t width;
        common::int32_t height;

        // color
        common::uint8_t red;
        common::uint8_t green;
        common::uint8_t blue;

        bool focusable;

    public:
        Widget(Widget *parent, common::int32_t x, common::int32_t y, common::int32_t width, common::int32_t height,
               common::uint8_t red, common::uint8_t green, common::uint8_t blue);
        ~Widget();

        virtual void draw(common::GraphicsContext *graphics_context);
        virtual void getFocus(Widget *widget);
        virtual void modelToScreen(common::int32_t &x, common::int32_t &y);

        virtual void onMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button);
        virtual void onMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button);
        virtual void onMouseMove(common::int32_t old_x, common::int32_t old_y, common::int32_t new_x, common::int32_t new_y);

        virtual bool containsCoordinate(common::int32_t x, common::int32_t y);
    };

    class CompositeWidget : public Widget
    {
        static constexpr int max_nof_childrens = 100;
        Widget *childrens[max_nof_childrens];
        int nof_childrens;
        Widget *focused_child;

    public:
        CompositeWidget(Widget *parent, common::int32_t x, common::int32_t y, common::int32_t width, common::int32_t height, common::uint8_t red, common::uint8_t green, common::uint8_t blue);
        ~CompositeWidget();

        virtual bool addChild(Widget *widget);

        virtual void draw(common::GraphicsContext *graphics_context) override;
        virtual void getFocus(Widget *widget) override;
        virtual void modelToScreen(common::int32_t &x, common::int32_t &y) override;

        virtual void onMouseDown(common::int32_t x, common::int32_t y, common::uint8_t button) override;
        virtual void onMouseUp(common::int32_t x, common::int32_t y, common::uint8_t button) override;
        virtual void onMouseMove(common::int32_t old_x, common::int32_t old_y, common::int32_t new_x, common::int32_t new_y) override;

        virtual void onKeyDown(char c);
        virtual void onKeyUp(char c);
    };
} // namespace myos::gui

#endif // MYOS_GUI_WIDGET_H
