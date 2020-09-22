
#include "pomi/System.h"
#include "Pomifactory.h"
#include "StateMenu.h"

int main() {
    Pomi::System::init();
    
    Pomifactory::run(&StateMenu::instance());
}
