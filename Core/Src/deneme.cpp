#include "deneme.h"

// EEPROM Harita Tablosu
const EepromMapEntry EepromSTM::eepromMap[] = {
    {REGION_CALIBRATION, 0x0000}, // Kalibrasyon
    {REGION_DIAGNOSTIC,  0x0100}, // Diagnostik
    {REGION_ENV_PARAMS,  0x0200}  // Çevresel Parametreler
};

// EepromSTM Constructor & Destructor
EepromSTM::EepromSTM() { /* Donanım başlatma kodu */ }
EepromSTM::~EepromSTM() { /* Kaynak temizleme işlemleri */ }

// EEPROM İşlemleri
bool EepromSTM::erase() {
    // EEPROM silme işlemi
    return true;
}

bool EepromSTM::write(EeAddr addr, const void* buf, size_t len) {
    // EEPROM yazma işlemi
    return true;
}

bool EepromSTM::read(EeAddr addr, void* buf, size_t len) {
    // EEPROM okuma işlemi
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
