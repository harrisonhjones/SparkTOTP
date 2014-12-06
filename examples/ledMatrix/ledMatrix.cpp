// OpenAuthentication Time-based One-time Password Algorithm (RFC 6238)

// SparkCore Library

// Original Arduino library by Luca Dentella (http://www.lucadentella.it)
// Adapted for SparkCore by Harrison Jones

// LED Matrix Example

// Usage: 
// 1. Open Generator.html in your browser, choose a password/secret, and give it a name
// 2. Scan the QR code with your phone in your authenticator app
// 3. Replace the Arduino HEX array hmacKey[] with the one given by the website
// 4. Wire up a MAX7219-based 8x8 LED Matrix (https://www.sparkfun.com/products/11861)
// 5. Flash the code to your core
// 6. Code will display automatically. Compare code against the authenticator application


#include "sha1.h"
#include "TOTP.h"

// Display Variables
unsigned char i;
unsigned char j; 
int charToShow = 0;
/*Port Definitions*/
int Max7219_pinCLK = D6;
int Max7219_pinCS = D4;
int Max7219_pinDIN = D5;
 
unsigned char disp1[13][8]={
    {0x00, 0x3C, 0x24, 0x24, 0x24, 0x24, 0x3C, 0x00}, //  0
    {0x00, 0x08, 0x18, 0x08, 0x08, 0x08, 0x3C, 0x00}, //  1
    {0x00, 0x3C, 0x04, 0x3C, 0x20, 0x20, 0x3C, 0x00}, //  2
    {0x00, 0x3C, 0x04, 0x1C, 0x04, 0x04, 0x3C, 0x00}, //  3
    {0x00, 0x24, 0x24, 0x3C, 0x04, 0x04, 0x04, 0x00}, //  4
    {0x00, 0x3C, 0x20, 0x3C, 0x04, 0x04, 0x3C, 0x00}, //  5
    {0x00, 0x3C, 0x20, 0x3C, 0x24, 0x24, 0x3C, 0x00}, //  6
    {0x00, 0x3C, 0x04, 0x08, 0x10, 0x10, 0x10, 0x00}, //  7
    {0x00, 0x3C, 0x24, 0x3C, 0x24, 0x24, 0x3C, 0x00}, //  8
    {0x00, 0x3C, 0x24, 0x3C, 0x04, 0x04, 0x04, 0x00}, //  9
    {0x00, 0x30, 0x18, 0x0C, 0x0C, 0x18, 0x30, 0x00}, //  <
    {0x00, 0x0C, 0x18, 0x30, 0x30, 0x18, 0x0C, 0x00}, //  >
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

// Max7219 Functions
void Write_Max7219_byte(unsigned char DATA);
void Write_Max7219(unsigned char address,unsigned char dat);
void Init_MAX7219(void);

// TOPT Variables
// The shared secret is MyLegoDoor
//                    M     y     L     e     g     o     D     o     o     r
uint8_t hmacKey[] = {0x4d, 0x79, 0x4c, 0x65, 0x67, 0x6f, 0x44, 0x6f, 0x6f, 0x72};

TOTP totp = TOTP(hmacKey, 10);
char code[7];

// State machine variables
int totpPosition = 0;

void setup() {
    pinMode(Max7219_pinCLK,OUTPUT);
    pinMode(Max7219_pinCS,OUTPUT);
    pinMode(Max7219_pinDIN,OUTPUT);
    delay(50);
    Init_MAX7219();
    
    for(j=0;j<12;j++)
    {
        for(i=1;i<9;i++)
            Write_Max7219(i,disp1[j][i-1]);
        delay(500);
    }
        
}


void loop() {
    char* newCode = totp.getCode((long)Time.now());
    if(strcmp(code, newCode) != 0) {
        strcpy(code, newCode);
        Serial.print("Code: ");
        Serial.println(code);
        totpPosition = 0;
    }
    
    // If it's the fist character show the ">" symbol instead to indicate the start of a code
    if(totpPosition == 0)
        charToShow = 10;
    else
        charToShow = newCode[totpPosition - 1] - 48;
    
    // Display the character
    for(i=1;i<9;i++)
        Write_Max7219(i,disp1[charToShow][i-1]);
    
    // If we are finished displaying the code reset our position counter
    if (totpPosition > 5)
        totpPosition = 0;
    else
        totpPosition++;
    
    // Show the character for 1/2 a second
    delay(500);
    
    // Clear the screen so double numbers are obvious (ie, "88" appears as "8 8")
    for(i=1;i<9;i++)
        Write_Max7219(i,disp1[12][i-1]);
    
    // Clear the screen for 50ms (a brief flash of nothing)
    delay(50);
      
}





 
 
 
void Write_Max7219_byte(unsigned char DATA) 
{   
    unsigned char i;
    digitalWrite(Max7219_pinCS,LOW);		
    for(i=8;i>=1;i--)
    {		  
        digitalWrite(Max7219_pinCLK,LOW);
        digitalWrite(Max7219_pinDIN,DATA&0x80);// Extracting a bit data
        DATA = DATA<<1;
        digitalWrite(Max7219_pinCLK,HIGH);
    }                                 
}
 
 
void Write_Max7219(unsigned char address,unsigned char dat)
{
    digitalWrite(Max7219_pinCS,LOW);
    Write_Max7219_byte(address);           //addressï¼Œcode of LED
    Write_Max7219_byte(dat);               //dataï¼Œfigure on LED 
    digitalWrite(Max7219_pinCS,HIGH);
}
 
void Init_MAX7219(void)
{
     Write_Max7219(0x09, 0x00);       //decoding ï¼šBCD
     Write_Max7219(0x0a, 0x03);       //brightness 
     Write_Max7219(0x0b, 0x07);       //scanlimitï¼›8 LEDs
     Write_Max7219(0x0c, 0x01);       //power-down modeï¼š0ï¼Œnormal modeï¼š1
     Write_Max7219(0x0f, 0x00);       //test displayï¼š1ï¼›EOTï¼Œdisplayï¼š0
}
