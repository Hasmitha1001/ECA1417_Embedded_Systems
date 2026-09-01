/* 
 * Assessment 3: Inter-Task Communication using Mailbox (RTOS / Embedded C)
 * Microcontroller: 8051 (AT89C51) / Keil C51 Compiler
 */

#include <reg51.h>
#include <stdio.h>

// Mailbox Status Return Codes
typedef enum {
    MAILBOX_EMPTY = 0,
    MAILBOX_FULL = 1,
    MAILBOX_SUCCESS = 2
} MailboxStatus;

// Message Structure definition
typedef struct {
    unsigned char task_id;
    unsigned int data_payload;
} Message;

// Mailbox Structure definition
typedef struct {
    Message msg;
    unsigned char is_full; // 1 if mail exists, 0 if empty
} Mailbox;

// Declare Global Mailbox Instance
Mailbox system_mailbox;

// Pin Definitions for Keil Debugger / Logic Simulation Output
sbit SENDER_LED = P2^0;
sbit RECEIVER_LED = P2^1;

// Function Declarations
void delay_ms(unsigned int ms);
MailboxStatus Mailbox_Post(Mailbox *mb, Message new_msg);
MailboxStatus Mailbox_Fetch(Mailbox *mb, Message *received_msg);
void Sender_Task(void);
void Receiver_Task(void);

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

// Mailbox Post Function (Task 1 / Sender)
MailboxStatus Mailbox_Post(Mailbox *mb, Message new_msg) {
    if (mb->is_full == 1) {
        return MAILBOX_FULL; // Cannot post, mailbox is occupied
    }
    mb->msg = new_msg;
    mb->is_full = 1; // Mark mailbox as full
    return MAILBOX_SUCCESS;
}

// Mailbox Fetch Function (Task 2 / Receiver)
MailboxStatus Mailbox_Fetch(Mailbox *mb, Message *received_msg) {
    if (mb->is_full == 0) {
        return MAILBOX_EMPTY; // Cannot fetch, mailbox is empty
    }
    *received_msg = mb->msg;
    mb->is_full = 0; // Clear mailbox after reading
    return MAILBOX_SUCCESS;
}

// Task 1: Producer / Sender Task
void Sender_Task(void) {
    static unsigned int telemetry_counter = 100;
    Message tx_message;
    MailboxStatus status;

    tx_message.task_id = 1; // Task ID 1
    tx_message.data_payload = telemetry_counter++;

    status = Mailbox_Post(&system_mailbox, tx_message);
    if (status == MAILBOX_SUCCESS) {
        SENDER_LED = 1; // Blink LED to indicate successful message post
        delay_ms(50);
        SENDER_LED = 0;
    }
}

// Task 2: Consumer / Receiver Task
void Receiver_Task(void) {
    Message rx_message;
    MailboxStatus status;

    status = Mailbox_Fetch(&system_mailbox, &rx_message);
    if (status == MAILBOX_SUCCESS) {
        RECEIVER_LED = 1; // Blink LED to indicate successful message fetch
        delay_ms(50);
        RECEIVER_LED = 0;
    }
}

// Main Scheduler Loop
void main(void) {
    // Initialize Mailbox
    system_mailbox.is_full = 0;
    P2 = 0x00; // Clear Port 2

    while (1) {
        // Step 1: Execute Sender Task
        Sender_Task();
        delay_ms(200);

        // Step 2: Execute Receiver Task
        Receiver_Task();
        delay_ms(200);
    }
}
