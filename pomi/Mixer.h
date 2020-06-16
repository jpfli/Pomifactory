
#pragma once

#include <cstdint>
#include "hw/Core.h"

namespace Pomi {

class Mixer {
    
    public:
    
        // Audio source used for generating sound
        struct Source {
                explicit Source(std::uint8_t chn) : tick(0), channel(chn) {}
                virtual ~Source() = default;
                
                virtual std::int32_t update() = 0;
                
                inline void play() { setChannel(channel, this); }
                
                inline void reset() { tick = 0; }
                
                std::uint32_t tick;
                std::uint8_t channel;
        };
        
        static constexpr std::uint32_t MATCHNUM = 1;
        
        Mixer() = delete;
        Mixer(const Mixer&) = delete;
        Mixer& operator =(const Mixer&) = delete;
        
        static inline void init(std::uint32_t frequency) {
            Hw::Core::enableDAC();
            Hw::Core::enableTimer(0, MATCHNUM, frequency);
        }
        
        static inline Source* getChannel(std::uint8_t channel) {
            return (channel < 4) ? _channels[channel % 4] : nullptr;
        }
        
        static inline void setChannel(std::uint8_t channel, Source* source) {
            if(channel < 4) {
                _channels[channel] = source;
                if(source != nullptr) {
                    source->reset();
                }
            }
        }
        
        // Sets the master volume. 256 equals unit gain.
        static inline void setVolume(std::uint32_t volume) {
            _multiplier = (volume * volume) >> 8;
        }
        
        static inline void onTimerMatch() {
            std::int32_t out = 0;
            if(_channels[0] != nullptr) {
                out += _channels[0]->update();
            }
            if(_channels[1] != nullptr) {
                out += _channels[1]->update();
            }
            if(_channels[2] != nullptr) {
                out += _channels[2]->update();
            }
            if(_channels[3] != nullptr) {
                out += _channels[3]->update();
            }
            
            out = (out * _multiplier + (1 << 15)) >> 16;
            Hw::Core::writeDAC((out > 127) ? 255 : ((out < -128) ? 0 : out + 128));
        }
    
    private:
    
        static Source* _channels[4];
        static std::int32_t _multiplier;
};

} // namespace Pomi
