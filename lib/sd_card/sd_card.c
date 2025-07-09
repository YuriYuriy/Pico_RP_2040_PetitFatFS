//----------------------------------------------------------------------------------------------------
// Lib by YuriYuriy 2025
//----------------------------------------------------------------------------------------------------
#include "sd_card.h"

//----------------------------------------------------------------------------------------------------
// Implemented only for Pico RP2040
// If you want to use a other microcontroller. This function need to be implemented
//----------------------------------------------------------------------------------------------------
void sd_card_spi_init(void)
{
    spi_init(SPI_PORT, 300*1000);

    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
}
void sd_card_write(uint8_t sd_card_data_write)
{
    #ifdef SD_CARD_USED_SPI
        spi_write_blocking(SPI_PORT, &sd_card_data_write, 1);
    #endif
    #ifdef SD_CARD_USED_DMA
        // None implemented
    #endif
}
uint8_t sd_card_read(void)
{
    #ifdef SD_CARD_USED_SPI
        uint8_t sd_card_buffer = 0xFF;
        uint8_t sd_card_data_read;;
        spi_write_read_blocking(SPI_PORT, &sd_card_buffer, &sd_card_data_read, 1);
        return sd_card_data_read;
    #endif
    #ifdef SD_CARD_USED_DMA
        // None implemented
    #endif
}
void sd_card_send_cs(uint8_t cs_send)
{
    gpio_put(PIN_CS, cs_send);
}
void sd_card_spi_frequency_work(uint32_t spi_frequency)
{
    spi_init(SPI_PORT, spi_frequency);
}
//----------------------------------------------------------------------------------------------------
// This function don't need to be implemented
//----------------------------------------------------------------------------------------------------
void sd_card_send_command(uint8_t cmd, uint8_t arg1, uint8_t arg2, uint8_t arg3, uint8_t arg4, uint8_t crc)
{
    uint8_t packet_cmd[6] = {cmd, arg1, arg2, arg3, arg4, crc};
    
    for (uint16_t i = 0; i < 6; i++) 
    {
        sd_card_write(packet_cmd[i]);
    }
}
uint8_t sd_card_check_R1(void)
{
    uint8_t result_R1 = 0xFF;

    for (uint16_t i = 0; i < 0xFFFF; i++) 
    {
        result_R1 = sd_card_read();
        if (result_R1 == 0x00 || result_R1 == 0x01) return result_R1;  // OK
    }

    return result_R1;  // Error
}
uint8_t sd_card_check_R3(void)
{
    uint8_t result_R3[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    result_R3[0] = sd_card_check_R1();

    if (result_R3[0] == 0x00 || result_R3[0] == 0x01)
    {
        for (uint8_t i = 1; i < 5; i++)
        {
            result_R3[i] = sd_card_read();
        }
        //printf("R3 response: %02X %02X %02X %02X %02X\n", result_R3[0], result_R3[1], result_R3[2], result_R3[3], result_R3[4]);
        return result_R3[0];  // OK
    }

    return result_R3[0];  // Error
}
uint8_t sd_card_check_R7(void)
{
    uint8_t result_R7[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    result_R7[0] = sd_card_check_R1();

    if (result_R7[0] == 0x00 || result_R7[0] == 0x01)
    {
        for (uint8_t i = 1; i < 5; i++)
        {
            result_R7[i] = sd_card_read();
        }
        return result_R7[4];  // OK
    }

    return result_R7[0];  // Error
}
void sd_card_check_busy(void)
{
    uint8_t result_busy = 0;

    for (uint16_t i = 0; i < 0xFFFF; i++) 
    {     
        result_busy = sd_card_read();
        if (result_busy == 0xFF) break;
    }
}
uint8_t sd_card_init(void)
{
    sd_card_spi_init();

    uint8_t result_init_sd_card = 0;

    for (uint8_t i = 0; i < 8; ++i)
    {
        sd_card_write(0xFF);
    }
    //----------SendCMD0----------------------------------------------------------------------------------
    sd_card_send_cs(0);
    sd_card_send_command(CMD0, 0x00, 0x00, 0x00, 0x00, 0x95);
    //----------CheckCMD0---------------------------------------------------------------------------------
    result_init_sd_card = sd_card_check_R1();
    sd_card_write(0xFF);
    //printf("CMD0: %X\n", result_init_sd_card);
    //----------ErrorCMD0---------------------------------------------------------------------------------
    if (result_init_sd_card != 0x01) 
    {
        sd_card_send_cs(1);
        
        return 1;
    }
    //----------SendCMD8----------------------------------------------------------------------------------
    sd_card_send_command(CMD8, 0x00, 0x00, 0x01, 0xAA, 0x87);
    sleep_ms(10);
    //----------CheckCMD8---------------------------------------------------------------------------------
    result_init_sd_card = sd_card_check_R7();
    sd_card_write(0xFF);
    //printf("CMD8: %X\n", result_init_sd_card);
    //----------ErrorCMD8---------------------------------------------------------------------------------
    if (result_init_sd_card != 0xAA) 
    {
        sd_card_send_cs(1);
        return 1;
    }
    //----------SendACMD58--------------------------------------------------------------------------------
    sd_card_send_command(CMD58, 0x00, 0x00, 0x00, 0x00, 0xFD);
    //----------CheckCMD58--------------------------------------------------------------------------------
    result_init_sd_card = sd_card_check_R3();
    sd_card_write(0xFF);
    //----------ErrorCMD58--------------------------------------------------------------------------------
    if (result_init_sd_card > 0x01) 
    {
        sd_card_send_cs(1);
        return 1;
    }
    //----------SendACMD41--------------------------------------------------------------------------------
    for (uint16_t i = 0; i < 0xFFFF; i++) 
    {
        sd_card_send_command(CMD55, 0x00, 0x00, 0x00, 0x00, 0x65);
        result_init_sd_card = sd_card_check_R1();
        ////printf("CMD55: %X\n", result_init_sd_card);
        sd_card_write(0xFF);

        sd_card_send_command(ACMD41, 0x40, 0x00, 0x00, 0x00, 0x77);
        result_init_sd_card = sd_card_check_R1();
        ////printf("CMD41: %X\n", result_init_sd_card);
        sd_card_write(0xFF);
        if (result_init_sd_card == 0x00) break;
    }
    //----------SendCMD58---------------------------------------------------------------------------------
    if (result_init_sd_card == 0x00)
    {
        sd_card_send_command(CMD58, 0x00, 0x00, 0x00, 0x00, 0xFD);
        //----------CheckCMD58--------------------------------------------------------------------------------
        result_init_sd_card = sd_card_check_R3();
        sd_card_write(0xFF);
        sd_card_send_cs(1);
        //printf("OK INIT: \n");
        sd_card_spi_frequency_work(SD_CARD_FREQUNCY_SPI);
        return 0;
    }
    sd_card_send_cs(1);
    //printf("ERROR ACMD41: \n");
    return 1;
}
uint8_t sd_card_write_fatfs(uint8_t *buff_write, uint8_t data_0, uint8_t data_1, uint8_t data_2, uint8_t data_3)
{   
    uint8_t result_w;
    
    //----------SendCMD24---------------------------------------------------------------------------------
    sd_card_send_cs(0);
    sd_card_send_command(CMD24, data_0, data_1, data_2, data_3, 0xFF);
    //----------CheckCMD24----------------------------------- ---------------------------------------------
    result_w = sd_card_check_R1();
    //printf("CMD24: %X\n", result_w);
    //----------ErrorCMD24--------------------------------------------------------------------------------
    if (result_w != 0x00) 
    {
        sd_card_send_cs(1);
        return 1;
    }
    //----------CheckBusyToken----------------------------------------------------------------------------
    sd_card_check_busy();
    //----------SendStartBlockToken-----------------------------------------------------------------------
    sd_card_write(0xFE);
    //----------SendData----------------------------------------------------------------------------------
    for (uint16_t i = 0; i < 512; i++)
    {
        sd_card_write(buff_write[i]);
    }
    //----------SendCRC-----------------------------------------------------------------------------------
    sd_card_write(0xFF);
    sd_card_write(0xFF);
    //----------CheckResponseToken------------------------------------------------------------------------
    result_w = sd_card_read();
    if (result_w != 0xE5)
    {
        //printf("Data Response Token: %X\n", result_w);
        //printf("ERROR write: %X\n", result_w);
        sd_card_send_cs(1);
        return 1;
    } 
    //printf("Data Response Token: %X\n", result_w);
    //----------CheckBusyToken----------------------------------------------------------------------------
    sd_card_check_busy();
    //printf("OK write:\n");
    sd_card_send_cs(1);
    return 0;
}   
uint8_t sd_card_read_fatfs(uint8_t *buff_read, uint8_t data_0, uint8_t data_1, uint8_t data_2, uint8_t data_3)
{   
    uint8_t buf_CRC[2] = {0, 0};
    uint8_t result_r;
        
    //----------SendCMD17-------------------------------------------------------------
    sd_card_send_cs(0);
    sd_card_send_command(CMD17, data_0, data_1, data_2, data_3, 0xFF);
    //----------CheckCMD17------------------------------------------------------------
    result_r = sd_card_check_R1();
    //printf("CMD17: %X\n", result_r);
    //----------ErrorCMD17------------------------------------------------------------
    if (result_r != 0x00) 
    {
        sd_card_send_cs(1);
        return result_r;
    }
    //----------CheckStartBlockToken--------------------------------------------------
    for (uint16_t i = 0; i < 0xFFFF; i++) 
    {     
        result_r = sd_card_read(); 
        if (result_r == 0xFE) break;
    }
    if (result_r != 0xFE)
    {
        //printf("ERROR read: %X\n", result_r);
        sd_card_send_cs(1);
        return 1;
    } 
    //----------GetData---------------------------------------------------------------
        for (uint16_t i = 0; i < 512; i++)
    {
        buff_read[i] = sd_card_read();
    }
    //----------GetCRC----------------------------------------------------------------
    buf_CRC[0] = sd_card_read(); 
    buf_CRC[1] = sd_card_read(); 
    //printf("OK read:\n");
    sd_card_send_cs(1);
    return 0;
} 