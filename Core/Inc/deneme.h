#ifndef DENEME_H
#define DENEME_H

#ifdef __cplusplus
extern "C" {
#endif

// C++ sınıfını temsil eden işlev prototipleri
void* araba_create(int kmVal);       // Constructor
void araba_setBit(void* arabaPtr, char bitId); // Method çağrısı
void araba_destroy(void* arabaPtr); // Destructor

#ifdef __cplusplus
}
#endif

#endif
