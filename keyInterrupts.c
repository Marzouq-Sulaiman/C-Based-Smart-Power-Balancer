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
/*******************************************************************************
 * This program uses KEY1-3 interrupts to select power modes and displays
 * the current voltage on HEX displays.
 ******************************************************************************/
//#include "system_info.h"

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

int main(void) {
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

    while(1) {
        // Display current voltage on HEX2-0
        int code = 0;
        switch(current_mode.voltage) {
            case 120:
                code = (bit_codes[1] << 16) | (bit_codes[2] << 8) | bit_codes[0];
                break;
            case 240:
                code = (bit_codes[2] << 16) | (bit_codes[4] << 8) | bit_codes[0];
                break;
            case 440:
                code = (bit_codes[4] << 16) | (bit_codes[4] << 8) | bit_codes[0];
                break;
        }
        *HEX3_HEX0_ptr = code;
    }
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
	printf("%d\n", *(KEY_ptr + 3));
	printf("%d\n", pressed);
	
    if(*(KEY_ptr + 3) == 0x2) {        // KEY1 pressed (120V)
        current_mode.voltage = 120;
        current_mode.mode_code = 0;
		printf("%d\n", current_mode.voltage);
    }
    else if(*(KEY_ptr + 3) == 0x4) {   // KEY2 pressed (240V)
        current_mode.voltage = 240;
        current_mode.mode_code = 1;
		printf("%d\n", current_mode.voltage);
    }
    else if(*(KEY_ptr + 3) == 0x8) {   // KEY3 pressed (440V)
        current_mode.voltage = 440;
        current_mode.mode_code = 2;
		printf("%d\n", current_mode.voltage);
    }
	
	*(KEY_ptr + 3) = pressed;  // Clear Edge Capture
	printf("%d\n", *(KEY_ptr + 3));
	printf("%d\n", pressed);
	
}

// Configure KEY1-3 interrupts
void set_KEYs(void) {
    volatile int *KEY_ptr = (int *)KEY_BASE;
    *(KEY_ptr + 3) = 0xF;  // Clear any existing presses
    *(KEY_ptr + 2) = 0xE;  // Enable interrupts for KEY1-3 (bits 1-3)
}