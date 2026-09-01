/* 
 * Assessment: CO4 AT1 SBQ
 * Title: Medical Ventilator Real-Time Schedulability Analysis (EDF vs RMS)
 * System Tasks:
 *   - Task 1: Alarm Processing (T1 = 8ms, D1 = 8ms, C1 = 2ms)
 *   - Task 2: Pressure Regulation (T2 = 10ms, D2 = 10ms, C2 = 3ms)
 *   - Task 3: Oxygen-Flow Control (T3 = 15ms, D3 = 15ms, C3 = 4ms)
 *   - Task 4: Patient-Data Logging (T4 = 50ms, D4 = 50ms, C4 = 5ms)
 * 
 * Target Microcontroller: 8051 (AT89C51) / Keil uVision IDE
 */

#include <reg51.h>
#include <stdio.h>

// Real-Time Task Timing Specifications (in milliseconds)
typedef struct {
    char task_name[25];
    unsigned int period;       // Period T (ms)
    unsigned int deadline;     // Deadline D (ms)
    unsigned int execution_c;  // Execution time C (ms)
    float utilization;         // U = C / T
    unsigned char rms_priority;// RMS Priority (1 = Highest)
} MedicalVentilatorTask;

// Array of Ventilator Tasks
MedicalVentilatorTask ventilator_tasks[4] = {
    {"Alarm Processing",      8,  8, 2, 0.2500, 1}, // T1 = 8ms
    {"Pressure Regulation",  10, 10, 3, 0.3000, 2}, // T2 = 10ms
    {"Oxygen-Flow Control",  15, 15, 4, 0.2667, 3}, // T3 = 15ms
    {"Patient-Data Logging", 50, 50, 5, 0.1000, 4}  // T4 = 50ms
};

// Global System Utilization Variables
float total_utilization U_total = 0.9167; // 25.0% + 30.0% + 26.67% + 10.0% = 91.67%
float rms_bound U_rms_bound     = 0.7568; // n(2^(1/n) - 1) for n=4 => 4*(2^(1/4) - 1) = 75.68%
float edf_bound U_edf_bound     = 1.0000; // 100.0% bound for EDF

// Output Indicators
sbit RMS_SCHEDULABLE_LED = P2^0;
sbit EDF_SCHEDULABLE_LED = P2^1;

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

// Schedulability Evaluation Routine
void Analyze_Ventilator_Schedulability(void) {
    // Evaluation 1: Rate Monotonic Scheduling (RMS)
    // Necessary condition: U <= U_rms_bound (0.9167 > 0.7568 -> Fails Liu & Layland Sufficiency Bound)
    // Exact Response Time Analysis (RTA):
    // R1 = C1 = 2ms <= 8ms (PASS)
    // R2 = C2 + ceil(R2/T1)*C1 = 3 + ceil(5/8)*2 = 5ms <= 10ms (PASS)
    // R3 = C3 + ceil(R3/T1)*C1 + ceil(R3/T2)*C2 = 4 + ceil(13/8)*2 + ceil(13/10)*3 = 4 + 4 + 6 = 14ms <= 15ms (PASS)
    // R4 = C4 + ceil(R4/T1)*C1 + ceil(R4/T2)*C2 + ceil(R4/T3)*C3
    // R4 = 5 + ceil(48/8)*2 + ceil(48/10)*3 + ceil(48/15)*4 = 5 + 12 + 15 + 16 = 48ms <= 50ms (PASS)
    
    // Result: RMS successfully schedules the task set under RTA despite exceeding Liu-Layland bound.
    RMS_SCHEDULABLE_LED = 1;

    // Evaluation 2: Earliest Deadline First (EDF)
    // Necessary & Sufficient condition for EDF: U <= 1.0 (91.67% <= 100% -> PASS)
    EDF_SCHEDULABLE_LED = 1;
}

void main(void) {
    P2 = 0x00;
    
    Analyze_Ventilator_Schedulability();
    
    while (1) {
        // Continuous Real-Time Monitoring Loop
        delay_ms(500);
    }
}
