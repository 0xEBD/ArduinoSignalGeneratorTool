// Digital Pattern Generator for 8 channels (D2–D9)
#define MAX_SEQUENCE_LENGTH 255
#define PATTERN_ARRAY_SIZE ((MAX_SEQUENCE_LENGTH + 7) / 8) // Calculate bytes needed

const uint8_t outputPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};

// Store patterns bit-packed: each row is a channel, columns are bytes holding 8 steps each
uint8_t channelPatterns[8][PATTERN_ARRAY_SIZE];
uint8_t sequenceLength = 8;      // Anzahl Schritte pro Sequenz
unsigned int delayMs = 100;      // Taktzeit zwischen Schritten
bool running = false;

void setup() {
    Serial.begin(9600);
    while (!Serial); // warten auf seriellen Monitor
    for (uint8_t i = 0; i < 8; i++) {
        pinMode(outputPins[i], OUTPUT);
        // Initialize patterns to all zeros
        for (uint8_t j = 0; j < PATTERN_ARRAY_SIZE; j++) {
            channelPatterns[i][j] = 0;
        }
    }
    Serial.println("Digital Pattern Generator Ready.");
}

void loop() {
    if (running) {
        runSequence();
    } else {
        showMenu();
    }
}

// ==================== Menüsteuerung ====================

void showMenu() {
    Serial.println(F("\n=== Digital Pattern Generator ==="));
    Serial.println(F("1) Set bit pattern for each channel"));
    Serial.println(F("2) Set sequence length"));
    Serial.println(F("3) Set delay between steps (ms)"));
    Serial.println(F("4) Start sequence output"));
    Serial.println(F("5) Stop output"));
    Serial.println(F("6) Show current configuration"));
    Serial.println(F("7) Exit"));
    Serial.print(F("> "));

    while (!Serial.available());
    char choice = Serial.read();

    switch (choice) {
        case '1': configPatterns(); break;
        case '2': configLength(); break;
        case '3': configDelay(); break;
        case '4': running = true; break;
        case '5': running = false; break;
        case '6': showConfig(); break;
        case '7': Serial.println("Bye!"); delay(500); break;
        default: Serial.println("Invalid option."); break;
    }
}

// ==================== Konfiguration ====================

void configPatterns() {
    Serial.println("\nEnter channel number (0–7): ");
    while (!Serial.available());
    int ch = Serial.parseInt();
    // Flush remaining input
    while (Serial.read() != '\n' && Serial.available());

    if (ch < 0 || ch > 7) {
        Serial.println("Invalid channel number.");
        return;
    }

    while (true) {
        Serial.print("Enter pattern (exactly ");
        Serial.print(sequenceLength);
        Serial.println(" bits, e.g. 10101010): ");

        char inputBuffer[MAX_SEQUENCE_LENGTH + 1]; // Buffer to read input
        int byteCount = 0;
        while (byteCount < sequenceLength) {
             if (Serial.available()) {
                 char c = Serial.read();
                 if (c == '\n' || c == '\r') {
                     if (byteCount > 0) break; // End of input if we have characters
                     else continue; // Ignore leading newline/CR
                 }
                 if (c != '0' && c != '1') {
                     Serial.println("\nError: Only 0 or 1 allowed. Please re-enter:");
                     byteCount = 0; // Reset buffer
                     // Flush remaining bad input
                     while(Serial.read() != '\n' && Serial.available());
                     continue;
                 }
                 if (byteCount < MAX_SEQUENCE_LENGTH) {
                    inputBuffer[byteCount++] = c;
                 } else {
                    // Input too long, discard extra chars until newline
                    while(Serial.read() != '\n' && Serial.available());
                    break;
                 }
             }
             // Add a small delay or yield() if needed for stability on some boards
             // delay(1);
        }
        inputBuffer[byteCount] = '\0'; // Null-terminate

        if (byteCount != sequenceLength) {
            Serial.print("Error: Pattern must be exactly ");
            Serial.print(sequenceLength);
            Serial.print(" bits long. You entered ");
            Serial.print(byteCount);
            Serial.println(" bits.");
            continue; // Ask for input again
        }

        // Clear the existing pattern bits for this channel
        for (uint8_t i = 0; i < PATTERN_ARRAY_SIZE; i++) {
            channelPatterns[ch][i] = 0;
        }

        // Convert the input string to bit-packed format
        for (uint8_t step = 0; step < sequenceLength; step++) {
            if (inputBuffer[step] == '1') {
                uint8_t byteIndex = step / 8;
                uint8_t bitIndex = step % 8;
                channelPatterns[ch][byteIndex] |= (1 << bitIndex); // Set the bit
            }
        }

        Serial.print("Pattern updated for channel ");
        Serial.print(ch);
        Serial.print(": ");
        // Print the stored pattern for verification
        for (uint8_t step = 0; step < sequenceLength; step++) {
             uint8_t byteIndex = step / 8;
             uint8_t bitIndex = step % 8;
             Serial.print((channelPatterns[ch][byteIndex] >> bitIndex) & 1 ? '1' : '0');
        }
        Serial.println();
        break; // Exit the while(true) loop
    }
}

void configLength() {
    Serial.println("\nEnter new sequence length (1–255): ");
    while (!Serial.available());
    int len = Serial.parseInt();
    // Flush remaining input
    while (Serial.read() != '\n' && Serial.available());

    if (len < 1 || len > MAX_SEQUENCE_LENGTH) { // Use MAX_SEQUENCE_LENGTH
        Serial.println("Invalid length.");
        return;
    }
    uint8_t oldLength = sequenceLength;
    sequenceLength = (uint8_t)len;

    // If the new length is shorter, clear bits beyond the new length
    // If longer, the new bits default to 0 (already handled by initialization or previous clearing)
    if (sequenceLength < oldLength) {
        for (int ch = 0; ch < 8; ch++) {
            for (int step = sequenceLength; step < oldLength; step++) {
                 uint8_t byteIndex = step / 8;
                 uint8_t bitIndex = step % 8;
                 // Clear the bit
                 channelPatterns[ch][byteIndex] &= ~(1 << bitIndex);
            }
        }
    }
    // Note: Existing patterns are kept up to the new length.
    // If the length increased, the new steps will be '0' unless set later.

    Serial.print("Sequence length set to ");
    Serial.println(sequenceLength);
}

void configDelay() {
    Serial.println("\nEnter delay between steps in ms: ");
    while (!Serial.available());
    int d = Serial.parseInt();
    if (d < 0) {
        Serial.println("Invalid delay.");
        return;
    }
    delayMs = d;
    Serial.print("Delay set to ");
    Serial.print(delayMs);
    Serial.println(" ms");
}

void showConfig() {
    Serial.println("\n--- Current Configuration ---");
    Serial.print("Sequence length: ");
    Serial.println(sequenceLength);
    Serial.print("Delay: ");
    Serial.print(delayMs);
    Serial.println(" ms");
    for (int i = 0; i < 8; i++) {
        Serial.print("Channel ");
        Serial.print(i);
        Serial.print(" (D");
        Serial.print(outputPins[i]);
        Serial.print("): ");
        // Read bits from the packed array
        for (uint8_t step = 0; step < sequenceLength; step++) {
             uint8_t byteIndex = step / 8;
             uint8_t bitIndex = step % 8;
             Serial.print((channelPatterns[i][byteIndex] >> bitIndex) & 1 ? '1' : '0');
        }
        Serial.println();
    }
    Serial.println("-----------------------------");
}

// ==================== Ausgabe ====================

void runSequence() {
    Serial.println("Running sequence. Press '5' to stop.");
    while (running) {
        for (uint8_t step = 0; step < sequenceLength; step++) {
            for (uint8_t ch = 0; ch < 8; ch++) {
                // Calculate byte and bit index for the current step
                uint8_t byteIndex = step / 8;
                uint8_t bitIndex = step % 8;

                // Extract the bit for this channel and step
                bool bitState = (channelPatterns[ch][byteIndex] >> bitIndex) & 1;

                digitalWrite(outputPins[ch], bitState ? HIGH : LOW);
            }
            delay(delayMs);
            if (Serial.available()) {
                char c = Serial.read();
                if (c == '5') {
                    running = false;
                    // Turn off all pins when stopping
                    for(uint8_t ch=0; ch<8; ch++) {
                        digitalWrite(outputPins[ch], LOW);
                    }
                    Serial.println("\nSequence stopped.");
                    return; // Exit runSequence immediately
                }
            }
        } // end step loop
        // Optional: Add check for stop condition here too if sequence length is very long
        // and delayMs is very short, to avoid waiting a full cycle.
    } // end while(running)

    // Sequence finished naturally (if running wasn't set to false by stop command)
    // Turn off all pins
    for(uint8_t ch=0; ch<8; ch++) {
        digitalWrite(outputPins[ch], LOW);
    }
    Serial.println("Sequence done.");
    // running = false; // Already false if loop exited naturally
}