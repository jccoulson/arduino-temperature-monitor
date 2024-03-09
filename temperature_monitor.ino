#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <TinyDHT.h>

#define DHTPIN 2     //digital pin 2 that dht temp sensor is plugged in
#define DHTTYPE DHT22   //temp sensor is type DHT22

//pin setup for OLED
#define SCLK_PIN 13  //clock, pin 13
#define DIN_PIN 11  // digital input, pin 11
#define CS_PIN   10  //chip select, pin 10
#define DC_PIN   7   //data command, pin 7
#define RST_PIN  8   //reset, pin 8

//color values used for background and text
#define BLACK   0x0000
#define BLUE 0x001F



//creating adafruit object with given pins
Adafruit_SSD1351 display = Adafruit_SSD1351(CS_PIN, DC_PIN, DIN_PIN, SCLK_PIN, RST_PIN);
DHT dht(DHTPIN, DHTTYPE); //init DHT temp/humdity sensor

//Global variables to track dht sensor input
float tempF = 0; 
float prevTemp = 0;
float humidity = 0;

//array needed for display of humidity graph
int pixelInfo[11] = {0}; //track points for graphs


void setup() {
  //setting initial values for humidity graph
  for(int i = 0; i < 11; i++)
  {
    pixelInfo[i] = 25;
  }

  //set display values, write text and draw graph outline that doesnt get refreshed
  display.begin();
  display.fillScreen(BLUE);  //background is blue
  display.setTextColor(BLACK);  //black text
  display.setTextSize(1);  //standard size text
  
  //draw outline of humidity graph
  display.drawRect(20,40, 60, 50, BLACK);

  //display humidity text
  display.setCursor(25, 31);
  display.print("Humidity");

  //display temp text
  display.setCursor(88, 6);
  display.print("Temp");
  
  //draw ticks on humidity graph
  for(int i = 0;i < 4; i++)
    display.drawLine(19, 79-(i*10), 21, 79-(i*10), BLACK);

  //display temp text
  display.setCursor(13, 85);
  display.print("0");

  //number for display on humidity y axis
  int hNum = 0;
  for(int i = 0; i < 4; i++)
  {
    hNum = (i+1)*10;
    display.setCursor(6, 75-i*10);
    display.print(hNum);
  }

  //begin sensor
  dht.begin();

}

void loop() {
  //read temperature in farenhieght
  tempF = dht.readTemperature(true);
  //read humidity
  humidity = dht.readHumidity();

  //display
  display.setCursor(82, 115);
  display.setTextColor(BLACK);

  //redraw temperature value when it has changed
  if(prevTemp != tempF)
  {
    //draw over previous temperature
    display.fillRect(82,115, 35, 10, BLUE);

    //display temperature
    display.print(tempF);
    display.print(F("F")); //F is to stores in flash memory instead of sram
  }


  //thermometer is updated only when there will be a change in graphic
  //graphic changes in increments of 10 degrees F
  if(((int)prevTemp%10) != ((int)tempF%10)) 
    draw_thermometer(tempF);

  //draw humidity graph
  draw_graph(tempF);

  //store previous temperature for comparison in next loop
  prevTemp = tempF;
}

void draw_thermometer(uint16_t temp){

  //draw over current theremometer to reset
  display.fillRect(89,15, 21, 96, BLUE);
  
  //draw outline of thermometer
  display.drawRect(89,15, 21, 96, BLACK);
  
  uint16_t color = 0xF800; //start with color red
  for(int i = (temp/10)*2; i >= 0 ; i--)
  {
    display.fillRect(90, 105-((i*10)/2), 20, 5, color);
    display.drawRect(90, 105-((i*10)/2), 20, 5, BLACK);
    uint16_t r = 0x1F; //red will stay at max red, 11111. If wanted to modify shift right 11, >>11
    uint16_t g = (color & 0x07E0) >> 5; //take green value from color and shift right
    uint16_t b = (color & 0x001F);   //extract blue, blue doesnt need shifted is already right most bits

 
    //slowly push towads white/yellow
    //increment by 5, even though this will go above 63 bit max, else option will correctly set
    g = (g + 4 <= 0x3F) ? (g + 4) : 0x3F; //0x3F is max green 6 bits 111111, add 5 if its not max yet
  
    //if green is max value, color is now yellow, max red and max green is yellow
    //when green is max start to modify blue to start pushing towards white
    if (g == 0x3F) {
      b = (b + 4 <= 0x1F) ? (b + 4) : 0x1F; //0x1F is max blue, 5 bits, 11111, if this reaches max color is now white
    }

    //reassemble the color into 16 bit format e.g. 11111, 111111, 00111 //this would be yellowish white
    color = (r << 11) | (g << 5) | b;
  }
}


void draw_graph(int temp)
{
   //refresh within the graph display rectangle
   display.fillRect(22,42, 56, 47, BLUE);

   //shift all elements in array to left
   for(int i = 0; i < 10; i++)
   {
      pixelInfo[i] = pixelInfo[i+1];
   }
   
   //new point to plot is current humidity
   if(humidity > 90) //put within graph boundaries ig it is going out of bounds
    pixelInfo[10] = 87;
   else if(humidity < 40)
    pixelInfo[10] = 42;
   else
    pixelInfo[10] = humidity;
  
  
   //track which element in array to display
   int pixelNum = 10;
  
   //Draw line between 10 different points
   for(int i = 0; i < 10; i++)
   {
    //each line will be 5 pixels long, so iterate by 5 pixels for x axis
    display.drawLine(75-(i*5), 110-pixelInfo[pixelNum],70-(i*5), 110-pixelInfo[pixelNum-1], BLACK);
    //decrement which element of array you are currenly on
    pixelNum--;
   }
}

/*
void display_sentiment(float temp)
{
  if(temp >80)
  {
    
  }
  else if(temp > 60)
  {
    display.fillRect(5,30, 15, 2, BLACK);
    display.fillRect(20,31, 2, 2, BLACK);
    
    display.fillRect(3,31, 2, 2, BLACK);
    display.fillRect(3,31, 2, 2, BLACK);
    
    display.fillRect(3,31, 2, 2, BLACK);
    display.fillRect(3,31, 2, 2, BLACK);
  }
  else
  {
    
  }
}
*/
