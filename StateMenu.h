
#pragma once

#include <cstdint>

#include "State.h"
#include "pomi/hw/I2C.h"
#include "pomi/Sample.h"
#include "StringBuffer.h"
#include "MenuBox.h"

#include "sounds/points11k.h"

class StateMenu final: public State {
    
    public:
    
        static inline StateMenu& instance() { return _instance; }
        
        ~StateMenu() = default;
        
        StateMenu(const StateMenu&) = delete;
        void operator=(const StateMenu&) = delete;
        
        void enter() override;
        void leave() override;
        void update() override;
    
    private:
    
        StateMenu(): _vol_label{"Vol: -"}, _menu_labels{"Game A", "Game B", _vol_label}, _menubox("New Game", _menu_labels, (220 - 86) / 2, 42, 86), 
            _hiscores("HighScores", _hiscore_labels, (220 - 86) / 2, 114, 86), _points_snd(0, points11k) {}
        
        static inline void _setVolume(std::int32_t level) {
            // These values correspond to {-inf, -30, -25, -20, -15, -10, -5, 0} decibels
            constexpr std::uint8_t vol[8] = {0, 3, 7, 12, 22, 40, 71, 127};
            Pomi::Hw::I2C::write(0x5e, vol[level]);
        }
        
        static StateMenu _instance;
        
        bool _background_drawn;
        
        char _vol_label[7];
        char* _menu_labels[3];
        MenuBox<3> _menubox;
        
        const char* _hiscore_labels[2];
        MenuBox<2> _hiscores;
        
        StringBuffer<16> _hiscore_a_label;
        StringBuffer<16> _hiscore_b_label;
        
        Pomi::Sample _points_snd;
};
