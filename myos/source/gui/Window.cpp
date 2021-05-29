#include <gui/Window.h>

using namespace myos::common;
using namespace myos::gui;

Window::Window(Widget *parent, int32_t x, int32_t y, int32_t width, int32_t height, uint8_t red, uint8_t green, uint8_t blue)
 :CompositeWidget(parent, x, y, width, height, red, green, blue), dragged{false}
{

}

Window::~Window()
{

}

void Window::onMouseDown(int32_t x, int32_t y, uint8_t button)
{
    dragged = (button == 1U);
    CompositeWidget::onMouseDown(x, y, button);
}

void Window::onMouseUp(int32_t x, int32_t y, uint8_t button)
{
    dragged = false;
    CompositeWidget::onMouseUp(x, y, button);
}

void Window::onMouseMove(int32_t old_x, int32_t old_y, int32_t new_x, int32_t new_y)
{
    if(dragged)
    {
        this->x += new_x - old_x;
        this->y += new_y - old_y;        
    }
    CompositeWidget::onMouseMove(old_x, old_y, new_x, new_y);
}
