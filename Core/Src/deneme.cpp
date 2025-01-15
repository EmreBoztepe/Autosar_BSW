#include "deneme.h"

// EEPROM Harita Tablosu
const EepromMapEntry EepromSTM::eepromMap[] = {
    {REGION_CALIBRATION, 0x0000}, // Kalibrasyon
    {REGION_DIAGNOSTIC,  0x0100}, // Diagnostik
    {REGION_ENV_PARAMS,  0x0200}  // Çevresel Parametreler
};

// EepromSTM Constructor & Destructor
EepromSTM::EepromSTM() { 
    
    //RDID komutu
    uint8_t buff[4] = {0x83, 0x00, 0x00, 0x00};

    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, buff, 4, 1000);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}
EepromSTM::~EepromSTM() { /* Kaynak temizleme işlemleri */ }

// EEPROM İşlemleri
bool EepromSTM::erase() {
    // EEPROM silme işlemi
    return true;
}

void EepromSTM::writeInstruction(void){
    
//uint8_t buff = 0x06;
//
//HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
//HAL_SPI_Transmit(&hspi1, &buff, 1, 1000);
//HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

    uint8_t buff[5] = {0x83, 0x00, 0x00, 0x00};

    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, buff, 5, 1000);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}

bool EepromSTM::write(EeAddr addr, const void* buf, size_t len) {
    
    writeInstruction();

    uint8_t buff2[5] = {0x02, 0x00, 0x00, 0x00, 0xAA};
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, buff2, 5, 1000);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

    return true;
}
uint8_t rxData[5];
bool EepromSTM::read(EeAddr addr, void* buf, size_t len) {
    uint8_t buff2[5] = {0x03, 0x00,0x00,0x00,0xff};
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, buff2, rxData, 5, 1000);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
    return true;
}

// Bölgeye Göre EEPROM'dan Okuma
bool EepromSTM::readEepromRegion(EepromRegion region, void* ramData) {
    // Haritayı tara ve bölgeyi bul
    for (const auto& entry : eepromMap) {
        if (entry.regionId == region) {
            // EEPROM'dan RAM'e oku
            return read(entry.startAddr, ramData); 
        }
    }
    return false; // Bölge bulunamadı
}

// Bölgeye Göre EEPROM'a Yazma
bool EepromSTM::writeEepromRegion(EepromRegion region, const void* ramData) {
    // Haritayı tara ve bölgeyi bul
    for (const auto& entry : eepromMap) {
        if (entry.regionId == region) {
            // RAM'deki veriyi EEPROM'a yaz
            return write(entry.startAddr, ramData); //burası muhtemelen gelen
        }
    }
    return false; // Bölge bulunamadı
}
