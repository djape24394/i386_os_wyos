#ifndef MYOS_DRIVERS_VIDEOGRAPHICSARRAY_H
#define MYOS_DRIVERS_VIDEOGRAPHICSARRAY_H

#include <common/types.h>
#include <hardware_communication/Port.h>
#include <drivers/Driver.h>

namespace myos
{
    namespace drivers
    {
        class VideoGraphicsArray
        {
        protected:
            hardware_communication::Port8bit misc_port;       // miscellaneous
            hardware_communication::Port8bit crtc_index_port; // Cathode Ray Tube Controller
            hardware_communication::Port8bit crtc_data_port;
            hardware_communication::Port8bit sequencer_index_port;
            hardware_communication::Port8bit sequencer_data_port;
            hardware_communication::Port8bit graphics_controller_index_port;
            hardware_communication::Port8bit graphics_controller_data_port;
            hardware_communication::Port8bit attribute_controller_index_port;
            hardware_communication::Port8bit attribute_controller_read_port;
            hardware_communication::Port8bit attribute_controller_write_port;
            hardware_communication::Port8bit attribute_controller_reset_port;

            void writeRegisters(common::uint8_t *registers);
            virtual common::uint8_t *getFrameBufferAddressSegment();
            virtual void putPixel(common::uint32_t x, common::uint32_t y, common::uint8_t color_index);
            virtual common::uint8_t getColorIndex(common::uint8_t red, common::uint8_t green, common::uint8_t blue);

        public:
            VideoGraphicsArray();
            ~VideoGraphicsArray();

            virtual bool isModeSupported(common::uint32_t width, common::uint32_t height, common::uint32_t color_depth);
            virtual bool setMode(common::uint32_t width, common::uint32_t height, common::uint32_t color_depth);
            virtual void putPixel(common::uint32_t x, common::uint32_t y, common::uint8_t red, common::uint8_t green, common::uint8_t blue);
        };
    } // namespace drivers
} // namespace myos

#endif
