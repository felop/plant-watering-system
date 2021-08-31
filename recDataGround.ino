#include <TimeLib.h>
#include <SD.h>
#include <SPI.h>

File DataFile;
const int chipSelect = BUILTIN_SDCARD;

IntervalTimer Timer;

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

void logData(){
  DataFile = SD.open("DataFile.csv", FILE_WRITE);
  DataFile.print(day());
  DataFile.print(" ");
  DataFile.print(hour());
  DataFile.print(" ");
  DataFile.println(minute());
  DataFile.close();
}

void setup()  {
  setSyncProvider(getTeensy3Time);
  Serial.begin(9600);
  while (!Serial);  // Wait for Arduino Serial Monitor to open
  delay(100);
  if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  DataFile = SD.open("DataFile.csv", FILE_WRITE);
  SD.remove("DataFile.csv");
  DataFile = SD.open("DataFile.csv", FILE_WRITE);
  if (DataFile) {
    DataFile.println("Heure");
    DataFile.close();
  } else {
    Serial.println("oups");
  }

  Timer.begin(logData,1000000*60*15);
}

void loop() {
}
