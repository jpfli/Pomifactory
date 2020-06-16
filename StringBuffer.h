
#pragma once

#include <cstdint>
#include <cstring>
#include <algorithm>

class StringBufferBase {
    
    public:
    
        template<unsigned Size>
        constexpr explicit StringBufferBase(char (&buffer)[Size]) : _buffer(buffer), _len(0), _capacity(Size - 1) {}
        
        virtual ~StringBufferBase() = default;
        
        StringBufferBase(const StringBufferBase&) = delete;
        StringBufferBase& operator =(const StringBufferBase&) = delete;
        
        // Conversion to const char*
        constexpr operator const char*() const { return _buffer; }
        constexpr unsigned int length() const { return _len; }
        constexpr unsigned int capacity() const { return _capacity; }
        
        void append(const char* str) { _stringReplace(_len, _len, str); }
        void insert(std::uint32_t idx, const char* str) { _stringReplace(idx, idx, str); }
        void replace(std::uint32_t start, std::uint32_t end, const char* str) { _stringReplace(start, end, str); }
        
        void append(std::int32_t val) { _intReplace(_len, _len, val); }
        void insert(std::uint32_t idx, std::int32_t val) { _intReplace(idx, idx, val); }
        void replace(std::uint32_t start, std::uint32_t end, std::int32_t val) { _intReplace(start, end, val); }
        
        void remove(std::uint32_t start, std::uint32_t end) { _stringReplace(start, end, ""); }
        void reverse() { _reverse(0, _len); }
    
    private:
    
        // Inserts a string between start and end indices
        void _stringReplace(std::uint32_t start, std::uint32_t end, const char* str) {
            if(start > _len) {
                start = _len;
            }
            if(end > _len) {
                end = _len;
            }
            if(start > end) {
                return;
            }
            
            std::uint32_t mid_len = strlen(str);
            std::uint32_t tail_len = 0;
            if(start + mid_len < _capacity) {
                tail_len = _len - end;
                if(start + mid_len + tail_len > _capacity) {
                    tail_len = _capacity - (start + mid_len);
                }
                // Copy the end half of the string to it's new location
                memmove(&_buffer[start + mid_len], &_buffer[end], tail_len);
            }
            else {
                mid_len = _capacity - start;
            }
            // Copy the new string to the middle
            memcpy(&_buffer[start], str, mid_len);
            
            _len = start + mid_len + tail_len;
            _buffer[_len] = '\0';
        }
        
        // Inserts a number between start and end indices
        void _intReplace(std::uint32_t start, std::uint32_t end, std::int32_t val, std::uint8_t base=10) {
            if(start > _len) {
                start = _len;
            }
            if(end > _len) {
                end = _len;
            }
            if(start > end) {
                return;
            }
            
            std::uint32_t num_len = (val < 0) ? 2 : 1;
            
            std::uint32_t val_abs = (val < 0) ? -val : val;
            std::uint32_t temp = val_abs;
            while(10 <= temp) {
                temp /= base;
                ++num_len;
            }
            
            std::uint32_t tail_len = 0;
            if(start + num_len < _capacity) {
                tail_len = _len - end;
                if(start + num_len + tail_len > _capacity) {
                    tail_len = _capacity - (start + num_len);
                }
                // Copy the end half of the string to it's new location
                memmove(&_buffer[start + num_len], &_buffer[end], tail_len);
            }
            else {
                // Number is too big to fit in the string
                while(num_len > _capacity - start) {
                    val_abs /= base;
                    --num_len;
                }
            }
            
            std::uint32_t idx = start + num_len - 1;
            do {
                std::uint8_t rem = val_abs % base;
                val_abs /= base;
                _buffer[idx] = (10 > rem) ? '0' + rem : 'A' + rem - 10;
                --idx;
            } while(0 < val_abs);
            
            if(val < 0) {
                _buffer[idx] = '-';
            }
            
            _len = start + num_len + tail_len;
            _buffer[_len] = '\0';
        }

        void _reverse(std::uint32_t start, std::uint32_t end) {
            while(start < end) {
                std::swap(_buffer[start], _buffer[end]);
                ++start;
                --end;
            }
        }
        
        char* const _buffer; // const pointer to non-const char
        std::uint32_t _len;
        const std::size_t _capacity;
};

template<unsigned Capacity>
class StringBuffer: public StringBufferBase {
    
    public:
    
        constexpr StringBuffer() : StringBufferBase(_buffer) {}
        
        StringBuffer(const char* text) : StringBufferBase(_buffer) {
            StringBufferBase::append(text);
        }
    
    private:
    
        char _buffer[Capacity+1];
};
