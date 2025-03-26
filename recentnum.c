/*
This code will allow the user to enter 
*/

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
//#include <system_info.h>

/* Function prototypes */
void HEX_PS2(char first, char second);
char scan_code_to_number(char scan_code);

// Global variables for number storage
int saved_number = 0;
char digit_buffer[2] = {0};
int digit_count = 0;

/*******************************************************************************
 * Main program: Collects up to 2 digits, saves on Enter press
 ******************************************************************************/
int main(void) {
    volatile int *PS2_ptr = (int *)PS2_BASE;
    int PS2_data, RVALID;
    int ignore_next = 0;  // Flag to skip break codes

    // Initialize display to show 0000 on all HEX
    HEX_PS2(0, 0);

    *(PS2_ptr) = 0xFF;  // PS/2 port reset

    while (1) {
        PS2_data = *(PS2_ptr);
        RVALID = PS2_data & 0x8000;

        if (RVALID) {
            char received_byte = PS2_data & 0xFF;

            if (ignore_next) {
                ignore_next = 0;  // Skip break code
            } else if (received_byte == 0xF0) {
                ignore_next = 1;  // Next byte is break code
            } else {
                // Handle Enter key (make code 0x5A)
                if (received_byte == 0x5A) {
                    // Save number and reset display
                    if (digit_count > 0) {
                        saved_number = digit_buffer[0];
                        if (digit_count == 2) {
                            saved_number = digit_buffer[0] * 10 + digit_buffer[1];
                        }
                    }
                    digit_count = 0;
                    digit_buffer[0] = digit_buffer[1] = 0;
                    HEX_PS2(0, 0);
                    printf("%d\n",saved_number);
                }
                // Handle number keys
                else {
                    char number = scan_code_to_number(received_byte);
                    if (number != 0xFF && digit_count < 2) {
                        digit_buffer[digit_count] = number;
                        digit_count++;
                        // Update display with latest digits
                        char first = digit_count > 1 ? digit_buffer[0] : 0;
                        char second = digit_buffer[digit_count-1];
                        HEX_PS2(first, second);
                    }
                }
            }
        }
    }
}

/****************************************************************************************
 * Converts PS/2 scan code to numeric character (0-9) or 0xFF for non-numbers
 ****************************************************************************************/
char scan_code_to_number(char scan_code) {
    switch (scan_code) {  // PS/2 make codes for number keys
        case 0x45: return 0;  // 0
        case 0x16: return 1;  // 1
        case 0x1E: return 2;  // 2
        case 0x26: return 3;  // 3
        case 0x25: return 4;  // 4
        case 0x2E: return 5;  // 5
        case 0x36: return 6;  // 6
        case 0x3D: return 7;  // 7
        case 0x3E: return 8;  // 8
        case 0x46: return 9;  // 9
        default:    return 0xFF;  // Non-numeric key
    }
}

/****************************************************************************************
 * Displays numbers on HEX1-0 and 0000 on HEX5-4 and HEX3-2
 ****************************************************************************************/
void HEX_PS2(char first, char second) {
    volatile int *HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
    volatile int *HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;

    // 7-segment codes for 0-9 (common anode)
    unsigned char seven_seg_decode_table[] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67
    };

    // Set HEX3-2 to 0, HEX1-0 to input digits
    unsigned char hex1_seg = seven_seg_decode_table[first];
    unsigned char hex0_seg = seven_seg_decode_table[second];
    *HEX3_HEX0_ptr = (0x3F << 24) | (0x3F << 16) | (hex1_seg << 8) | hex0_seg;

    // Turn off HEX5-4
    *HEX5_HEX4_ptr = 0x3F3F; // Display 0 on HEX5 and HEX4
}