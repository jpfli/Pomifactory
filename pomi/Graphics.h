
#pragma once

#include <cstdint>
#include "hw/Display.h"

namespace Pomi {

class Graphics {
    
    public:
    
        static constexpr unsigned int PALETTE_SIZE = 16;
        
        constexpr Graphics(const std::uint16_t* palette_rgb565, const unsigned char* font)
        : _palette(palette_rgb565), _transp_color(0), _clip_x(0), _clip_y(0), _clip_w(width()), _clip_h(height()), _font(font), _cursor_x(0), _cursor_y(0)
        {}

        ~Graphics() = default;
        
        Graphics(const Graphics&) = delete;
        Graphics& operator =(const Graphics&) = delete;
        
        static constexpr std::uint32_t width() { return 220; }
        static constexpr std::uint32_t height() { return 176; }
        
        void setPalette(const std::uint16_t (&palette)[PALETTE_SIZE]);
        void setTransparentColor(std::uint8_t color);
        void setFont(const unsigned char* font);
        
        void setClippingRegion();
        void setClippingRegion(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h);
        
        void clear(std::uint8_t color);
        
        void drawPixel(std::int32_t x, std::int32_t y, std::uint8_t color);
        
        void drawHLine(std::int32_t x, std::int32_t y, std::int32_t w, std::uint8_t color);
        void drawVLine(std::int32_t x, std::int32_t y, std::int32_t h, std::uint8_t color);
        
        void drawRectangle(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, std::uint8_t color);
        void drawFilledRectangle(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, std::uint8_t color);
        
        inline void drawImage(const std::uint8_t* bitmap, std::int32_t x, std::int32_t y, bool flip_x = false, bool flip_y = false) {
            drawImage(bitmap, 0, 0, bitmap[0], bitmap[1], x, y, flip_x, flip_y);
        }
        void drawImage(const std::uint8_t* bitmap, std::int32_t x_src, std::int32_t y_src, std::int32_t w, std::int32_t h, std::int32_t x_dst, std::int32_t y_dst, bool flip_x = false, bool flip_y = false);
        
        inline void drawImage1BPP(const std::uint8_t* bitmap, std::uint8_t color, std::int32_t x, std::int32_t y, bool flip_x = false, bool flip_y = false) {
            drawImage1BPP(bitmap, color, 0, 0, bitmap[0], bitmap[1], x, y, flip_x, flip_y);
        }
        void drawImage1BPP(const std::uint8_t* bitmap, std::uint8_t color, std::int32_t x_src, std::int32_t y_src, std::int32_t w, std::int32_t h, std::int32_t x_dst, std::int32_t y_dst, bool flip_x = false, bool flip_y = false);
        
        void print(const char* text, std::uint8_t color);
        void print(const char* text, std::int32_t x, std::int32_t y, std::uint8_t color);
        
        static constexpr std::uint16_t toRGB565(std::uint8_t red, std::uint8_t green, std::uint8_t blue) {
            return ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
        }
    
    private:
    
        void _renderText(const char* text, std::uint8_t color);
        std::uint8_t _renderChar(char c, std::int32_t x, std::int32_t y, std::uint8_t color);
        
        const std::uint16_t* _palette;
        
        std::uint8_t _transp_color;
        std::int32_t _clip_x;
        std::int32_t _clip_y;
        std::int32_t _clip_w;
        std::int32_t _clip_h;
        
        const unsigned char* _font;
        std::int32_t _cursor_x;
        std::int32_t _cursor_y;
};

} // namespace Pomi
