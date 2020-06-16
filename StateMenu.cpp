
#include "pomi/Graphics.h"
#include "pomi/Input.h"
#include "StateMenu.h"
#include "StatePlayGame.h"
#include "Pomifactory.h"

StateMenu StateMenu::_instance;

void StateMenu::enter() {
    Pomi::Graphics& gfx = Pomifactory::graphics();
    
    gfx.drawImage(Pomifactory::backgroundImage(), 0, 0);
    
    _menubox.show(gfx);
    
    // Load highscores from a cookie if there is one
    Pomifactory::loadCookie();
    
    _hiscore_a_label.replace(0, _hiscore_a_label.length(), "A:      ");
    _hiscore_a_label.append(Pomifactory::highscoreA());
    _hiscore_a_label.replace(2, 2 + _hiscore_a_label.length() - 9, "");
    _hiscore_labels[0] = _hiscore_a_label;
    
    _hiscore_b_label.replace(0, _hiscore_b_label.length(), "B:      ");
    _hiscore_b_label.append(Pomifactory::highscoreB());
    _hiscore_b_label.replace(2, 2 + _hiscore_b_label.length() - 9, "");
    _hiscore_labels[1] = _hiscore_b_label;
    
    _hiscores.show(gfx);
}

void StateMenu::leave() {
    _menubox.hide(Pomifactory::graphics());
    _hiscores.hide(Pomifactory::graphics());
}

void StateMenu::update() {
    Pomi::Graphics& gfx = Pomifactory::graphics();
    
    std::uint8_t selected = (_menubox.selected() < 2) ? _menubox.selected() : 0;
    
    if(Pomi::Input::pressed(Pomi::Input::Button::A)) {
        _menubox.setHighlighted(selected);
    }
    else if(_menubox.highlighted() < 2) {
        if(Pomi::Input::released(Pomi::Input::Button::A)) {
            _menubox.setHighlighted(-1);
            
            StatePlayGame::instance().init((selected == 0) ? GameInfo::Mode::A : GameInfo::Mode::B);
            Pomifactory::changeState(&StatePlayGame::instance());
            
            return;
        }
    }
    else {
        if(Pomi::Input::pressed(Pomi::Input::Button::UP)) {
            if(selected > 0) {
                selected = 0;
            }
        }
        else if(Pomi::Input::pressed(Pomi::Input::Button::DOWN)) {
            if(selected < 1) {
                selected = 1;
            }
        }
    }
    
    _menubox.setSelected(selected);
    _menubox.draw(gfx);
}
