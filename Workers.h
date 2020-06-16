
#pragma once

#include <cstdint>
#include "SpriteSet.h"
#include "Sprite.h"
#include "pomi/Graphics.h"
#include "pomi/Sample.h"

#include "images/player1.h"
#include "images/player2.h"
#include "images/player3.h"
#include "images/player4.h"
#include "images/player5.h"
#include "images/player6.h"
#include "images/player7.h"

#include "sounds/climb11k.h"
#include "sounds/lift11k.h"

class Workers {
    
    public:
    
        Workers() : _rhs_sprites(_RHS_SPRITES), _lhs_sprites(_LHS_SPRITES), _climb_snd(0, climb11k), _lift_snd(0, lift11k) { //, _status{1} {
            _rhs_sprites.set(2);
            _lhs_sprites.set(0);
        }
        
        ~Workers() = default;
        
        Workers(const Workers&) = delete;
        Workers& operator =(const Workers&) = delete;
        
        inline std::uint8_t rhsPosition() { return _rhs_status.position; }
        
        inline void setRhsPosition(std::uint8_t pos) {
            _rhs_sprites.reset();
            
            if(pos < 5) {
                _rhs_status.position = pos;
                _rhs_sprites.set(2 * pos + _rhs_status.frame);
            }
        }
        
        inline std::uint8_t lhsPosition() const { return _lhs_status.position; }
        
        inline void setLhsPosition(std::uint8_t pos) {
            _lhs_sprites.reset();
            
            if(pos < 5) {
                _lhs_status.position = pos;
                _lhs_sprites.set(2 * pos + _lhs_status.frame);
            }
        }
        
        inline std::uint8_t rhsFrame() const { return _rhs_status.frame; }
        
        inline void setRhsFrame(std::uint8_t frame) {
            _rhs_sprites.reset();
            
            if(frame < 2) {
                _rhs_status.frame = frame;
                _rhs_sprites.set(2 * _rhs_status.position + frame);
            }
        }
        
        inline std::uint8_t lhsFrame() const { return _lhs_status.frame; }
        
        inline void setLhsFrame(std::uint8_t frame) {
            _lhs_sprites.reset();
            if(frame < 2) {
                _lhs_status.frame = frame;
                _lhs_sprites.set(2 * _lhs_status.position + frame);
            }
        }
        
        void update();
        
        void move(std::int8_t rhs_dir, std::int8_t lhs_dir);
        
        inline void draw(Pomi::Graphics& gfx, const std::uint8_t* background) {
            _rhs_sprites.draw(gfx, background);
            _lhs_sprites.draw(gfx, background);
        }
    
    private:
    
        struct WorkerStatus {
            std::uint8_t position;
            std::uint8_t frame;
            std::uint8_t buffered_move;
            std::uint8_t busy_timer;
        };
        
        static constexpr Sprite _RHS_SPRITES[10] = {
            Sprite(162, 98, player3, false), Sprite(154, 98, player3, true), 
            Sprite(154, 66, player1, true), Sprite(154, 66, player2, true), 
            Sprite(154, 34, player1, true), Sprite(154, 34, player2, true), 
            Sprite(162, 66, player4, false), Sprite(162, 66, player5, false), 
            Sprite(162, 66, player6, false), Sprite(162, 66, player7, false)
        };
        
        static constexpr Sprite _LHS_SPRITES[10] = {
            Sprite(42, 82, player1, false), Sprite(42, 82, player2, false),
            Sprite(42, 50, player1, false), Sprite(42, 50, player2, false),
            Sprite(42, 18, player1, false), Sprite(34, 18, player3, true),
            Sprite(34, 104, player4, true), Sprite(34, 104, player5, true),
            Sprite(34, 82, player6, true), Sprite(34, 82, player7, true)
        };
        
        SpriteSet<10> _rhs_sprites;
        SpriteSet<10> _lhs_sprites;
        
        WorkerStatus _rhs_status;
        WorkerStatus _lhs_status;
        
        Pomi::Sample _climb_snd;
        Pomi::Sample _lift_snd;
};
