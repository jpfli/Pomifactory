
#include <cstdint>
#include "Workers.h"
#include "Pomifactory.h"
#include "AssemblyBay.h"

void Workers::update() {
    AssemblyBay::FeedLine& feed_line = Pomifactory::assemblyBay().feedLine();
    AssemblyBay::AssemblyLines& asm_lines = Pomifactory::assemblyBay().assemblyLines();
    
    constexpr unsigned int BUSY_TICKS = 6; // How many ticks the worker will spend lifting one item
    
    // Right hand side worker
    if(rhsFrame() == 0) {
        bool item = (rhsPosition() == 0) ? feed_line.pickItem() : asm_lines.pickItem(2 * rhsPosition() - 1);
        if(item) {
            asm_lines.putItem(2 * rhsPosition());
            
            setRhsFrame(1);
            _rhs_status.busy_timer = BUSY_TICKS;
            
            Pomifactory::gameInfo().increaseScore();
            
            _lift_snd.play();
        }
    }
    else if(_rhs_status.busy_timer > 0) {
        --_rhs_status.busy_timer;
        if(_rhs_status.busy_timer == 0) {
            setRhsFrame(0);
        }
    }
    
    // Left hand side worker
    if(lhsFrame() == 0) {
        bool item = asm_lines.pickItem(2 * lhsPosition());
        if(item) {
            if(lhsPosition() == 2) {
                Pomifactory::truck().putItem();
            }
            else {
                asm_lines.putItem(2 * lhsPosition() + 1);
            }
            
            setLhsFrame(1);
            _lhs_status.busy_timer = BUSY_TICKS;
            
            Pomifactory::gameInfo().increaseScore();
            
            _lift_snd.play();
        }
    }
    else if(_lhs_status.busy_timer > 0) {
        --_lhs_status.busy_timer;
        if(_lhs_status.busy_timer == 0) {
            setLhsFrame(0);
        }
    }
}

void Workers::move(std::int8_t rhs_dir, std::int8_t lhs_dir) {
    AssemblyBay::FeedLine& feed_line = Pomifactory::assemblyBay().feedLine();
    AssemblyBay::AssemblyLines& asm_lines = Pomifactory::assemblyBay().assemblyLines();
    
    // Right hand side worker
    if(rhsFrame() == 0) {
        bool item = (rhsPosition() == 0) ? feed_line.peekItem() : asm_lines.peekItem(2 * rhsPosition() - 1);
        
        if(!item) {
            std::int8_t pos = rhsPosition() + ((rhs_dir != 0) ? rhs_dir : _rhs_status.buffered_move);
            if(pos > 2) {
                pos = 2;
            }
            else if(pos < 0) {
                pos = 0;
            }
            
            if(pos != rhsPosition()) {
                setRhsPosition(pos);
                _climb_snd.play();
            }
        }
        _rhs_status.buffered_move = 0;
    }
    else if(rhs_dir != 0) {
        _rhs_status.buffered_move = rhs_dir;
    }
    
    // Left hand side worker
    if(lhsFrame() == 0) {
        bool item = asm_lines.peekItem(2 * lhsPosition());
        
        if(!item) {
            std::int8_t pos = lhsPosition() + ((lhs_dir != 0) ? lhs_dir : _lhs_status.buffered_move);
            if(pos > 2) {
                pos = 2;
            }
            else if(pos < 0) {
                pos = 0;
            }
            
            if(pos != lhsPosition()) {
                setLhsPosition(pos);
                _climb_snd.play();
            }
        }
        _lhs_status.buffered_move = 0;
    }
    else if(lhs_dir != 0) {
        _lhs_status.buffered_move = lhs_dir;
    }
}
