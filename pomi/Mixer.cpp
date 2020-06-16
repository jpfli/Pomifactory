
#include <cstdint>
#include "Mixer.h"

Pomi::Mixer::Source* Pomi::Mixer::_channels[4];
std::int32_t Pomi::Mixer::_multiplier = 256;
