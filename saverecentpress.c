/*******************************************************************************
 * This file provides address values that exist in the DE1-SoC Computer
 ******************************************************************************/
#ifndef __SYSTEM_INFO__
#define __SYSTEM_INFO__
#define BOARD "DE1-SoC"
/* Memory */
#define DDR_BASE 0x40000000
#define DDR_END 0x7FFFFFFF
#define SDRAM_BASE 0x00000000
#define SDRAM_END 0x03FFFFFF
#define FPGA_PIXEL_BUF_BASE 0x08000000
#define FPGA_PIXEL_BUF_END 0x0803FFFF
#define FPGA_CHAR_BASE 0x09000000
#define FPGA_CHAR_END 0x09001FFF
/* Cyclone V FPGA devices */
#define LED_BASE 0xFF200000
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define JP1_BASE 0xFF200060
#define JP2_BASE 0xFF200070
#define PS2_BASE 0xFF200100
#define PS2_DUAL_BASE 0xFF200108
#define JTAG_UART_BASE 0xFF201000
#define IrDA_BASE 0xFF201020
#define TIMER_BASE 0xFF202000
#define TIMER_2_BASE 0xFF202020
#define AV_CONFIG_BASE 0xFF203000
#define RGB_RESAMPLER_BASE 0xFF203010
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define CHAR_BUF_CTRL_BASE 0xFF203030
#define AUDIO_BASE 0xFF203040
#define VIDEO_IN_BASE 0xFF203060
#define EDGE_DETECT_CTRL_BASE 0xFF203070
#define ADC_BASE 0xFF204000
/* Cyclone V HPS devices */
#define MTIME_BASE 0xFF202100
#endif

/* Function prototype */
void HEX_PS2(char previous, char current);

/*******************************************************************************
 * This program displays the last two bytes received from the PS/2 port on HEX3-0.
 ******************************************************************************/
int main(void) {
    volatile int *PS2_ptr = (int *)PS2_BASE;
    int PS2_data, RVALID;
    char previous = 0, current = 0;
    int ignore_next = 0; // Flag to skip break codes after 0xF0

    *(PS2_ptr) = 0xFF; // Reset PS/2 port

    while (1) {
        PS2_data = *(PS2_ptr);
        RVALID = PS2_data & 0x8000;

        if (RVALID) {
            char received_byte = PS2_data & 0xFF;

            if (ignore_next) {
                // Skip this byte (break code after 0xF0)
                ignore_next = 0;
            } else if (received_byte == 0xF0) {
                // Next byte is a break code - flag to skip it
                ignore_next = 1;
            } else {
                // Valid key press - update display
                previous = current;
                current = received_byte;
                HEX_PS2(previous, current);
            }
        }
    }
}

/****************************************************************************************
 * Displays two bytes (previous and current) on HEX3-0 in hexadecimal format.
 ****************************************************************************************/
void HEX_PS2(char previous, char current) {
    volatile int *HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
    volatile int *HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;

    // Lookup table for 7-segment displays (0-F)
    unsigned char seven_seg_decode_table[] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
    };

    unsigned char hex_segs[4] = {0, 0, 0, 0}; // Stores segment codes for HEX3-0
    unsigned int shift_buffer = ((unsigned int)previous << 8) | (current & 0xFF);
    int i;

    // Extract each nibble and convert to 7-segment code
    for (i = 0; i < 4; ++i) {
        unsigned char nibble = shift_buffer & 0x0F;
        hex_segs[i] = seven_seg_decode_table[nibble];
        shift_buffer >>= 4;
    }

    // Update HEX displays
    *HEX3_HEX0_ptr = *(int *)hex_segs; // Display previous and current on HEX3-0
    *HEX5_HEX4_ptr = 0; // Turn off HEX5-4
    // hello 
}