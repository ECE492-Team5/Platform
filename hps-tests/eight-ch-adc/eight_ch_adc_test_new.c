#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/mman.h>

/****** NEW ********
*
*HOW TO COMPILE:
*
*Installing libjson: https://linuxprograms.wordpress.com/2010/05/20/install-json-c-in-linux/
*
*Compiling: gcc -o eight_ch_adc_test_new eight_ch_adc_test_new.c -l json
*
********************/

//JSON lib
#include <json/json.h>
//Time
#include <time.h>

// Information from Terasic's DE0-Nano-SoC_My_First_HPS-Fpga manual on the 
// DE0-Nano-SoC_v1.1.0_SystemCD:
//     Make sure the Altera Provided SoC EDS headers are included during build.
//     These headers are found in Quartus' Installation folder
//     /opt/altera/14.0/embedded/ip/altera/hps/altera_hps/hwlib/include
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"

// The hps_0 header file created with sopc-create-header-file utility.
#include "hps_0.h"

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

void getDate(char date_buffer[]);
void generateJSON(int channel, int value);

int main(int argc, char **argv)
{
    void *base;
    uint32_t *adc_base;
    int memdevice_fd;
    int i;
    const int nReadNum = 10;
    int value;

    int channel = 0x00 & 0x07;
    
    if(argc != 2) {
        
    } else {
        channel = ((uint8_t) atoi(argv[1])) & 0x07; 
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
    adc_base = (uint32_t*) (base + ((ALT_LWFPGASLVS_OFST + ADC_LTC2308_0_BASE) & HW_REGS_MASK));

    printf("ADC BASE ADDR = 0x%x\n", adc_base);

    // IOWR(adc_base, 0x01, nReadNum);
    *(adc_base + 0x01) = nReadNum;

    //IOWR(adc_base, 0x00, (channel << 1) | 0x00);
    //IOWR(adc_base, 0x00, (channel << 1) | 0x01);
    //IOWR(adc_base, 0x00, (channel << 1) | 0x00);
    
    *adc_base = (channel << 1) | 0x00;
    *adc_base = (channel << 1) | 0x01;
    *adc_base = (channel << 1) | 0x00;

    printf("wrote: 0x%04x", ((channel << 1) | 0x01));
    
    usleep(1);

    //while( (IORD(adc_base, 0x00) & 0x01) == 0x00 );
    while( (*adc_base & 0x01) == 0x00);

    for(i = 0; i < nReadNum; ++i) {
        // value = IORD(adc_base, 0x01); 
        value = *(adc_base + 0x01);
        printf("CH%d = %.3fV (0x%04x)\n", channel, (float)value/1000.0, value);

        /****** NEW ********/
        //Generate JSON
        generateJSON(channel, value);

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

/****** NEW ********/
//Gets time and copies it to the input buffer
void getDate(char date_buffer[]) {
    time_t timer;
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    if (tm_info == NULL) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }

    strftime(date_buffer, 30, "%Y-%m-%dT%H:%M:%S", tm_info);
}

/****** NEW ********/
//Generates the JSON file and outputs it to current directory 
void generateJSON(int channel, int value) {
    
    //Buffer to hold the date
    char date_buffer[30];
    //Buffer to hold temporary path name
    char path_buffer_temp[30];
    //Buffer to hold actual path name
    char path_buffer[30];

    //Get the date
    getDate(date_buffer);

    //Initialize new libjson JSON object
    json_object *j_sensor_obj = json_object_new_object();
    //Initialize new libjson INT object (for channel)
    json_object *j_sensor_channel_int = json_object_new_int(channel);
    //Initialize new libjson INT object (for value)
    json_object *j_sensor_value_int = json_object_new_int(value);
    //Initialize new libjson String object (for Date)
    json_object *j_date_string = json_object_new_string(date_buffer);

    //Add the INT and String objects to JSON object 
    json_object_object_add(j_sensor_obj, "Channel", j_sensor_channel_int);
    json_object_object_add(j_sensor_obj, "Voltage", j_sensor_value_int);
    json_object_object_add(j_sensor_obj, "Date", j_date_string);

    //Generate path buffers (temp has a ~)
    snprintf(path_buffer_temp, 30, "./sensor_%d~.json", channel);
    snprintf(path_buffer, 30, "./sensor_%d.json", channel);

    //All the file IO stuff...
    FILE *fp_sensor;
    fp_sensor = fopen(path_buffer, "w");
    if (fp_sensor == NULL) {
        fprintf(stderr, "Can't Open File Sensor_%d\n", channel);
        exit(EXIT_FAILURE);
    }

    fprintf(fp_sensor, "%s\n", 
        json_object_to_json_string_ext(j_sensor_obj, JSON_C_TO_STRING_PRETTY));
    fclose(fp_sensor);

    //Rename (This is atomic)
    rename(path_buffer_temp, path_buffer); 
}
