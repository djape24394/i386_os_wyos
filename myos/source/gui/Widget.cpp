#include <gui/Widget.h>

using namespace myos::common;
using namespace myos::gui;

Widget::Widget(Widget *parent, int32_t x, int32_t y, int32_t width, int32_t height, uint8_t red, uint8_t green, uint8_t blue)
    : parent{parent}, x{x}, y{y}, width{width}, height{height}, red{red}, green{green}, blue{blue}, focusable{true}
{
}

Widget::~Widget() {}

void Widget::getFocus(Widget *widget)
{
    if (parent != nullptr)
    {
        parent->getFocus(widget);
    }
}
void Widget::modelToScreen(int32_t &x, int32_t &y)
{
    if (parent != nullptr)
    {
        parent->modelToScreen(x, y);
    }
    x += this->x;
    y += this->y;
}

void Widget::draw(GraphicsContext *graphics_context)
{
    int32_t x_apsolute{0};
    int32_t y_apsolute{0};
    modelToScreen(x_apsolute, y_apsolute);

    graphics_context->fillRectangle(x_apsolute, y_apsolute, width, height, red, green, blue);
}

bool Widget::containsCoordinate(int32_t x, int32_t y)
{
    return (this->x <= x) && (x < this->x + width) && 
           (this->y <= y) && (y < this->y + height);
}

void Widget::onMouseDown(int32_t x, int32_t y, uint8_t button)
{
    if(focusable)
    {
        getFocus(this);
    }
}
void Widget::onMouseUp(int32_t x, int32_t y, uint8_t button)
{
}
void Widget::onMouseMove(int32_t old_x, int32_t old_y, int32_t new_x, int32_t new_y)
{
}

CompositeWidget::CompositeWidget(Widget *parent, int32_t x, int32_t y, int32_t width, int32_t height, uint8_t red, uint8_t green, uint8_t blue)
: Widget(parent, x, y, width, height, red, green, blue), focused_child{nullptr}, nof_childrens{0}
{
}

CompositeWidget::~CompositeWidget(){}

void CompositeWidget::draw(GraphicsContext *graphics_context)
{
    Widget::draw(graphics_context);
    for(int i = nof_childrens - 1; i >= 0; i--)
    {
        childrens[i]->draw(graphics_context);
    }
}
void CompositeWidget::getFocus(Widget *widget)
{
    this->focused_child = widget;
    if(parent != nullptr)
    {
        getFocus(this);
    }
}
void CompositeWidget::modelToScreen(int32_t &x, int32_t &y)
{

}

void CompositeWidget::onMouseDown(int32_t x, int32_t y, uint8_t button)
{
    for(int i = 0; i < nof_childrens; i++)
    {
        if(childrens[i]->containsCoordinate(x - this->x, y - this->y))
        {
            childrens[i]->onMouseDown(x - this->x, y - this->y, button);
            break;
        }
    }
}
void CompositeWidget::onMouseUp(int32_t x, int32_t y, uint8_t button)
{
    for(int i = 0; i < nof_childrens; i++)
    {
        if(childrens[i]->containsCoordinate(x - this->x, y - this->y))
        {
            childrens[i]->onMouseUp(x - this->x, y - this->y, button);
            break;
        }
    }
}
void CompositeWidget::onMouseMove(int32_t old_x, int32_t old_y, int32_t new_x, int32_t new_y)
{
    int first_child = -1;
    for(int i = 0; i < nof_childrens; i++)
    {
        if(childrens[i]->containsCoordinate(old_x - this->x, old_y - this->y))
        {
            childrens[i]->onMouseMove(old_x - this->x, old_y - this->y, new_x - this->x, new_y - this->y);
            first_child = i;
            break;
        }
    }

    for(int i = 0; i < nof_childrens; i++)
    {
        if(childrens[i]->containsCoordinate(new_x - this->x, new_y - this->y))
        {
            if(first_child != i) childrens[i]->onMouseMove(old_x - this->x, old_y - this->y, new_x - this->x, new_y - this->y);
            break;
        }
    }
}

void CompositeWidget::onKeyDown(char c)
{
    if(focused_child != nullptr)
    {
        focused_child->onKeyDown(c);
    }
}

void CompositeWidget::onKeyUp(char c)
{
    if(focused_child != nullptr)
    {
        focused_child->onKeyUp(c);
    }
}

bool CompositeWidget::addChild(Widget* widget)
{

    if(nof_childrens < max_nof_childrens)
    {
        childrens[nof_childrens++] = widget;
        return true;
    }
    return false;
}
