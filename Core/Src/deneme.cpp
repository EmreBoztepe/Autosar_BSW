#include "deneme.h"

#define ST_MANUFACTURER_CODE    0x20    //addr 0x00
#define SPI_FAMILY_CODE         0x00    //addr 0x01
#define MEMORY_DENSITY_CODE     0x13    //addr 0x02


#define WREN  0b00000110  // Write Enable
#define WRDI  0b00000100  // Write Disable
#define RDSR  0b00000101  // Read Status Register
#define WRSR  0b00000001  // Write Status Register
#define READ  0b00000011  // Read from Memory Array
#define WRITE 0b00000010  // Write to Memory Array
#define RDID  0b10000011  // Read Identification Page
#define WRID  0b10000010  // Write Identification Page
#define RDLS  0b10000011  // Read Identification Page Lock Status
#define LID   0b10000010  // Lock the Identification Page

// EEPROM Harita Tablosu
const EepromMapEntry EepromSTM::eepromMap[] = {
    {REGION_CALIBRATION, 0x0000}, // Kalibrasyon
    {REGION_DIAGNOSTIC,  0x0100}, // Diagnostik
    {REGION_ENV_PARAMS,  0x0200}  // Çevresel Parametreler
};

// EepromSTM Constructor & Destructor
EepromSTM::EepromSTM(SPI_HandleTypeDef* spiPhy)
    : spiPhyUnit(spiPhy){driverState=EEPROM_INIT_FAIL; }

EepromSTM::~EepromSTM(){}


bool EepromSTM::validateDevice()
{
	uint8_t txData[5] = {RDID, 0x00, 0x00, 0x00, 0xff};
	uint8_t rxData[5] = {0};

	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1, txData, rxData, sizeof(txData), 1000);
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

	// EEPROM üretici kodunu doğrulayın
	if (rxData[4] == ST_MANUFACTURER_CODE) {
		driverState = EEPROM_INITIALIZED;
		return true;
	}

	driverState = EEPROM_INIT_FAIL;
	return false;
}


bool EepromSTM::init()
{
	bool retVal;

	if(HAL_SPI_GetState(spiPhyUnit) != HAL_SPI_STATE_READY)
	{
		retVal = false;
	}
	else
	{
		retVal = validateDevice();
	}

	return retVal;
}

bool EepromSTM::writeEnableInstruction(){
    
    uint8_t txData = WREN;
    bool retVal = 0;

    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    retVal = HAL_SPI_Transmit(&hspi1, &txData, 1, 1000);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

    return retVal;
}

bool EepromSTM::write(EeAddr addr, void* buf, size_t len) {
    
    bool retVal = 0;

    uint8_t* data = static_cast<uint8_t*>(buf);

    const uint16_t pageSize = 256; // EEPROM'un sayfa boyutu

    while (len > 0) 
    {
        // Mevcut sayfada kalan alanı hesapla
        uint16_t bytesToWrite = pageSize - (addr % pageSize); // Mevcut sayfada ne kadar yer kaldı

        if (bytesToWrite > len) 
        {
            bytesToWrite = len; // Eğer kalan veri daha küçükse, hepsini gönder
        }

        retVal = writeEnableInstruction(); //write enable

        uint8_t txHeader[4]; // WRITE + 24-bit adres
        txHeader[0] = WRITE;
        txHeader[1] = static_cast<uint8_t>((addr & 0xFF0000) >> 16); // Adresin üst byte'ı
        txHeader[2] = static_cast<uint8_t>((addr & 0x00FF00) >> 8);  // Adresin orta byte'ı
        txHeader[3] = static_cast<uint8_t>(addr & 0x0000FF);         // Adresin alt byte'ı

        // Yazma işlemini başlat
        HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); // CS'yi aktif et

        // İlk olarak WRITE komutunu ve adresi gönder
        HAL_SPI_Transmit(&hspi1, txHeader, sizeof(txHeader), 1000);

        // Ardından veriyi gönder
        HAL_SPI_Transmit(&hspi1, data, bytesToWrite, 1000);

        HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET); // CS'yi deaktif et

            // Kalan uzunluğu ve adresi güncelle
        len -= bytesToWrite;  // Kalan veri miktarını azalt
        addr += bytesToWrite; // Adresi ileri taşı
        data += bytesToWrite; // Verinin başlangıç konumunu güncelle

        HAL_Delay(4);
    }
    return retVal;
}

bool EepromSTM::read(EeAddr addr, void* buf, size_t len) {
    // READ komutunu ve adresi hazırlayın
    uint8_t txHeader[4];
    txHeader[0] = 0x03; // READ komut kodu
    txHeader[1] = static_cast<uint8_t>((addr & 0xFF0000) >> 16); // Üst byte
    txHeader[2] = static_cast<uint8_t>((addr & 0x00FF00) >> 8);  // Orta byte
    txHeader[3] = static_cast<uint8_t>(addr & 0x0000FF);         // Alt byte

    // SPI ile gönderim ve okuma işlemi
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); // CS'yi aktif et

    // READ komutu ve adresi gönder
    HAL_SPI_Transmit(&hspi1, txHeader, sizeof(txHeader), 1000);

    // Veriyi oku
    HAL_SPI_Receive(&hspi1, static_cast<uint8_t*>(buf), len, 1000);


    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET); // CS'yi pasif et

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
bool EepromSTM::writeEepromRegion(EepromRegion region, void* ramData) {
    // Haritayı tara ve bölgeyi bul
    for (const auto& entry : eepromMap) {
        if (entry.regionId == region) {
            // RAM'deki veriyi EEPROM'a yaz
            return write(entry.startAddr, ramData); //burası muhtemelen gelen
        }
    }
    return false; // Bölge bulunamadı
}
