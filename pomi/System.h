
#pragma once

#include <cstdint>
#include "hw/Core.h"

namespace Pomi {

class System {
    
    public:
    
        System() = delete;
        System(const System&) = delete;
        System& operator =(const System&) = delete;
        
        static inline void init() { Hw::Core::enableSysTick(); }
        
        static inline std::uint32_t currentTimeMillis() {
            std::uint32_t uptime = _uptime;
            std::uint32_t millis = Hw::Core::sysTickMillis();
            
            // If _uptime has changed, then SysTick counter has reset and should be read again
            return uptime != _uptime ? (_uptime + Hw::Core::sysTickMillis()) : (uptime + millis);
        }
        static inline std::uint32_t currentTimeMicros() {
            std::uint32_t uptime = _uptime;
            std::uint32_t micros = Hw::Core::sysTickMicros();
            
            // If _uptime has changed, then SysTick counter has reset and should be read again
            return uptime != _uptime ? (1000 * _uptime + Hw::Core::sysTickMicros()) : (1000 * uptime + micros);
        }
        
        static inline void onSysTick() { _uptime += 100; }
        
    private:
    
        static volatile std::uint32_t _uptime;
};

} // namespace Pomi
