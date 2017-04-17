// The code in this program can be cited back to https://learn.sparkfun.com/tutorials/sparkfun-bme280-breakout-hookup-guide.
// The code in this program extracts temperature, pressure, altitude and humidity from the BME280,
// it then posts data retreived to https://data.sparkfun.com/streams/bGJbO8GMjzFNbzA0YwVY.

//Add all libraries


#include "ESP8266WiFi.h"
#include "Wire.h"
#include "SparkFunBME280.h"
#include <Phant.h>

BME280 mySensor;
// Initialise WIFI parameters.
const char WiFiSSID[] = "Swapnil's iPhone";
const char WiFiPSK[] = "8s5esfhw0ng8m";

// Initialise website parameters.
const char PhantHost[] = "data.sparkfun.com";
const char PublicKey[] = "bGJbO8GMjzFNbzA0YwVY";
const char PrivateKey[] = "VpMm09pkRAsEzK7JdZnd";

const unsigned long postRate = 1000 * 60 * 30;
unsigned long lastPost = 0;

void setup()
{
  // Initialise the BME280 settings
  mySensor.settings.commInterface = I2C_MODE;
  mySensor.settings.I2CAddress = 0x77;
  //***Operation settings*****************************//
  mySensor.settings.runMode = 3; // 3, Normal mode
  mySensor.settings.tStandby = 0; // 0, 0.5ms
  mySensor.settings.filter = 0; // 0, filter off
  //tempOverSample can be:
  // 0, skipped
  // 1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.settings.tempOverSample = 1;
  //pressOverSample can be:
  // 0, skipped
  // 1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.settings.pressOverSample = 1;
  //humidOverSample can be:
  // 0, skipped
  // 1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.settings.humidOverSample = 1;

  Serial.begin(115200);
  Serial.print("Program Started\n");
  Serial.print("Starting BME280... ");
  
  connectWiFi();
}
void loop()
{
  // In this void loop we shall indefinetly post
  // data to data.sparkfun.com. 
  delay(1000);
  unsigned int delaytime;
  Serial.println("Posting to Phant!");
  for (int i = 0; i < 10; i++)
  {
    if (postToPhant())
    {
      lastPost = millis();
      Serial.println("Post Suceeded!");
    }
    else // If the Phant post failed
    {
      Serial.println("Post failed, will try again.");
    }
    delaytime = postRate;
    delay(2000); // Short delay, then next post
  }

}
void connectWiFi()
{
  // This is a function which is called in void setup.
  // This function connects the WIFi enabled microcontroller
  // to the internet.  
  delay(1);
  Serial.println("Connecting to: " + String(WiFiSSID));
  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFiSSID, WiFiPSK);

  while (WiFi.status() != WL_CONNECTED) //Waiting for WIFI to connect.
  {

    delay(15000);
    Serial.print("Still waiting");

  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int postToPhant()
{

  // Declare an object from the Phant library - phant
  Phant phant(PhantHost, PublicKey, PrivateKey);
  // Add the three field/value pairs defined by our stream:

  phant.add("temperature", mySensor.readTempC()); // Add temperature to data.sparkfun.com phant que.
  phant.add("humidity", mySensor.readFloatHumidity()); // Add humifity to data.sparkfun.com phant que.
  phant.add("altitude", mySensor.readFloatAltitudeMeters()); // Add altitude to data.sparkfun.com phant que.
  phant.add("pressure", mySensor.readFloatPressure() / 1000); // Add pressure to data.sparkfun.com phant que.

  // Now connect to data.sparkfun.com, and post our data:
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(PhantHost, httpPort))
  {
    // If we fail to connect, return 0.
    return 0;
  }
  // If we successfully connected, print our Phant post:
  client.print(phant.post());
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');

  }
  //Print each row in the loop
  //Start with temperature, as that data is needed for accurate compensation.
  //Reading the temperature updates the compensators of the other functions
  //in the background.

  Serial.print(mySensor.readTempC(),2);
  Serial.print(",");
  Serial.print(mySensor.readFloatPressure(),0);
  Serial.print(",");
  Serial.print(mySensor.readFloatAltitudeMeters(),3);
  Serial.print(",");
  Serial.print(mySensor.readFloatHumidity(),0);
  Serial.print(",");

  return 1; // Return success
}
void printError(byte error)
// If there is a problem retreiving data from the I2C
// pins then this function will print out the error.
{
  Serial.print("I2C error: ");
  Serial.print(error, DEC);
  Serial.print(", ");
  switch (error)
  {
    case 0:
      Serial.println("success");
      break;
    default:
      Serial.println("unknown error");
  }
}
