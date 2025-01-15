#ifndef DENEME_H
#define DENEME_H

#include <stddef.h>
#include "spi.h"
// EEPROM kimlikleri
enum EepromRegion {
    REGION_CALIBRATION,
    REGION_DIAGNOSTIC,
    REGION_ENV_PARAMS
};

typedef unsigned int EeAddr;

// EEPROM Harita Girişi
struct EepromMapEntry {
    EepromRegion regionId;  // Bölge kimliği
    EeAddr startAddr;       // EEPROM başlangıç adresi
};

// Eeprom Sınıfı
class Eeprom {
public:
    virtual ~Eeprom() {}
    virtual bool erase() { return false; }
    virtual bool write(EeAddr addr, const void* buf, size_t len) = 0;
    virtual bool read(EeAddr addr, void* buf, size_t len) = 0;

        // Template methods
    template <typename T>
    bool write(EeAddr addr, const T& data) {
        return write(addr, &data, sizeof(T));
    }

    template <typename T>
    bool read(EeAddr addr, T& data) {
        return read(addr, &data, sizeof(T));
    }
};

class EepromSTM : public Eeprom {
public:
    EepromSTM();
    ~EepromSTM() override;

    bool erase() override;
    bool write(EeAddr addr, const void* buf, size_t len) override;
    bool read(EeAddr addr, void* buf, size_t len) override;

    // Template metotlar� burada yeniden tan�ml�yoruz
    using Eeprom::write;
    using Eeprom::read;

    // Eklenen Metodlar
    bool readEepromRegion(EepromRegion region, void* ramData);
    bool writeEepromRegion(EepromRegion region, const void* ramData);

private:
    static const EepromMapEntry eepromMap[]; // EEPROM Haritası
    void writeInstruction(void);
};

#endif
