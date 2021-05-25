#include <drivers/VideoGraphicsArray.h>

using namespace myos::drivers;
using namespace myos::common;
using namespace myos::hardware_communication;

void VideoGraphicsArray::writeRegisters(uint8_t *registers)
{
    // misc
    misc_port.write(*(registers++));

    // sequancer
    for(uint8_t i = 0; i < (uint8_t)5U; i++)
    {
        sequencer_index_port.write(i);
        sequencer_data_port.write(*(registers++));
    }

    // crtc
    // we need to lock it, send data and unlock it again
    crtc_index_port.write(0x03U);
    crtc_data_port.write(crtc_data_port.read() | 0x80U);
    crtc_index_port.write(0x11U);
    crtc_data_port.write(crtc_data_port.read() & (~0x80U));

    registers[0x03U] = registers[0x03U] | 0x80U;
    registers[0x11U] = registers[0x11U] & (~0x80U);
    for(uint8_t i = 0; i < (uint8_t)25U; i++)
    {
        crtc_index_port.write(i);
        crtc_data_port.write(*(registers++));
    }

    // crtc
    for(uint8_t i = 0; i < (uint8_t)9U; i++)
    {
        graphics_controller_index_port.write(i);
        graphics_controller_data_port.write(*(registers++));
    }

    // attribute controler
    for(uint8_t i = 0; i < (uint8_t)21U; i++)
    {
        (void)attribute_controller_reset_port.read();
        attribute_controller_index_port.write(i);
        attribute_controller_write_port.write(*(registers++));
    }

    (void)attribute_controller_reset_port.read();
    attribute_controller_index_port.write(0x20U);
}

uint8_t* VideoGraphicsArray::getFrameBufferAddressSegment()
{
    graphics_controller_index_port.write(0x06U);
    uint8_t segment_number = ((graphics_controller_data_port.read() >> 2) & 0x3U);
    switch (segment_number)
    {
    case 0U: return (uint8_t*) 0x00000U; break;
    case 1U: return (uint8_t*) 0xA0000U; break;
    case 2U: return (uint8_t*) 0xB0000U; break;
    case 3U: return (uint8_t*) 0xB8000U; break;
    default:
        // just for compiler
        return (uint8_t*) 0x0U;
        break;
    }
}

void VideoGraphicsArray::putPixel(int32_t x, int32_t y, uint8_t color_index)
{
    if(x < 0 || x >= 320 || y < 0 || y >= 200) return;
    uint8_t *pixel_address = getFrameBufferAddressSegment() + 320 * y + x;
    *pixel_address = color_index;
}

uint8_t VideoGraphicsArray::getColorIndex(uint8_t red, uint8_t green, uint8_t blue)
{
    if(red == 0x00U && green == 0x00U && blue == 0x00U) return 0x00U; // black
    if(red == 0x00U && green == 0x00U && blue == 0xA8U) return 0x01U; // blue
    if(red == 0x00U && green == 0xA8U && blue == 0x00U) return 0x02U; // green
    if(red == 0xA8U && green == 0x00U && blue == 0x00U) return 0x04U; // red
    if(red == 0xFFU && green == 0xFFU && blue == 0xFFU) return 0x3FU; // white
    return 0x00U;
}

VideoGraphicsArray::VideoGraphicsArray()
    : misc_port(0x3C2U),       // miscellaneous
      crtc_index_port(0x3D4U), // Cathode Ray Tube Controller
      crtc_data_port(0x3D5U),
      sequencer_index_port(0x3C4U),
      sequencer_data_port(0x3C5U),
      graphics_controller_index_port(0x3CEU),
      graphics_controller_data_port(0x3CFU),
      attribute_controller_index_port(0x3C0U),
      attribute_controller_read_port(0x3C1U),
      attribute_controller_write_port(0x3C0U),
      attribute_controller_reset_port(0x3DAU)
{
}

VideoGraphicsArray::~VideoGraphicsArray() {}

bool VideoGraphicsArray::isModeSupported(uint32_t width, uint32_t height, uint32_t color_depth)
{
    return (width == 320U) && (height == 200U) && (color_depth == 8U);
}
bool VideoGraphicsArray::setMode(uint32_t width, uint32_t height, uint32_t color_depth)
{

    if(!isModeSupported(width, height, color_depth))
    {
        return false;
    }

    unsigned char g_320x200x256[] =
    {
        /* MISC */
            0x63,
        /* SEQ */
            0x03, 0x01, 0x0F, 0x00, 0x0E,
        /* CRTC */
            0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
            0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
            0xFF,
        /* GC */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
            0xFF,
        /* AC */
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x41, 0x00, 0x0F, 0x00, 0x00
    };
    writeRegisters(g_320x200x256);
    return true;
}

void VideoGraphicsArray::putPixel(int32_t x, int32_t y, uint8_t red, uint8_t green, uint8_t blue)
{
    putPixel(x, y, getColorIndex(red, green, blue));
}

void VideoGraphicsArray::fillRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t red, uint8_t green, uint8_t blue)
{
    for(int32_t i = y; i < y + height; i++)
    {
        for(int32_t j = x; j < x + width; j++)
        {
            putPixel(j, i, red, green, blue);
        }
    }
}
