
#pragma once

#include <cstdint>
#include <cstring>
#include "pomi/Graphics.h"
#include "Pomifactory.h"

template<unsigned Size>
class MenuBox {
    
    public:
    
        constexpr MenuBox(char* const title, const char* const labels[Size], std::int32_t x, std::int32_t y, std::int32_t width):
            _title(title), _labels(labels), _selected_idx(-1), _highlighted_idx(-1), _x(x), _y(y), _width(width)
        {}
        
        ~MenuBox() = default;
        
        MenuBox(const MenuBox&) = delete;
        MenuBox& operator =(const MenuBox&) = delete;
        
        std::uint8_t selected() const { return _selected_idx; }
        void setSelected(std::uint8_t idx) {
            if(_selected_idx != idx) {
                _selected_idx = idx;
                _dirty = true;
            }
        }
        
        std::uint8_t highlighted() const { return _highlighted_idx; }
        void setHighlighted(std::uint8_t idx) {
            if(_highlighted_idx != idx) {
                _highlighted_idx = idx;
                _dirty = true;
            }
        }
        
        inline void show(Pomi::Graphics& gfx) {
            std::uint32_t h = 0 + 11 * Size;
            
            gfx.drawFilledRectangle(_x, _y + 1, _width, h + 14 + 4, 2);
            gfx.drawHLine(_x + 1, _y, _width - 2, 2);
            gfx.drawHLine(_x + 1, _y + h + 15 + 4, _width - 2, 2);
            
            gfx.drawRectangle(_x + 2, _y + 13, _width - 4, h + 4, 9);
            
            gfx.drawFilledRectangle(_x + 4, _y + 15, _width - 8, h, 12);
            
            gfx.print(_title, _x + (_width - 7 * strlen(_title) + 1) / 2, _y + 2, 14);
            
            _dirty = true;
            draw(gfx);
        }
        
        inline void hide(Pomi::Graphics& gfx) const {
            std::uint32_t h = 16 + 4 + 11 * Size;
            gfx.drawImage(Pomifactory::backgroundImage(), _x, _y, _width, h, _x, _y);
        }
        
        inline void draw(Pomi::Graphics& gfx) {
            if(!_dirty) return;
            
            for(std::uint32_t idx = 0; idx < Size; ++idx) {
                std::uint8_t bgcol = (idx == _highlighted_idx) ? 3 : 12;
                std::uint8_t fgcol = (idx == _highlighted_idx) ? 12 : 3;
                
                std::int32_t label_y = _y + 16 + 11 * idx; // 89 - 11 * Size / 2 + 11 * idx;
                
                gfx.drawFilledRectangle(_x + 4, label_y - 1, _width - 8, 11, bgcol);
                if(idx == _selected_idx) {
                    gfx.print(">", _x + 5, label_y, fgcol);
                    gfx.print(_labels[idx], _x + 12, label_y, fgcol);
                    gfx.print("<", fgcol);
                }
                else {
                    gfx.print(_labels[idx], _x + 12, label_y, fgcol);
                }
            }
            
            _dirty = false;
        }
    
    private:
    
        char* const _title;
        const char* const* _labels;
        
        const std::uint32_t _x;
        const std::uint32_t _y;
        const std::uint32_t _width;
        
        bool _dirty;
        std::uint8_t _selected_idx;
        std::uint8_t _highlighted_idx;
};
