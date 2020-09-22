
#pragma once

namespace Pomi {
namespace Hw {

class Core {
    
    public:
    
        Core() = delete;
        Core(const Core&) = delete;
        Core& operator =(const Core&) = delete;
        
        static inline void enableSysTick() {
            volatile unsigned int *SYSTICK = reinterpret_cast<unsigned int*>(0xe000e010);
            // sysTick[1] = 4800000 - 1; // OSCT = 0
            SYSTICK[1] = 7200000 - 1; // OSCT = 2
            SYSTICK[2] = 0;
            constexpr unsigned int CPU_CLOCK = 4;
            constexpr unsigned int TICKINT = 2;
            constexpr unsigned int ENABLE = 1;
            SYSTICK[0] = CPU_CLOCK | TICKINT | ENABLE;
        }
        
        static inline unsigned int sysTickMillis() {
            volatile unsigned int* SYSTICK = reinterpret_cast<unsigned int*>(0xe000e010);
            // return (((SYSTICK[1] - SYSTICK[2]) >> 7) * 89478) >> 25; // Divide by 48000 (OSCT = 0)
            return (((SYSTICK[1] - SYSTICK[2]) >> 7) * 59652) >> 25; // Divide by 72000 (OSCT = 2)
        }
        
        static inline unsigned int sysTickMicros() {
            volatile unsigned int* SYSTICK = reinterpret_cast<unsigned int*>(0xe000e010);
            // return (((SYSTICK[1] - SYSTICK[2]) >> 7) * 87381) >> 15; // Divide by 48 (OSCT = 0)
            return (((SYSTICK[1] - SYSTICK[2]) >> 7) * 58254) >> 15; // Divide by 72 (OSCT = 2)
        }
        
        static inline void enableDAC() {
            volatile unsigned int* PIO1 = reinterpret_cast<unsigned int*>(0x40044060);
            volatile unsigned int* PIO2 = reinterpret_cast<unsigned int*>(0x400440f0);
            volatile unsigned int* DIR = reinterpret_cast<unsigned int*>(0xa0002000);
            PIO1[28] = PIO1[29] = PIO1[30] = PIO1[31] = 1 << 7;
            PIO2[20] = PIO2[21] = PIO2[22] = PIO2[23] = 1 << 7;
            DIR[1] |= (1 << 28) | (1 << 29) | (1 << 30) | (1 << 31);
            DIR[2] |= (1 << 20) | (1 << 21) | (1 << 22) | (1 << 23);
        }
        
        static inline void writeDAC(unsigned char out) {
            volatile unsigned char* P1 = reinterpret_cast<unsigned char*>(0xa0000020);
            volatile unsigned char* P2 = reinterpret_cast<unsigned char*>(0xa0000040);
            P1[28] = out & 1; out >>= 1;
            P1[29] = out & 1; out >>= 1;
            P1[30] = out & 1; out >>= 1;
            P1[31] = out & 1; out >>= 1;
            P2[20] = out & 1; out >>= 1;
            P2[21] = out & 1; out >>= 1;
            P2[22] = out & 1; out >>= 1;
            P2[23] = out;
        }
        
        static inline unsigned int clockFrequency() {
            volatile unsigned int* SYSCON = reinterpret_cast<unsigned int*>(0x40048000);
            unsigned int msel = (SYSCON[2] & 0x1f) + 1;     // Get feedback divider value
            return 12000000 * msel / SYSCON[30];
        }
        
        static inline void enableTimer(unsigned int number, unsigned int matchnum, unsigned int frequency) {
            volatile unsigned int* SYSCON = reinterpret_cast<unsigned int*>(0x40048000);
            SYSCON[32] |= (number == 0) ? (1 << 9) : (1 << 10); // Enable clock for the timer
            
            volatile unsigned int* TIMER = reinterpret_cast<unsigned int*>((number == 0) ? 0x40014000 : 0x40018000);
            TIMER[1] = 1 << 1;                                  // Reset and disable timer counters
            TIMER[6 + matchnum] = clockFrequency() / frequency; // Set match value (timer's duration)
            TIMER[5] |= 1 << (3 * matchnum);                    // Generate interrupt when timer equals match value
            TIMER[5] |= 1 << (3 * matchnum + 1);                // Reset timer on match
            TIMER[1] = 1 << 0;                                  // Enable the timer counter
            
            // Enable timer interrupt
            volatile unsigned int* NVIC_ISER0 = reinterpret_cast<unsigned int*>(0xe000e100);
            *NVIC_ISER0 = (number == 0) ? (1 << 18) : (1 << 19);
        }
        
        static inline void disableTimer(unsigned int number) {
            // Disable timer interrupt
            volatile unsigned int* NVIC_ICER0 = reinterpret_cast<unsigned int*>(0xe000e180);
            *NVIC_ICER0 = (number == 0) ? (1 << 18) : (1 << 19);
            
            volatile unsigned int* TIMER = reinterpret_cast<unsigned int*>((number == 0) ? 0x40014000 : 0x40018000);
            TIMER[1] = 1 << 1;                                  // Reset and disable timer counters
        }
        
        static inline void enableIRQ() {
            __asm volatile ("cpsie i" : : : "memory");
        }
        
        static inline void disableIRQ() {
            __asm volatile ("cpsid i" : : : "memory");
        }
};

} // namespace Hw
} // namespace Pomi
