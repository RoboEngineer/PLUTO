/*************************************************** 
Modified vs1053 library including Adafuit_VS1053.h and Adafruit_VS1053.cpp, modified sdfat libarary including SdFatConfig
 ****************************************************/

// include SPI, MP3 and SD libraries

#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <Adafruit_VS1053.h>

// These are the pins used for the breakout example
#define BREAKOUT_RESET  29      // VS1053 reset pin (output)
#define BREAKOUT_CS     30     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    28      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 24    // Chip Select for SD card
#define BUFFPIXELCOUNT 160  // size of the buffer in pixels
#define SD_SPI_SPEED SPI_HALF_SPEED // SD card SPI speed, try SPI_FULL_SPEED

SdFat SD; // set filesystem
SdFile bmpFile; // set filesystem

#define error(s) SD.errorHalt_P(PSTR(s))


// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 23       // VS1053 Data request, ideally an Interrupt pin



Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  //Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);


////

void setup() {

  pinMode(13, OUTPUT);
  pinMode(12,OUTPUT);
 digitalWrite(13,1);
 digitalWrite(12,1);
 
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Library Test");

  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  progmemPrint(PSTR("Initializing SD card..."));

  if (!SD.begin(CARDCS, SD_SPI_SPEED)){
    progmemPrintln(PSTR("failed!"));
    return;
  }
  progmemPrintln(PSTR("OK!"));
  
  // list files
  printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20,20);

  /***** Two interrupt options! *******/ 
  // This option uses timer0, this means timer1 & t2 are not required
  // (so you can use 'em for Servos, etc) BUT millis() can lose time
  // since we're hitchhiking on top of the millis() tracker
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT);
  
  // This option uses a pin interrupt. No timers required! But DREQ
  // must be on an interrupt pin. For Uno/Duemilanove/Diecimilla
  // that's Digital #2 or #3
  // See http://arduino.cc/en/Reference/attachInterrupt for other pins
  // *** This method is preferred
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
}

void loop() {  
  // Alternately, we can just play an entire file at once
  // This doesn't happen in the background, instead, the entire
  // file is played and the program will continue when it's done!
  musicPlayer.playFullFile("track001.ogg");

  // Start playing a file, then we can do stuff while waiting for it to finish
  if (! musicPlayer.startPlayingFile("track001.mp3")) {
    Serial.println("Could not open file track001.mp3");
    while (1);
  }
  Serial.println(F("Started playing"));

  while (musicPlayer.playingMusic) {
    // file is now playing in the 'background' so now's a good time
    // to do something else like handling LEDs or buttons :)
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Done playing music");
}


/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

void progmemPrint(const char *str) {
  char c;
  while (c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
  progmemPrint(str);
  Serial.println();
}


