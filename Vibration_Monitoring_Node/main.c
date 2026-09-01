#include <reg51.h>
#include <stdio.h>

// Pin Definitions for ADC0804 (Vibration Sensor Signal Conditioning Interface)
sbit ADC_RD = P3^0;
sbit ADC_WR = P3^1;
sbit ADC_INTR = P3^2;
#define ADC_DATA P1

// Pin Definitions for Status Indicators
sbit LED_NORMAL = P2^0;
sbit LED_WARNING = P2^1;
sbit LED_ALARM = P2^2;
sbit BUZZER = P2^3;

// Vibration Threshold Constants (g-force / raw ADC units)
#define THRESHOLD_WARNING 120  // Approx 2.5g vibration
#define THRESHOLD_ALARM   180  // Approx 4.0g severe vibration

// Function Declarations
void delay_ms(unsigned int ms);
void uart_init(void);
void uart_send_char(char ch);
void uart_send_string(char *str);
unsigned char read_vibration_adc(void);
void process_vibration_data(unsigned char vib_val);

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

// UART Initialization for 9600 Baud @ 11.0592 MHz Crystal
void uart_init(void) {
    TMOD |= 0x20; // Timer 1 Mode 2 (8-bit Auto-Reload)
    TH1 = 0xFD;   // 9600 Baud rate
    TR1 = 1;      // Start Timer 1
    SCON = 0x50;  // 8-bit UART, REN enabled
}

void uart_send_char(char ch) {
    SBUF = ch;
    while (TI == 0);
    TI = 0;
}

void uart_send_string(char *str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

unsigned char read_vibration_adc(void) {
    unsigned char adc_val;
    
    // Start ADC conversion (WR Pulse Low to High)
    ADC_WR = 0;
    delay_ms(1);
    ADC_WR = 1;
    
    // Wait for Conversion Complete (INTR active Low)
    while (ADC_INTR == 1);
    
    // Read Digital Data
    ADC_RD = 0;
    delay_ms(1);
    adc_val = ADC_DATA;
    ADC_RD = 1;
    
    return adc_val;
}

void process_vibration_data(unsigned char vib_val) {
    char buffer[50];
    float g_force = ((float)vib_val / 255.0) * 5.0; // Scaled acceleration in g-units
    
    if (vib_val < THRESHOLD_WARNING) {
        // Normal State
        LED_NORMAL = 1;
        LED_WARNING = 0;
        LED_ALARM = 0;
        BUZZER = 0;
        
        sprintf(buffer, "[VIB NODE] Status: NORMAL  | ADC: %3d | Accel: %.2fg\r\n", (int)vib_val, g_force);
    } 
    else if (vib_val >= THRESHOLD_WARNING && vib_val < THRESHOLD_ALARM) {
        // Warning State
        LED_NORMAL = 0;
        LED_WARNING = 1;
        LED_ALARM = 0;
        BUZZER = 0;
        
        sprintf(buffer, "[VIB NODE] Status: WARNING | ADC: %3d | Accel: %.2fg\r\n", (int)vib_val, g_force);
    } 
    else {
        // Critical Alarm State
        LED_NORMAL = 0;
        LED_WARNING = 0;
        LED_ALARM = 1;
        BUZZER = 1; // Trigger acoustic alarm
        
        sprintf(buffer, "[VIB NODE] Status: ALARM!! | ADC: %3d | Accel: %.2fg | HIGH VIBRATION DETECTED!\r\n", (int)vib_val, g_force);
    }
    
    // Transmit Telemetry over UART Serial
    uart_send_string(buffer);
}

void main(void) {
    unsigned char vib_sample;
    
    P1 = 0xFF; // Port 1 as Input for ADC
    P2 = 0x00; // Port 2 as Output for Status LEDs and Buzzer
    
    uart_init();
    
    uart_send_string("\r\n========================================\r\n");
    uart_send_string(" INDUSTRIAL VIBRATION MONITORING NODE\r\n");
    uart_send_string(" Data Acquisition & Serial Telemetry System\r\n");
    uart_send_string("========================================\r\n\r\n");
    
    while (1) {
        vib_sample = read_vibration_adc();
        process_vibration_data(vib_sample);
        delay_ms(500); // 500ms sampling rate (2 Hz acquisition)
    }
}
