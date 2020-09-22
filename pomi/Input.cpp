
#include <cstdint>
#include "Input.h"
#include "hw/Buttons.h"
#include "System.h"

using namespace Pomi;
using namespace Pomi::Hw;

std::uint8_t Input::_states(0);
std::uint8_t Input::_prev_states(0);
std::uint8_t Input::_last_btn(0);
std::uint32_t Input::_time_next_repeat(0);

void Input::update(std::uint32_t now) {
    _prev_states = _states;
    _states = (Buttons::left() ? Button::LEFT : 0) |
              (Buttons::right() ? Button::RIGHT : 0) |
              (Buttons::up() ? Button::UP : 0) |
              (Buttons::down() ? Button::DOWN : 0) |
              (Buttons::a() ? Button::A : 0) |
              (Buttons::b() ? Button::B : 0) |
              (Buttons::c() ? Button::C : 0);
    
    std::uint8_t pressed = _states & (~_prev_states);
    
    // Handle auto-repeat
    if(pressed != 0) { // New button was pressed
        // Find out which button was pressed and save it in _last_btn
        _last_btn = 1;
        while((pressed & _last_btn) == 0) {
            _last_btn <<= 1;
        }
        
        // Set the time when button starts automatically repeating
        _time_next_repeat = now + REPEAT_DELAY;
    }
    else if((_states & _last_btn) != 0) { // Last pressed button is being held down
        if(static_cast<std::int32_t>(now - _time_next_repeat) >= 0) { // Is it time to repeat the held button?
            // Simulate button press by clearing last button from previous states
            _prev_states &= ~_last_btn;
            
            // Set the time when button is repeated next time
            _time_next_repeat = now + REPEAT_RATE;
        }
    }
}
