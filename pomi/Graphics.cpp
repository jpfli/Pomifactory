
// Font redering function based on the implementation in PokittoLib by Jonne Valola (BSD License)

#include <cstdint>
#include "Graphics.h"
#include "hw/Display.h"

using namespace Pomi;
using namespace Pomi::Hw;

void Graphics::setPalette(const uint16_t (&palette_rgb565)[PALETTE_SIZE]) {
    _palette = palette_rgb565;
}

void Graphics::setTransparentColor(std::uint8_t color) {
    _transp_color = color;
}

void Graphics::setFont(const unsigned char* font) {
    _font = font;
}

void Graphics::setClippingRegion() {
    _clip_x = 0;
    _clip_y = 0;
    _clip_w = width();
    _clip_h = height();
}

void Graphics::setClippingRegion(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h) {
    if(x < 0) {
        w += (x + w < 0) ? 0 : x;
        x = 0;
    }
    if(x + w > width()) {
        x = (x >= width()) ? width() : x;
        w = width() - x;
    }
    
    if(y < 0) {
        h += (y + h < 0) ? 0 : y;
        y = 0;
    }
    if(y + h > height()) {
        y = (y >= height()) ? height() : y;
        h = height() - y;
    }
    
    _clip_x = x;
    _clip_y = y;
    _clip_w = w;
    _clip_h = h;
}

void Graphics::clear(std::uint8_t color) {
    Display::beginStream();
    Display::writeData(_palette[color]);
    for(std::uint32_t count = 1; count < width() * height(); ++count) {
        Display::writeData();
    }
}

void Graphics::drawPixel(std::int32_t x, std::int32_t y, std::uint8_t color) {
    if((static_cast<std::uint32_t>(x) < width()) || (static_cast<std::uint32_t>(y) < height())) {
        Display::setY(y);
        Display::setX(x);
        
        Display::beginStream();
        Display::writeData(_palette[color]);
    }
}

void Graphics::drawHLine(std::int32_t x, std::int32_t y, std::int32_t w, std::uint8_t color) {
    if((y < _clip_y) || (y >= _clip_y + _clip_h)) {
        return;
    }
    
    if(x < _clip_x) {
        w -= _clip_x - x;
        x = _clip_x;
    }
    if(x + w > _clip_x + _clip_w) {
        w = _clip_x + _clip_w - x;
    }
    
    Display::setY(y);
    Display::setX(x);
    
    Display::beginStream();
    Display::writeData(_palette[color]);
    while(w > 1) {
        Display::writeData();
        --w;
    }
}

void Graphics::drawVLine(std::int32_t x, std::int32_t y, std::int32_t h, std::uint8_t color) {
    if((x < _clip_x) || (x >= _clip_x + _clip_w)) {
        return;
    }
    
    if(y < _clip_y) {
        h -= _clip_y - y;
        y = _clip_y;
    }
    if(y + h > _clip_y + _clip_h) {
        h = _clip_y + _clip_h - y;
    }
    
    Display::setY(y);
    Display::setX(x);
    
    Display::setDirection(Display::Direction::VERTICAL);
    Display::beginStream();
    
    Display::writeData(_palette[color]);
    while(h > 1) {
        Display::writeData();
        --h;
    }
    Display::setDirection(Display::Direction::HORIZONTAL);
}

void Graphics::drawRectangle(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, std::uint8_t color) {
    drawHLine(x, y, w - 1, color);
    drawVLine(x + w - 1, y, h - 1, color);
    drawHLine(x + 1, y + h - 1, w - 1, color);
    drawVLine(x, y + 1, h - 1, color);
}

void Graphics::drawFilledRectangle(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, std::uint8_t color) {
    if((x + w < _clip_x) || (x >= _clip_x + _clip_w)) {
        return;
    }
    if((y + h < _clip_y) || (y >= _clip_y + _clip_h)) {
        return;
    }
    
    if(x < _clip_x) {
        w -= _clip_x - x;
        x = _clip_x;
    }
    if(x + w > _clip_x + _clip_w) {
        w = _clip_x + _clip_w - x;
    }
    
    if(y < _clip_y) {
        h -= _clip_y - y;
        y = _clip_y;
    }
    if(y + h > _clip_y + _clip_h) {
        h = _clip_y + _clip_h - y;
    }
    
    Display::setHorizontalRange(x, x + w - 1);
    Display::setY(y);
    Display::setX(x);
    
    unsigned int count = w * h;
    
    Display::beginStream();
    Display::writeData(_palette[color]);
    while(count > 1) {
        Display::writeData();
        --count;
    }
    
    Display::setHorizontalRange(0, width() - 1);
}

void Graphics::drawImage(const std::uint8_t* bitmap, std::int32_t x_src, std::int32_t y_src, std::int32_t w, std::int32_t h, std::int32_t x_dst, std::int32_t y_dst, bool flip_x, bool flip_y) {
    if((x_dst + w < _clip_x) || (x_dst >= _clip_x + _clip_w) || (y_dst + h < _clip_y )|| (y_dst >= _clip_y + _clip_h)) {
        return; // Bitmap is off drawing area
    }
    
    std::int32_t w_bmp = ~1 & (bitmap[0] + 1); // make width multiple of 2
    bitmap += 2; // Move to the start of pixel data
    
    // Vertically crop image to the clipping region
    if(y_dst < _clip_y) {
        y_src = _clip_y - y_dst;
        h -= _clip_y - y_dst;
        y_dst = _clip_y;
    }
    if(y_dst + h >= _clip_y + _clip_h) {
        h = _clip_y + _clip_h - y_dst;
    }
    
    // Horizontally crop image to the clipping region
    if(x_dst < _clip_x) {
        x_src += _clip_x - x_dst;
        w -= _clip_x - x_dst;
        x_dst = _clip_x;
    }
    if(x_dst + w > _clip_x + _clip_w) {
        w = _clip_x + _clip_w - x_dst;
    }
    
    std::int8_t x_inc = 1;
    std::int32_t x_end = x_src + w;
    if(flip_x) {
        x_inc = -1;
        x_end = x_src - 1;
        x_src = x_end + w;
    }
    
    std::int8_t y_inc = 1;
    std::int32_t y_end = y_src + h;
    if(flip_y) {
        y_inc = -1;
        y_end = y_src - 1;
        y_src = y_end + h;
    }
    
    for(std::int32_t y_pos = y_src; y_pos != y_end; y_pos += y_inc) {
        std::uint32_t skip = 0;
        // Jump to the beginning of row
        Display::setY(y_dst + (y_pos - y_src) * y_inc);
        Display::setX(x_dst);
        Display::beginStream();
        
        for(std::int32_t x_pos = x_src; x_pos != x_end; x_pos += x_inc) {
            std::uint32_t color = bitmap[(y_pos * w_bmp + x_pos) / 2];
            color = (x_pos & 1) ? (color & 0xf) : (color >> 4);
            
            if(color != _transp_color) {
                if(skip > 0) {
                    // Jump to current position on this row
                    Display::setX(x_dst + (x_pos - x_src) * x_inc);
                    Display::beginStream();
                    skip = 0;
                }
                Display::writeData(_palette[color]);
            }
            else {
                // Just ignore transparent pixels until next non-transparent pixel
                ++skip;
            }
        }
    }
    
//     // This alternative loop works on emulator but not quite right on hw
//     Display::setHorizontalRange(x_dst, x_dst + w - 1);
//     Display::setY(y_dst);
//     Display::setX(x_dst);
//     Display::beginStream();
    
//     std::uint32_t count = 0;
//     for(std::int32_t y_pos = y_src; y_pos != y_end; y_pos += y_inc) {
//         for(std::int32_t x_pos = x_src; x_pos != x_end; x_pos += x_inc) {
//             std::uint8_t color = bitmap[(y_pos * w_bmp + x_pos) / 2];
//             color = (x_pos & 1) ? (color & 0xf) : (color >> 4);
            
//             if(color != _transp_color) {
//                 if(count > 0) {
//                     // Jump to current position on this row
//                     Display::setX(x_dst + (x_pos - x_src) * x_inc);
//                     Display::beginStream();
//                     count = 0;
//                 }
//                 Display::writeData(_palette[color]);
//             }
//             else {
//                 ++count;
//             }
//         }
//         if(count > 0) {
//             // Jump to next row
//             Display::setY(y_dst + (y_pos - y_src) * y_inc + 1);
//         }
//     }
    
//     Display::setHorizontalRange(0, width() - 1);
// }
}

void Graphics::drawImage1BPP(const std::uint8_t* bitmap, std::uint8_t color, std::int32_t x_src, std::int32_t y_src, std::int32_t w, std::int32_t h, std::int32_t x_dst, std::int32_t y_dst, bool flip_x, bool flip_y) {
    if((x_dst + w < _clip_x) || (x_dst >= _clip_x + _clip_w) || (y_dst + h < _clip_y )|| (y_dst >= _clip_y + _clip_h)) {
        return; // Bitmap is off drawing area
    }
    
    std::int32_t w_bmp = ~7 & (bitmap[0] + 7); // make width multiple of 8
    bitmap += 2; // Move to the start of pixel data
    
    // Vertically crop image to the clipping region
    if(y_dst < _clip_y) {
        y_src = _clip_y - y_dst;
        h -= _clip_y - y_dst;
        y_dst = _clip_y;
    }
    if(y_dst + h >= _clip_y + _clip_h) {
        h = _clip_y + _clip_h - y_dst;
    }
    
    // Horizontally crop image to the clipping region
    if(x_dst < _clip_x) {
        x_src += _clip_x - x_dst;
        w -= _clip_x - x_dst;
        x_dst = _clip_x;
    }
    if(x_dst + w > _clip_x + _clip_w) {
        w = _clip_x + _clip_w - x_dst;
    }
    
    std::int8_t x_inc = 1;
    std::int32_t x_end = x_src + w;
    if(flip_x) {
        x_inc = -1;
        x_end = x_src - 1;
        x_src = x_end + w;
    }
    
    std::int8_t y_inc = 1;
    std::int32_t y_end = y_src + h;
    if(flip_y) {
        y_inc = -1;
        y_end = y_src - 1;
        y_src = y_end + h;
    }
    
    for(std::int32_t y_pos = y_src; y_pos != y_end; y_pos += y_inc) {
        std::uint32_t skip = 0;
        // Jump to the beginning of row
        Display::setY(y_dst + (y_pos - y_src) * y_inc);
        Display::setX(x_dst);
        Display::beginStream();
        
        std::uint32_t bit_mask = 1 << (7 - x_src % 8);
        
        for(std::int32_t x_pos = x_src; x_pos != x_end; x_pos += x_inc) {
            std::uint32_t pixels = bitmap[(y_pos * w_bmp + x_pos) / 8];
            
            if(pixels & bit_mask) {
                if(skip > 0) {
                    // Jump to current position on this row
                    Display::setX(x_dst + (x_pos - x_src) * x_inc);
                    Display::beginStream();
                    skip = 0;
                }
                Display::writeData(_palette[color]);
            }
            else {
                // Just ignore transparent pixels until next non-transparent pixel
                ++skip;
            }
            
            if(x_inc > 0) {
                bit_mask = (bit_mask >> 1) ? (bit_mask >> 1) : (1 << 7); // 8 bit rotate right
            }
            else {
                bit_mask = ((bit_mask << 1) >> 8) ? 1 : (bit_mask << 1); // 8 bit rotate left
            }
        }
    }
}

void Graphics::print(const char* text, std::uint8_t color) {
    const std::int32_t begin_x = _cursor_x;
    _renderText(text, color);
    _cursor_x = begin_x;
}

void Graphics::print(const char* text, std::int32_t x, std::int32_t y, std::uint8_t color) {
    _cursor_x = x;
    _cursor_y = y;
    _renderText(text, color);
}

void Graphics::_renderText(const char* text, std::uint8_t color) {
    const std::uint8_t font_height = *(_font + 1);
    const std::uint8_t font_capsonly = *(_font + 3);
    
    std::int32_t carriage_x = _cursor_x;
    while(*text) {
        if(' ' <= *text) {
            char c = *text;
            if(font_capsonly && c >= 'a') {
                c -= 'a'-'A';
            }
            carriage_x += _renderChar(c, carriage_x, _cursor_y, color);
        }
        else if('\n' == *text) {	    //line feed
            carriage_x = _cursor_x;
            _cursor_y += font_height + 1;
        }
        else if('\r' == *text) {		//carriage return
            carriage_x = _cursor_x;
        }
        ++text;
    }
    _cursor_x = carriage_x;
}

std::uint8_t Graphics::_renderChar(char c, std::int32_t x, std::int32_t y, std::uint8_t color) {
    const std::uint8_t char_width = *_font;
	const std::uint8_t char_height = *(_font + 1);
    const char font_startchar = *(_font + 2);
    
    const std::uint8_t hbytes = (char_height + 7) >> 3; // GLCD fonts are arranged w+1 times h/8 bytes
    
    const std::uint8_t char_idx = c - font_startchar;
    const std::uint8_t* bitmap = _font + 4 + char_idx * (char_width * hbytes + 1);
    const std::uint8_t num_bytes = *bitmap;
    
    for(std::uint8_t i = 0; i < num_bytes; ++i) {
        ++bitmap;
        std::uint16_t bitcolumn = *bitmap;
        if(hbytes > 1) {
            ++bitmap;
            bitcolumn |= (*bitmap) << 8; // Add second byte for 16 bit high fonts
        }
        for(std::uint8_t j = 0; j <= char_height; ++j) {
            if(bitcolumn & 1) {
                drawPixel(x, y + j, color);
            }
            bitcolumn >>= 1;
        }
        ++x;
    }
    return num_bytes + 1;
}
