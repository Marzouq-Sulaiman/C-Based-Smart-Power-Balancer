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


#include <stdbool.h>

#endif

#define BIT_CODES_SIZE 10

//variables from merged code
int pixel_buffer_start;
int activeKEY;

residentialVoltage = 110;
commercialVoltage = 220;
industrialVoltage = 440;

double residentialPowerLimit = 1440;
double commercialPowerLimit = 2400;
double industrialPowerLimit = 6200;

void swap(int* one, int* two) {
    int temp = *one;
    *one = *two;
    *two = temp;
  }
  
  void plot_pixel(int x, int y, short int line_color);
  
  void clear_screen() {
    for (int x = 0; x < 320; x++) {
      for (int y = 0; y < 240; y++) {
        plot_pixel(x, y, 0x0);  // pass black as colour to draw with
      }
    }
  }
  
  void draw_box(int x, int y, int width, int height, short int colour) {
    int rightEdge = x + width;
    int lowerEdge = y + height;
    for (int j = y; j < lowerEdge; j++) draw_line(x, j, rightEdge, j, colour);
  }
  
  void draw_line(int x0, int y0, int x1, int y1, int colourHex) {
    bool isSteep = abs(y1 - y0) > abs(x1 - x0);
    if (isSteep) {
      swap(&x0, &y0);
      swap(&x1, &y1);
    }
  
    if (x0 > x1) {
      swap(&x0, &x1);
      swap(&y0, &y1);
    }
  
    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = -(deltax / 2);
    int y = y0;
    int y_step;
  
    if (y0 < y1) {
      y_step = 1;
  
    } else {
      y_step = -1;
    }
  
    for (int x = x0; x <= x1; x++) {
      if (isSteep) {
        plot_pixel(y, x, colourHex);
      } else {
        plot_pixel(x, y, colourHex);
        error = error + deltay;
      }
      if (error > 0) {
        y = y + y_step;
        error = error - deltax;
      }
    }
  }
  
  void drawChar(int x, int y, char presentChar, short int color) {
    char fontMap[63][7] = {
  
        {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001},  // A
        {0b11110, 0b10001, 0b11110, 0b10001, 0b10001, 0b10001, 0b11110},  // B
        {0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110},  // C
        {0b11100, 0b10010, 0b10001, 0b10001, 0b10001, 0b10010, 0b11100},  // D
        {0b11111, 0b10000, 0b11100, 0b10000, 0b10000, 0b10000, 0b11111},  // E
        {0b11111, 0b10000, 0b11100, 0b10000, 0b10000, 0b10000, 0b10000},  // F
        {0b01110, 0b10001, 0b10000, 0b10111, 0b10001, 0b10001, 0b01110},  // G
        {0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001, 0b10001},  // H
        {0b01110, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110},  // I
        {0b00111, 0b00010, 0b00010, 0b00010, 0b10010, 0b10010, 0b01100},  // J
        {0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001},  // K
        {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111},  // L
        {0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001},  // M
        {0b10001, 0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001},  // N
        {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110},  // O
        {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000},  // P
        {0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101},  // Q
        {0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001},  // R
        {0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110},  // S
        {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100},  // T
        {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110},  // U
        {0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b01010, 0b00100},  // V
        {0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b10101, 0b01010},  // W
        {0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001},  // X
        {0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100},  // Y
        {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111},  // Z
  
        {0b00000, 0b00000, 0b01110, 0b00001, 0b01111, 0b10001, 0b01111},  // a
        {0b10000, 0b10000, 0b10110, 0b11001, 0b10001, 0b11001, 0b10110},  // b
        {0b00000, 0b00000, 0b01110, 0b10000, 0b10000, 0b10000, 0b01110},  // c
        {0b00001, 0b00001, 0b01101, 0b10011, 0b10001, 0b10011, 0b01101},  // d
        {0b00000, 0b00000, 0b01110, 0b10001, 0b11111, 0b10000, 0b01110},  // e
        {0b00110, 0b01001, 0b01000, 0b11100, 0b01000, 0b01000, 0b01000},  // f
        {0b00000, 0b01111, 0b10001, 0b10001, 0b01111, 0b00001, 0b01110},  // g
        {0b10000, 0b10000, 0b10110, 0b11001, 0b10001, 0b10001, 0b10001},  // h
        {0b00100, 0b00000, 0b01100, 0b00100, 0b00100, 0b00100, 0b01110},  // i
        {0b00010, 0b00000, 0b00110, 0b00010, 0b00010, 0b10010, 0b01100},  // j
        {0b10000, 0b10000, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010},  // k
        {0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110},  // l
        {0b00000, 0b00000, 0b11010, 0b10101, 0b10101, 0b10101, 0b10101},  // m
        {0b00000, 0b00000, 0b10110, 0b11001, 0b10001, 0b10001, 0b10001},  // n
        {0b00000, 0b00000, 0b01110, 0b10001, 0b10001, 0b10001, 0b01110},  // o
        {0b00000, 0b00000, 0b10110, 0b11001, 0b11001, 0b10110, 0b10000},  // p
        {0b00000, 0b00000, 0b01101, 0b10011, 0b10011, 0b01101, 0b00001},  // q
        {0b00000, 0b00000, 0b10110, 0b11001, 0b10000, 0b10000, 0b10000},  // r
        {0b00000, 0b00000, 0b01111, 0b10000, 0b01110, 0b00001, 0b11110},  // s
        {0b01000, 0b01000, 0b11100, 0b01000, 0b01000, 0b01001, 0b00110},  // t
        {0b00000, 0b00000, 0b10001, 0b10001, 0b10001, 0b10011, 0b01101},  // u
        {0b00000, 0b00000, 0b10001, 0b10001, 0b01010, 0b01010, 0b00100},  // v
        {0b00000, 0b00000, 0b10001, 0b10001, 0b10101, 0b10101, 0b01010},  // w
        {0b00000, 0b00000, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001},  // x
        {0b00000, 0b00000, 0b10001, 0b10001, 0b01111, 0b00001, 0b01110},  // y
        {0b00000, 0b00000, 0b11111, 0b00010, 0b00100, 0b01000, 0b11111},  // z
  
        {0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110},  // 0
        {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110},  // 1
        {0b01110, 0b10001, 0b00001, 0b00110, 0b01000, 0b10000, 0b11111},  // 2
        {0b01110, 0b10001, 0b00001, 0b00110, 0b00001, 0b10001, 0b01110},  // 3
        {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010},  // 4
        {0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110},  // 5
        {0b00110, 0b01000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110},  // 6
        {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000},  // 7
        {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110},  // 8
        {0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00010, 0b01100},  // 9
  
        {0b00000, 0b00000, 0b00100, 0b00000, 0b00000, 0b00100, 0b01000}  // ;
  
    };
  
    int index;
  
    if (presentChar >= 'A' && presentChar <= 'Z')
      index = presentChar - 'A';
    else if (presentChar >= 'a' && presentChar <= 'z')
      index = 26 + (presentChar - 'a');
  
    else if (presentChar >= '0' && presentChar <= '9') {
      index = 52 + (presentChar - '0');
    }
  
    else if (presentChar == ';') {
      index = 62;
    }
  
    else
      return;
    for (int row = 0; row < 7; row++) {
      for (int col = 0; col < 5; col++) {
        if ((fontMap[index][row] >> (4 - col)) & 1)
          plot_pixel(x + col, y + row, color);
      }
    }
  }
  
  void drawText(char* str, int x, int y, short int color) {
    while (*str != 0) {
      drawChar(x, y, *str, color);
      x += 6;  // spacing between characters
      str++;
    }
  }
  
  void setupVGA(){
    draw_box(20, 25, 80, 140, 0x001F);
    draw_box(120, 25, 80, 140, 0x07E0);
    draw_box(220, 25, 80, 140, 0xF81F);
  
    // drawText();
    drawText("110V", 48, 15, 0xFFFF);
    drawText("220V", 148, 15, 0xFFFF);
    drawText("440V", 248, 15, 0xFFFF);
  }

  void plot_pixel(int x, int y, short int line_color) {
    volatile short int* one_pixel_address;
  
    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
  
    *one_pixel_address = line_color;
  }
  

//END OF MERGED FUNCTIONS

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

// Global variables for device management
int currents[9] = {0};      // Stores current values for devices A, B, C
int selected_device = 0;    // 0 for A, 1 for B, 2 for C and add 3 for future ones
int input_mode = 0;         // 0: display saved value, 1: entering new value
char digit_buffer[2] = {0}; // Buffer for entered digits
int digit_count = 0;        // Number of digits entered
char received_byte;

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

void updateDisplayedCurrent() {
    //char* current = getUpdatedCurrent();

    char buffers[9][3];  

    for (int i = 0; i < 9; i++) {
        buffers[i][0] = (currents[i] / 10) + '0'; 
        buffers[i][1] = (currents[i] % 10) + '0'; 
        buffers[i][2] = '\0';                      
    }

  //   if (received_byte == 0x1C){ //A
  //     activeKEY = 
  //   }
  //   received_byte == 0x21 //C
                   
  // if (current_mode.mode_code == 0){
  //     activeKEY = 0
  // }



    if (current_mode.mode_code == 0 == 0){
        if (selected_device == 0){
            drawText(buffers[0], 70, 45, 0xFFFF);
        }
        else if (selected_device == 1){
            drawText(buffers[1], 70, 95, 0xFFFF);
        }
        else if (selected_device == 2){
            drawText(buffers[2], 70, 135, 0xFFFF);
        }
    }
    else if (current_mode.mode_code == 1){
        if (selected_device == 0){
            drawText(buffers[3], 170, 45, 0xFFFF);
        }
        else if (selected_device == 1){
            drawText(buffers[4], 170, 95, 0xFFFF);
        }
        else if (selected_device == 2){
            drawText(buffers[5], 170, 135, 0xFFFF);
        }
    }
    else if (current_mode.mode_code == 2){
        if (selected_device == 0){
            drawText(buffers[6], 270, 45, 0xFFFF);
        }
        else if (selected_device == 1){
            drawText(buffers[7], 270, 95, 0xFFFF);
        }
        else if (selected_device == 2){
            drawText(buffers[8], 270, 135, 0xFFFF);
        }
    }
}



/*******************************************************************************
 * Main program: Handles device selection and current input via PS/2 keyboard
 ******************************************************************************/
int main(void) {

    //MERGED START
    volatile int* pixel_ctrl_ptr = (int*)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;

    clear_screen();
    setupVGA();
    //MERGED END



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

        //START MERGED
        if (currents){
            
        }
        if (residentialVoltage* (currents[0]*SW_on_off[0] + currents[1]*SW_on_off[1] + currents[2]*SW_on_off[2]) > residentialPowerLimit){
            draw_box(20, 25, 300, 140, 0xFFFF);
            drawText("MASTER CAUTION: Residential Power Limit Exceeded", 120, 120, 0x0000);
            drawText("Power Disconnected To Prevent Equipment Damage", 120, 140, 0x0000);
            drawText("Resolve Issue Before Restarting System", 120, 160, 0x0000);
        }
        else if (commercialVoltage* (currents[3]*SW_on_off[3] + currents[4]*SW_on_off[4] + currents[5]*SW_on_off[5]) > commercialPowerLimit){
            draw_box(20, 25, 300, 140, 0xFFFF);
            drawText("MASTER CAUTION: Commercial Power Limit Exceeded", 120, 120, 0x0000);
            drawText("Power Disconnected To Prevent Equipment Damage", 120, 140, 0x0000);
            drawText("Resolve Issue Before Restarting System", 120, 160, 0x0000);
        }
        else if (industrialVoltage* (currents[6]*SW_on_off[6] + currents[7]*SW_on_off[7] + currents[8]*SW_on_off[8]) > industrialPowerLimit){
            draw_box(20, 25, 300, 140, 0xFFFF);
            drawText("MASTER CAUTION: Industrial Power Limit Exceeded", 120, 120, 0x0000);
            drawText("Power Disconnected To Prevent Equipment Damage", 120, 140, 0x0000);
            drawText("Resolve Issue Before Restarting System", 120, 160, 0x0000);
        }
        //END MERGED

        PS2_data = *(PS2_ptr);
        RVALID = PS2_data & 0x8000;

        if (RVALID && current_mode.voltage == 120) {
            received_byte = PS2_data & 0xFF;

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

                        // Get previous current value for this device
                        int previous_current = currents[selected_device];
                        
                        // Update device count only when transitioning between 0/non-zero
                        if (previous_current == 0 && entered_current != 0) {
                            if (total_devices < 9) total_devices++;
                        } 
                        else if (previous_current != 0 && entered_current == 0) {
                            if (total_devices > 0) total_devices--;
                        }

                        printf("num devices: %d\n", total_devices);

                        currents[selected_device] = entered_current;
                        input_mode = 0;
                        // Display saved current
                        update_display(selected_device, entered_current/10, entered_current%10);
                    }
                    digit_count = 0;
                    digit_buffer[0] = digit_buffer[1] = 0;
                    printf("the voltage group is 120 and the currents for the group are:\n");
                    for (int i = 0; i < 9; i++){
                        printf("device %d: %d\n", i, currents[i]);
                    }

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
            received_byte = PS2_data & 0xFF;

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

                        // Get previous current value for this device
                        int previous_current = currents[selected_device];
                        
                        // Update device count only when transitioning between 0/non-zero
                        if (previous_current == 0 && entered_current != 0) {
                            if (total_devices < 9) total_devices++;
                        } 
                        else if (previous_current != 0 && entered_current == 0) {
                            if (total_devices > 0) total_devices--;
                        }

                        printf("num devices: %d\n", total_devices);

                        currents[selected_device] = entered_current;
                        input_mode = 0;
                        // Display saved current
                        update_display(selected_device, entered_current/10, entered_current%10);
                    }
                    digit_count = 0;
                    digit_buffer[0] = digit_buffer[1] = 0;
                    printf("the voltage group is 220 and the currents for the group are:\n");
                    for (int i = 0; i < 9; i++){
                        printf("device %d: %d\n", i, currents[i]);
                    }
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
            received_byte = PS2_data & 0xFF;

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

                        // Get previous current value for this device
                        int previous_current = currents[selected_device];
                        
                        // Update device count only when transitioning between 0/non-zero
                        if (previous_current == 0 && entered_current != 0) {
                            if (total_devices < 9) total_devices++;
                        } 
                        else if (previous_current != 0 && entered_current == 0) {
                            if (total_devices > 0) total_devices--;
                        }

                        printf("num devices: %d\n", total_devices);

                        currents[selected_device] = entered_current;
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

        if (total_devices == 0) {
            *LEDR_ptr = 0;
        } else {
            // Use correct bitmask for the number of devices
            *LEDR_ptr = *SW_ptr & SW_codes[total_devices];
        }

        for (int i = 0; i < 9; i++) {
            SW_on_off[i] = (*LEDR_ptr >> i) & 1;
        }

        totalPower = 0;
        for (int i = 0; i < 9; i++) {
            int voltage;
            if (i < 3) voltage = 120;
            else if (i < 6) voltage = 220;
            else voltage = 240;
            
            power[i] = currents[i] * voltage * SW_on_off[i];
            totalPower += power[i];
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
