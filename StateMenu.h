
#pragma once

#include <cstdint>
#include "State.h"
#include "pomi/Graphics.h"
#include "Pomifactory.h"

#include "pomi/hw/Core.h"
#include "StringBuffer.h"

#include "MenuBox.h"

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
    
        StateMenu(): _menubox("New Game", _menu_labels, (220 - 86) / 2, 42, 86), _hiscores("HighScores", _hiscore_labels, (220 - 86) / 2, 114, 86) {}
        
        static StateMenu _instance;
    
        bool _background_drawn;
        
        static constexpr char* _menu_labels[2] = { "Game A", "Game B" };
        MenuBox<2> _menubox;
        
        const char* _hiscore_labels[2];
        MenuBox<2> _hiscores;
        
        StringBuffer<16> _hiscore_a_label;
        StringBuffer<16> _hiscore_b_label;
};
