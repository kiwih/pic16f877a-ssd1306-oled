/* 
 * File:   i2c.h
 * Author: hammond
 *
 * Created on 11 May 2019, 4:15 PM
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common.h"

void I2C_Initialize(const unsigned long feq_K); //begin I2C as master

void I2C_Hold(void);
void I2C_Begin(void);
void I2C_End(void);

void I2C_Write(unsigned data);
unsigned short I2C_Read(unsigned short ack);    


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

