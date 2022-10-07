 //12-bit default resolution; external power supply
#include<OneWire.h>
#include<SD.h>
#include<SPI.h>

File myFile;
char filename[] = "Data.CSV";
const int chipSelect = 4;
#define OneWireBusPin 2

float Temp01, Temp02;

OneWire ds(OneWireBusPin);  //2
byte addr1[8];         //to hold 64-bit ROM Codes of DS1
byte data[9];        //buffer to hold data coming from DS18B20
float celsius;
float fahrenheit;
//unsigned long previousMillis = 0;
//const long interval = 10*1000;  

void setup() 
{
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(8, INPUT_PULLUP);
  
  //---initialize SD Card------------------------------------------
  if (!SD.begin(4)) 
  {
    Serial.print("sd card init failed!");
    return;
  }
  Serial.println("SD Card init. OK!");
  
  SD.remove(filename);
  //---------------------------------------------------------------
  
  ds.reset();
  ds.search(addr1);  //collect 64-bit ROM code from sensor (DS1)
  
  Serial.print("Address of DS-1: ");
  for (int i=0; i<8; i++)
  {
    if(addr1[i] < 0x10)
    {
        Serial.print('0');
    }
    // Serial.print(addr1[i], HEX);   //show on I2C LCD 
  }
  Serial.println();

void loop()
{
//  unsigned long currentMillis = millis();
//  if (currentMillis - previousMillis >= interval) {
//    previousMillis = currentMillis;
    probe();
    recordSDCard();
    if(digitalRead(8) == LOW)
    {
   //  myFile.close();
      readFromSDCard();
      HERE: goto HERE;
    }
    //delay(5*60*1000UL);
    delay(1000);
//  }
}

void probe()
{
 //----------------------------
 ds.reset();       //bring 1-Wire into idle state
 ds.select(addr1); //slect with DS-1 with address addr1
 ds.write(0x44);    //conversion command
 delay(1000);   //data ready withinh DS18B20 or poll status word
 //---------------------------

 ds.reset();
 ds.select(addr1);  //selectimg the desired DS18B20
 ds.write(0xBE);    //Function command to read Scratchpad Memory (9Byte)
 ds.read_bytes(data, 9); //data comes from DS and are saved into buffer data[8]
 //---------------------------------

  int16_t raw = (data[1] << 8) | data[0]; //---data[0] and data[1] contains temperature data : 12-bit resolution-----
  celsius = (float)raw / 16.0;  //12-bit resolution
  fahrenheit = celsius * 1.8 + 32;
  Temp01 = fahrenheit;
  Serial.print(fahrenheit);
//  Temp01 = celsius;
//  Serial.print(celsius);
}

void recordSDCard()
{
  myFile = SD.open(filename, FILE_WRITE);
  if(myFile)
  {
    myFile.println(Temp01);
    myFile.close();
    Serial.println("//----------------");
    Serial.println("Data stored in SD Card! ");
    Serial.println("//----------------");   
  }
  else
  {
    Serial.println("File can't be opened!");  
  }
}

void readFromSDCard()
{
  myFile = SD.open(filename, FILE_READ);
  if(myFile)
  {
      while(myFile.available())
      {
        char x = (char)myFile.read();
        Serial.print(x);
      }
      myFile.close();   
   }
   else
   {
      Serial.println("File can't be opened!");
   }
}
