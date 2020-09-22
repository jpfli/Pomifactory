
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
    
    std::uint8_t selected = _menubox.selected();
    
    if(_menubox.highlighted() >= 0) {
        if(Pomi::Input::released(Pomi::Input::Button::A)) {
            _menubox.setHighlighted(-1);
            Pomifactory::changeState((selected == 0) ? static_cast<State*>(&StatePlayGame::instance()) : static_cast<State*>(&StateMenu::instance()));
            return;
        }
    }
    else {
        if(Pomi::Input::pressed(Pomi::Input::Button::A)) {
            _menubox.setHighlighted(selected);
        }
        else {
            if(Pomi::Input::pressed(Pomi::Input::Button::UP)) {
                selected = (selected - 1) >= 0 ? (selected - 1) : 1;
            }
            else if(Pomi::Input::pressed(Pomi::Input::Button::DOWN)) {
                selected = (selected + 1) < 2 ? (selected + 1) : 0;
            }
        }
    }
    
    _menubox.setSelected(selected);
    _menubox.draw(gfx);
}
