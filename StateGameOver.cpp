
#include "pomi/Input.h"
#include "StateGameOver.h"
#include "StateMenu.h"
#include "StatePlayGame.h"
#include "Pomifactory.h"
#include "GameInfo.h"

StateGameOver StateGameOver::_instance;

void StateGameOver::enter() {
    _cookie_found = true;
    
    std::uint32_t highscore = (Pomifactory::gameInfo().mode() == GameInfo::Mode::A) ? Pomifactory::highscoreA() : Pomifactory::highscoreB();
    
    if(highscore < Pomifactory::gameInfo().score()) {
        // Update cookie with the new highscore
        if(Pomifactory::gameInfo().mode() == GameInfo::Mode::A) {
            Pomifactory::setHighscoreA(Pomifactory::gameInfo().score());
        }
        else {
            Pomifactory::setHighscoreB(Pomifactory::gameInfo().score());
        }
        
        _cookie_found = Pomifactory::cookieExists();
        if(_cookie_found) {
            // Save the new highscore to an existing cookie
            Pomifactory::saveCookie();
        }
    }
    
    if(_cookie_found) {
        _gameover_menu.show(Pomifactory::graphics());
    }
    else {
        // No cookie found. Ask if one should be created
        _cookie_menu.show(Pomifactory::graphics());
    }
}

void StateGameOver::leave() {
    if(_cookie_found) {
        _gameover_menu.hide(Pomifactory::graphics());
    }
    else {
        _cookie_menu.hide(Pomifactory::graphics());
    }
}

void StateGameOver::update() {
    MenuBox<2>& menubox = _cookie_found ? _gameover_menu : _cookie_menu;
    std::uint8_t selected = (menubox.selected() < 2) ? menubox.selected() : 0;
    
    if(Pomi::Input::pressed(Pomi::Input::Button::A)) {
        menubox.setHighlighted(selected);
    }
    else if(menubox.highlighted() < 2) {
        if(Pomi::Input::released(Pomi::Input::Button::A)) {
            menubox.setHighlighted(-1);
            
            if(_cookie_found) {
                if(selected == 0) {
                    StatePlayGame::instance().init(Pomifactory::gameInfo().mode());
                    Pomifactory::changeState(&StatePlayGame::instance());
                }
                else {
                    Pomifactory::changeState(&StateMenu::instance());
                }
            }
            else {
                if(selected == 0) {
                    // Create new cookie
                    Pomifactory::saveCookie();
                }
                Pomifactory::changeState(&StateMenu::instance());
            }
            
            return;
        }
    }
    else {
        if(Pomi::Input::pressed(Pomi::Input::Button::UP)) {
            if(selected > 0) {
                selected = 0;
            }
        }
        if(Pomi::Input::pressed(Pomi::Input::Button::DOWN)) {
            if(selected < 1) {
                selected = 1;
            }
        }
    }
    
    menubox.setSelected(selected);
    menubox.draw(Pomifactory::graphics());
}
