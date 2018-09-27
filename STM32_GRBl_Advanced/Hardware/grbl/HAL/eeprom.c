#include "eeprom.h"
#include <string.h>


static uint8_t EepromData[EEPROM_SIZE];


void EE_Init(void)
{
	memcpy(EepromData, (uint8_t*)EEPROM_START_ADDRESS, EEPROM_SIZE);
}

uint8_t EE_ReadByte(uint16_t VirtAddress)
{
	return EepromData[VirtAddress];
}

void EE_WriteByte(uint16_t VirtAddress, uint8_t Data)
{
	EepromData[VirtAddress] = Data;
}

uint8_t EE_ReadByteArray(uint8_t *DataOut, uint16_t VirtAddress, uint16_t size)
{
	uint8_t data, checksum = 0;

	for(; size > 0; size--) {
		data = EE_ReadByte(VirtAddress++);
		checksum = (checksum << 1) || (checksum >> 7);
		checksum += data;
		*(DataOut++) = data;
	}

	data = EE_ReadByte(VirtAddress);
	if(data == checksum) {
		return 1;
	}

	return 0;
}

void EE_WriteByteArray(uint16_t VirtAddress, uint8_t *DataIn, uint16_t size)
{
	unsigned char checksum = 0;

	for(; size > 0; size--) {
		checksum = (checksum << 1) || (checksum >> 7);
		checksum += *DataIn;
		EE_WriteByte(VirtAddress++, *(DataIn++));
	}

	EE_WriteByte(VirtAddress, checksum);
}

void EE_Program(void)
{
	uint16_t data = 0;
	EE_Erase();

//	FLASH_Unlock();
	HAL_FLASH_Unlock();

//	for(uint16_t i = 0; i < EEPROM_SIZE; ++i) {
////		FLASH_ProgramByte(EEPROM_START_ADDRESS + i, EepromData[i]);
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,EEPROM_START_ADDRESS + i, EepromData[i]);
//	}
	
		for(uint16_t i = 0; i < EEPROM_SIZE/2; ++i) {
//		FLASH_ProgramByte(EEPROM_START_ADDRESS + i, EepromData[i]);
			data =  (EepromData[2*i]) | (EepromData[2*i+1]<<8); //HAL����С�ǰ���(16λ)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,EEPROM_START_ADDRESS + i*2, data);//��ַ����������������⣬addr_offset������ż��
	}

//	FLASH_Lock();
		HAL_FLASH_Lock();
}

void EE_Erase(void)
{
//	FLASH_Unlock();
  HAL_FLASH_Unlock();
//��ʼ��FLASH_EraseInitTypeDef     
	FLASH_EraseInitTypeDef f;     
	f.TypeErase = FLASH_TYPEERASE_PAGES;     
	f.PageAddress = EEPROM_START_ADDRESS;     
	f.NbPages = 1;     //����PageError ����һ������    
	uint32_t PageError = 0;     //���ò�������     
	HAL_FLASHEx_Erase(&f, &PageError);
//	FLASH_EraseSector(FLASH_SECTOR, VOLTAGE_RANGE);

//	FLASH_Lock();
	HAL_FLASH_Lock();
}


/* ���� */
#if 0
uint32_t writeFlashData = 0x55555555;
uint32_t addr = 0x08007000;
uint32_t addr_offset = 0;
uint8_t writeFlashData1 = 0x55;

//FLASHд�����ݲ���
void writeFlashTest(void)
{
		uint8_t temp1 = *(__IO uint8_t*)(addr);//û����������ȡ�����ݾ���0xff
	uint16_t temp2 = *(__IO uint16_t*)(addr);
	uint32_t temp3 = *(__IO uint32_t*)(addr);
	
//	uint16_t xx = writeFlashData & 0xff;
//	writeFlashData = xx | temp;
	printf("data1:0x%x\r\n",temp1);
	printf("data2:0x%x\r\n",temp2);
	printf("data3:0x%x\r\n",temp3);
	
    //1������FLASH
  HAL_FLASH_Unlock();

    //2������FLASH
    //��ʼ��FLASH_EraseInitTypeDef
    FLASH_EraseInitTypeDef f;
    f.TypeErase = FLASH_TYPEERASE_PAGES;
    f.PageAddress = addr;
    f.NbPages = 1;
    //����PageError
    uint32_t PageError = 0;
    //���ò�������
    HAL_FLASHEx_Erase(&f, &PageError);
	
//	uint8_t temp1 = *(__IO uint8_t*)(addr);//û����������ȡ�����ݾ���0xff
//	uint16_t temp2 = *(__IO uint16_t*)(addr);
//	uint32_t temp3 = *(__IO uint32_t*)(addr); 
temp2 &= 0xff00;
   uint16_t writeFlashData2 =  writeFlashData1 | temp2;
//	
////	uint16_t xx = writeFlashData & 0xff;
////	writeFlashData = xx | temp;
//	printf("data1:0x%x\r\n",temp1);
//	printf("data2:0x%x\r\n",temp2);
//	printf("data3:0x%x\r\n",temp3);

    //3����FLASH��д
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr+addr_offset, writeFlashData2);//��ַ����������������⣬addr_offset������ż��

    //4����סFLASH
  HAL_FLASH_Lock();
}

//FLASH��ȡ���ݲ���
void printFlashTest(void)
{
	uint32_t addr_a = addr+addr_offset;
  uint32_t temp = *(__IO uint32_t*)(addr_a);
	uint32_t temp1 = *(__IO uint32_t*)(addr_a+1);

    printf("addr:0x%x, data:0x%x\r\n", addr_a, temp);
	printf("addr+1:0x%x, data:0x%x\r\n", addr_a+1, temp1);
}
#endif
