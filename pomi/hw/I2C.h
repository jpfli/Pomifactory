
#pragma once

namespace Pomi {
namespace Hw {

class I2C {
    
    public:
    
        I2C() = delete;
        I2C(const I2C&) = delete;
        I2C& operator =(const I2C&) = delete;
        
        static inline void write(unsigned char address, unsigned char data) {
            _start();
            
            _sendFrame(address & ~1);
            _sendFrame(data);
            
            _stop();
        }
    
    private:
    
        static constexpr unsigned int SDA = 4;
        static constexpr unsigned int SCL = 5;
        
        static inline void _wait() {
            volatile unsigned int delay = 10;
            while(delay-- > 0);
        }
        
        static inline void _start() {
            volatile unsigned int* DIR = reinterpret_cast<unsigned int*>(0xa0002000);
            volatile unsigned char* P0 = reinterpret_cast<unsigned char*>(0xa0000000);
            
            P0[SDA] = 1;
            P0[SCL] = 1;
            
            // Set SDA and SCL as outputs
            DIR[0] |= (1<<SDA) | (1<<SCL);
            
            _wait();
            P0[SDA] = 0;
            _wait();
            P0[SCL] = 0;
        }
        
        static inline void _stop() {
            volatile unsigned int* DIR = reinterpret_cast<unsigned int*>(0xa0002000);
            volatile unsigned char* P0 = reinterpret_cast<unsigned char*>(0xa0000000);
            
            P0[SDA] = 0;
            P0[SCL] = 1;
            _wait();
            P0[SDA] = 1;
        }
        
        static inline bool _sendFrame(unsigned char value) {
            volatile unsigned int* DIR = reinterpret_cast<unsigned int*>(0xa0002000);
            volatile unsigned char* P0 = reinterpret_cast<unsigned char*>(0xa0000000);
            
            // Send out 8 bits, most significant bit first
            for(std::uint32_t bit = 8; bit > 0; --bit) {
                _wait();
                P0[SDA] = (value >> (bit - 1)) & 1;
                P0[SCL] = 1;
                _wait();
                P0[SCL] = 0;
            }
            
            // Set SDA as input and ask for the ACK bit
            DIR[0] &= ~(1<<SDA);
            P0[SCL] = 1;
            _wait();
            
            // Read ACK bit and set SDA as output
            bool ack = !P0[SDA];
            P0[SCL] = 0;
            DIR[0] |= (1<<SDA);
            
            return ack;
        }
};

} // namespace Hw
} // namespace Pomi

