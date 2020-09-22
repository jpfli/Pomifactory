
#pragma once

#include <cstdint>
#include "hw/Buttons.h"

namespace Pomi {

class Input {
    
    public:
    
        enum Button : std::uint8_t { NONE=0, LEFT=1<<0, RIGHT=1<<1, UP=1<<2, DOWN=1<<3, A=1<<4, B=1<<5, C=1<<6 };
        
        Input() = delete;
        Input(const Input&) = delete;
        Input& operator =(const Input&) = delete;
        
        static void update(std::uint32_t now);
        
        static inline bool held(std::uint8_t buttons) { return (_states & buttons) != 0; }
        static inline bool pressed(std::uint8_t buttons) { return (_states & (~_prev_states) & buttons) != 0; }
        static inline bool released(std::uint8_t buttons) { return ((~_states) & _prev_states & buttons) != 0; }
    
    private:
    
        static constexpr std::uint32_t REPEAT_DELAY = 500; // Delay before repeat starts
        static constexpr std::uint32_t REPEAT_RATE = 150;  // Button repeat interval
        
        static std::uint8_t _states;
        static std::uint8_t _prev_states;
        
        static std::uint8_t _last_btn;
        
        static std::uint32_t _time_next_repeat;
};

} // namespace Pomi
