
#pragma once

#include <cstdint>
#include <type_traits>

template<unsigned Size>
class BitSet {
    
    public:
    
        using StorageType = std::conditional_t<Size <= 8, uint8_t,
                            std::conditional_t<Size <= 16, uint16_t,
                            std::conditional_t<Size <= 32, uint32_t, std::uint64_t>>>;
        
        constexpr BitSet() = default;
        
        constexpr BitSet(const StorageType& bits) : _bits(bits) {}
        
        ~BitSet() = default;
        
        BitSet(const BitSet<Size>& other) : _bits(other._bits) {}
        
        BitSet<Size>& operator =(const BitSet<Size>& other) {
            _bits = other._bits;
            return *this;
        }
        
        inline bool test(std::size_t pos) const { return _bits & (1 << pos); }
        inline bool all() const { return _bits == (1 << Size) - 1; }
        inline bool any() const { return _bits != 0; }
        inline bool none() const { return _bits == 0; }
        
        inline std::size_t count() const {
            std::size_t count = 0;
            for(std::size_t pos = 0; pos < Size; ++pos) {
                if(_bits & (1 << pos)) ++count;
            }
            return count;
        }
        
        static constexpr std::uint32_t size() { return Size; }
        
        inline BitSet<Size> operator&=(const BitSet<Size>& other) const { return BitSet<Size>(_bits & other._bits); }
        inline BitSet<Size> operator|=(const BitSet<Size>& other) const { return BitSet<Size>(_bits | other._bits); }
        inline BitSet<Size> operator^=(const BitSet<Size>& other) const { return BitSet<Size>(_bits ^ other._bits); }
        inline BitSet<Size> operator~() const { return BitSet<Size>((~_bits) & _MASK); }
        
        inline void operator<<=(std::size_t pos) { _bits = ((_bits << pos) & _MASK); }
        inline BitSet<Size> operator<<(std::size_t pos) const { return BitSet<Size>((_bits << pos) & _MASK); }
        inline void operator>>=(std::size_t pos) { _bits >>= pos; }
        inline BitSet<Size> operator>>(std::size_t pos) const { return BitSet<Size>(_bits >> pos); }
        
        inline void set() { _bits = _MASK; }
        inline void set(std::size_t pos) { _bits |= 1 << pos; }
        inline void reset() { _bits = 0; }
        inline void reset(std::size_t pos) { _bits &= ~(1 << pos); }
        
        inline void set(std::size_t pos, bool value) { (value) ? set(pos) : reset(pos); }
        
        inline void flip() { _bits = (~_bits) & _MASK; }
        inline void flip(std::size_t pos) { _bits ^= 1 << pos; }
        
        inline StorageType to_uint() const { return _bits; }

    private:
        
        static constexpr unsigned _MASK = (1 << Size) - 1;
        
        StorageType _bits;
};

template<unsigned Size>
inline BitSet<Size> operator&(const BitSet<Size>& lhs, const BitSet<Size>& rhs ) {
    return BitSet<Size>(lhs.to_uint() & rhs.to_uint());
}

template<unsigned Size>
inline BitSet<Size> operator|(const BitSet<Size>& lhs, const BitSet<Size>& rhs ) {
    return BitSet<Size>(lhs.to_uint() | rhs.to_uint());
}

template<unsigned Size>
inline BitSet<Size> operator^(const BitSet<Size>& lhs, const BitSet<Size>& rhs ) {
    return BitSet<Size>(lhs.to_uint() ^ rhs.to_uint());
}
