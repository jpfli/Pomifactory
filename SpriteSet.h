
#pragma once

#include <cstdint>
#include <bitset>
#include "pomi/Graphics.h"
#include "Sprite.h"
#include "BitSet.h"

// SpriteSet class is kind of like bit set where every bit controls visibility of corresponding sprite
template<unsigned Size>
class SpriteSet {
    
    public:
    
        constexpr SpriteSet(const Sprite (&sprites)[Size]) : _sprites(sprites), _drawn_state(), _state() {}
        ~SpriteSet() = default;
        
        SpriteSet(const SpriteSet&) = delete;
        SpriteSet& operator =(const SpriteSet&) = delete;
        
        inline void draw(Pomi::Graphics& gfx, const std::uint8_t* background) {
            clear(gfx, background, (~_state) & _drawn_state);
            draw(gfx, _state);
            _drawn_state = _state;
        }
        
        inline bool test(std::size_t pos) const { return _state.test(pos); }
        inline bool all() const { return _state.all(); }
        inline bool any() const { return _state.any(); }
        inline bool none() const { return !any(); }
        
        inline std::uint32_t count() const { return _state.count(); }
        
        static constexpr std::uint32_t size() { return Size; }
        
        inline void operator<<= (std::size_t pos) { _state <<= pos; }
        inline void operator>>= (std::size_t pos) { _state >>= pos; }
        
        inline void set() { _state.set(); }
        inline void set(std::size_t pos, bool value=true) { _state.set(pos, value); }
        
        inline void reset() { _state.reset(); }
        inline void reset(std::size_t pos) { _state.reset(pos); }
        
        inline void flip() { _state.flip(); }
        inline void flip(std::size_t pos) { _state.flip(pos); }
    
    private:
    
        inline void clear(Pomi::Graphics& gfx, const std::uint8_t* background, const BitSet<Size>& clear_bits) const {
            for(std::size_t idx = 0; idx < Size; ++idx) {
                if(clear_bits.test(idx) != 0) {
                    const Sprite& sprite = _sprites[idx];
                    const std::int32_t x = sprite.x();
                    const std::int32_t y = sprite.y();
                    gfx.drawImage(background, x, y, sprite.width(), sprite.height(), x, y);
                }
            }
        }
        
        inline void draw(Pomi::Graphics& gfx, const BitSet<Size>& set_bits) const {
            for(std::size_t idx = 0; idx < Size; ++idx) {
                if(set_bits.test(idx) != 0) {
                    _sprites[idx].draw(gfx);
                }
            }
        }
        
        const Sprite (&_sprites)[Size];
        
        BitSet<Size> _drawn_state;
        BitSet<Size> _state;
};
