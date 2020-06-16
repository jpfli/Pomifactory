
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
        
        static inline std::uint32_t currentTimeMillis() { return _uptime + Hw::Core::sysTickMillis(); }
        
        static inline void onSysTick() { _uptime += 100; }
        
    private:
    
        static volatile std::uint32_t _uptime;
};

} // namespace Pomi
