// OpenAuthentication Time-based One-time Password Algorithm (RFC 6238)

// SparkCore Library

// Original Arduino library by Luca Dentella (http://www.lucadentella.it)
// Adapted for SparkCore by Harrison Jones

// Simple Example

// Usage: 
// 1. Goto http://www.lucadentella.it/OTP/ and choose a 10 digit password/secret and give it a name
// 2. Scan the QR code with your phone in your authenticator app
// 3. Replace the Arduino HEX array hmacKey[] with the one given by the website
// 4. Flash the code to your core
// 5. Open up a serial connection and press anykey
// 6. Compare the code against the authenticator application

#include "sha1.h"
#include "TOTP.h"

// The shared secret is MyLegoDoor
//                    M     y     L     e     g     o     D     o     o     r
uint8_t hmacKey[] = {0x4d, 0x79, 0x4c, 0x65, 0x67, 0x6f, 0x44, 0x6f, 0x6f, 0x72};

TOTP totp = TOTP(hmacKey, 10);
char code[7];


void setup() {
  Serial.begin(9600);
  while(!Serial.available()) Spark.process();
  Serial.println("OpenAuthentication Time-based One-time Password Spark Library");
  Serial.println("Original Arduino library by Luca Dentella");
  Serial.println("Adapted for SparkCore by Harrison Jones");
  Serial.println();
}

void loop() {
    char* newCode = totp.getCode((long)Time.now());
    if(strcmp(code, newCode) != 0) {
        strcpy(code, newCode);
        Serial.print("Code: ");
        Serial.println(code);
    }
}
