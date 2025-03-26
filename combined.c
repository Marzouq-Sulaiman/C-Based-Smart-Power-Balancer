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

#define BIT_CODES_SIZE 10
const char bit_codes[BIT_CODES_SIZE] = { 
    0x3F, 0x06, 0x5B, 0x4F, 0x66,  // 0-4
    0x6D, 0x7D, 0x07, 0x7F, 0x67   // 5-9
};

// Power mode structure
typedef struct {
    int voltage;
    int mode_code;
} PowerMode;

static void handler(void) __attribute__ ((interrupt ("machine")));
void set_KEYs(void);
void KEY_ISR(void);

// Current system state
volatile PowerMode current_mode = {120, 0};  // Default to 120V mode

/* Function prototypes */
void update_display(int device, char first, char second);
char scan_code_to_number(char scan_code);

// Global variables keyboard
int currents[9] = {0};      // Stores current values for devices A, B, C
int selected_device = 0;    // 0 for A, 1 for B, 2 for C and add 3 for future ones
int input_mode = 0;         // 0: display saved value, 1: entering new value
char digit_buffer[2] = {0}; // Buffer for entered digits
int digit_count = 0;        // Number of digits entered

// global variables for the switches
int SW_on_off[9] = {0};     // stores the values for each of the switches (whether its on or off)
int total_devices = 0;      // stores the number of devices currently connected to the power groups
const int SW_codes[10] = {  // Bitmasks for up to 9 devices (LEDs 0-8)
    0x000, 0x001, 0x003, 0x007, 0x00F, 
    0x01F, 0x03F, 0x07F, 0x0FF, 0x1FF
};

// global variables for power
int totalPower = 0;
int power[9] = {0};

/*******************************************************************************
 * Main program: Handles device selection and current input via PS/2 keyboard
 ******************************************************************************/
int main(void) {
    /*
    This section will set up the interrupts used in the program
    */
   //--------------------------------------------------------------------------//
    volatile int *SW_ptr = (int *) SW_BASE;
    volatile int *LEDR_ptr = (int *) LEDR_BASE;
    volatile int *HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;

    set_KEYs();  // Configure KEY interrupts

    // Set up interrupt handler
    int mstatus_val, mtvec_val, mie_val;
    mstatus_val = 0b1000;  // MIE bit mask
    
    // Disable interrupts during setup
    __asm__ volatile ("csrc mstatus, %0" :: "r"(mstatus_val));
    mtvec_val = (int)&handler;
    __asm__ volatile ("csrw mtvec, %0" :: "r"(mtvec_val));

    __asm__ volatile ("csrr %0, mie" : "=r"(mie_val));
    __asm__ volatile ("csrc mie, %0" :: "r"(mie_val));
	
    mie_val = 0x50000;  // Enable key and SW (bit 7 and 28)
	
	// set interrupt enables
    __asm__ volatile ("csrs mie, %0" :: "r"(mie_val));

    // Enable global interrupts
    __asm__ volatile ("csrs mstatus, %0" :: "r"(mstatus_val));
    //--------------------------------------------------------------------------//
    /*
    This section will set up the keyboard
    */
    //--------------------------------------------------------------------------//
    volatile int *PS2_ptr = (int *)PS2_BASE;
    int PS2_data, RVALID;
    int ignore_next = 0;  // Flag to skip break codes

    // Initialize display to show device A and 00
    update_display(selected_device, 0, 0);

    *(PS2_ptr) = 0xFF;  // Reset PS/2 port
    //--------------------------------------------------------------------------//
    while (1) {
        PS2_data = *(PS2_ptr);
        RVALID = PS2_data & 0x8000;

        if (RVALID && current_mode.voltage == 120) {
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
                        if (total_devices < 9){
                            total_devices++;
                        }
                        // Display saved current
                        update_display(selected_device, entered_current/10, entered_current%10);
                    }
                    digit_count = 0;
                    digit_buffer[0] = digit_buffer[1] = 0;
                    printf("the voltage group is 120 and the currents for the group are:\n");
                    for (int i = 0; i < 9; i++){
                        printf("device %d: %d\n", i, currents[i]);
                    }
                    printf("%d\n", totalPower);
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
        else if (RVALID && current_mode.voltage == 220){
            char received_byte = PS2_data & 0xFF;

            if (ignore_next) {
                ignore_next = 0;  // Skip break code
            } else if (received_byte == 0xF0) {
                ignore_next = 1;  // Next byte is break code
            } else {
                // Handle device selection (a, b, c)
                if (received_byte == 0x1C) { // 'a' pressed
                    selected_device = 3;
                    input_mode = 0;
                    digit_count = 0;
                    update_display(selected_device, currents[selected_device]/10, currents[selected_device]%10);
                } else if (received_byte == 0x32) { // 'b' pressed
                    selected_device = 4;
                    input_mode = 0;
                    digit_count = 0;
                    update_display(selected_device, currents[selected_device]/10, currents[selected_device]%10);
                } else if (received_byte == 0x21) { // 'c' pressed
                    selected_device = 5;
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
                        if (total_devices < 9){
                            total_devices++;
                        }
                        
                        // Display saved current
                        update_display(selected_device, entered_current/10, entered_current%10);
                    }
                    digit_count = 0;
                    digit_buffer[0] = digit_buffer[1] = 0;
                    printf("the voltage group is 220 and the currents for the group are:\n");
                    for (int i = 0; i < 9; i++){
                        printf("device %d: %d\n", i, currents[i]);
                    }
                    printf("%d\n", totalPower);
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
        else if (RVALID && current_mode.voltage == 240){
            char received_byte = PS2_data & 0xFF;

            if (ignore_next) {
                ignore_next = 0;  // Skip break code
            } else if (received_byte == 0xF0) {
                ignore_next = 1;  // Next byte is break code
            } else {
                // Handle device selection (a, b, c)
                if (received_byte == 0x1C) { // 'a' pressed
                    selected_device = 6;
                    input_mode = 0;
                    digit_count = 0;
                    update_display(selected_device, currents[selected_device]/10, currents[selected_device]%10);
                } else if (received_byte == 0x32) { // 'b' pressed
                    selected_device = 7;
                    input_mode = 0;
                    digit_count = 0;
                    update_display(selected_device, currents[selected_device]/10, currents[selected_device]%10);
                } else if (received_byte == 0x21) { // 'c' pressed
                    selected_device = 8;
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
                        if (total_devices < 9){
                            total_devices++;
                        }
                        input_mode = 0;
                        // Display saved current
                        update_display(selected_device, entered_current/10, entered_current%10);
                    }
                    digit_count = 0;
                    digit_buffer[0] = digit_buffer[1] = 0;
                    printf("the voltage group is 240 and the currents for the group are:\n");
                    for (int i = 0; i < 9; i++){
                        printf("device %d: %d\n", i, currents[i]);
                    }
                    printf("%d\n", totalPower);
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
        // this section will focus on letting the user turn on and off the appliances
        if (total_devices == 0) {
            *LEDR_ptr = 0;
        } else {
            // Use correct bitmask for the number of devices
            *LEDR_ptr = *SW_ptr & SW_codes[total_devices];
        }

        /*
        for (int i = 0; i < total_devices; i++){
            if ((*LEDR_ptr >> i) & 1){
                SW_on_off[i] = 1;
            } else {
                SW_on_off[i] = 0;
            }
        }
        */
       
        // Update switch states
        for (int i = 0; i < 9; i++) {
            SW_on_off[i] = (*LEDR_ptr >> i) & 1;
        }


        // this section will be for calculating the power of the power groups and appliances and finding the total power
        // and display it on the hexes
        /*
        for (int i = 0; i < 3; i++){
            power[i] = currents[i] * 120 * SW_on_off[i];
        }
        for (int i = 3; i < 6; i++){
            power[i] = currents[i] * 220 * SW_on_off[i];
        }
        for (int i = 6; i < 9; i++){
            power[i] = currents[i] * 240 * SW_on_off[i];
        }

        for (int i = 0; i < 9; i++){
            totalPower += power[i];
        */
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
    unsigned char seven_seg_letters[] = {0x77, 0x7C, 0x39, 0x77, 0x7C, 0x39, 0x77, 0x7C, 0x39};

    unsigned char hex5_seg = (device >= 0 && device <= 8) ? seven_seg_letters[device] : 0x00;
    unsigned char hex4_seg = 0x3F;  // HEX4 is off (0)

    unsigned char hex1_seg = (first < 10) ? seven_seg_decode_table[first] : 0x00;
    unsigned char hex0_seg = (second < 10) ? seven_seg_decode_table[second] : 0x00;

    // Update HEX5-4 and HEX3-0
    *HEX5_HEX4_ptr = (hex5_seg << 8) | hex4_seg;
    *HEX3_HEX0_ptr = (0x3F << 24) | (0x3F << 16) | (hex1_seg << 8) | hex0_seg;
}

void set_KEYs(void) {
    volatile int *KEY_ptr = (int *)KEY_BASE;
    *(KEY_ptr + 3) = 0xF;  // Clear any existing presses
    *(KEY_ptr + 2) = 0xE;  // Enable interrupts for KEY1-3 (bits 1-3)
}

// Interrupt handler
void handler(void) {
    int mcause_val;
    __asm__ volatile ("csrr %0, mcause" : "=r"(mcause_val));
    
    // Check for external interrupt (mcause 11)
    if(mcause_val == 0x80000012) {
        KEY_ISR();
		printf("key pressed\n");
    }
}

// KEY interrupt service routine
void KEY_ISR(void) {
    volatile int *KEY_ptr = (int *)KEY_BASE;
	
	int KEY_value;
	KEY_value = *(KEY_ptr); // read the pushbutton KEY values
	
    int pressed = *(KEY_ptr + 3);  // Read Edge Capture
	// printf("%d\n", *(KEY_ptr + 3));
	// printf("%d\n", pressed);
	
    if(*(KEY_ptr + 3) == 0x2) {        // KEY1 pressed (120V)
        current_mode.voltage = 120;
        current_mode.mode_code = 0;
		printf("now in %d\n", current_mode.voltage);
    }
    else if(*(KEY_ptr + 3) == 0x4) {   // KEY2 pressed (220V)
        current_mode.voltage = 220;
        current_mode.mode_code = 1;
		printf("now in %d\n", current_mode.voltage);
    }
    else if(*(KEY_ptr + 3) == 0x8) {   // KEY3 pressed (240V)
        current_mode.voltage = 240;
        current_mode.mode_code = 2;
		printf("now in %d\n", current_mode.voltage);
    }
	
	*(KEY_ptr + 3) = pressed;  // Clear Edge Capture
	// printf("%d\n", *(KEY_ptr + 3));
	// printf("%d\n", pressed);
	
}