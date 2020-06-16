
#pragma once

#include <cstdint>
#include "pomi/Graphics.h"
#include "SpriteSet.h"

#include "images/item6.h"
#include "images/truck1_img.h"
#include "images/truck2_img.h"
#include "images/truck3_img.h"

class Truck {
    
    public:
    
        constexpr Truck() : _rhs_cargo_sprites(_RHS_CARGO_SPRITES), _lhs_cargo_sprites(_LHS_CARGO_SPRITES), _truck_sprites(_TRUCK_SPRITES) {
            _truck_sprites.set(0);
        }
        
        ~Truck() = default;
        
        Truck(const Truck&) = delete;
        Truck& operator =(const Truck&) = delete;
        
        bool update() {
            // Iterate through left hand side cargo items and drop each item one position down if there is empty position beneath
            for(std::size_t pos = _lhs_cargo_sprites.size() - 1; pos > 0; --pos) {
                if(!_lhs_cargo_sprites.test(pos)) {
                    _lhs_cargo_sprites.set(pos, _lhs_cargo_sprites.test(pos - 1));
                    _lhs_cargo_sprites.reset(pos - 1);
                }
            }
            
            // Top most item on right hand side jumps over to left side if there are more items on right hand side
            if(_rhs_cargo_sprites.test(0) && _rhs_cargo_sprites.count() > _lhs_cargo_sprites.count()) {
                _lhs_cargo_sprites.set(0);
                _rhs_cargo_sprites.reset(0);
            }
            
            // Iterate through right hand side cargo items and drop each item one position down if there is empty position beneath
            for(std::size_t pos = _rhs_cargo_sprites.size() - 1; pos > 0; --pos) {
                if(!_rhs_cargo_sprites.test(pos)) {
                    _rhs_cargo_sprites.set(pos, _rhs_cargo_sprites.test(pos - 1));
                    _rhs_cargo_sprites.reset(pos - 1);
                }
            }
            
            return true;
        }
        
        bool full() const {
            std::size_t count = 0;
            count += _rhs_cargo_sprites.count() + _lhs_cargo_sprites.count();
            return (count >= 8);
        }
        
        void putItem(bool item=true) {
            _rhs_cargo_sprites.set(0, item);
        }
        
        void resetCargo() {
            _rhs_cargo_sprites.reset();
            _lhs_cargo_sprites.reset();
        }
        
        std::size_t frame() const {
            std::size_t frame = 0;
            while(frame < _truck_sprites.size() && !_truck_sprites.test(frame)) {
                ++frame;
            }
            return frame;
        }
        
        void setFrame(std::size_t frame) {
            _truck_sprites.reset();
            if(frame < 3) {
                _truck_sprites.set(frame);
            }
            else {
                _truck_sprites.reset();
            }
        }
        
        void draw(Pomi::Graphics& gfx, const std::uint8_t* background) {
            _rhs_cargo_sprites.draw(gfx, background);
            _lhs_cargo_sprites.draw(gfx, background);
            _truck_sprites.draw(gfx, background);
        }
    
    private:
    
        static constexpr Sprite _RHS_CARGO_SPRITES[] = { Sprite(24, 21, item6), Sprite(22, 33, item6), Sprite(22, 43, item6), Sprite(22, 53, item6), Sprite(22, 63, item6) };
        static constexpr Sprite _LHS_CARGO_SPRITES[] = { Sprite(14, 23, item6), Sprite(12, 33, item6), Sprite(12, 43, item6), Sprite(12, 53, item6), Sprite(12, 63, item6) };
        static constexpr Sprite _TRUCK_SPRITES[] = { Sprite(2, 58, truck1_img), Sprite(2, 58, truck2_img), Sprite(2, 58, truck3_img) };
        
        SpriteSet<5> _rhs_cargo_sprites;
        SpriteSet<5> _lhs_cargo_sprites;
        
        SpriteSet<3> _truck_sprites;
};
