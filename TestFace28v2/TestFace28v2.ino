#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include <SystemFont5x7.h>
#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_FT6206.h>

#include "facedata.h"

#define TFT_RST 8
#define TFT_DC 9
#define TFT_CS 10

#define SD_CS 4		// Chip Select for SD card

#define BUFFPIXELCOUNT 160	// size of the buffer in pixels
#define SD_SPI_SPEED SPI_HALF_SPEED	// SD card SPI speed, try SPI_FULL_SPEED

SdFat sd; // set filesystem
SdFile bmpFile; // set filesystem


// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, TFT_RST);
Adafruit_FT6206 ctp = Adafruit_FT6206();

// store error strings in flash to save RAM
#define error(s) sd.errorHalt_P(PSTR(s))

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define PINK    0xFD79

//variable define
  bool drawstatus=0;
  bool menustatus=0;
  bool touchstatus=0;
  int px=0;
  int py=0;


void setup() {
  Serial.begin(115200);
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println(F("ILI9341 Test!"));

  tft.begin();
  progmemPrint(PSTR("Initializing SD card..."));

	if (!sd.begin(SD_CS, SD_SPI_SPEED)){
		progmemPrintln(PSTR("failed!"));
		return;
	}
	progmemPrintln(PSTR("OK!"));
  ctp.begin();
    if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }

  Serial.println("Capacitive touchscreen started");
  // read diagnostics (optional but can help debug problems)
  tft.setRotation(iliRotation90);
  tft.fillScreen(BLACK);
  //  tft.fillRoundRect(40,130,80,20,5,PINK);
  //  tft.fillRoundRect(200,130,80,20,5,PINK);
  tft.drawBitmap(leftEyenor, 0, 40, 120, 110, WHITE);
  tft.drawBitmap(rightEyenor, 200, 40, 120, 110, WHITE);
  Serial.println(F("Done!"));
}

void loop()
{

  touchstatus=ctp.touched();
  TS_Point p = ctp.getPoint();
  px = map(p.y, 0, 320, 320, 0);
  py = p.x;
  
if (menustatus==0) 
{
  if (! touchstatus) {

    return;
  }

  
  if (px<100 && py>120)
  {
    eyeAkward(1000);
  }
  else if (px>200 && py>120)
  {
    eyeLove(1000);
  }
  else if (px>=100 && px<= 200 && py>120)
  {
    eyeBlink(2);
  }
  else
  {
      bmpDraw("apple.bmp",0,0);
      menustatus=1;
  }
}
else
{
	if (! touchstatus) {
		if(drawstatus==1)
		{
			bmpDraw("apple.bmp",0,0);
			drawstatus=0;
		}

    return;
  }

 

  
  if (drawstatus==0)
  {
  bmpDraw("pingguo.bmp",0,0);
  drawstatus=1;
  }
     if (px>200 && py>120)
  {
  	menustatus=0;
      tft.fillScreen(BLACK);
    tft.drawBitmap(leftEyenor, 0, 40, 120, 110, WHITE);
    tft.drawBitmap(rightEyenor, 200, 40, 120, 110, WHITE);
  }

}

}

void eyeBlink(int counts)
{
  for (int i=1;i<=counts;i++)
  {
  tft.fillRect(40, 40, 80, 80, BLACK);
  tft.fillRect(200, 40, 80, 80, BLACK);
  tft.drawBitmap(eyeHalf, 40, 40, 80, 80, WHITE);
  tft.drawBitmap(eyeHalf, 200, 40, 80, 80, WHITE);
  tft.fillRect(40, 40, 80, 80, BLACK);
  tft.fillRect(200, 40, 80, 80, BLACK);
  tft.drawBitmap(eyeQuater, 40, 40, 80, 80, WHITE);
  tft.drawBitmap(eyeQuater, 200, 40, 80, 80, WHITE);
  tft.fillRect(40, 40, 80, 80, BLACK);
  tft.fillRect(200, 40, 80, 80, BLACK);
  tft.drawBitmap(eyeHalf, 40, 40, 80, 80, WHITE);
  tft.drawBitmap(eyeHalf, 200, 40, 80, 80, WHITE);
  tft.fillRect(40, 40, 80, 80, BLACK);
  tft.fillRect(200, 40, 80, 80, BLACK);
  tft.drawBitmap(eyeFull, 40, 40, 80, 80, WHITE);
  tft.drawBitmap(eyeFull, 200, 40, 80, 80, WHITE);
  delay(100);
  }
  delay(1000);
}
void eyeAkward(int duration)
{
    tft.fillRect(40, 40, 80, 80, BLACK);
    tft.drawBitmap(eyeJiong, 40, 40, 80, 80, WHITE);
    delay(duration);
    tft.fillRect(40, 40, 80, 80, BLACK);
    tft.drawBitmap(eyeFull, 40, 40, 80, 80, WHITE);
}
void eyeLove(int duration)
{
      tft.fillRect(40, 40, 80, 80, BLACK);
  tft.fillRect(200, 40, 80, 80, BLACK);
    tft.drawBitmap(eyeHeart, 40, 40, 80, 80, PINK);
    tft.drawBitmap(eyeHeart, 200, 40, 80, 80, PINK);
    delay(duration);
  tft.fillRect(40, 40, 80, 80, BLACK);
  tft.fillRect(200, 40, 80, 80, BLACK);
  tft.drawBitmap(eyeFull, 40, 40, 80, 80, WHITE);
  tft.drawBitmap(eyeFull, 200, 40, 80, 80, WHITE);
}
// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's RAM but
// makes loading a little faster.  

void bmpDraw(char* filename, int x, int y) {

	SdFile   bmpFile;
	int      bmpWidth, bmpHeight;   // W+H in pixels
	uint8_t  bmpDepth;              // Bit depth (currently must be 24)
	uint8_t	 headerSize;
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;     // Not always = bmpWidth; may have padding
	uint32_t fileSize;
	boolean  goodBmp = false;       // Set to true on valid header parse
	boolean  flip = true;        // BMP is stored bottom-to-top
	uint16_t w, h, row, col;
	uint8_t  r, g, b;
	uint32_t pos = 0, startTime;

	if ((x >= tft.width()) || (y >= tft.height())) return;

	progmemPrint(PSTR("Loading image '"));
	Serial.print(filename);
	Serial.println('\'');
	startTime = millis();
	// Open requested file on SD card
	if (!bmpFile.open(filename, O_READ)) {
		Serial.println("File open failed.");
		return;
	}
	else {
		//Serial.println("File opened.");
	}

	// Parse BMP header
	if (read16(bmpFile) == 0x4D42) { // BMP signature
		fileSize = read32(bmpFile);
		//progmemPrint(PSTR("File size: ")); Serial.println(fileSize);
		(void)read32(bmpFile); // Read & ignore creator bytes
		bmpImageoffset = read32(bmpFile); // Start of image data
		//progmemPrint(PSTR("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
		// Read DIB header
		headerSize = read32(bmpFile);
		//progmemPrint(PSTR("Header size: ")); Serial.println(headerSize);
		bmpWidth = read32(bmpFile);
		bmpHeight = read32(bmpFile);
		if (read16(bmpFile) == 1) { // # planes -- must be '1'
			bmpDepth = read16(bmpFile); // bits per pixel
			//progmemPrint(PSTR("Bit Depth: ")); Serial.println(bmpDepth);
			if (read32(bmpFile) == 0) // 0 = uncompressed
			{
				//progmemPrint(PSTR("Image size: "));
				//Serial.print(bmpWidth);
				//Serial.print('x');
				//Serial.println(bmpHeight);

				// If bmpHeight is negative, image is in top-down order.
				// This is not canon but has been observed in the wild.
				if (bmpHeight < 0) {
					bmpHeight = -bmpHeight;
					flip = false;
				}

				// Crop area to be loaded
				w = bmpWidth;
				h = bmpHeight;
				if ((x + w - 1) >= tft.width())  w = tft.width() - x;
				if ((y + h - 1) >= tft.height()) h = tft.height() - y;
			
				// Set TFT address window to clipped image bounds
				tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

				if (bmpDepth == 16)	//565 format
				{
					goodBmp = true; // Supported BMP format -- proceed!

					uint16_t buffer[BUFFPIXELCOUNT]; // pixel buffer

					bmpFile.seekSet(54);	//skip header
					uint32_t totalPixels = (uint32_t)bmpWidth*(uint32_t)bmpHeight;
					uint16_t numFullBufferRuns = totalPixels / BUFFPIXELCOUNT;
					for (uint32_t p = 0; p < numFullBufferRuns; p++) {
						// read pixels into the buffer
						bmpFile.read(buffer, 2 * BUFFPIXELCOUNT);
						// push them to the diplay
						tft.pushColors(buffer, 0, BUFFPIXELCOUNT);
						
					}

					// render any remaining pixels that did not fully fit the buffer
					uint32_t remainingPixels = totalPixels % BUFFPIXELCOUNT;
					if (remainingPixels > 0)
					{
						bmpFile.read(buffer, 2 * remainingPixels);
						tft.pushColors(buffer, 0, remainingPixels);
					}

				}
				else if (bmpDepth == 24)	// standard 24bit bmp
				{
					goodBmp = true; // Supported BMP format -- proceed!
					uint16_t bufferSize = min(w, BUFFPIXELCOUNT);
					uint8_t  sdbuffer[3 * bufferSize]; // pixel in buffer (R+G+B per pixel)
					uint16_t lcdbuffer[bufferSize];  // pixel out buffer (16-bit per pixel)

					// BMP rows are padded (if needed) to 4-byte boundary
					rowSize = (bmpWidth * 3 + 3) & ~3;

					for (row = 0; row < h; row++) { // For each scanline...
						// Seek to start of scan line.  It might seem labor-
						// intensive to be doing this on every line, but this
						// method covers a lot of gritty details like cropping
						// and scanline padding.  Also, the seek only takes
						// place if the file position actually needs to change
						// (avoids a lot of cluster math in SD library).

						if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
							pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
						else     // Bitmap is stored top-to-bottom
							pos = bmpImageoffset + row * rowSize;
						if (bmpFile.curPosition() != pos) { // Need seek?
							bmpFile.seekSet(pos);
						}

						for (col = 0; col < w; col += bufferSize)
						{
							// read pixels into the buffer
							bmpFile.read(sdbuffer, 3 * bufferSize);

							// convert color
							for (int p = 0; p < bufferSize; p++)
							{
								b = sdbuffer[3 * p];
								g = sdbuffer[3 * p + 1];
								r = sdbuffer[3 * p + 2];
								lcdbuffer[p] = tft.color565(r, g, b);
							}
							// push buffer to TFT
							tft.pushColors(lcdbuffer, 0, bufferSize);
						}

						// render any remaining pixels that did not fully fit the buffer
						uint16_t remainingPixels = w % bufferSize;
						if (remainingPixels > 0)
						{
							bmpFile.read(sdbuffer, 3 * remainingPixels);

							for (int p = 0; p < remainingPixels; p++)
							{
								b = sdbuffer[3 * p];
								g = sdbuffer[3 * p + 1];
								r = sdbuffer[3 * p + 2];
								lcdbuffer[p] = tft.color565(r, g, b);
							}

							tft.pushColors(lcdbuffer, 0, remainingPixels);
						}
					}
				}
				else
				{
					progmemPrint(PSTR("Unsupported Bit Depth."));
				}

				if (goodBmp)
				{
					progmemPrint(PSTR("Loaded in "));
					Serial.print(millis() - startTime);
					Serial.println(" ms");
				}
			}
		}
	}

	bmpFile.close();
	if (!goodBmp) progmemPrintln(PSTR("BMP format not recognized."));
}


// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(SdFile& f) {
	uint16_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read(); // MSB
	return result;
}

uint32_t read32(SdFile& f) {
	uint32_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read();
	((uint8_t *)&result)[2] = f.read();
	((uint8_t *)&result)[3] = f.read(); // MSB
	return result;
}

// Copy string from flash to serial port
// Source string MUST be inside a PSTR() declaration!
void progmemPrint(const char *str) {
	char c;
	while (c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
	progmemPrint(str);
	Serial.println();
}

//void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex
//{
//	char tmp[length*5+1];
//	byte first;
//	byte second;
//	for (int i=0; i<length; i++) {
//		first = (data[i] >> 4) & 0x0f;
//		second = data[i] & 0x0f;
//		// base for converting single digit numbers to ASCII is 48
//		// base for 10-16 to become upper-case characters A-F is 55
//		// note: difference is 7
//		tmp[i*5] = 48; // add leading 0
//		tmp[i*5+1] = 120; // add leading x
//		tmp[i*5+2] = first+48;
//		tmp[i*5+3] = second+48;
//		tmp[i*5+4] = 32; // add trailing space
//		if (first > 9) tmp[i*5+2] += 7;
//		if (second > 9) tmp[i*5+3] += 7;
//	}
//	tmp[length*5] = 0;
//	Serial.print(tmp);
//}
