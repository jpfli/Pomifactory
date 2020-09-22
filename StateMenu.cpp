
#include "pomi/Graphics.h"
#include "pomi/Input.h"
#include "StateMenu.h"
#include "StatePlayGame.h"
#include "Pomifactory.h"

StateMenu StateMenu::_instance;

void StateMenu::enter() {
    std::int8_t vol = _menu_labels[2][5] - '0';
    if(vol < 0 || vol > 7) {
        constexpr std::uint32_t VOLUME = 4000;
        vol = Pomi::Hw::Eeprom::read(VOLUME) >> 5;
        _menu_labels[2][5] = '0' + vol;
        
        _setVolume(vol);
    }
    
    Pomi::Graphics& gfx = Pomifactory::graphics();
    
    gfx.drawImage(Pomifactory::backgroundImage(), 0, 0);
    
    if(_menubox.selected() < 0) _menubox.setSelected(0);
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
    
    std::int8_t selected = _menubox.selected();
    
    if(_menubox.highlighted() >= 0) {
        if(Pomi::Input::released(Pomi::Input::Button::A)) {
            _menubox.setHighlighted(-1);
            
            StatePlayGame::instance().init((selected == 0) ? GameInfo::Mode::A : GameInfo::Mode::B);
            Pomifactory::changeState(&StatePlayGame::instance());
            
            return;
        }
    }
    else {
        if(selected < 2 && Pomi::Input::pressed(Pomi::Input::Button::A)) {
            _menubox.setHighlighted(selected);
        }
        else {
            if(selected == 2) {
                std::int8_t vol = _menu_labels[2][5] - '0';
                if(Pomi::Input::pressed(Pomi::Input::Button::LEFT)) {
                    vol = (vol - 1) > 0 ? vol - 1 : 0;
                }
                else if(Pomi::Input::pressed(Pomi::Input::Button::RIGHT)) {
                    vol = (vol + 1) < 7 ? vol + 1 : 7;
                }
                
                if(vol != _menu_labels[2][5] - '0') {
                    _menu_labels[2][5] = '0' + vol;
                    _menubox.touch();
                    
                    _setVolume(vol);
                    
                    _points_snd.play();
                }
            }
            
            if(Pomi::Input::pressed(Pomi::Input::Button::UP)) {
                selected = (selected - 1) >= 0 ? (selected - 1) : 2;
            }
            else if(Pomi::Input::pressed(Pomi::Input::Button::DOWN)) {
                selected = (selected + 1) < 3 ? (selected + 1) : 0;
            }
        }
    }
    
    _menubox.setSelected(selected);
    _menubox.draw(gfx);
}
