
#pragma once

#include <cstdint>
#include "pomi/Graphics.h"

class Sprite {
    
    public:
    
        constexpr Sprite(std::int32_t x, std::int32_t y, const std::uint8_t* bitmap, bool flip_x = false, bool flip_y = false) : _x(x), _y(y), _bitmap(bitmap), _flip_x(flip_x), _flip_y(flip_y) {}
        
        ~Sprite() = default;
        
        Sprite(const Sprite&) = delete;
        Sprite& operator =(const Sprite&) = delete;
        
        constexpr std::int32_t x() const { return _x; }
        constexpr std::int32_t y() const { return _y; }
        constexpr std::int32_t width() const { return _bitmap[0]; }
        constexpr std::int32_t height() const { return _bitmap[1]; }
        
        inline void draw(Pomi::Graphics& gfx) const {
            if(_bitmap != nullptr) {
                gfx.drawImage(_bitmap, _x, _y, _flip_x, _flip_y);
            }
        }
        
    private:
    
        const std::int32_t _x;
        const std::int32_t _y;
        
        const std::uint8_t* _bitmap;
        
        const bool _flip_x;
        const bool _flip_y;
};
