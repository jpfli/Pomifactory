
#pragma once

#include <cstdint>
#include "State.h"
#include "GameInfo.h"
#include "pomi/Sample.h"

#include "sounds/points11k.h"

class StatePlayGame final: public State {
    
    public:
    
        static inline StatePlayGame& instance() { return _instance; }
        
        ~StatePlayGame() = default;
        
        StatePlayGame(const StatePlayGame&) = delete;
        void operator=(const StatePlayGame&) = delete;
        
        void init(GameInfo::Mode mode);
        
        void enter() override;
        void leave() override;
        void update() override;
    
    private:
    
        enum class State { Playing, ItemMissed, TruckFull };
        
        StatePlayGame(): _points_snd(0, points11k) {}
        
        void updatePlaying();
        void updateItemMissed();
        void updateTruckFull();
        
        static StatePlayGame _instance;
        
        std::uint8_t _tick;
        std::uint32_t _steps;
        
        std::uint8_t _seq_tick;
        std::uint32_t _seq_steps;
        
        std::uint8_t _ticks_per_step;
        
        State _state;
        
        std::uint8_t _rhs_position;
        std::uint8_t _lhs_position;
        
        std::uint32_t _new_score;
        
        Pomi::Sample _points_snd;
};
