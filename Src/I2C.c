#include "i2c.h"

#define Min_PCLK_for_I2C_100kHz    2000000 /* Hz */
#define I2C_100kHz  100000 /* Hz */
#define TimeOut_I2C 1000

/* I2C 2 channel 100 kHz SCL */
void I2C1_Init(uint32_t ClockSpeed) {
    uint32_t PCLK_MHz;
    if (ClockSpeed < Min_PCLK_for_I2C_100kHz) {
#ifdef __DEBUG
        printf("PCLK2 low 2 MHz!");
#endif
        while (1);
    }
    PCLK_MHz = ClockSpeed / 1000000UL;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	I2C1->CR1 = I2C_CR1_SWRST;
    I2C1->CR1 = 0; // SWRST=0, PE=0
	I2C1->CR2 = PCLK_MHz; // 16 MHz
	I2C1->CCR = ClockSpeed / (2 * I2C_100kHz); //80;
	I2C1->TRISE = PCLK_MHz + 1; //17;
	I2C1->CR1 |= I2C_CR1_PE; // Enable I2C
}

void I2C1_DeInit(void) {
	I2C1->CR1 = 0;
	RCC->APB1ENR &= ~(RCC_APB1ENR_I2C1EN);
}

bool I2C_SendStart(void) {
    int Timeout = TimeOut_I2C;

	I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB) && --Timeout);
#ifdef __DEBUG
    if (Timeout == 0) printf("Timeout I2C1 send Start");
#endif
    return (Timeout > 0);
}

bool I2C_SendAddress(uint8_t Address) {
    uint16_t Status;
    int Timeout = TimeOut_I2C;

	Status = I2C1->SR1;
    I2C1->DR = Address;
    while ((((Status = I2C1->SR1) & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0) && (--Timeout));
    Status = I2C1->SR2;
#ifdef __DEBUG
    if (Timeout == 0) printf("Timeout I2C1 send Address");
#endif
    return (Status & I2C_SR1_ADDR);
}

bool I2C_SendData(uint8_t Data) {
    int Timeout = TimeOut_I2C;

	I2C1->DR = Data;
    while(!(I2C1->SR1 & I2C_SR1_TXE) && (--Timeout));
#ifdef __DEBUG
    if (Timeout == 0) printf("Timeout I2C1 send Data");
#endif
    return (Timeout > 0);
}

bool I2C_ReceiveData(uint8_t *Data) {
		uint8_t Result;
    int Timeout = TimeOut_I2C;

	while(!(I2C1->SR1 & I2C_SR1_RXNE) && (--Timeout));
    Result = I2C1->DR;
    *Data = Result;
#ifdef __DEBUG
    if (Timeout == 0) printf("Timeout I2C2 receive Data");
#endif
    return (Timeout > 0);
}

bool I2C_SendStop(void) {
    int Timeout = TimeOut_I2C;
   
		I2C1->CR1 |= (I2C_CR1_STOP);
    while((I2C1->SR2 & I2C_SR2_BUSY) && --Timeout);
#ifdef __DEBUG
    if (Timeout == 0) printf("Timeout I2C1 send Stop");
#endif
    return (0);
}

bool I2C_SendByte(uint8_t Address, uint8_t Register, uint8_t Data) {
    if(!I2C_SendStart()) return (0);
    if(!I2C_SendAddress(Address)) return I2C_SendStop();
    if(!I2C_SendData(Register)) return I2C_SendStop();
    if(!I2C_SendData(Data)) return I2C_SendStop();
    I2C_SendStop();
    return (1);
}

bool I2C_ReceiveWord(uint8_t Address, uint8_t Register, uint16_t *Data) {
		uint8_t Temp;
    if(!I2C_SendStart()) return (0);
    if(!I2C_SendAddress(Address)) return I2C_SendStop();
    if(!I2C_SendData(Register)) return I2C_SendStop();
    if(!I2C_SendStart()) return (0);
    if(!I2C_SendAddress(Address | 1)) return I2C_SendStop();
		I2C1->CR1 |= I2C_CR1_ACK;
		if(!I2C_ReceiveData(&Temp));
		*Data = (uint16_t)Temp;
		I2C1->CR1 &= ~(I2C_CR1_ACK);
    I2C_SendStop();
    if(!I2C_ReceiveData(&Temp));
		*Data |= Temp << 8;
    return (1);
}

bool I2C_ReceiveDWord(uint8_t Address, uint8_t Register, uint16_t *Data1, uint16_t *Data2) {
		uint8_t Temp;
    if(!I2C_SendStart()) return (0);
    if(!I2C_SendAddress(Address)) return I2C_SendStop();
    if(!I2C_SendData(Register)) return I2C_SendStop();
    if(!I2C_SendStart()) return (0);
    if(!I2C_SendAddress(Address | 1)) return I2C_SendStop();
		I2C1->CR1 |= I2C_CR1_ACK;
		if(!I2C_ReceiveData(&Temp));
		*Data1 = (uint16_t)Temp;
		if(!I2C_ReceiveData(&Temp));
		*Data1 |= Temp << 8;
		if(!I2C_ReceiveData(&Temp));
		*Data2 = (uint16_t)Temp;
		I2C1->CR1 &= ~(I2C_CR1_ACK);
    I2C_SendStop();
    if(!I2C_ReceiveData(&Temp));
		*Data2 |= Temp << 8;
    return (1);
}
