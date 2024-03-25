#include <msp430.h>
#include <string.h>

#define RXD BIT1 // P1.1
#define BUTTON BIT3
#define LED BIT0

char receivedData[50]; // Buffer to store received data
volatile unsigned int dataPos = 0; // Current position in buffer
volatile int dataReceived = 0; // Flag for received data
volatile int lastTick = 0;

enum {
    baltzer,
    bellScience,
    angelStatue,
    chapel,
    work,
    home,
    parents,
    inTransit
} currentLocation = inTransit;

// Define the number of steps for each location
int steps[] = {0, 3, 9, 15, 21, 27, 33, 39, 45};

// Function to calculate steps needed to move from current to desired position
int calculateSteps(int desiredLocation, int currentLocation) {
    int stepsToMove = steps[desiredLocation] - steps[currentLocation];
    if (stepsToMove < 0) {
        stepsToMove += 48; // Assuming there are 48 steps in a full rotation
    }
    return stepsToMove;
}

void moveFace(int tickAmount) {
    int i;
    for (i = 0; i < tickAmount; i++) {
        P2OUT = BIT0;
        __delay_cycles(1000000 / 2);
        P2OUT = BIT1;
        __delay_cycles(1000000 / 2);
        P2OUT = BIT2;
        __delay_cycles(1000000 / 2);
        P2OUT = BIT3;
        __delay_cycles(1000000 / 2);
    }
}

void debounce(){
    __delay_cycles(15000);
}

void main(void) {
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    // Motor setup
    P2DIR |= BIT0 | BIT1 | BIT2 | BIT3;
    P1DIR |= LED;
    P1REN |= BUTTON; // Enable pull-up resistor for the switch
    P1OUT |= BUTTON; // Pull-UP resistor rather than pull-DOWN resistor

    // UART setup
    P1SEL |= RXD; // P1.1 = RXD
    P1SEL2 |= RXD; // P1.1 = RXD
    UCA0CTL1 |= UCSSEL_2; // SMCLK
    UCA0BR0 = 104; // 1MHz 9600 baud
    UCA0BR1 = 0; // 1MHz 9600 baud
    UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
    IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt

    __bis_SR_register(GIE); // Enable global interrupts

    // MAIN LOOP
    while (1) {
        if (dataReceived) {
            char desiredLocation = "";
            // Process received data
            if (strcmp(receivedData, "Balzer Technology") == 0) {
                desiredLocation = baltzer;
            } else if (strcmp(receivedData, "Bell Science") == 0) {
                desiredLocation = bellScience;
            } else if (strcmp(receivedData, "Angel Statue") == 0) {
                desiredLocation = angelStatue;
            } else if (strcmp(receivedData, "Chapel") == 0) {
                desiredLocation = chapel;
            } else if (strcmp(receivedData, "Work") == 0) {
                desiredLocation = work;
            } else if (strcmp(receivedData, "Home") == 0) {
                desiredLocation = home;
            } else if (strcmp(receivedData, "Parents") == 0) {
                desiredLocation = parents;
            } else {
                desiredLocation = inTransit;
            }

            if (desiredLocation) {
                int stepsToMove = calculateSteps(desiredLocation, currentLocation);

                // Move the motor according to stepsToMove
                calculateSteps(desiredLocation, currentLocation);
                moveFace(stepsToMove);

                currentLocation = desiredLocation; // Update current location
            }

            // Clear buffer and flags
            memset(receivedData, 0, sizeof(receivedData));
            dataPos = 0;
            dataReceived = 0;
        }
    }
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
    receivedData[dataPos++] = UCA0RXBUF; // Store char
    if (receivedData[dataPos - 1] == '\n' || receivedData[dataPos - 1] == '\r') {
        receivedData[dataPos - 1] = '\0';
        dataReceived = 1; // Set flag
    }
}
