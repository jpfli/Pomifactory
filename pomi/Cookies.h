
// Based on PokittoLib's PokittoCookie.cpp by Jonne Valola (BSD License)

#pragma once

#include <cstdint>
#include <cstdlib>
#include "hw/Eeprom.h"

namespace Pomi {

class Cookies {
    
    public:
    
        explicit Cookies() {}
        ~Cookies() {}
        
        Cookies(const Cookies&) = delete;
        Cookies& operator =(const Cookies&) = delete;
        
        template<typename T>
        static inline bool loadCookie(const char* label, T& data_ob) {
            return _loadCookie(label, reinterpret_cast<std::uint8_t*>(&data_ob), sizeof(T));
        }
        
        template<typename T>
        static inline bool saveCookie(const char* label, T& data_ob) {
            return _saveCookie(label, reinterpret_cast<std::uint8_t*>(&data_ob), sizeof(T));
        }
        
        static inline std::int32_t findKey(const char* label) {
            for(std::uint8_t key = 0; MAX_KEYS > key; ++key) {
                if(_keyLabelMatches(key, label)) {
                    return key;
                }
            }
            return -1;
        }
    
    private:
    
        static constexpr unsigned int MAX_KEYS = 48;
        static constexpr unsigned int KEY_LEN = 8;
        static constexpr unsigned int MAX_BLOCKS = 112;
        static constexpr unsigned int BLOCK_SIZE = 32;
        static constexpr unsigned int BLOCK_ALLOCATED_BIT = 0x80;
        static constexpr unsigned int KEY_INDEX_MASK = 0x7F;
        
        static inline bool _loadCookie(const char* label, std::uint8_t* data, std::uint32_t size) {
            std::int32_t key = findKey(label);
            if(key < 0) {
                // Key not found
                return false;
            }
            
            std::uint8_t num_blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
            if(_numAssignedBlocks(key) < num_blocks) {
                // Number of assigned blocks is less than blocks needed for the data
                return false;
            }
            
            constexpr unsigned int OFFSET = MAX_KEYS * KEY_LEN + MAX_BLOCKS;
            
            std::int32_t block = -1;
            std::uint32_t idx = 0;
            while(idx < size) {
                if(idx % BLOCK_SIZE == 0) {
                    block = _findBlock(key, block + 1);
                    if(block < 0) {
                        // Failed to find next block
                        return false;
                    }
                }
                data[idx] = Hw::Eeprom::read(OFFSET + block * BLOCK_SIZE + idx % BLOCK_SIZE);
                ++idx;
            }
            
            return true;
        }
        
        static inline bool _saveCookie(const char* label, const std::uint8_t* data, std::uint32_t size) {
            std::int32_t key = findKey(label);
            if(key < 0) {
                key = _addKey(label);
                if(key < 0) {
                    // No free key slots
                    return false;
                }
            }
            
            std::uint8_t num_blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
            std::uint8_t num_assigned = _numAssignedBlocks(key);
            
            // Free extra allocated blocks if there are any
            while(num_assigned > num_blocks) {
                _deleteBlock(_findLastBlock(key, MAX_BLOCKS));
                --num_assigned;
            }
            
            // Allocate more blocks if needed
            if(num_assigned < num_blocks) {
                if(_numFreeBlocks() < num_blocks - num_assigned) {
                    // Not enough free blocks
                    return false;
                }
                
                while(num_assigned < num_blocks) {
                    _allocateBlock(key);
                    ++num_assigned;
                }
            }
            
            constexpr unsigned int OFFSET = MAX_KEYS * KEY_LEN + MAX_BLOCKS;
            
            std::int32_t block = -1;
            std::uint32_t idx = 0;
            while(idx < size) {
                if(idx % BLOCK_SIZE == 0) {
                    block = _findBlock(key, block + 1);
                    if(block < 0) {
                        // Failed to find next block
                        return false;
                    }
                }
                Hw::Eeprom::write(OFFSET + block * BLOCK_SIZE + idx % BLOCK_SIZE, data[idx]);
                ++idx;
            }
            
            return true;
        }
        
        static inline std::int32_t _addKey(const char* label) {
            // Find free key slot
            std::uint8_t key = 0;
            while(Hw::Eeprom::read(key * KEY_LEN) != '\0') {
                ++key;
                if(key >= MAX_KEYS) {
                    return -1;
                }
            }
            
            // Assign key label to the found free slot
            for(std::uint8_t i_label = 0; i_label < KEY_LEN; ++i_label) {
                Hw::Eeprom::write(key * KEY_LEN + i_label, label[i_label]);
                if(label[i_label] == '\0') {
                    break;
                }
            }
            
            return key;
        }
        
        static inline bool _keyLabelMatches(std::uint8_t key, const char* label) {
            for(std::uint8_t i_label = 0; i_label < KEY_LEN; ++i_label) {
                if(Hw::Eeprom::read(key * KEY_LEN + i_label) != label[i_label]) {
                    return false;
                }
                else if(label[i_label] == '\0') {
                    return i_label > 0;
                }
            }
            return true;
        }
        
        static inline std::uint8_t _numFreeBlocks() {
            std::uint8_t num_blocks = 0;
            for(std::uint8_t i_block = 0; MAX_BLOCKS > i_block; ++i_block) {
                if(_getBlockOwner(i_block) < 0) {
                    ++num_blocks;
                }
            }
            return num_blocks;
        }
        
        static inline std::uint8_t _numAssignedBlocks(std::uint8_t key) {
            std::uint8_t num_blocks = 0;
            for(std::uint32_t i_block = 0; MAX_BLOCKS > i_block; ++i_block) {
                if(_getBlockOwner(i_block) == key) {
                    ++num_blocks;
                }
            }
            return num_blocks;
        }
        
        static inline std::int32_t _getBlockOwner(std::int32_t block) {
            if(MAX_BLOCKS <= block || block < 0) {
                return -1;
            }
            
            std::uint8_t val = Hw::Eeprom::read(MAX_KEYS * KEY_LEN + block);
            return (val & BLOCK_ALLOCATED_BIT) ? (val & KEY_INDEX_MASK) : -1;
        }
        
        static inline std::int32_t _findBlock(std::uint8_t key, std::int32_t start_block) {
            if(0 > start_block) {
                start_block = 0;
            }
            while(MAX_BLOCKS > start_block) {
                if(_getBlockOwner(start_block) == key) {
                    return start_block;
                }
                ++start_block;
            }
            return -1;
        }
        
        static inline std::int32_t _findLastBlock(std::uint8_t key, std::int32_t end_block) {
            if(MAX_BLOCKS - 1 < end_block) {
                end_block = MAX_BLOCKS-1;
            }
            while(0 <= end_block) {
                if(_getBlockOwner(end_block) == key) {
                    return end_block;
                }
                --end_block;
            }
            return -1;
        }
        
        static inline bool _allocateBlock(std::uint8_t key) {
            for(std::int32_t block = 0; MAX_BLOCKS > block; ++block) {
                std::uint8_t val = Hw::Eeprom::read(MAX_KEYS * KEY_LEN + block);
                if(!(val & BLOCK_ALLOCATED_BIT)) {
                    Hw::Eeprom::write(MAX_KEYS * KEY_LEN + block, key | BLOCK_ALLOCATED_BIT);
                    return true;
                }
            }
            return false;
        }
        
        static inline void _deleteBlock(std::int32_t block) {
            if(MAX_BLOCKS > block) {
                Hw::Eeprom::write(MAX_KEYS * KEY_LEN + block, 0);
                for(std::uint32_t idx = 0; BLOCK_SIZE > idx; ++idx) {
                    // wipe the block data
                    Hw::Eeprom::write(MAX_KEYS * KEY_LEN + MAX_BLOCKS + block * BLOCK_SIZE + idx, 0);
                }
            }
        }

};

} // namespace Pomi
