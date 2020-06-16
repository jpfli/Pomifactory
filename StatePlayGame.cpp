
#include <cstdint>
#include "StatePlayGame.h"
#include "StatePauseGame.h"
#include "StateGameOver.h"
#include "Pomifactory.h"
#include "GameInfo.h"
#include "pomi/Graphics.h"
#include "pomi/Input.h"

StatePlayGame StatePlayGame::_instance;

void StatePlayGame::init(GameInfo::Mode mode) {
    Pomifactory::reset(mode);
    
    _tick = 0;
    _steps = 0;
    _state = State::Playing;
    
    _ticks_per_step = (mode == GameInfo::Mode::B) ? 9 : 12;
}

void StatePlayGame::enter() {
}

void StatePlayGame::leave() {
}

void StatePlayGame::update() {
    if(_state == State::Playing) {
        updatePlaying();
    }
    else if(_state == State::ItemMissed) {
        updateItemMissed();
    }
    else if(_state == State::TruckFull) {
        updateTruckFull();
    }
    
    Pomifactory::draw();
    
    if(Pomi::Input::pressed(Pomi::Input::Button::C)) {
        Pomifactory::changeState(&StatePauseGame::instance());
        return;
    }
}

void StatePlayGame::updatePlaying() {
    if(Pomifactory::gameInfo().numMisses() >= 3) {
        Pomifactory::changeState(&StateGameOver::instance());
        return;
    }
    
    if((_tick & 3) == 0) {
        Pomifactory::truck().update();
        if(Pomifactory::truck().full()) {
            _seq_tick = 0;
            _seq_steps = 0;
            _state = State::TruckFull;
            _new_score = Pomifactory::gameInfo().score() + 10;
            return;
        }
    }
    
    Pomifactory::workers().update();
    
    if(_tick == 0) {
        Pomifactory::assemblyBay().update(_steps);
        
        AssemblyBay::FeedLine& feed_line = Pomifactory::assemblyBay().feedLine();
        AssemblyBay::AssemblyLines& asm_lines = Pomifactory::assemblyBay().assemblyLines();
        
        if(feed_line.overflow() || asm_lines.overflow()) {
            _seq_tick = 0;
            _seq_steps = 0;
            _state = State::ItemMissed;
            return;
        }
    }
    
    std::int8_t rhs_move = 0;
    if(Pomi::Input::pressed(Pomi::Input::Button::A)) {
        rhs_move = 1;
    }
    else if(Pomi::Input::pressed(Pomi::Input::Button::B)) {
        rhs_move = -1;
    }
    
    std::int8_t lhs_move = 0;
    if(Pomi::Input::pressed(Pomi::Input::Button::UP)) {
        lhs_move = 1;
    }
    else if(Pomi::Input::pressed(Pomi::Input::Button::DOWN)) {
        lhs_move = -1;
    }
    
    Pomifactory::workers().move(rhs_move, lhs_move);
    
    ++_tick;
    if(_tick >= _ticks_per_step) {
        _tick = 0;
        ++_steps;
    }
}

void StatePlayGame::updateItemMissed() {
    AssemblyBay::FeedLine& feed_line = Pomifactory::assemblyBay().feedLine();
    AssemblyBay::AssemblyLines& asm_lines = Pomifactory::assemblyBay().assemblyLines();
    
    if(_seq_steps > 9) {
        if(Pomi::Input::pressed(Pomi::Input::Button::UP | Pomi::Input::Button::DOWN | Pomi::Input::Button::A | Pomi::Input::Button::B)) {
            _state = State::Playing;
            
            if(Pomifactory::boss().position() == Boss::Position::Left) {
                Pomifactory::workers().setLhsPosition(0);
                Pomifactory::workers().setLhsFrame(0);
            }
            else {
                Pomifactory::workers().setRhsPosition(1);
                Pomifactory::workers().setRhsFrame(0);
            }
            
            Pomifactory::boss().setPosition(Boss::Position::None);
            
            feed_line.clearOverflow();
            asm_lines.clearOverflow();
            
            return;
        }
    }
    
    if(_seq_tick == 0) {
        switch(_seq_steps) {
            case 0 ... 5:
                Pomifactory::assemblyBay().update(_seq_steps);
                break;
            
            case 6:
                if(feed_line.overflow()) {
                    feed_line.clearOverflow();
                    Pomifactory::smashedItems().set(0);
                }
                else {
                    for(std::size_t idx = 0; idx < AssemblyBay::NUM_ASM_LINES; ++idx) {
                        if(asm_lines.overflow(idx)) {
                            asm_lines.clearOverflow(idx);
                            Pomifactory::smashedItems().set(2 - (idx & 1));
                            break;
                        }
                    }
                }
                
                break;
            
            case 7: break;
            
            case 8:
                if(Pomifactory::smashedItems().test(0) || Pomifactory::smashedItems().test(1)) {
                    Pomifactory::boss().setPosition(Boss::Position::Right);
                    Pomifactory::workers().setRhsPosition(3);
                }
                else if(Pomifactory::smashedItems().test(2)) {
                    Pomifactory::boss().setPosition(Boss::Position::Left);
                    Pomifactory::workers().setLhsPosition(3);
                }
                Pomifactory::smashedItems().reset();
                
                Pomifactory::gameInfo().increaseMisses();
                
                break;
            
            default:
                Pomifactory::boss().setFrame(Pomifactory::boss().frame() ^ 1);
                if(Pomifactory::boss().position() == Boss::Position::Left) {
                    Pomifactory::workers().setLhsFrame(Pomifactory::workers().lhsFrame() ^ 1);
                }
                else {
                    Pomifactory::workers().setRhsFrame(Pomifactory::workers().rhsFrame() ^ 1);
                }
        }
        
        Pomifactory::truck().update();
    }
    
    ++_seq_tick;
    if(_seq_tick >= _ticks_per_step) {
        _seq_tick = 0;
        ++_seq_steps;
    }
}

void StatePlayGame::updateTruckFull() {
    if(_seq_steps < 2) {
        Pomifactory::workers().update();
    }
    else if((Pomifactory::gameInfo().score() < _new_score) && (_seq_tick == 0)) {
        Pomifactory::gameInfo().increaseScore();
        
        _points_snd.play();
    }
    
    if(_seq_tick == 0) {
        switch(_seq_steps) {
            case 5 ... 12:
                if(_seq_steps == 7) Pomifactory::truck().resetCargo();
                Pomifactory::truck().setFrame(((_seq_steps - 3) >> 1) - 1);
                break;
            
            case 13:
                Pomifactory::workers().setRhsPosition(4);
                Pomifactory::workers().setLhsPosition(4);
                break;
            
            case 14 ... 17:
                Pomifactory::workers().setRhsFrame(Pomifactory::workers().rhsFrame() ^ 1);
                Pomifactory::workers().setLhsFrame(Pomifactory::workers().lhsFrame() ^ 1);
                break;
            
            case 18:
                Pomifactory::truck().setFrame(0);
                break;
            
            case 22:
                Pomifactory::boss().setPosition(Boss::Position::Both);
                Pomifactory::workers().setRhsPosition(3);
                Pomifactory::workers().setLhsPosition(3);
                break;
            
            case 24:
                Pomifactory::boss().setPosition(Boss::Position::None);
                Pomifactory::workers().setRhsPosition(1);
                Pomifactory::workers().setLhsPosition(0);
                break;
            
            case 26:
                _state = State::Playing;
                return;
        }
        
        Pomifactory::truck().update();
    }
    
    if(_seq_steps >= 24) {
        if(Pomi::Input::pressed(Pomi::Input::Button::UP | Pomi::Input::Button::DOWN | Pomi::Input::Button::A | Pomi::Input::Button::B)) {
            _state = State::Playing;
            return;
        }
    }
    
    ++_seq_tick;
    if (_seq_tick >= _ticks_per_step) {
        _seq_tick = 0;
        ++_seq_steps;
    }
}
