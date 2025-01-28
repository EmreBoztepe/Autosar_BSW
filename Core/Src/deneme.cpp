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
    : spiPhyUnit(spiPhy),currentData(nullptr),driverState(EEPROM_INIT_FAIL),
    currentState(EEPROM_IDLE),currentAddress(0), remainingLength(0), bytesToWrite(0){}

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

bool EepromSTM::writeEnableInstruction()
{
    uint8_t txData = WREN;
    bool retVal = 0;

    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    retVal = HAL_SPI_Transmit(&hspi1, &txData, 1, 1000);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

    return retVal;
}

bool EepromSTM::write(EeAddr addr, void* buf, size_t len)
{
    bool retVal = 0;

    uint8_t* data = static_cast<uint8_t*>(buf);

    retVal = writeEnableInstruction(); //write enable

    uint8_t txHeader[4]; // WRITE + 24-bit adres
    txHeader[0] = WRITE;
    txHeader[1] = static_cast<uint8_t>((addr & 0xFF0000) >> 16); // Adresin üst byte'ı
    txHeader[2] = static_cast<uint8_t>((addr & 0x00FF00) >> 8);  // Adresin orta byte'ı
    txHeader[3] = static_cast<uint8_t>(addr & 0x0000FF);         // Adresin alt byte'ı

    // Yazma işlemini başlat
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); // CS'yi aktif et

    HAL_SPI_Transmit(&hspi1, txHeader, sizeof(txHeader), 1000);// İlk olarak WRITE komutunu ve adresi gönder
    
    HAL_SPI_Transmit(&hspi1, data, len, 1000);// Ardından veriyi gönder

    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET); // CS'yi deaktif et

    return retVal;
}

bool EepromSTM::read(EeAddr addr, void* buf, size_t len)
{
    uint8_t txHeader[4];
    txHeader[0] = 0x03; // READ komut kodu
    txHeader[1] = static_cast<uint8_t>((addr & 0xFF0000) >> 16); // Üst byte
    txHeader[2] = static_cast<uint8_t>((addr & 0x00FF00) >> 8);  // Orta byte
    txHeader[3] = static_cast<uint8_t>(addr & 0x0000FF);         // Alt byte

    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET); // CS'yi aktif et

    HAL_SPI_Transmit(&hspi1, txHeader, sizeof(txHeader), 1000);// READ komutu ve adresi gönder
    
    HAL_SPI_Receive(&hspi1, static_cast<uint8_t*>(buf), len, 1000);// Veriyi oku

    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET); // CS'yi pasif et

    return true;
}


bool EepromSTM::isEepromReady()
{
    uint8_t txData = RDSR;
    uint8_t rxData = 0;

    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(spiPhyUnit, &txData, 1, 1000);
    HAL_SPI_Receive(spiPhyUnit, &rxData, 1, 1000);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

    return !(rxData & 0x01); // Yazma işlemi tamamlanmışsa hazırdır
}

void EepromSTM::eepromMainFunction() {
    switch (currentState) {
        case EEPROM_IDLE:
            // İşlem bekleniyor, herhangi bir yazma isteği yok
            break;

        case WRITE_ENABLE:
            if (writeEnableInstruction() == HAL_OK)
            {
                currentState = SEND_DATA;
            }
            break;

        case SEND_DATA:
        {
            bytesToWrite = pageSize - (currentAddress % pageSize);
            if (bytesToWrite > remainingLength) {
                bytesToWrite = remainingLength;
            }

            if (write(currentAddress, currentData, bytesToWrite) == HAL_OK)
            {
                remainingLength -= bytesToWrite;
                currentAddress += bytesToWrite;
                currentData += bytesToWrite;

                if (remainingLength == 0)
                {
                    currentState = WAIT_READY;
                }
                else
                {

                }
            }
            else
            {
                currentState = EEPROM_IDLE;
            }
            break;
        }

        case WAIT_READY:

             currentState = EEPROM_IDLE;

            break;
    }
}

bool EepromSTM::startWrite(EeAddr addr, void* data, size_t len)
{
    if (currentState != EEPROM_IDLE)
    {
        return false;
    }

    currentAddress = addr;
    currentData = static_cast<uint8_t*>(data);
    remainingLength = len;
    currentState = WRITE_ENABLE;

    return true;
}

e_eepromState EepromSTM::getEepromState()
{
    return currentState;
}
