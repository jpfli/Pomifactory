
#pragma once

namespace Pomi {
namespace Hw {

class Display {
    
    public:
    
        Display() = delete;
        Display(const Display&) = delete;
        Display& operator =(const Display&) = delete;
        
        enum class Direction { HORIZONTAL = 0x1038, VERTICAL = 0x1030 };
        
        static inline void setDirection(Direction dir) {
            writeCommand(0x03);
            writeData(static_cast<unsigned int>(dir));
        }
        
        static inline void setX(unsigned int x) {
            writeCommand(0x21);
            writeData(x);
        }
        
        static inline void setY(unsigned int y) {
            writeCommand(0x20);
            writeData(y);
        }
        
        static inline void beginStream() {
            writeCommand(0x22);
        }
        
        static inline void writeData(unsigned int data) {
            volatile unsigned int* LCD = reinterpret_cast<unsigned int*>(0xa0002000);
            LCD[DATABUS] = data << 3; // Write to data bus
            LCD[WR_CLR] = WR_PIN;     // Clear WR
            LCD[WR_SET] = WR_PIN;     // Set WR
        }
        
        static inline void writeData() {
            volatile unsigned int* LCD = reinterpret_cast<unsigned int*>(0xa0002000);
            LCD[WR_CLR] = WR_PIN;     // Clear WR
            LCD[WR_SET] = WR_PIN;     // Set WR
        }
        
        static inline void enterReadMode(unsigned int x, unsigned int y) {
            setX(x);
            setY(y);
            beginStream();
            
            // Switch data bus to input
            volatile unsigned int* LPC_GPIO_PORT_DIR2 = reinterpret_cast<unsigned int*>(0xA0002008);
            *LPC_GPIO_PORT_DIR2 &= ~(0xffff << 3);
            
            readBGR565(); // First read returns carbage
        }
        
        static inline void leaveReadMode() {
            // Switch data bus to output
            volatile unsigned int* LPC_GPIO_PORT_DIR2 = reinterpret_cast<unsigned int*>(0xA0002008);
            *LPC_GPIO_PORT_DIR2 |= (0xffff << 3);
        }
        
        static inline unsigned int readBGR565() {
            constexpr unsigned int LCD_RD_PIN = 1 << 24;
            volatile unsigned int* LPC_GPIO_PORT_CLR1 = reinterpret_cast<unsigned int*>(0xA0002284);
            volatile unsigned int* LPC_GPIO_PORT_SET1 = reinterpret_cast<unsigned int*>(0xA0002204);
            volatile unsigned int* LPC_GPIO_PORT_MPIN2 = reinterpret_cast<unsigned int*>(0xA0002188);
            
            *LPC_GPIO_PORT_CLR1 = LCD_RD_PIN;     // Clear RD
            *LPC_GPIO_PORT_SET1 = LCD_RD_PIN;     // Set RD
            return (*LPC_GPIO_PORT_MPIN2) >> 3;   // Read from data bus
        }
        
        static inline void setHorizontalRange(unsigned int start, unsigned int end) {
            writeCommand(0x38);
            writeData(end);
            writeCommand(0x39);
            writeData(start);
        }
        
        static inline void setVerticalRange(unsigned int start, unsigned int end) {
            writeCommand(0x36);
            writeData(end);
            writeCommand(0x37);
            writeData(start);
        }
        
        static inline void oscillatorControl(unsigned int val) {
            writeCommand(0x0f);
            writeData(val);
        }
    
    private:
    
        static constexpr unsigned int DATABUS = 0x62; // GPIO_PORT_MPIN2 (0xA0002188)
        static constexpr unsigned int DCX_SET = 0x80; // GPIO_PORT_SET0 (0xA0002200)
        static constexpr unsigned int WR_SET = 0x81;  // GPIO_PORT_SET1 (0xA0002204)
        static constexpr unsigned int DCX_CLR = 0xa0; // GPIO_PORT_CLR0 (0xA0002280)
        static constexpr unsigned int WR_CLR = 0xa1;  // GPIO_PORT_CLR1 (0xA0002284)
        
        static constexpr unsigned int DCX_PIN = 1 << 2;
        static constexpr unsigned int WR_PIN = 1 << 12;
        
        static inline void writeCommand(unsigned int number) {
            volatile unsigned int* LCD = reinterpret_cast<unsigned int*>(0xa0002000);
            LCD[DCX_CLR] = DCX_PIN;     // Clear DCX (Enter command mode)
            LCD[WR_CLR] = WR_PIN;       // Clear WR
            LCD[DATABUS] = number << 3; // Write Data Bus
            LCD[WR_SET] = WR_PIN;       // Set WR
            LCD[DCX_SET] = DCX_PIN;     // Set DCX (Resume data mode)
        }
};

} // namespace Hw
} // namespace Pomi
