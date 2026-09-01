#include <reg51.h>
#include <stdio.h>

// Pin Definitions for 16x2 LCD
sbit RS = P3^4;
sbit EN = P3^5;
#define LCD_DATA P2

// Pin Definitions for ADC0804
sbit ADC_RD = P3^0;
sbit ADC_WR = P3^1;
sbit ADC_INTR = P3^2;
#define ADC_DATA P1

// PWM Control Pin for HVAC Blower/Cooler Fan
sbit PWM_FAN = P3^7;

// PID Controller Parameters
float Kp = 4.0;
float Ki = 0.1;
float Kd = 1.0;

// Variables
unsigned char current_temp = 0;
unsigned char setpoint = 25; // Target temperature in Celsius
float error = 0.0, prev_error = 0.0, integral = 0.0, derivative = 0.0;
float pid_output = 0.0;
unsigned char pwm_duty = 0;

// Function Declarations
void delay_ms(unsigned int ms);
void lcd_cmd(unsigned char cmd);
void lcd_data_char(unsigned char dat);
void lcd_string(char *str);
void lcd_init(void);
unsigned char read_adc(void);
void calculate_pid(void);
void pwm_control(unsigned char duty);

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

void lcd_cmd(unsigned char cmd) {
    LCD_DATA = cmd;
    RS = 0;
    EN = 1;
    delay_ms(2);
    EN = 0;
}

void lcd_data_char(unsigned char dat) {
    LCD_DATA = dat;
    RS = 1;
    EN = 1;
    delay_ms(2);
    EN = 0;
}

void lcd_string(char *str) {
    while (*str) {
        lcd_data_char(*str++);
    }
}

void lcd_init(void) {
    lcd_cmd(0x38); // 8-bit mode, 2 lines, 5x7 matrix
    lcd_cmd(0x0C); // Display ON, Cursor OFF
    lcd_cmd(0x06); // Entry mode, increment cursor
    lcd_cmd(0x01); // Clear display
    delay_ms(5);
}

unsigned char read_adc(void) {
    unsigned char adc_val;
    ADC_WR = 0;
    delay_ms(1);
    ADC_WR = 1;
    
    while (ADC_INTR == 1); // Wait for conversion complete
    
    ADC_RD = 0;
    delay_ms(1);
    adc_val = ADC_DATA;
    ADC_RD = 1;
    return adc_val;
}

void calculate_pid(void) {
    error = (float)current_temp - (float)setpoint;
    
    // Proportional term
    // Integral term with saturation anti-windup
    integral += error;
    if (integral > 100.0) integral = 100.0;
    if (integral < -100.0) integral = -100.0;
    
    // Derivative term
    derivative = error - prev_error;
    prev_error = error;
    
    // PID Equation
    pid_output = (Kp * error) + (Ki * integral) + (Kd * derivative);
    
    // Clamp PID Output to PWM Range (0 - 100%)
    if (pid_output > 100.0) pid_output = 100.0;
    if (pid_output < 0.0) pid_output = 0.0;
    
    pwm_duty = (unsigned char)pid_output;
}

void pwm_control(unsigned char duty) {
    unsigned char i;
    if (duty == 0) {
        PWM_FAN = 0;
        delay_ms(10);
        return;
    }
    if (duty >= 100) {
        PWM_FAN = 1;
        delay_ms(10);
        return;
    }
    
    for (i = 0; i < 100; i++) {
        if (i < duty)
            PWM_FAN = 1;
        else
            PWM_FAN = 0;
        delay_ms(1);
    }
}

void main(void) {
    char buffer[16];
    
    P1 = 0xFF; // Set Port 1 as Input for ADC
    P2 = 0x00; // LCD Data Port
    PWM_FAN = 0;
    
    lcd_init();
    lcd_cmd(0x80);
    lcd_string("INDUSTRIAL HVAC");
    lcd_cmd(0xC0);
    lcd_string("PID CONTROL SYS");
    delay_ms(2000);
    lcd_cmd(0x01);
    
    while (1) {
        current_temp = read_adc();
        calculate_pid();
        
        // Update LCD
        lcd_cmd(0x80); // Line 1
        sprintf(buffer, "SP:%dC Temp:%dC", (int)setpoint, (int)current_temp);
        lcd_string(buffer);
        
        lcd_cmd(0xC0); // Line 2
        sprintf(buffer, "PWM Fan: %d%%  ", (int)pwm_duty);
        lcd_string(buffer);
        
        pwm_control(pwm_duty);
    }
}
