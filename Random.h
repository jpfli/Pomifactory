
#pragma once

#include <cstdint>

class Random {
    
    public:
    
        explicit constexpr Random(std::uint32_t seed) : _state(xorSeed(seed)) {}
        ~Random() = default;
        Random(const Random&) = delete;
        Random& operator =(const Random&) = delete;
        
        inline void setSeed(std::uint32_t seed) { _state = xorSeed(seed); }
        
        constexpr std::uint32_t next(std::uint32_t bits) {
            _state ^= _state << 17;
            _state ^= _state >> 13;
            _state ^= _state << 5;
            return _state >> (32 - bits);
        }
        
        inline bool nextBoolean() { return next(1) != 0; }
        inline std::uint32_t nextInt() { return next(32); }
        
        inline std::uint32_t nextInt(std::uint32_t bound) {
            std::uint32_t mask  = bound - 1;
            mask |= mask >> 16;
            mask |= mask >> 8;
            mask |= mask >> 4;
            mask |= mask >> 2;
            mask |= mask >> 1;
            
            std::uint32_t ret;
            do {
                ret = next(31) & mask;
            } while(ret >= bound);
            
            return ret;
        }
    
    private:
    
        static constexpr std::uint32_t xorSeed(std::uint32_t seed) { return seed ^ 0x5DEECE66; }
        
        std::uint32_t _state;
};
