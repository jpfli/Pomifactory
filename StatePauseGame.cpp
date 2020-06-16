
#include "pomi/Graphics.h"
#include "pomi/Input.h"
#include "StatePauseGame.h"
#include "StatePlayGame.h"
#include "StateMenu.h"
#include "Pomifactory.h"

StatePauseGame StatePauseGame::_instance;

void StatePauseGame::enter() {
    _menubox.setSelected(0);
    _menubox.show(Pomifactory::graphics());
}

void StatePauseGame::leave() {
    _menubox.hide(Pomifactory::graphics());
}

void StatePauseGame::update() {
    Pomi::Graphics& gfx = Pomifactory::graphics();
    
    std::uint8_t selected = (_menubox.selected() < 2) ? _menubox.selected() : 0;
    
    if(Pomi::Input::pressed(Pomi::Input::Button::A)) {
        _menubox.setHighlighted(selected);
    }
    else if(_menubox.highlighted() < 2) {
        if(Pomi::Input::released(Pomi::Input::Button::A)) {
            _menubox.setHighlighted(-1);
            
            Pomifactory::changeState((selected == 0) ? static_cast<State*>(&StatePlayGame::instance()) : static_cast<State*>(&StateMenu::instance()));
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
    
    _menubox.setSelected(selected);
    _menubox.draw(gfx);
}
