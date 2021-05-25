#include <gui/Desktop.h>

using namespace myos::common;
using namespace myos::gui;

Desktop::Desktop(int32_t width, int32_t height, uint8_t red, uint8_t green, uint8_t blue)
: CompositeWidget(nullptr, 0, 0, width, height, red, green, blue), MouseEventHandler()
{
    mouse_x = width / 2;
    mouse_y = height / 2;
}

Desktop::~Desktop()
{
}

void Desktop::draw(GraphicsContext *graphics_context)
{
    CompositeWidget::draw(graphics_context);

    for(int i = 0; i < 4; i++)
    {
        graphics_context->putPixel(mouse_x + i, mouse_y, 0xFFU, 0xFFU, 0xFFU);
        graphics_context->putPixel(mouse_x - i, mouse_y, 0xFFU, 0xFFU, 0xFFU);
        graphics_context->putPixel(mouse_x, mouse_y + i, 0xFFU, 0xFFU, 0xFFU);
        graphics_context->putPixel(mouse_x, mouse_y - i, 0xFFU, 0xFFU, 0xFFU);
    }
}

void Desktop::onMouseDown(uint8_t button)
{
    CompositeWidget::onMouseDown(mouse_x, mouse_y, button);
}

void Desktop::onMouseUp(uint8_t button)
{
    CompositeWidget::onMouseUp(mouse_x, mouse_y, button);
}
void Desktop::onMouseMove(int delta_x, int delta_y)
{
    // we will divide this movement by 4 to make mouse cursos move slover
    delta_x /= 4;
    delta_y /= 4;

    int32_t new_mouse_x = mouse_x + delta_x;
    if(new_mouse_x < 0) new_mouse_x = 0;
    if(new_mouse_x >= width) new_mouse_x = width - 1;

    int32_t new_mouse_y = mouse_y + delta_y;
    if(new_mouse_y < 0) new_mouse_y = 0;
    if(new_mouse_y >= height) new_mouse_y = height - 1;

    CompositeWidget::onMouseMove(mouse_x, mouse_y, new_mouse_x, new_mouse_y);

    mouse_x = new_mouse_x;
    mouse_y = new_mouse_y;
}
