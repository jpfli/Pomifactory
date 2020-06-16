
#pragma once

class State {
    
    public:
    
        State() = default;
        virtual ~State() = default;
        
        State(const State&) = delete;
        State& operator =(const State&)= delete;
        
        virtual void enter() = 0;
        virtual void leave() = 0;
        virtual void update() = 0;
};
