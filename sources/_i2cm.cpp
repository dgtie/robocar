/*
 * i2c.cpp
 *
 *  Created on: 29 Oct 2018
 *      Author: ensylam
 */

#include <stm32f10x.h>
#include <lamos.h>

/*
 * PB10 = I2C2_SCL
 * PB11 = I2C2_SDA
 */

namespace
{

void (*state)(int), (*callback)(void);

SERVICE constructor(void) {
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	int i = GPIOB->CRH;
	i &= 0xffff00ff;
	i |= 0x0000ee00;	// e => AFOD-2MHz
	GPIOB->CRH = i;
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;	// 36 MHz
	I2C2->CR2 = 36;
	I2C2->CCR = I2C_CCR_FS | 30;	// 30 * 3 * 400 = 36000
	I2C2->TRISE = 11;
	NVIC_EnableIRQ(I2C2_EV_IRQn);
	NVIC_EnableIRQ(I2C2_ER_IRQn);
	return [](Service&){
          if (callback) (*callback)();
          callback = 0;
        };
}

Service master(constructor());

char address; int count;
volatile char *buffer;
int read_cnt; char *read_buf;

void ev5(int);

void ev8(int sr) {
	if (count) {
		I2C2->DR = *buffer++;
		count--;
	} else if (sr & I2C_SR1_BTF) {
		if (read_buf) {
			address |= 1; count = read_cnt; buffer = read_buf; state = ev5;
			I2C2->CR1 |= I2C_CR1_START;
			I2C2->DR = address;	// aim to clear TXE and BTF
		} else {
			I2C2->CR1 = I2C_CR1_STOP;
			I2C2->CR2 &= ~ (I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);
		}
	}
}

void ev7(int sr) {
	if (sr & I2C_SR1_BTF) {
		if (count == 2)	I2C2->CR1 = I2C_CR1_STOP;
		else {
			I2C2->CR1 &= ~I2C_CR1_ACK;
			I2C2->CR2 |= I2C_CR2_ITBUFEN;
		}
		*buffer++ = I2C2->DR; count--;
	}
	*buffer++ = I2C2->DR;
	if (!--count) {
		read_buf = 0;
		I2C2->CR2 &= ~ (I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);
	}
	else if (count == 1) {
		I2C2->CR1 = I2C_CR1_STOP;
		return master.append(0);
	}
	if (count == 3) I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
}

void ev6(int) {
	I2C2->SR2;		// clear ADDR
	if (count) {
		if (address & 1) {
			state = ev7;
			if (count == 2) {
				I2C2->CR1 &= ~I2C_CR1_ACK;
				I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
			}
			else I2C2->CR1 |= count == 1 ? I2C_CR1_STOP : I2C_CR1_ACK;
			if (count == 3) I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
		} else state = ev8;
	} else {
		read_buf = 0;
		I2C2->CR1 |= I2C_CR1_STOP;
		I2C2->CR2 &= ~ (I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);
	}
}

void ev5(int){
	I2C2->DR = address;
	state = ev6;
	if (count == 2) if (address & 1) I2C2->CR1 |= I2C_CR1_POS | I2C_CR1_ACK;
}

} //anonymous

void i2cm_write(char addr, int n, char *c) {
	I2C2->CR1 = I2C_CR1_PE;							// enable peripheral
	address = addr; count = n; buffer = c;
	state = ev5;
	I2C2->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN;
	I2C2->CR1 |= I2C_CR1_START;
}

void i2cm_read(char addr, char reg, int n, char *c, void (*f)(void)){
	*c = reg; callback = f;
	read_cnt = n; read_buf = c;
	i2cm_write(addr, 1, c);
}

namespace user
{

bool i2cmWrite(char addr, int n, char *c) {
	i2cm_write(addr, n, c);
	while (I2C2->CR2 & I2C_CR2_ITEVTEN);
	while (I2C2->SR2 & I2C_SR2_BUSY);
	I2C2->CR1 = 0;									// disable peripheral
	return buffer;
}

bool i2cmRead(char addr, char reg, int n, char *c) {
	read_cnt = n; read_buf = c;
	return i2cmWrite(addr, 1, &reg);
}

} // user

extern "C" {

void I2C2_EV_IRQHandler(void) {	(*state)(I2C2->SR1); }

void I2C2_ER_IRQHandler(void) {
	if (I2C2->SR1 & I2C_SR1_AF) I2C2->CR1 = I2C_CR1_STOP;
	// else I don't know what to do
	buffer = read_buf = 0;	// indicates failure
	I2C2->CR2 &= ~ (I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);
}

}
