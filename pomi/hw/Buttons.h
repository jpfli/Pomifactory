
#pragma once

namespace Pomi {
namespace Hw {

class Buttons {
    
    public:
    
        Buttons() = delete;
        Buttons(const Buttons&) = delete;
        Buttons& operator =(const Buttons&) = delete;
        
        static inline bool a() { return *(reinterpret_cast<volatile char*>(0xa0000000 + 1 * 0x20 + 9)); }
        static inline bool b() { return *(reinterpret_cast<volatile char*>(0xa0000000 + 1 * 0x20 + 4)); }
        static inline bool c() { return *(reinterpret_cast<volatile char*>(0xa0000000 + 1 * 0x20 + 10)); }
        static inline bool up() { return *(reinterpret_cast<volatile char*>(0xa0000000 + 1 * 0x20 + 13)); }
        static inline bool down() { return *(reinterpret_cast<volatile char*>(0xa0000000 + 1 * 0x20 + 3)); }
        static inline bool left() { return *(reinterpret_cast<volatile char*>(0xa0000000 + 1 * 0x20 + 25)); }
        static inline bool right() { return *(reinterpret_cast<volatile char*>(0xa0000000 + 1 * 0x20 + 7)); }
    
};

} // namespace Hw
} // namespace Pomi
