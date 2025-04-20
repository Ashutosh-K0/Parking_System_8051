#include <reg51.h>

// Pin definitions
sbit RS = P3^0;         // LCD Register Select
sbit RW = P3^1;         // LCD Read/Write
sbit EN = P3^2;         // LCD Enable
sbit ENTRY_MOTOR1 = P3^4;  // Entry gate motor control 1
sbit ENTRY_MOTOR2 = P3^5;  // Entry gate motor control 2
sbit EXIT_MOTOR1 = P3^6;   // Exit gate motor control 1
sbit EXIT_MOTOR2 = P3^7;   // Exit gate motor control 2
sbit ENTRY_SENSOR = P0^0;  // Entry sensor button
sbit EXIT_SENSOR = P0^1;   // Exit sensor button

#define LCD_PORT P2     // LCD data port
#define TOTAL_SLOTS 5  // Total parking capacity

// Global variables
unsigned char available_slots = TOTAL_SLOTS;
const unsigned char secret_pin[] = "123";  // Secret 3-digit PIN
unsigned char input_pin[4];                // User input buffer (3 digits + null)
unsigned char pin_index = 0;               // Current PIN position

// Keypad mapping for 4x3 keypad
const unsigned char keymap[4][3] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};

// Function prototypes
void delay_ms(unsigned int ms);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char dat);
void lcd_string(unsigned char *str);
void lcd_init(void);
unsigned char keypad_scan(void);
void get_pin(void);
void update_display(void);
void open_gate(unsigned char gate);

// Delay function
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i=0; i<ms; i++)
        for(j=0; j<1275; j++);  // Calibrated for 11.0592 MHz crystal
}

// LCD command function
void lcd_cmd(unsigned char cmd) {
    LCD_PORT = cmd;
    RS = 0;
    RW = 0;
    EN = 1;
    delay_ms(2);
    EN = 0;
}

// LCD data function
void lcd_data(unsigned char dat) {
    LCD_PORT = dat;
    RS = 1;
    RW = 0;
    EN = 1;
    delay_ms(2);
    EN = 0;
}

// LCD string display function
void lcd_string(unsigned char *str) {
    while(*str)
        lcd_data(*str++);
}

// LCD initialization
void lcd_init(void) {
    delay_ms(20);
    lcd_cmd(0x38);  // 8-bit mode, 2 lines, 5x7 dots
    delay_ms(5);
    lcd_cmd(0x0C);  // Display on, cursor off
    lcd_cmd(0x06);  // Increment cursor
    lcd_cmd(0x01);  // Clear display
    delay_ms(2);
}

// Keypad scan function
unsigned char keypad_scan(void) {
    unsigned char row, col;
    unsigned char row_mask[] = {0xFE, 0xFD, 0xFB, 0xF7}; // Setting each row low

    // Loop through each row
    for(row = 0; row < 4; row++) {
        P1 = 0xFF;               // Set all pins to high first (deactivate rows)
        P1 = row_mask[row];      // Activate the current row (set it low)
        delay_ms(2);

        // Check each column (P1.4 to P1.6)
        for(col = 0; col < 3; col++) {
            if(!(P1 & (1 << (col + 4)))) {  // Check if the column is low
                while(!(P1 & (1 << (col + 4))));  // Wait for key release
                delay_ms(10);                     // Debounce
                return keymap[row][col];          // Return the key pressed
            }
        }
    }

    return 0; // No key pressed
}

// Get 3-digit PIN from keypad
void get_pin(void) {
    unsigned char key, i, valid;

    while(1) {
        for(i = 0; i < 4; i++) {
            input_pin[i] = 0;
        }

        lcd_cmd(0x01);
        lcd_string("Enter PIN:");
        lcd_cmd(0xC0);

        pin_index = 0;
        while(pin_index < 3) {
            key = keypad_scan();
            if(key) {
                input_pin[pin_index] = key;
                lcd_data(key);
                pin_index++;
            }
        }

        // Validate 3-digit PIN
        valid = 1;
        for(i = 0; i < 3; i++) {
            if(input_pin[i] != secret_pin[i]) {
                valid = 0;
                break;
            }
        }

        if(valid) {
            return;
        } else {
            lcd_cmd(0x01);
            lcd_string("Invalid PIN");
            delay_ms(1000);
        }
    }
}

// Update LCD with available slots
void update_display(void) {
    lcd_cmd(0x01);
    lcd_string("Available Slots:");
    lcd_cmd(0xC0);

    if(available_slots == 0) {
        lcd_string("PARKING FULL");
    } else {
        lcd_data((available_slots / 10) + '0');
        lcd_data((available_slots % 10) + '0');
    }
}

// Open gate function with 5-second delay
void open_gate(unsigned char gate) {
    TMOD = 0x01;  // Timer 0, Mode 1 (16-bit)

    if(gate == 1) {
        ENTRY_MOTOR1 = 1;
        ENTRY_MOTOR2 = 0;
        if(available_slots > 0) {
            available_slots--;
        }
    } else {
        EXIT_MOTOR1 = 1;
        EXIT_MOTOR2 = 0;
        if(available_slots < TOTAL_SLOTS) {
            available_slots++;
        }
    }

    // Set the timer to delay for approximately 5 seconds
    TH0 = 0x3C; // High byte for 5-second delay (0x3C00 = 5 seconds)
    TL0 = 0xB0; // Low byte for 5-second delay (0xB0 = 176)

    TF0 = 0;
    TR0 = 1;    // Start Timer 0

    while(!TF0); // Wait for timer overflow (5 seconds)
    TR0 = 0;     // Stop Timer 0
    TF0 = 0;     // Clear Timer Flag

    // Stop the motors after 5 seconds
    ENTRY_MOTOR1 = 0;
    ENTRY_MOTOR2 = 0;
    EXIT_MOTOR1 = 0;
    EXIT_MOTOR2 = 0;
}

// Main function
void main(void) {
    P0 = 0xFF;

    lcd_init();
    update_display();

    while(1) {
        // Entry Handling
        if(ENTRY_SENSOR == 0) {
            delay_ms(20);
            if(ENTRY_SENSOR == 0) {
                if(available_slots > 0) {
                    get_pin();
                    open_gate(1);
                } else {
                    lcd_cmd(0x01);
                    lcd_string("PARKING FULL");
                    delay_ms(2000);
                }
                update_display();
                while(ENTRY_SENSOR == 0);
                delay_ms(20);
            }
        }

        // Exit Handling
        if(EXIT_SENSOR == 0) {
            delay_ms(20);
            if(EXIT_SENSOR == 0) {
                open_gate(0);
                update_display();
                while(EXIT_SENSOR == 0);
                delay_ms(20);
            }
        }
    }
}