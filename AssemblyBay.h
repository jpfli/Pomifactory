
#pragma once

#include <cstdint>
#include "pomi/Graphics.h"
#include "pomi/Sample.h"
#include "SpriteSet.h"
#include "BitSet.h"

#include "pomi/System.h"
#include "Random.h"

#include "images/item1.h"
#include "images/item2.h"
#include "images/item3.h"
#include "images/item4.h"
#include "images/item5.h"
#include "images/item6.h"

#include "sounds/conveyor11k.h"
#include "sounds/miss11k.h"

class AssemblyBay {
    
    public:
    
        static constexpr std::size_t NUM_ASM_LINES = 5;
        
        struct FeedLine {
            inline void putItem(bool item=true) { _sprites.set(0, item); }
            
            inline bool peekItem() const { return _sprites.test(_sprites.size() - 1) && !_overflow; }
            
            bool pickItem() {
                bool item = peekItem();
                if(item) _sprites.reset(_sprites.size() - 1);
                return item;
            }
            
            inline std::size_t itemCount() const { return _sprites.count(); }
            
            inline bool overflow() const { return _overflow; }
            
            inline void setOverflow() { _overflow |= _sprites.test(_sprites.size() - 1); }
            
            inline void clearOverflow() {
                if(_overflow) {
                    _sprites.reset(_sprites.size() - 1);
                    _overflow = false;
                }
            }
            
            inline bool advance() {
                _sprites <<= 1;
                return _sprites.any();
            }
            
            inline void reset() {
                _sprites.reset();
                _overflow = false;
            }
            
            inline void draw(Pomi::Graphics& gfx, const std::uint8_t* background) {
                gfx.setClippingRegion(122, 16, 88, 112);
                _sprites.draw(gfx, background);
                gfx.setClippingRegion();
            }
            
            SpriteSet<3> _sprites;
            bool _overflow;
        };
        
        struct AssemblyLines {
            inline void putItem(std::size_t pos, bool item=true) { items_pending.set(pos, item); }
            
            inline bool peekItem(std::size_t pos) const {
                return output_sprites[pos].test(output_sprites[0].size() - 1) && !_overflows.test(pos);
            }
            
            bool pickItem(std::size_t pos) {
                bool item = peekItem(pos);
                if(item) output_sprites[pos].reset(output_sprites[0].size() - 1);
                return item;
            }
            
            std::size_t itemCount() const {
                std::size_t count = 0;
                for(std::size_t idx = 0; idx < NUM_ASM_LINES; ++idx) {
                    count += input_sprites[idx].count() + output_sprites[idx].count();
                }
                return count + items_pending.count() + items_processing.count();
            }
            
            inline bool overflow() const { return _overflows.any(); }
            
            inline bool overflow(std::size_t pos) const { return _overflows.test(pos); }
            
            inline void setOverflow(std::size_t pos) {
                _overflows.set(pos);
            }
            
            inline void clearOverflow() {
                for(std::size_t idx = 0; idx < NUM_ASM_LINES; ++idx) {
                    clearOverflow(idx);
                }
            }
            
            inline void clearOverflow(std::size_t pos) {
                if(_overflows.test(pos)) {
                    output_sprites[pos].reset(output_sprites[0].size() - 1);
                    _overflows.reset(pos);
                }
            }
            
            inline void reset() {
                for(std::size_t idx = 0; idx < NUM_ASM_LINES; ++idx) {
                    input_sprites[idx].reset();
                    output_sprites[idx].reset();
                }
                items_pending.reset();
                items_processing.reset();
                _overflows.reset();
            }
            
            inline bool advance(std::size_t pos) {
                output_sprites[pos] <<= 1;
                output_sprites[pos].set(0, items_processing.test(pos));
                items_processing.set(pos, input_sprites[pos].test(input_sprites[0].size() - 1));
                input_sprites[pos] <<= 1;
                
                return input_sprites[pos].any() || output_sprites[pos].any() || items_processing.test(pos);
            }
            
            inline void draw(Pomi::Graphics& gfx, const uint8_t* background) {
                for(std::size_t idx = 0; idx < NUM_ASM_LINES; ++idx) {
                    gfx.setClippingRegion((idx & 1) == 0 ? 122 : 2, 16, 96, 112);
                    bool item = input_sprites[idx].test(0);
                    input_sprites[idx].set(0, item || items_pending.test(idx));
                    input_sprites[idx].draw(gfx, background);
                    input_sprites[idx].set(0, item);
                    
                    gfx.setClippingRegion((idx & 1) == 0 ? 2 : 122, 16, 96, 112);
                    output_sprites[idx].draw(gfx, background);
                    gfx.setClippingRegion();
                }
            }
            
            SpriteSet<4> input_sprites[NUM_ASM_LINES];
            SpriteSet<4> output_sprites[NUM_ASM_LINES];
            BitSet<NUM_ASM_LINES> items_pending;
            BitSet<NUM_ASM_LINES> items_processing;
            BitSet<NUM_ASM_LINES> _overflows;
        };
        
        AssemblyBay()
        : _feed_line{_FEED_LINE_SPRITES, false}, 
          _asm_lines{
              {_INPUT_LINE_SPRITES[0], _INPUT_LINE_SPRITES[1], _INPUT_LINE_SPRITES[2], _INPUT_LINE_SPRITES[3], _INPUT_LINE_SPRITES[4]}, 
              {_OUTPUT_LINE_SPRITES[0], _OUTPUT_LINE_SPRITES[1], _OUTPUT_LINE_SPRITES[2], _OUTPUT_LINE_SPRITES[3], _OUTPUT_LINE_SPRITES[4]}}, 
          _random(0),
          _conveyor_snd(0, conveyor11k),
          _miss_snd(0, miss11k)
        {}
        
        ~AssemblyBay() = default;
        
        AssemblyBay(const AssemblyBay&) = delete;
        AssemblyBay& operator =(const AssemblyBay&) = delete;
        
        inline FeedLine& feedLine() { return _feed_line; }
        inline AssemblyLines& assemblyLines() { return _asm_lines; }
        
        void reset();
        
        bool update(std::uint32_t steps);
        
        void draw(Pomi::Graphics& gfx, const std::uint8_t* background);
        
    private:
        
        static constexpr Sprite _FEED_LINE_SPRITES[3] = { Sprite(204, 97, item1), Sprite(196, 97, item1), Sprite(188, 97, item1) };
        
        static constexpr Sprite _INPUT_LINE_SPRITES[NUM_ASM_LINES][4] = {
            {Sprite(141, 97, item1), Sprite(133, 97, item1), Sprite(125, 97, item1), Sprite(117, 97, item1)},
            {Sprite(70, 81, item2), Sprite(78, 81, item2), Sprite(86, 81, item2), Sprite(94, 81, item2)},
            {Sprite(141, 65, item3), Sprite(133, 65, item3), Sprite(125, 65, item3), Sprite(117, 65, item3)},
            {Sprite(70, 49, item4), Sprite(78, 49, item4), Sprite(86, 49, item4), Sprite(94, 49, item4)},
            {Sprite(141, 33, item5), Sprite(133, 33, item5), Sprite(125, 33, item5), Sprite(117, 33, item5)},
        };
        
        static constexpr Sprite _OUTPUT_LINE_SPRITES[NUM_ASM_LINES][4] = {
            {Sprite(90, 97, item2), Sprite(82, 97, item2), Sprite(74, 97, item2), Sprite(66, 97, item2)},
            {Sprite(121, 81, item3), Sprite(129, 81, item3), Sprite(137, 81, item3), Sprite(145, 81, item3)},
            {Sprite(90, 65, item4), Sprite(82, 65, item4), Sprite(74, 65, item4), Sprite(66, 65, item4)},
            {Sprite(121, 49, item5), Sprite(129, 49, item5), Sprite(137, 49, item5), Sprite(145, 49, item5)},
            {Sprite(90, 33, item6), Sprite(82, 33, item6), Sprite(74, 33, item6), Sprite(66, 33, item6)}
        };
        
        FeedLine _feed_line;
        AssemblyLines _asm_lines;
        
        Random _random;
        
        Pomi::Sample _conveyor_snd;
        Pomi::Sample _miss_snd;
};
