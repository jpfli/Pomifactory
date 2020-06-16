
#pragma once

#include <cstdint>
#include "Sprite.h"
#include "SpriteSet.h"
#include "StringBuffer.h"
#include "pomi/Graphics.h"

#include "images/game_a_text.h"
#include "images/game_b_text.h"
#include "images/miss_text.h"
#include "images/miss.h"

class GameInfo {
    
    public:
    
        enum class Mode: std::uint8_t { A, B };
        
        GameInfo(std::int32_t x, std::int32_t y): _sprites(_SPRITES), _score_x(x), _score_y(y), _score(0), _prev_score(-1) {}
        
        inline Mode mode() { return _sprites.test(1) ? Mode::B : Mode::A; }
        
        inline void setMode(Mode mode) {
            if(mode == Mode::A) {
                _sprites.set(0);
                _sprites.reset(1);
            }
            else {
                _sprites.reset(0);
                _sprites.set(1);
            }
        }
        
        inline std::uint32_t score() { return _score; }
        
        inline void increaseScore() { ++_score; }
        
        inline void clearScore() { _score = 0; }
        
        inline std::uint32_t numMisses() {
            std::uint32_t idx = 3;
            while(_sprites.test(idx)) {
                ++idx;
                if(idx > 5) break;
            }
            return idx - 3;
        }
        
        inline void increaseMisses() {
            _sprites.set(2); // Make sure the miss-text sprite is shown
            
            std::uint32_t idx = 3;
            while(_sprites.test(idx)) {
                ++idx;
                if(idx == 5) break;
            }
            _sprites.set(idx);
        }
        
        inline void decreaseMisses() {
            std::uint32_t idx = 5;
            while(!_sprites.test(idx)) {
                --idx;
                if(idx == 3) {
                    _sprites.reset(2);   // Hide the "miss" sprite
                    break;
                }
            }
            _sprites.reset(idx);
        }
        
        inline void clearMisses() {
            for(std::uint32_t idx = 2; idx < 6; ++idx) {
                _sprites.reset(idx);
            }
        }
        
        inline void draw(Pomi::Graphics& gfx, const std::uint8_t* background) {
            _sprites.draw(gfx, background);
            
            StringBuffer<8> text;
            text.append(_score);
            
            if(_prev_score != _score) {
                _prev_score = _score;
                gfx.drawImage(background, _score_x, _score_y, 7 * 7, 9, _score_x, _score_y);
            }
            gfx.print(text, _score_x + 7 * (7 - text.length()), _score_y, 3);
        }
    
    private:
    
        static constexpr Sprite _SPRITES[6] = { Sprite(79, 18, game_a_text, false), Sprite(79, 18, game_b_text, false), Sprite(116, 18, miss_text, false), Sprite(134, 17, miss, false), Sprite(144, 17, miss, false), Sprite(154, 17, miss, false) };
        
        SpriteSet<6> _sprites;
        
        const std::int16_t _score_x;
        const std::int16_t _score_y;
        
        std::uint32_t _score;
        std::uint32_t _prev_score;
};
