
#include <ee.h>

uint32_t getHexAddressPage(int dataPage) {
	uint32_t bits       = PAGE_SECTOR * dataPage;
	uint32_t hexAddress = FLASH_INIT + bits;
	return hexAddress;
}

void memoryPageErase (uint32_t memoryPage) {
	HAL_StatusTypeDef eraseHandler;

	HAL_FLASH_Unlock();

	eraseHandler = FLASH_WaitForLastOperation(500);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR);
	FLASH_PageErase(FLASH_BANK_1, memoryPage);

	eraseHandler = FLASH_WaitForLastOperation(500);
	CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
	HAL_FLASH_Lock();
}

void writeData(uint32_t hexPage, int dataA, int dataB, int dataC, char dataD, float dataE) {
	/* Raman: Note: It is important to erase before writing */
	memoryPageErase(DATA_PAGE);

	/* Type punning */
	union {
	  float    from;
	  uint64_t to;
	} pun = { .from = dataE };

	HAL_FLASH_Unlock();

	HAL_FLASH_Program(TYPEPROGRAM_DOUBLEWORD, hexPage, dataA);
	HAL_FLASH_Program(TYPEPROGRAM_DOUBLEWORD, hexPage + DATA_SPACE, dataB);
	HAL_FLASH_Program(TYPEPROGRAM_DOUBLEWORD, hexPage + (DATA_SPACE*2), dataC);
	HAL_FLASH_Program(TYPEPROGRAM_DOUBLEWORD, hexPage + (DATA_SPACE*3), dataD);
	// Raman:
	HAL_FLASH_Program(TYPEPROGRAM_DOUBLEWORD, hexPage + (DATA_SPACE*4), pun.to /* (uint64_t)dataE */);

	HAL_FLASH_Lock();
}

uint32_t retrieveDataFromAddress (uint32_t hexAddress) {
	return *(uint32_t*)hexAddress;
}

float retrieveDataFromAddressFloat (uint32_t hexAddress) {
	return *(float*)&hexAddress;
}
