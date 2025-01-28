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

typedef enum EepromState {
    EEPROM_IDLE,
    WRITE_ENABLE,
    SEND_DATA,
    WAIT_READY
}e_eepromState;

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
};

class EepromSTM : public Eeprom {
public:
    EepromSTM(SPI_HandleTypeDef *spiPyh);
    ~EepromSTM() override;

    bool init();
    e_eepromState getEepromState();
    bool read(EeAddr addr, void* buf, size_t len) override;
    bool startWrite(EeAddr addr, void* data, size_t len);
    void eepromMainFunction();
    

private:
    bool writeEnableInstruction();
    bool validateDevice();
    bool isEepromReady();
    bool write(EeAddr addr, void* buf, size_t len) override;

    static const EepromMapEntry eepromMap[]; // EEPROM Haritası
    const uint16_t pageSize = 256; // EEPROM'un sayfa boyutu
    SPI_HandleTypeDef *spiPhyUnit;
    uint8_t* currentData = nullptr;
    e_EepromStatus driverState;
    e_eepromState currentState;
    EeAddr currentAddress = 0;
    size_t remainingLength = 0;
    size_t bytesToWrite = 0;

};

#endif
