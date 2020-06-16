
#include "Mixer.h"
#include "System.h"
#include "hw/Display.h"

using Pomi::Mixer;
using Pomi::System;

extern "C" void SysTick_Handler() {
    System::onSysTick();
}

extern "C" void TIMER32_0_IRQHandler() {
    volatile unsigned int* TIMER = reinterpret_cast<unsigned int*>(0x40014000);
    if((TIMER[0] & (1 << Mixer::MATCHNUM)) != 0) {
        TIMER[0] = 1 << Mixer::MATCHNUM;
        
        Mixer::onTimerMatch();
    }
}
