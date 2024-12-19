#include "deneme.h"

// EepromSTM metod tanımları

EepromSTM::EepromSTM() { /* Donanım başlatma kodu */ }

EepromSTM::~EepromSTM() { /* Kaynak temizleme işlemleri */ }

bool EepromSTM::erase() {
    // STM için EEPROM silme işlemi
    return true;
}

bool EepromSTM::write(EeAddr addr, const void* buf, size_t len) {
    // STM için yazma işlemi
    return true;
}

bool EepromSTM::read(EeAddr addr, void* buf, size_t len) {
    // STM için okuma işlemi
    return true;
}
