
#include "pomi/System.h"
#include "pomi/Mixer.h"
#include "Pomifactory.h"
#include "StateMenu.h"

using Pomi::System;
using Pomi::Mixer;

int main() {
    System::init();
    Mixer::init(11025);
    Mixer::setVolume(128);
    
    Pomifactory::run(&StateMenu::instance());
}
