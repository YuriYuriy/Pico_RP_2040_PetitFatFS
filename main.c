//----------------------------------------------------------------------------------------------------
//by YuriYuriy
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

