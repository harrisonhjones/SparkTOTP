// OpenAuthentication Time-based One-time Password Algorithm (RFC 6238)

// SparkCore Library

// Original Arduino library by Luca Dentella (http://www.lucadentella.it)
// Adapted for SparkCore by Harrison Jones

// LED Matrix Example

// Usage: 
// 1. Goto http://www.lucadentella.it/OTP/ and choose a 10 digit password/secret and give it a name
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
{0x3C,0x42,0x42,0x42,0x42,0x42,0x42,0x3C},//0
{0x10,0x18,0x14,0x10,0x10,0x10,0x10,0x10},//1
{0x7E,0x2,0x2,0x7E,0x40,0x40,0x40,0x7E},//2
{0x3E,0x2,0x2,0x3E,0x2,0x2,0x3E,0x0},//3
{0x8,0x18,0x28,0x48,0xFE,0x8,0x8,0x8},//4
{0x3C,0x20,0x20,0x3C,0x4,0x4,0x3C,0x0},//5
{0x3C,0x20,0x20,0x3C,0x24,0x24,0x3C,0x0},//6
{0x3E,0x22,0x4,0x8,0x8,0x8,0x8,0x8},//7
{0x0,0x3E,0x22,0x22,0x3E,0x22,0x22,0x3E},//8
{0x3E,0x22,0x22,0x3E,0x2,0x2,0x2,0x3E},//9
{0x88,0x44,0x21,0x11,0x11,0x22,0x44,0x88}, // >>
{0x11,0x22,0x44,0x88,0x88,0x44,0x22,0x11}, // <<
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // " "
};

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
 
 
 
