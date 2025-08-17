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
#include "main.h"

FATFS fs;
FRESULT fr;
DIR dir;
FILINFO fno;

int main()
{
    stdio_init_all();
    printf("Start!\n");

    printf("Mounting...\n");
    fr = pf_mount(&fs);
    if (fr != FR_OK)
    {
        printf("Mounting error: %d\n", fr);
        return 1;
    }

    while (1)
    {     
        printf("Opening the root directory...\n");
        fr = pf_opendir(&dir, "/");
        if (fr != FR_OK)
        {
            printf("Error opening the catalog: %d\n", fr);
            return 1;
        }
        printf("SD Card contents:\n");

        while (1)
        {
            fr = pf_readdir(&dir, &fno);
            if (fr != FR_OK || fno.fname[0] == 0)
                break;

            if (fno.fattrib & AM_DIR)
                printf("[DIR]  %s\n", fno.fname);
            else
                printf("       %s\n", fno.fname);
        }

        printf("Done.\n");
        printf("\n");
        sleep_ms(5000);
    }
    

}
