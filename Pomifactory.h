
#pragma once

#include <array>
#include <cstdint>
#include "pomi/Graphics.h"
#include "pomi/System.h"
#include "pomi/Cookies.h"
#include "SpriteSet.h"
#include "Sprite.h"
#include "State.h"

#include "GameInfo.h"
#include "Workers.h"
#include "Boss.h"
#include "AssemblyBay.h"
#include "Truck.h"

#include "images/palette.h"
#include "images/smash.h"

class Pomifactory {
    
    public:
    
        struct Smashes {
            inline void set(std::size_t pos) { sprites.set(pos); }
            
            inline bool test(std::size_t pos) { return sprites.test(pos); }
            
            inline bool any() { return sprites.any(); }
            
            inline void reset() { sprites.reset(); }
            
            inline void draw() { sprites.draw(_gfx, Pomifactory::backgroundImage()); }
            
            SpriteSet<3> sprites;
        };
        
        Pomifactory() = delete;
        Pomifactory(const Pomifactory&) = delete;
        Pomifactory& operator =(const Pomifactory&) = delete;
        
        static void run(State* state);
        
        static void reset(GameInfo::Mode mode);
        
        static inline AssemblyBay& assemblyBay() { return _asm_bay; }
        static inline Truck& truck() { return _truck; }
        static inline Workers& workers() { return _workers; }
        static inline Boss& boss() { return _boss; }
        static inline Smashes& smashedItems() { return _smashes; }
        
        static inline GameInfo& gameInfo() { return _game_info; }
        
        static inline bool cookieExists() { return (Pomi::Cookies::findKey(COOKIE_LABEL) >= 0); }
        static inline void loadCookie() { Pomi::Cookies::loadCookie(COOKIE_LABEL, _cookie); }
        static inline void saveCookie() { Pomi::Cookies::saveCookie(COOKIE_LABEL, _cookie); }
        static inline std::uint32_t highscoreA() { return _cookie.highscore_a; }
        static inline void setHighscoreA(std::uint32_t score) { _cookie.highscore_a = score; }
        static inline std::uint32_t highscoreB() { return _cookie.highscore_b; }
        static inline void setHighscoreB(std::uint32_t score) { _cookie.highscore_b = score; }
        
        static inline void changeState(State* state) { _next_state = state; }
        
        static inline Pomi::Graphics& graphics() { return _gfx; }
        
        static const std::uint8_t* backgroundImage();
        
        static void draw();
    
    private:
    
        struct Cookie {
            std::uint8_t version;
            std::uint32_t highscore_a;
            std::uint32_t highscore_b;
        };
        
        static inline void limitFPS(std::uint8_t fps) {
            std::uint32_t wait = _time_last_frame + 1000;
            do {
                _time_last_frame = Pomi::System::currentTimeMillis() * fps;
            } while(_time_last_frame < wait);
        }
        
        static constexpr char* COOKIE_LABEL = "POMIFCTR";
        
        static constexpr Sprite _SMASH_SPRITES[] = { Sprite(186, 119, smash, false), Sprite(149, 119, smash, true), Sprite(62, 119, smash, false) };
        
        static constexpr auto _palette = []{
            std::array<std::uint16_t, 16> pal{};
            for(std::size_t idx = 0; idx < 16; ++idx) {
                pal[idx] = Pomi::Graphics::toRGB565(palette[3 * idx + 0], palette[3 * idx + 1], palette[3 * idx + 2]);
            }
            return pal;
        }();
        
        static Pomi::Graphics _gfx;
        
        static std::uint32_t _time_last_frame;
        
        static AssemblyBay _asm_bay;
        
        static Truck _truck;
        
        static Smashes _smashes;
        
        static Boss _boss;
        
        static Workers _workers;
        
        static GameInfo _game_info;
        
        static Cookie _cookie;
        
        static State * _cur_state;
        static State * _next_state;
};
