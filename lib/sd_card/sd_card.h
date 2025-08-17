//----------------------------------------------------------------------------------------------------
// Program to work with an SD card for Petit FatFS
// Copyright (C) 2025 YuriYuri (Yuriy Golovin)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------------------------------
#ifndef _SD_CARD
#define _SD_CARD

#include "main.h"

//----------------------------------------------------------------------------------------------------
// Commands SD Card
//----------------------------------------------------------------------------------------------------
#define CMD0    0x40 + 0   // GO_IDLE_STATE 
#define CMD8    0x40 + 8   // SEND_IF_COND проверки рабочего состояния интерфейса SD-карты
#define CMD10   0x40 + 10  // READ_CSD
#define CMD17   0x40 + 17  // READ_SINGLE_BLOCK
#define CMD24   0x40 + 24  // 
#define CMD55   0x40 + 55  // APP_CMD 
#define CMD58   0x40 + 58  // READ_OCR 
#define ACMD41  0x40 + 41  // SD_SEND_OP_COND инициализации и проверки, завершена ли 

//----------------------------------------------------------------------------------------------------
// Used interface SD Card
//----------------------------------------------------------------------------------------------------
#define SD_CARD_USED_SPI
//#define SD_CARD_USED_DMA

//----------------------------------------------------------------------------------------------------
// Pins for SPI SD Card
//----------------------------------------------------------------------------------------------------
#define SPI_PORT spi0
#define PIN_MISO  16
#define PIN_CS    20
#define PIN_SCK   18
#define PIN_MOSI  19

//----------------------------------------------------------------------------------------------------
// Frequncy SPI SD Card
//----------------------------------------------------------------------------------------------------
#define SD_CARD_FREQUNCY_SPI 20*1000*1000

//----------------------------------------------------------------------------------------------------
// Pins for indicator of work SD Card
//----------------------------------------------------------------------------------------------------
#define PIN_IND_SD_CARD  25
#define INI_IND_SD_CARD  gpio_init(PIN_IND_SD_CARD); gpio_set_dir(PIN_IND_SD_CARD, GPIO_OUT);
#define IND_SD_CARD_OFF  gpio_put(PIN_IND_SD_CARD, 0);
#define IND_SD_CARD_ON   gpio_put(PIN_IND_SD_CARD, 1);

//----------------------------------------------------------------------------------------------------
// Function lib
//----------------------------------------------------------------------------------------------------
uint8_t sd_card_init(void);
uint8_t sd_card_write_fatfs(uint8_t *buff_write, uint8_t data_0, uint8_t data_1, uint8_t data_2, uint8_t data_3);
uint8_t sd_card_read_fatfs(uint8_t *buff_read, uint8_t data_0, uint8_t data_1, uint8_t data_2, uint8_t data_3);

#endif
