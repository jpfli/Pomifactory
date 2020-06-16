
#pragma once

#include <cstdint>
#include "State.h"
#include "MenuBox.h"

class StatePauseGame final: public State {
    
    public:
    
        static inline StatePauseGame& instance() { return _instance; }
        
        ~StatePauseGame() = default;
        
        StatePauseGame(const StatePauseGame&) = delete;
        void operator=(const StatePauseGame&) = delete;
        
        void enter() override;
        void leave() override;
        void update() override;
    
    private:
    
        StatePauseGame(): _menubox("Paused", _menu_labels, (220 - 86) / 2, 42, 86) {}
        
        static StatePauseGame _instance;
        
        static constexpr char* _menu_labels[2] = { "Resume", "Abort" };
        MenuBox<2> _menubox;
};
