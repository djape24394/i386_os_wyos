#ifndef MYOS_GUI_DESKTOP_H
#define MYOS_GUI_DESKTOP_H

#include <gui/Widget.h>
#include <drivers/MouseDriver.h>

namespace myos
{
    namespace gui
    {
        class Desktop: public CompositeWidget, public drivers::MouseEventHandler
        {
        protected:
            common::int32_t mouse_x;
            common::int32_t mouse_y;
        public:
            Desktop(common::int32_t width, common::int32_t height, common::uint8_t red, common::uint8_t green, common::uint8_t blue);
            ~Desktop();

            virtual void draw(common::GraphicsContext *graphics_context) override;

            virtual void onMouseDown(myos::common::uint8_t button) override;
            virtual void onMouseUp(myos::common::uint8_t button) override;
            virtual void onMouseMove(int delta_x, int delta_y) override;
        };
    } // namespace gui
} // namespace myos

#endif // MYOS_GUI_DESKTOP_H
