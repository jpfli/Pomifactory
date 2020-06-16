
#pragma once

#include <cstdint>
#include "State.h"
#include "MenuBox.h"

class StateGameOver final: public State {
    
    public:
    
        static inline StateGameOver& instance() { return _instance; }
        
        ~StateGameOver() = default;
        
        StateGameOver(const StateGameOver&) = delete;
        void operator=(const StateGameOver&) = delete;
        
        void enter() override;
        void leave() override;
        void update() override;
    
    private:
    
        StateGameOver(): _cookie_menu("Highscore", _cookie_menu_labels, (220 - 100) / 2, 42, 100), _gameover_menu("Game Over", _gameover_menu_labels, (220 - 86) / 2, 42, 86) {}
        
        static StateGameOver _instance;
        
        static constexpr char* _cookie_menu_labels[2] = { "Save Cookie", "Don't Save" };
        MenuBox<2> _cookie_menu;
        static constexpr char* _gameover_menu_labels[2] = { "Retry", "Menu" };
        MenuBox<2> _gameover_menu;
        
        bool _cookie_found;
};
