#include "deneme.h"

class araba {
public:
    int age;
    char name;
    araba(int kmVal);
    void setBit(char bitId);

private:
    int km;
};

araba::araba(int kmVal) : km(kmVal) {}

void araba::setBit(char newAge) {
    age = newAge;
}

// C işlevleri
extern "C" {
    void* araba_create(int kmVal) {
        return new araba(kmVal);
    }

    void araba_setBit(void* arabaPtr, char bitId) {
        static_cast<araba*>(arabaPtr)->setBit(bitId);
    }

    void araba_destroy(void* arabaPtr) {
        delete static_cast<araba*>(arabaPtr);
    }
}
