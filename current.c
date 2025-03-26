/*
This code allows the user to select a device (A, B, C) using the PS/2 keyboard,
enter a current value for the device, and save it by pressing Enter.
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

/* Function prototypes */
void update_display(int device, char first, char second);
char scan_code_to_number(char scan_code);

// Global variables for device management
int currents[3] = {0};      // Stores current values for devices A, B, C
int selected_device = 0;    // 0 for A, 1 for B, 2 for C
int input_mode = 0;         // 0: display saved value, 1: entering new value
char digit_buffer[2] = {0}; // Buffer for entered digits
int digit_count = 0;        // Number of digits entered

/*******************************************************************************
 * Main program: Handles device selection and current input via PS/2 keyboard
 ******************************************************************************/
int main(void) {
    volatile int *PS2_ptr = (int *)PS2_BASE;
    int PS2_data, RVALID;
    int ignore_next = 0;  // Flag to skip break codes

    // Initialize display to show device A and 00
    update_display(selected_device, 0, 0);

    *(PS2_ptr) = 0xFF;  // Reset PS/2 port

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
                // Handle device selection (a, b, c)
                if (received_byte == 0x1C) { // 'a' pressed
                    selected_device = 0;
                    input_mode = 0;
                    digit_count = 0;
                    update_display(selected_device, currents[selected_device]/10, currents[selected_device]%10);
                } else if (received_byte == 0x32) { // 'b' pressed
                    selected_device = 1;
                    input_mode = 0;
                    digit_count = 0;
                    update_display(selected_device, currents[selected_device]/10, currents[selected_device]%10);
                } else if (received_byte == 0x21) { // 'c' pressed
                    selected_device = 2;
                    input_mode = 0;
                    digit_count = 0;
                    update_display(selected_device, currents[selected_device]/10, currents[selected_device]%10);
                }
                // Handle Enter key
                else if (received_byte == 0x5A) { // Enter pressed
                    if (input_mode == 1 && digit_count > 0) {
                        // Save the entered current
                        int entered_current = digit_buffer[0];
                        if (digit_count == 2) {
                            entered_current = digit_buffer[0] * 10 + digit_buffer[1];
                        }
                        currents[selected_device] = entered_current;
                        input_mode = 0;
                        // Display saved current
                        update_display(selected_device, entered_current/10, entered_current%10);
                    }
                    digit_count = 0;
                    digit_buffer[0] = digit_buffer[1] = 0;
                }
                // Handle number keys
                else {
                    char number = scan_code_to_number(received_byte);
                    if (number != 0xFF) {
                        if (input_mode == 0) {
                            // Start new input
                            input_mode = 1;
                            digit_count = 0;
                            digit_buffer[0] = digit_buffer[1] = 0;
                        }
                        if (digit_count < 2) {
                            digit_buffer[digit_count] = number;
                            digit_count++;
                            // Update display with entered digits
                            char first = digit_count > 1 ? digit_buffer[0] : 0;
                            char second = digit_buffer[digit_count-1];
                            update_display(selected_device, first, second);
                        }
                    }
                }
            }
        }
    }
}

/*******************************************************************************
 * Converts PS/2 scan code to numeric character (0-9) or 0xFF for non-numbers
 ******************************************************************************/
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

/*******************************************************************************
 * Displays the selected device and current on HEX displays
 * device: 0 (A), 1 (B), 2 (C)
 * first: first digit, second: second digit
 ******************************************************************************/
void update_display(int device, char first, char second) {
    volatile int *HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
    volatile int *HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;

    // 7-segment codes for numbers (common anode)
    unsigned char seven_seg_decode_table[] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67
    };
    // 7-segment codes for A, B, C
    unsigned char seven_seg_letters[] = {0x77, 0x7C, 0x39};

    unsigned char hex5_seg = (device >= 0 && device <= 2) ? seven_seg_letters[device] : 0x00;
    unsigned char hex4_seg = 0x3F;  // HEX4 is off (0)

    unsigned char hex1_seg = (first < 10) ? seven_seg_decode_table[first] : 0x00;
    unsigned char hex0_seg = (second < 10) ? seven_seg_decode_table[second] : 0x00;

    // Update HEX5-4 and HEX3-0
    *HEX5_HEX4_ptr = (hex5_seg << 8) | hex4_seg;
    *HEX3_HEX0_ptr = (0x3F << 24) | (0x3F << 16) | (hex1_seg << 8) | hex0_seg;
}