#ifndef DENEME_H
#define DENEME_H

#include <stddef.h>

typedef unsigned int EeAddr;

class Eeprom
{
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

class EepromSTM : public Eeprom
{
public:
    EepromSTM();
    ~EepromSTM() override;

    bool erase() override;
    bool write(EeAddr addr, const void* buf, size_t len) override;
    bool read(EeAddr addr, void* buf, size_t len) override;

    // Template metotlarý burada yeniden tanýmlýyoruz
    using Eeprom::write;
    using Eeprom::read;
};

#endif
