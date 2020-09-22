
#include "Pomifactory.h"
#include "pomi/Mixer.h"
#include "pomi/Graphics.h"
#include "pomi/System.h"
#include "pomi/Input.h"
#include "pomi/Cookies.h"
#include "pomi/hw/Eeprom.h"
#include "StringBuffer.h"

#include "GameInfo.h"
#include "Workers.h"
#include "Boss.h"
#include "AssemblyBay.h"
#include "Truck.h"

#include "State.h"
#include "StatePlayGame.h"

#include "fonts/lcdFont.h"
#include "images/background.h"
#include "images/button_mid.h"
#include "images/button_up.h"
#include "images/button_down.h"
#include "images/cbtn_down.h"

using Pomi::Mixer;
using Pomi::Graphics;
using Pomi::Input;
using Button = Pomi::Input::Button;

extern "C" void __wrap_exit(int);

Graphics Pomifactory::_gfx(&_palette[0], lcdFont);

std::uint32_t Pomifactory::_time_last_frame = 0;

AssemblyBay Pomifactory::_asm_bay;
Truck Pomifactory::_truck;
Pomifactory::Smashes Pomifactory::_smashes = {_SMASH_SPRITES};
GameInfo Pomifactory::_game_info(168, 18);
Boss Pomifactory::_boss;
Workers Pomifactory::_workers;

Pomifactory::Cookie Pomifactory::_cookie;

State* Pomifactory::_cur_state(nullptr);
State* Pomifactory::_next_state(nullptr);

void Pomifactory::reset(GameInfo::Mode mode) {
    _game_info.setMode(mode);
    _game_info.clearScore();
    _game_info.clearMisses();
    
    _truck.setFrame(0);
    _truck.resetCargo();
    
    _smashes.reset();
    
    _asm_bay.reset();
    
    _boss.setPosition(Boss::Position::None);
    
    _workers.setRhsPosition(1);
    _workers.setRhsFrame(0);
    _workers.setLhsPosition(0);
    _workers.setLhsFrame(0);
}

void Pomifactory::run(State* state) {
    loaderScreen();
    
    Mixer::init(11025);
    Mixer::setVolume(181);
    
    changeState(state);
    
    while(true) {
        if(_next_state != nullptr) {
            if(_cur_state != nullptr) {
                _cur_state->leave();
            }
            _cur_state = _next_state;
            _next_state = nullptr;
            _cur_state->enter();
        }
        
        constexpr std::uint8_t FRAMES_PER_SECOND = 30;
        limitFPS(FRAMES_PER_SECOND);
        
        Pomi::Input::update(Pomi::System::currentTimeMillis());
        
        _cur_state->update();
    }
}

const std::uint8_t* Pomifactory::backgroundImage() {
    return background;
}

void Pomifactory::draw() {
    // Draw right side button
    if(Input::held(Button::A)) {
        _gfx.drawImage(button_up, 190, 144, true);
    }
    else if(Input::held(Button::B)) {
        _gfx.drawImage(button_down, 190, 144, true);
    }
    else {
        _gfx.drawImage(background, 190, 144, button_up[0], button_up[1], 190, 144);
    }
    
    // Draw left side button
    if(Input::held(Button::UP)) {
        _gfx.drawImage(button_up, 14, 144);
    }
    else if(Input::held(Button::DOWN)) {
        _gfx.drawImage(button_down, 14, 144);
    }
    else {
        _gfx.drawImage(background, 14, 144, button_up[0], button_up[1], 14, 144);
    }
    
    if(Input::held(Button::C)) {
        _gfx.drawImage(cbtn_down, 136, 135);
    }
    else {
        _gfx.drawImage(background, 136, 135, cbtn_down[0], cbtn_down[1], 136, 135);
    }
    
    _game_info.draw(_gfx, background);
    _smashes.draw();
    _boss.draw(_gfx, background);
    _truck.draw(_gfx, background);
    _asm_bay.draw(_gfx, background);
    _workers.draw(_gfx, background);
}

void Pomifactory::loaderScreen() {
    _gfx.clear(12);
    
    _gfx.drawImage(player2, 82, 56);
    _gfx.drawImage(item3, 106, 59);
    _gfx.drawImage(player3, 115, 56, true);
    
    _gfx.print("Press   for loader", 47, 96, 3);
    _gfx.print("C", 89, 96, 2);
    
    constexpr std::uint32_t LOADERWAIT = 4002;
    std::int8_t wait = Pomi::Hw::Eeprom::read(LOADERWAIT);
    if((wait <= 0) || (wait > 5)) wait = 3;
    std::uint32_t exit_time = Pomi::System::currentTimeMillis() + 1000 * wait;
    
    std::int32_t counter = -1;
    while(true) {
        Input::update(0);
        
        if(Input::pressed(Input::Button::C)) {
            __wrap_exit(0);
        }
        
        std::int32_t counter_now = (exit_time - Pomi::System::currentTimeMillis() + 999) / 1000;
        if(counter_now <= 0 || Input::pressed(Input::Button::A | Input::Button::B)) {
            break;
        }
        else if(counter_now != counter) {
            counter = counter_now;
            
            StringBuffer<5> text;
            text.append(counter);
            
            std::int32_t x = 110 - 7 * text.length() / 2;
            
            _gfx.drawFilledRectangle(x, 120, 7 * text.length(), 11, 12);
            _gfx.print(text, x, 120, 2);
        }
    }
    
    do {
        Input::update(0);
    } while(Input::held(Input::Button::A | Input::Button::B | Input::Button::C));
}
