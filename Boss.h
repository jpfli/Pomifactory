
#pragma once

#include <cstdint>
#include "pomi/Graphics.h"
#include "SpriteSet.h"

#include "images/boss1.h"
#include "images/boss2.h"

class Boss {
    
    public:
    
        enum class Position : std::uint8_t { None=0, Right=1, Left=2, Both=3 };
        
        constexpr Boss() : _sprites(_BOSS_SPRITES) {}
        
        ~Boss() = default;
        
        Boss(const Boss&) = delete;
        Boss& operator =(const Boss&) = delete;
        
        void setPosition(Position pos) {
            _sprites.reset();
            _sprites.set(0, (static_cast<std::uint8_t>(pos) & 1) != 0);
            _sprites.set(2, (static_cast<std::uint8_t>(pos) & 2) != 0);
        }
        
        Position position() const {
            return static_cast<Position>(_sprites.test(0) | _sprites.test(1) | ((_sprites.test(2) | _sprites.test(3)) << 1));
        }
        
        std::size_t frame() {
            return (_sprites.test(1) || _sprites.test(3)) ? 1 : 0;
        }
        
        void setFrame(std::size_t frame_idx) {
            if(frame_idx > 1) frame_idx = 1;
            
            if(frame() != frame_idx) {
                if(frame_idx > 0) {
                    _sprites <<= 1;
                }
                else {
                    _sprites >>= 1;
                }
            }
        }
        
        void draw(Pomi::Graphics& gfx, const std::uint8_t* background) {
            _sprites.draw(gfx, background);
        }
    
    private:
    
        static constexpr Sprite _BOSS_SPRITES[] = {
            Sprite(194, 66, boss1, true), Sprite(194, 66, boss2, true), Sprite(2, 104, boss1), Sprite(2, 104, boss2)
        };
        
        SpriteSet<4> _sprites;
};
