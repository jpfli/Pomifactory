
#pragma once

#include <cstdint>
#include "Mixer.h"

namespace Pomi {

class Sample : public Mixer::Source {
    
    public:
    
        template<unsigned Length>
        constexpr Sample(std::uint8_t channel, const std::uint8_t (&sampledata)[Length]) : Source(channel), _sampledata(sampledata), _len(Length) {}
        
        virtual ~Sample() = default;
        
        Sample(const Sample&) = delete;
        Sample& operator =(const Sample&) = delete;
        
        virtual std::int32_t update() {
            if(Source::tick < _len) {
                const std::int32_t out = _sampledata[Source::tick];
                ++Source::tick;
                return (out - 128) << 8;
            }
            return 0;
        }
    
    private:
    
        const std::uint8_t* _sampledata;
        const std::uint32_t _len;
};

}; // namepsace Pomi
