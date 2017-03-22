// This program is adapted from  Terasic's DE0-Nano-SoC_My_First_HPS-Fpga
// template project.

// Modified by: Satyen Akolkar
// Date: March 22 2017 

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/mman.h>

// Information from Terasic's DE0-Nano-SoC_My_First_HPS-Fpga manual on the 
// DE0-Nano-SoC_v1.1.0_SystemCD:
//     Make sure the Altera Provided SoC EDS headers are included during build.
//     These headers are found in Quartus' Installation folder
//     /opt/altera/14.0/embedded/ip/altera/hps/altera_hps/hwlib/include
// 
// These header files have been copied to /usr/local/include on the board so
// gcc will automatically find them.

#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"

// The hps_0 header file created with sopc-create-header-file utility.
// This file is also copied into /usr/local/include on the board.
#include "hps_0.h"

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

int main(int argc, char **argv)
{
    void *base;
    void *leds_base;
    int memdevice_fd;
    uint8_t led_mask = 0xFE;
    int num_led_shifts;
    int i;

    if(argc != 2) {
        printf("Performing 24 LED shifts.\n");
        num_led_shifts = 24;
    } else {
        num_led_shifts = atoi(argv[1]);
    }

    // Open /dev/mem device
    if( (memdevice_fd = open("/dev/mem", (O_RDWR | O_SYNC))) < 0) {
        perror("Unable to open \"/dev/mem\".");
        exit(EXIT_FAILURE);
    }

    // mmap the HPS registers
    base = (uint32_t*) mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, memdevice_fd, HW_REGS_BASE); 
    if(base == MAP_FAILED) {
        perror("mmap() failed.");
        close(memdevice_fd);
        exit(EXIT_FAILURE);
    }

    // derive leds base address from base HPS registers
    leds_base = (uint32_t*) (base + ((ALT_LWFPGASLVS_OFST + LEDS_PIO_0_BASE) & HW_REGS_MASK));

    for(i = 0; i < num_led_shifts; ++i) {
        
        // Set LEDS according to mask
        *(uint32_t *) leds_base = ~led_mask;

        // Wait half a second
        usleep(500000);
        
        // Perform wrap-around or shift LED mask
        if(led_mask == 0x7F) {
            led_mask = 0xFE;
        } else {
            led_mask <<= 1;
            led_mask |= 0x01;
        }
    }

    // unmap and close /dev/mem 
    if( munmap(base, HW_REGS_SPAN) < 0) {
        perror("munmap() failed.");
        close(memdevice_fd);
        exit(EXIT_FAILURE);
    }

    close(memdevice_fd);
    exit(EXIT_SUCCESS);
}
