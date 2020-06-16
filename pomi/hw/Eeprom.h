
#pragma once

namespace Pomi {
namespace Hw {

class Eeprom {
    
    public:
    
        Eeprom() = delete;
        Eeprom(const Eeprom&) = delete;
        Eeprom& operator =(const Eeprom&) = delete;
        
        static inline void write(unsigned int address, unsigned char value) {
            using IAP_call = int(*)(unsigned int[], unsigned int[]);
            
            constexpr unsigned int WRITE_EEPROM = 61;
            constexpr unsigned int COUNT = 1;
            unsigned int command[5] = { WRITE_EEPROM, address, reinterpret_cast<unsigned int>(&value), COUNT, Core::clockFrequency() / 1000 };
            unsigned int result[4];
            
            Core::disableIRQ();
            IAP_call iap_entry = reinterpret_cast<IAP_call>(IAP_LOCATION);
            iap_entry(command, result);
            Core::enableIRQ();
        }
        
        static inline unsigned char read(unsigned int address) {
            using IAP_call = int(*)(unsigned int[], unsigned int[]);
            
            unsigned char value;
            
            constexpr unsigned int READ_EEPROM = 62;
            constexpr unsigned int COUNT = 1;
            unsigned int command[5] = { READ_EEPROM, address, reinterpret_cast<unsigned int>(&value), COUNT, Core::clockFrequency() / 1000 };
            unsigned int result[4];
            
            Core::disableIRQ();
            IAP_call iap_entry = reinterpret_cast<IAP_call>(IAP_LOCATION);
            iap_entry(command, result);
            Core::enableIRQ();
            
            return (0 == result[0]) ? value : -1;
        }
    
    private:
    
        static constexpr unsigned int IAP_LOCATION = 0x1fff1ff1; // LPC1347 IAP entry address
};

} // namespace Hw
} // namespace Pomi
