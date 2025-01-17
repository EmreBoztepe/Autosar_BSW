#ifndef DENEME_H
#define DENEME_H

#include <stddef.h>
#include "spi.h"
#include <cstring>

// EEPROM kimlikleri
enum EepromRegion {
    REGION_CALIBRATION,
    REGION_DIAGNOSTIC,
    REGION_ENV_PARAMS
};


typedef enum EepromDriverStatus
{
	EEPROM_INIT_FAIL,
	EEPROM_INITIALIZED
}e_EepromStatus;

typedef uint32_t EeAddr;

// EEPROM Harita Girişi
struct EepromMapEntry {
    EepromRegion regionId;  // Bölge kimliği
    EeAddr startAddr;       // EEPROM başlangıç adresi
};


class Eeprom {
public:
    virtual ~Eeprom() {}
    virtual bool erase() { return false; }
    virtual bool write(EeAddr addr, void* buf, size_t len) = 0;
    virtual bool read(EeAddr addr, void* buf, size_t len) = 0;

        // Template methods
    template <typename T>
    bool write(EeAddr addr,  T& data) {
        return write(addr, &data, sizeof(T));
    }

    template <typename T>
    bool read(EeAddr addr, T& data) {
        return read(addr, &data, sizeof(T));
    }
};

class EepromSTM : public Eeprom {
public:
    EepromSTM(SPI_HandleTypeDef *spiPyh);
    ~EepromSTM() override;

    bool init();
    bool write(EeAddr addr, void* buf, size_t len) override;
    bool read(EeAddr addr, void* buf, size_t len) override;

    // Template metotlar� burada yeniden tan�ml�yoruz
    using Eeprom::write;
    using Eeprom::read;

    // Eklenen Metodlar
    bool readEepromRegion(EepromRegion region, void* ramData);
    bool writeEepromRegion(EepromRegion region, void* ramData);


private:
    e_EepromStatus driverState;
    static const EepromMapEntry eepromMap[]; // EEPROM Haritası
    SPI_HandleTypeDef *spiPhyUnit;
    bool writeEnableInstruction();
    bool validateDevice();

};

#endif
