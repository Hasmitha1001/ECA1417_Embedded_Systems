/* 
 * Assessment: CO3 AT2 ABQ
 * Title: Hybrid Periodic & Event-Driven Task Management System using RTOS Mechanisms
 * Microcontroller: 8051 (AT89C51) / Keil C51 Compiler
 */

#include <reg51.h>
#include <stdio.h>

// RTOS Binary Semaphore / Event Flag
typedef struct {
    volatile unsigned char count;
} Semaphore;

// Global RTOS Primitive Mechanisms
Semaphore Event_Semaphore; // Used for Event-Driven Task Synchronization
volatile unsigned int system_ticks = 0;

// IO Pin Definitions
sbit EXTERNAL_EVENT_INT0 = P3^2; // INT0 Pin for Interrupt-driven event
sbit PERIODIC_TASK_LED = P2^0;   // LED indicating periodic task execution
sbit EVENT_TASK_LED = P2^1;      // LED indicating event-driven task execution

// Function Declarations
void delay_ms(unsigned int ms);
void Timer0_ISR(void);
void External_INT0_ISR(void);
void Semaphore_Init(Semaphore *sem, unsigned char init_val);
unsigned char Semaphore_Take(Semaphore *sem);
void Semaphore_Give(Semaphore *sem);
void Periodic_Task_100ms(void);
void Event_Driven_Task(void);

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

// Semaphore Primitive Functions
void Semaphore_Init(Semaphore *sem, unsigned char init_val) {
    sem->count = init_val;
}

unsigned char Semaphore_Take(Semaphore *sem) {
    if (sem->count > 0) {
        sem->count--;
        return 1; // Take successful
    }
    return 0; // Semaphore unavailable
}

void Semaphore_Give(Semaphore *sem) {
    if (sem->count < 255) {
        sem->count++;
    }
}

// Timer 0 Interrupt Service Routine - Periodic System Tick Generator (10ms)
void Timer0_ISR(void) interrupt 1 {
    TH0 = 0xDC; // Reload Timer 0 for 10ms interrupt @ 12MHz
    TL0 = 0x00;
    system_ticks++;
}

// External Interrupt 0 ISR - Hardware Event Trigger
void External_INT0_ISR(void) interrupt 0 {
    // Unblock the Event-Driven Task by Posting/Giving Semaphore
    Semaphore_Give(&Event_Semaphore);
}

// Periodic Task: Runs strictly every 100ms (10 System Ticks)
void Periodic_Task_100ms(void) {
    static unsigned int last_execution_tick = 0;
    
    if ((system_ticks - last_execution_tick) >= 10) { // 10 ticks * 10ms = 100ms
        last_execution_tick = system_ticks;
        
        // Toggle Periodic Task Output Indicator
        PERIODIC_TASK_LED = !PERIODIC_TASK_LED;
    }
}

// Event-Driven Task: Executes immediately upon External Event Semaphore signal
void Event_Driven_Task(void) {
    if (Semaphore_Take(&Event_Semaphore)) {
        // High-Priority Event Handler Code
        EVENT_TASK_LED = 1;
        delay_ms(100);
        EVENT_TASK_LED = 0;
    }
}

// System Hardware & Interrupt Initialization
void System_Init(void) {
    P2 = 0x00; // Output port initialization
    Semaphore_Init(&Event_Semaphore, 0); // Event semaphore starts locked
    
    // Configure Timer 0 for Periodic Tick (Mode 1 16-bit)
    TMOD &= 0xF0;
    TMOD |= 0x01;
    TH0 = 0xDC;
    TL0 = 0x00;
    ET0 = 1; // Enable Timer 0 Interrupt
    TR0 = 1; // Start Timer 0
    
    // Configure External Interrupt 0 (INT0) for Event-Driven Task Triggering
    EX0 = 1; // Enable INT0 Interrupt
    IT0 = 1; // Trigger INT0 on Falling Edge
    
    EA = 1;  // Enable Global Interrupts
}

void main(void) {
    System_Init();
    
    while (1) {
        // Preemptive/Cooperative RTOS Task Scheduler Loop
        
        // 1. Process Event-Driven Task (High Priority - Asynchronous)
        Event_Driven_Task();
        
        // 2. Process Periodic Task (Deterministic Time-Triggered)
        Periodic_Task_100ms();
    }
}
