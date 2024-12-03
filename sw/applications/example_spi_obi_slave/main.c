/**
 * @file main.c
 * @brief spi obi slave functionality test
 *
 *
*/


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "x-heep.h"
#include "spi_host.h"
#include "buffer.h"



#define DIV_ROUND_UP(numerator, denominator) ((numerator + denominator - 1) / denominator)
#define LOWER_BYTE_16_BITS(bytes)(bytes & 0xFF)
#define UPPER_BYTE_16_BITS(bytes)((bytes & 0xFF00) >> 8)
#define LOWER_BYTES_32_BITS(bytes)(bytes & 0xFFFF)
#define UPPER_BYTES_32_BITS(bytes)((bytes & 0xFFFF0000) >> 16)
#define WRITE_SPI_SLAVE_REG_1 0x11
#define WRITE_SPI_SLAVE_REG_2 0x20
#define WRITE_SPI_SLAVE_REG_3 0x30
#define READ_SPI_SLAVE_CMD 0xB
#define WRITE_SPI_SLAVE_CMD 0x2
#define WORD_SIZE_IN_BYTES 4
#define MAX_DATA_SIZE 0x10000
#define DUMMY_CYCLES 0x20 //Dummy cycles are required otherwise the obi master and slave are too slow to process the data
#define BASE_ADDRESS 0x0000  // Starting address of the target memory
#define MAX_DATASETS 0
#define DATA_SET_COUNT (current_address >> 2)


typedef enum {
    // Everithing went well
    SPI_SLAVE_FLAG_OK                       = 0x0000,
    // The SPI variabled passed was a null pointer
    SPI_SLAVE_FLAG_NULL_PTR                 = 0x0001,
    //The SPI was not properly initalized
    SPI_SLAVE_FLAG_NOT_INIT                 = 0x0002,
    //The target address is invalid
    SPI_SLAVE_FLAG_ADDRESS_INVALID          = 0x0003,
    // The CSID was out of the bounds specified in SPI_HOST_PARAM_NUM_C_S 
    SPI_SLAVE_FLAG_CSID_INVALID             = 0x0004,
    //The amount of data it too much for the SPI SLAVE IP to handle
    SPI_SLAVE_FLAG_SIZE_OF_DATA_EXCEEDED    = 0x0005,
    // The CMD FIFO is currently full so couldn't write command
    SPI_SLAVE_FLAG_COMMAND_FULL             = 0x0008,
    // The specified speed is not valid so couldn't write command
    SPI_SLAVE_FLAG_SPEED_INVALID            = 0x0010,
    // The TX Queue is full, thus could not write to TX register
    SPI_SLAVE_FLAG_TX_QUEUE_FULL            = 0x0020,
    // The RX Queue is empty, thus could not read from RX register
    SPI_SLAVE_FLAG_RX_QUEUE_EMPTY           = 0x0040,
    // The SPI is not ready
    SPI_SLAVE_FLAG_NOT_READY                = 0x0080,
    // The event to enable is not a valid event
    SPI_SLAVE_FLAG_EVENT_INVALID            = 0x0100,
    // The error irq to enable is not a valid error irq
    SPI_SLAVE_FLAG_ERROR_INVALID            = 0x0200 
} spi_slave_flags_e;

typedef struct {
    uint32_t address;
    uint16_t length;
    const char *data_name;
} spi_data_info_t;


spi_host_t* spi_hst; //Removed specific memory assignement
uint32_t compare_data[DATA_LENGTH/4]; //!!! I have to check whether the process reads out more data than it sent in
static uint32_t current_address = BASE_ADDRESS;


spi_slave_flags_e spi_slave_init(spi_host_t* spi_host);
static spi_slave_flags_e spi_host_write(uint32_t addr, uint32_t *data, uint16_t length);
spi_slave_flags_e spi_slave_read(uint32_t addr, void* data, uint16_t length, uint8_t dummy_cycles);
bool check_address_validity(uint32_t addr, void* data, uint16_t length);
void print_array(const char *label, uint32_t *array, uint16_t size);
static void configure_spi();
void write_word_big_endian(uint32_t word);
void write_dummy_cycles(uint8_t cycles);
void spi_host_wait(uint8_t cycles);
uint32_t make_word_compatible_for_spi_host(uint32_t word);
void make_compare_data_compatible(uint32_t *compare_data, uint16_t length);
void write_wrap_length(uint16_t length);
uint32_t allocate_address(uint16_t data_length);
void store_metadata(const char *data_name, uint16_t length);
spi_data_info_t get_metadata(const char *data_name);
void print_metadata();


int main(int argc, char *argv[])
{   
    spi_data_info_t *data_table = malloc(MAX_DATASETS * sizeof(spi_data_info_t));

    spi_hst = spi_host1;
    spi_return_flags_e flags;
    flags = spi_slave_init(spi_hst);
    if (flags != SPI_SLAVE_FLAG_OK){
        printf("Failure to initialize\n Error code: %d", flags);
        return EXIT_FAILURE;
    }
    printf("Checkpoint");
    flags = spi_host_write(TARGET_ADDRESS, test_data, DATA_LENGTH);
    if (flags != SPI_SLAVE_FLAG_OK){
        printf("Failure to write\n Error code: %d", flags);
        return EXIT_FAILURE;
    }

    flags = spi_slave_read(TARGET_ADDRESS, compare_data, DATA_LENGTH, DUMMY_CYCLES);
    if (flags != SPI_SLAVE_FLAG_OK){
        printf("Failure to read\n Error code: %d", flags);
        return EXIT_FAILURE;
    }
    print_array("Test Data", test_data, DATA_LENGTH);
    make_compare_data_compatible(compare_data, DATA_LENGTH);
    print_array("Compare Data", compare_data, DATA_LENGTH);
    if (memcmp(test_data, compare_data, DATA_LENGTH) != 0) {
        printf("Failure to retrieve correct data\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


spi_slave_flags_e spi_slave_read(uint32_t addr, void* data, uint16_t length, uint8_t dummy_cycles){
    // ??? Is this necessary? What happens when address is invalid or data exceeds memory? What is the size of the memory?
    if (check_address_validity(addr, data, length) != true) return SPI_SLAVE_FLAG_ADDRESS_INVALID;

    write_dummy_cycles(dummy_cycles);

    write_wrap_length(length);
 
    // Set up segment parameters -> send commands
    const uint32_t send_cmd_byte = spi_create_command((spi_command_t){
        .len        = 0,                    // 1 Byte (The SPI SLAVE IP can only take one CMD byte at a time)
        .csaat      = true,                 // Command not finished e.g. CS remains low after transaction
        .speed      = SPI_SPEED_STANDARD,   // Single speed
        .direction  = SPI_DIR_TX_ONLY       // Write only
    });




    spi_write_byte(spi_hst, READ_SPI_SLAVE_CMD);
    spi_wait_for_ready(spi_hst);

    spi_set_command(spi_hst, send_cmd_byte);
    spi_wait_for_ready(spi_hst);

    //write address
    write_word_big_endian(addr);

    spi_host_wait(dummy_cycles);

    // Set up segment parameters -> read length bytes
    const uint32_t cmd_read = spi_create_command((spi_command_t){ //!!!!! I'm assuming this configurates the SPI host IP hence using the Host RX FIFO capacities
        .len        = length-1,             // len bytes
        .csaat      = false,                // End command
        .speed      = SPI_SPEED_STANDARD,   // Single speed
        .direction  = SPI_DIR_RX_ONLY       // Read only
    });
    spi_set_command(spi_hst, cmd_read);
    spi_wait_for_ready(spi_hst);

    /*
     * Set RX watermark to length. The watermark is in words.
     * If the length is not a multiple of 4, the RX watermark is set to length/4+1
     * to take into account the extra bytes.
     * If the length is higher then the RX FIFO depth, the RX watermark is set to
     * RX FIFO depth. In this case the flag is not set to 0, so the loop will
     * continue until all the data is read.
    */
    int flag = 1;
    int to_read = 0;
    int i_start = 0;
    int length_original = length;
    uint32_t *data_32bit = (uint32_t *)data;
    while (flag) {
        if (length >= SPI_HOST_PARAM_RX_DEPTH) {
            spi_set_rx_watermark(spi_hst, SPI_HOST_PARAM_RX_DEPTH>>2);
            length -= SPI_HOST_PARAM_RX_DEPTH;
            to_read += SPI_HOST_PARAM_RX_DEPTH;
        }
        else {
            spi_set_rx_watermark(spi_hst, (length%4==0 ? length>>2 : (length>>2)+1));
            to_read += length;
            flag = 0;
        }
        // Wait till SPI RX FIFO is full (or I read all the data)
        spi_wait_for_rx_watermark(spi_hst);
        // Read data from SPI RX FIFO
        for (int i = i_start; i < to_read>>2; i++) {
            spi_read_word(spi_hst, &data_32bit[i]); // Writes a full word
        }
        // Update the starting index
        i_start += SPI_HOST_PARAM_RX_DEPTH>>2;
    }
    // Take into account the extra bytes (if any)
    if (length_original % 4 != 0) {
        uint32_t last_word = 0;
        spi_read_word(spi_hst, &last_word);
        memcpy(&data_32bit[length_original>>2], &last_word, length%4);
    }

    return SPI_SLAVE_FLAG_OK; // Success
}


spi_slave_flags_e spi_slave_init(spi_host_t* spi_host) {
 
    // Set the global spi variable to the one passed as argument.
    spi_hst = spi_host;

    // Enable SPI host device
    spi_return_flags_e test = spi_set_enable(spi_hst, true);
    if(test != SPI_FLAG_OK){
        printf("test\n");
        printf("test value %d", test);
        return SPI_SLAVE_FLAG_NOT_INIT;
    };

    // Enable SPI output
    if(spi_output_enable(spi_hst, true) != SPI_FLAG_OK){
        printf("testa");
        return SPI_SLAVE_FLAG_NOT_INIT;
    };
    // Configure SPI Master<->Slave connection on CSID 0
    configure_spi();

    // Set CSID
    if(spi_set_csid(spi_hst, 0) != SPI_FLAG_OK){
        printf("testb");
        return SPI_SLAVE_FLAG_CSID_INVALID;
    };

    return SPI_SLAVE_FLAG_OK; // Success
}

bool check_address_validity(uint32_t addr, void* data, uint16_t length){
    //TODO 
    return true;
}


static spi_slave_flags_e spi_host_write(uint32_t addr, uint32_t *data, uint16_t length) {
    if(DIV_ROUND_UP(length, WORD_SIZE_IN_BYTES) > MAX_DATA_SIZE){
        return SPI_SLAVE_FLAG_SIZE_OF_DATA_EXCEEDED;
    }

    write_wrap_length(length);

    const uint8_t write_byte_cmd = WRITE_SPI_SLAVE_CMD;
    spi_write_word(spi_hst, write_byte_cmd);
    const uint32_t cmd_write = spi_create_command((spi_command_t){
        .len        = 0,
        .csaat      = true,
        .speed      = SPI_SPEED_STANDARD,
        .direction  = SPI_DIR_TX_ONLY
    });
    spi_set_command(spi_hst, cmd_write);
    spi_wait_for_ready(spi_hst);

    //Write address
    write_word_big_endian(addr);

    /*
     * Place data in TX FIFO
     * In simulation it do not wait for the flash to be ready, so we must check
     * if the FIFO is full before writing.
    */

    uint32_t *data_32bit = (uint32_t *)data;
    for (int i = 0; i < length>>2; i++) {
        spi_wait_for_tx_not_full(spi_hst);
        spi_write_word(spi_hst, make_word_compatible_for_spi_host(data_32bit[i]));
    }
    if (length % 4 != 0) {
        uint32_t last_word = 0;
        memcpy(&last_word, &data[length - length % 4], length % 4);
        spi_wait_for_tx_not_full(spi_hst);
        spi_write_word(spi_hst, make_word_compatible_for_spi_host(last_word));
    }


    /*
     * Set up segment parameters -> send data.
    */
    const uint32_t cmd_write_2 = spi_create_command((spi_command_t){
        .len        = length-1,
        .csaat      = false,
        .speed      = SPI_SPEED_STANDARD,
        .direction  = SPI_DIR_TX_ONLY
    });
    spi_set_command(spi_hst, cmd_write_2);
    spi_wait_for_ready(spi_hst);

    return SPI_SLAVE_FLAG_OK; // Success
}





static void configure_spi() {
    // Configure SPI clock
    uint16_t clk_div = 0;

    // SPI Configuration
    // Configure chip 0 (slave)
    const uint32_t chip_cfg = spi_create_configopts((spi_configopts_t){
        .clkdiv     = clk_div,
        .csnidle    = 0xF,
        .csntrail   = 0xF,
        .csnlead    = 0xF,
        .fullcyc    = false,
        .cpha       = 0,
        .cpol       = 0            //
    });
    spi_set_configopts(spi_hst, 0, chip_cfg);
}


void write_word_big_endian(uint32_t word){

    uint32_t sorted_word = make_word_compatible_for_spi_host(word);
    spi_write_word(spi_hst, sorted_word);
    spi_wait_for_ready(spi_hst);        // Set up segment parameters -> send command and address
    const uint32_t cmd_read_1 = spi_create_command((spi_command_t){
        .len        = 3,                 // 4 Bytes
        .csaat      = true,              // Command not finished
        .speed      = SPI_SPEED_STANDARD, // Single speed
        .direction  = SPI_DIR_TX_ONLY      // Write only
    });
    // Load segment parameters to COMMAND register
    spi_set_command(spi_hst, cmd_read_1);
    spi_wait_for_ready(spi_hst);

}

void write_dummy_cycles(uint8_t cycles){

    

    const uint32_t send_cmd_byte = spi_create_command((spi_command_t){
        .len        = 0,                    // 1 Byte (The SPI SLAVE IP can only take one CMD byte at a time)
        .csaat      = true,                 // Command not finished e.g. CS remains low after transaction
        .speed      = SPI_SPEED_STANDARD,   // Single speed
        .direction  = SPI_DIR_TX_ONLY       // Write only
    });

     // Load command to TX FIFO
    spi_write_byte(spi_hst, WRITE_SPI_SLAVE_REG_1);
    spi_wait_for_ready(spi_hst);

     // Load segment parameters to COMMAND register
    spi_set_command(spi_hst, send_cmd_byte);
    spi_wait_for_ready(spi_hst);

      // Load command to TX FIFO
    spi_write_byte(spi_hst, cycles);
    spi_wait_for_ready(spi_hst);

     // Load segment parameters to COMMAND register
    spi_set_command(spi_hst, send_cmd_byte);
    spi_wait_for_ready(spi_hst);

    

}

void write_wrap_length(uint16_t length){

    uint8_t wrap_length_cmds[4] =   {WRITE_SPI_SLAVE_REG_2              //Write register 2 
                                  ,LOWER_BYTE_16_BITS(length>>2)         //Wraplength low 
                                  ,WRITE_SPI_SLAVE_REG_3              //Write register 3
                                  ,UPPER_BYTE_16_BITS(length>>2)};       //Wraplength high
 
    // Set up segment parameters -> send commands
    const uint32_t send_cmd_byte = spi_create_command((spi_command_t){
        .len        = 0,                    // 1 Byte (The SPI SLAVE IP can only take one CMD byte at a time)
        .csaat      = true,                 // Command not finished e.g. CS remains low after transaction
        .speed      = SPI_SPEED_STANDARD,   // Single speed
        .direction  = SPI_DIR_TX_ONLY       // Write only
    });

    for(int i = 0; i < 4; i++){ 
        // Load command to TX FIFO
        spi_write_byte(spi_hst, wrap_length_cmds[i]);
        spi_wait_for_ready(spi_hst);

        // Load segment parameters to COMMAND register
        spi_set_command(spi_hst, send_cmd_byte);
        spi_wait_for_ready(spi_hst);
    }

}

void spi_host_wait(uint8_t cycles){
    if(cycles == 0){
        return;
    }
    uint8_t placeholder = 0;
    const uint32_t send_cmd_byte = spi_create_command((spi_command_t){
        .len        = cycles,             // Number of dummy cycles
        .csaat      = true,                 // Command not finished e.g. CS remains low after transaction
        .speed      = SPI_SPEED_STANDARD,   // Single speed
        .direction  = SPI_DIR_DUMMY         // Dummy
    });


    // Load segment parameters to COMMAND register
    spi_set_command(spi_hst, send_cmd_byte);
    spi_wait_for_ready(spi_hst);

}

uint32_t allocate_address(uint16_t data_length) {
    uint32_t allocated_address = current_address;
    
    // Ensure word alignment for addresses
    uint32_t aligned_length = DIV_ROUND_UP(data_length, WORD_SIZE_IN_BYTES) * WORD_SIZE_IN_BYTES;
    current_address += aligned_length;

    if (current_address > MAX_DATA_SIZE) {
        printf("Error: SPI slave memory exceeded.\n");
        exit(EXIT_FAILURE);
    }

    return allocated_address;
}


void store_metadata(const char *data_name, uint16_t length) {
    uint32_t address = allocate_address(length);
    if (DATA_SET_COUNT >= MAX_DATASETS) {
        printf("Error: Data table is full.\n");
        return;
    }
    data_table[DATA_SET_COUNT+ 1] = (spi_data_info_t){data_name, address, length};
}

void print_metadata() {
    printf("Stored Data Information:\n");
    for (uint16_t i = 0; i < DATA_SET_COUNT; i++) {
        printf("Name: %s, Address: 0x%08X, Length: %d bytes\n",
               data_table[i].data_name, data_table[i].address, data_table[i].length);
    }
}

spi_data_info_t get_metadata(const char *req_data_name){
    for (uint16_t i = 0; i < DATA_SET_COUNT; i++) {
        if (strcmp(data_table[i].data_name, req_data_name) == 0) {
            return data_table[i];
        }
    }
}



void print_array(const char *label, uint32_t *array, uint16_t size) {
    printf("%s: [", label);
    for (uint16_t i = 0; i < size>>2; i++) {
        printf("%d", array[i]);
        if (i < size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

uint32_t make_word_compatible_for_spi_host(uint32_t word){
    return (LOWER_BYTE_16_BITS(LOWER_BYTES_32_BITS(word)) << 24) 
        | (UPPER_BYTE_16_BITS(LOWER_BYTES_32_BITS(word)) << 16) 
        | (LOWER_BYTE_16_BITS(UPPER_BYTES_32_BITS(word)) << 8) 
        | UPPER_BYTE_16_BITS(UPPER_BYTES_32_BITS(word));
}

void make_compare_data_compatible(uint32_t *compare_data, uint16_t length){
    for(uint16_t i = 0; i < length>>2; i++){
        compare_data[i] = make_word_compatible_for_spi_host(compare_data[i]);
    }    
}