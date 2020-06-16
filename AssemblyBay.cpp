
#include <cstdint>
#include "AssemblyBay.h"
#include "Pomifactory.h"
#include "pomi/Graphics.h"

void AssemblyBay::reset() {
    _random.setSeed(Pomi::System::currentTimeMillis());
    
    _feed_line.reset();
    _asm_lines.reset();
}
bool AssemblyBay::update(std::uint32_t steps) {
    if(_feed_line.overflow()) {
        // Feed line has overflowed, so blink the last sprite
        _feed_line._sprites.set(_feed_line._sprites.size() - 1, !_feed_line._sprites.test(_feed_line._sprites.size() - 1));
    }
    else if(_asm_lines.overflow()) {
        constexpr std::size_t last = _asm_lines.output_sprites[0].size() - 1;
        for(std::size_t idx = 0; idx < NUM_ASM_LINES; ++idx) {
            if(_asm_lines.overflow(idx)) {
                // Found the overflowed assembly line - blink the last sprite
                _asm_lines.output_sprites[idx].set(last, !_asm_lines.output_sprites[idx].test(last));
                break;
            }
        }
    }
    else {
        // Set feed line overflow flag if there is item at the last position
        if((steps & 1) == 0 && _feed_line.peekItem()) {
            _feed_line.setOverflow();
        }
        
        for(std::size_t idx = 0; idx < NUM_ASM_LINES; ++idx) {
            // Set assembly line overflow flag if there is item at the last position
            if((steps & 1) != (idx & 1) && _asm_lines.peekItem(idx)) {
                _asm_lines.setOverflow(idx);
            }
        }
        
        if(!_feed_line.overflow() && !_asm_lines.overflow()) {
            bool ret_val = false;
            
            if((steps & 1) == 0) {
                ret_val |= _feed_line.advance();
            }
            
            for(std::size_t idx = 0; idx < NUM_ASM_LINES; ++idx) {
                if((steps & 1) != (idx & 1)) {
                    ret_val |= _asm_lines.advance(idx);
                }
                else {
                    _asm_lines.input_sprites[idx].set(0, _asm_lines.items_pending.test(idx));
                    _asm_lines.items_pending.reset(idx);
                }
            }
            
            if((steps % 6) == 0) {
                std::uint32_t num_items = _feed_line.itemCount() + _asm_lines.itemCount();
                std::uint32_t items_max = (Pomifactory::gameInfo().mode() == GameInfo::Mode::B) ? 3 : 2;
                items_max += (100 + steps) / 200;
                if(_random.nextInt(100) < ((num_items == 0) ? 71 : ((1829 * (items_max - num_items)) >> 8))) {
                    ret_val = true;
                    _feed_line.putItem(true);
                }
            }
            
            if(ret_val) {
                _conveyor_snd.play();
            }
            
            return ret_val;
        }
        else {
            _miss_snd.play();
        }
    }
    
    return false;
}

void AssemblyBay::draw(Pomi::Graphics& gfx, const std::uint8_t* background) {
    _feed_line.draw(gfx, background);
    _asm_lines.draw(gfx, background);
}
