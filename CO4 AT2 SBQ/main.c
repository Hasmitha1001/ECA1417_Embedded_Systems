/* 
 * Assessment: CO4 AT2 SBQ
 * Title: Real-Time Message Queue Dynamics & Overflow Analysis in RTOS
 * Queue Capacity N = 5 messages
 * Arrival Rate  lambda = 2 msg/ms
 * Service Rate  mu = 1 msg/ms
 * 
 * Target Microcontroller: 8051 (AT89C51) / Keil uVision IDE
 */

#include <reg51.h>
#include <stdio.h>

// Queue Constants
#define QUEUE_CAPACITY 5
#define ARRIVAL_RATE   2 // msg/ms
#define SERVICE_RATE   1 // msg/ms
#define NET_GROWTH_RATE (ARRIVAL_RATE - SERVICE_RATE) // 1 msg/ms

// System Queue State Structure
typedef struct {
    unsigned char queue_count;
    unsigned int current_time_ms;
    unsigned char is_overflow;
} MessageQueue;

MessageQueue rtos_queue;

// Pin Indicator for Queue Full/Overflow Alert
sbit OVERFLOW_ALERT_LED = P2^0;

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

// Function to simulate real-time queue accumulation
void Simulate_Queue_Dynamics(void) {
    rtos_queue.queue_count = 0;
    rtos_queue.current_time_ms = 0;
    rtos_queue.is_overflow = 0;

    // Simulate tick-by-tick queue filling until capacity N=5 is reached
    while (rtos_queue.queue_count < QUEUE_CAPACITY) {
        delay_ms(1); // 1 ms time step
        rtos_queue.current_time_ms++;
        
        // Net growth: +1 msg per 1 ms
        rtos_queue.queue_count += NET_GROWTH_RATE;
    }

    // At t = 5 ms, Queue is Full (5 messages)
    rtos_queue.is_overflow = 1;
    OVERFLOW_ALERT_LED = 1; // Trigger alert LED
}

void main(void) {
    P2 = 0x00;
    Simulate_Queue_Dynamics();
    
    while (1) {
        delay_ms(500);
    }
}
