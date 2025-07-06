/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"

// Add YuriYuriy 2025
static uint8_t write_buffer[512];
static uint16_t write_index = 0;
static DWORD write_sector = 0;

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void)
{
	DSTATUS stat;

	// Add YuriYuriy 2025 ----------------------------------------
	uint8_t status_sd_card_init = sd_card_init();
	if (status_sd_card_init == 0)
	{
		stat = RES_OK;
	}
	else
	{
		stat = STA_NOINIT;
	}
	//------------------------------------------------------------
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE* buff,		/* Pointer to the destination object */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Offset in the sector */
	UINT count		/* Byte count (bit15:destination) */
)
{
	DRESULT res;

	// Add YuriYuriy 2025 ----------------------------------------
	uint8_t temp_sector[512];
  uint8_t result_read_sd_card = sd_card_read_fatfs(temp_sector, (sector >> 24) & 0xFF, (sector >> 16) & 0xFF, (sector >> 8) & 0xFF, sector & 0xFF);

  if (result_read_sd_card != 0)
  {
		res = RES_ERROR;
    return res;
  }
	else
	{
  	memcpy(buff, temp_sector + offset, count);
		res = RES_OK;
		return res;
	}
	//------------------------------------------------------------
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep (
	const BYTE* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
)
{
	DRESULT res;

	// Add YuriYuriy 2025 ----------------------------------------
	if (buff)
	{
		memcpy(&write_buffer[write_index], buff, sc);
    write_index += sc;
		res = RES_OK;
    return res;
	}
	else
	{
    if (sc)
    {
    	write_index = 0;
      write_sector = sc;
      res = RES_OK;
    	return res;
    }
    else
    {
    	uint8_t result_write_sd_card = sd_card_write_fatfs(write_buffer, (write_sector >> 24) & 0xFF, (write_sector >> 16) & 0xFF, (write_sector >> 8) & 0xFF, write_sector & 0xFF);
      if (result_write_sd_card == 0)
			{
      	res = RES_OK;
    		return res;
			}
			else
			{
      	res = RES_ERROR;
    		return res;
			}
  	}
	}
	//------------------------------------------------------------
}

