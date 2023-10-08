

#ifndef __24C02_H
#define __24C02_H

#define AT24C02_WADDR   0xA0   //写24c02的地址
#define AT24C02_RADDR   0xA1   //读24c02的地址


unsigned char AT24C02_ReadOneByte(unsigned char);
void AT24C02_WriteOneByte(unsigned char,unsigned char);
void AT24C02_WritePage(unsigned char WriteAddr,unsigned char *DataToWrite);
void AT24C02_ReadPage(unsigned char ReadAddr, unsigned char *Redata);

#endif
















