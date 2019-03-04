/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 
 */

/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Ethernet.h>
#include "IP6Address.h"

//==========================================================================
// 20190220 Taylor
// Adafruit ssd1306_128x64_i2c

#include <Wire.h>
#include "Adafruit-GFX-Library\Adafruit_GFX.h"
#include "Adafruit-GFX-Library\Adafruit_GFX.cpp"

#include "Adafruit_SSD1306\Adafruit_SSD1306.h"
#include "Adafruit_SSD1306\Adafruit_SSD1306.cpp"

//==========================================================================

//#define DEBUG_CHATSERVER_INO_ETHERNET_BEGIN

//==========================================================================
// 20190220 Taylor
// Adafruit ssd1306_128x64_i2c

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

//==========================================================================

// LCD Flip NETWORK Address Time(sec)
#define LCD_FLIP_SEC  5000
#define LCD_MAX_LENGTH  20

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:

byte mac[] = {
0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

byte ip6_lla[] = {
0xfe,0x80, 0x00,0x00,
0x00,0x00, 0x00,0x00,
0x02,0x00, 0xdc,0xff,
0xfe,0x57, 0x57,0x61
};

byte ip6_gua[] = {
0x20,0x01,0x02,0xb8,
0x00,0x10,0xFF,0xFE,
0x00,0x00,0xdc,0xff,
0xfe,0x57,0x57,0x61
};

byte ip6_sn6[] = {
0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,
0x00,0x00,0x00, 0x00,
0x00,0x00,0x00,0x00
};

byte ip6_gw6[] = {
0xfe, 0x80, 0x00,0x00,
0x00,0x00,0x00,0x00,
0x02,0x00, 0x87,0xff,
0xfe,0x08, 0x4c,0x81
};

IP6Address ip(192, 168, 0, 4);
IP6Address myDns(192, 168, 0, 1);
IP6Address gateway(192, 168, 0, 1);
IP6Address subnet(255, 255, 0, 0);

IP6Address lla(ip6_lla, 16);
IP6Address gua(ip6_gua, 16);
IP6Address sn6(ip6_sn6, 16);
IP6Address gw6(ip6_gw6, 16);

//==========================================================================
// 20190220 Taylor
// Adafruit ssd1306_128x64_i2c

static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

String strFromIP;
String rcvString;
unsigned long print_settime = 0;
bool print_set = false;
//==========================================================================

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //==========================================================================
  // 20190220 Taylor
  // Adafruit ssd1306_128x64_i2c

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {    // 20190220 this OLED must be 0x3C. original 0x3D
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  //==========================================================================

  Serial.println("Ethernet WebServer Example");

  // start the Ethernet connection and the server:
  #if 0
  
  // Manual
  Ethernet.begin(mac, ip, myDns, gateway, subnet, lla, gua, sn6, gw6);

  #else

  // Auto RSRA
  if(-1 == Ethernet.begin(mac, ip, myDns, gateway, subnet))
  {
    Serial.println("Address Auto Configuration Failed");
    Serial.println("Set IP by Manual ");
    Ethernet.begin(mac, ip, myDns, gateway, subnet, lla, gua, sn6, gw6);
  }
  #endif

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin(1);
  Serial.println("Web Server address:");
  Serial.print("LLA:");
  Serial.println(Ethernet.linklocalAddress());
  
  Serial.print("GUA:");
  Serial.println(Ethernet.globalunicastAddress());
  Serial.println(Ethernet.localIP());

  printLCDNetwork();
  print_settime = millis();
}

void loop() {

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    bool currentLineIsBlank = true;
    rcvString = "";

    while (client.connected()) {
      
      if (client.available()) {
        char c = client.read();
        strFromIP += c;
        Serial.write(c);

        //Serial.println(client.readString());

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

          // send web page
          client.println("<html>");
            client.println("<!-- WIZnet W6100 Arduino Ethernet Shield Test by Taylor WIZnet Co., Ltd. -->");
            client.println("<head>");

              // process
              if(1 == ProcessValue()) {
                // Passing 'GET /favicon.ico HTTP/1.1' except FireFox
                
                // ======================================================================
                // Print to display Host IP Address

                clearsetLCD(0, 0);
                display.println("From\n");
                display.println(client.remoteIP());
                display.display();
                // ======================================================================

                drawmessage(rcvString);
                Serial.println("Drawed.");
              }

              client.println("<title>W6100 Ethernet Shield Webserver</title>");
            client.println("</head>");

            client.println("<body>");
              client.println("<h1>WIZnet W6100 Dual Stack WebServer Demo</h1><br>");
              
              client.print("Your ");
              if(6 == client.IPVis()) {
                client.print("IPv6 Address is ");

              } else {
                client.print("IPv4 Address is ");
              }

              client.print(client.remoteIP());
              client.print(".<br>");

              client.println("<form action=\"/\">");
                client.println("<p>");
                #if 1
                // not Empty Text
                client.print  ("  Send a message to W6100 WebServer : <input type=\"text\" name=\"v\" value=\"");
                  client.print  (rcvString);
                  client.print  ("\"><br>");
                #else
                // Empty Text
                client.print  ("  Send a message to W6100 WebServer : <input type=\"text\" name=\"v\"");
                  client.print  (rcvString);
                  client.print  ("><br>");
                #endif
                client.println("</p>");
                client.println("<p><input type=\"submit\" value=\"SEND\"></p>");
              client.println("</form>");
            client.println("</body>");
          client.println("</html>");

          Serial.print(strFromIP);
          strFromIP = "";    // finished with request, empty string
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();

    Serial.println("client disconnected");

    print_settime = millis();
    print_set = true;

  } else {

    if(print_set == true) {

      if((millis() - print_settime) > LCD_FLIP_SEC) {

        printLCDNetwork();

        print_settime = millis();
        print_set = false;
      }
    }
  }
}

//==========================================================================
//  Original
//  https://github.com/zenmanenergy/ESP8266-Arduino-Examples/blob/master/helloWorld_urlencoded/urlencode.ino
// 

uint8_t h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((uint8_t)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((uint8_t)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((uint8_t)c - 'A' + 10);
    }
    return(0);
}
//==========================================================================

void drawmessage(String message) {
  display.println("");
  display.println("Message : \n");
  display.print(message);
  display.display();
}

int ProcessValue()
{
  int start = -1;
  int stops = -1;
  char a = 0;
  char b = 0;
  char c = 0;
  int n;
  uint8_t cnt = 0;

  start = strFromIP.indexOf("?v=");
  PRINTLINE();
  PRINTVAR(start);
  Serial.println(strFromIP);

  rcvString = "";

  if (start != -1 && start < 10) {
    PRINTLINE();
    n = start + 3; // (9 because "?v=")

    while (a != ' ') {
      a = strFromIP.charAt(n);

      if(a == '+') {
        rcvString += ' ';
      } else if(a == '%') {
        b = strFromIP.charAt(++n); // first
        c = strFromIP.charAt(++n); // second

        a = (h2int(b) << 4) | h2int(c);
        rcvString += a;
      } else {
        rcvString += a;
      }
      cnt++;
      n++;

      if(cnt > (LCD_MAX_LENGTH-1))
      {
        break;
      }
    }

    
    return 1;
  } else {
    return -1;
  }
}

//==========================================================================
// 20190220 Taylor
// Adafruit ssd1306_128x64_i2c

void printLCDNetwork() {

  clearsetLCD(0, 0);

  display.println(Ethernet.linklocalAddress());
  display.println(" ");
  display.println(Ethernet.globalunicastAddress());
  display.println(" ");
  display.println(Ethernet.localIP());

  display.display();
}

void clearsetLCD(uint8_t x, uint8_t y) {

  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(x, y);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
}

void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=0; i<256; i++) {
    if(i == '\n') display.write(' ');
    else          display.write(i);
  }

  display.display();
  delay(2000);
}

void testdrawchar(String Test) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.print(Test);
  display.display();
}

//==========================================================================