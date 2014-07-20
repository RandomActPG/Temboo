#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <Temboo.h>
#include <string.h>
#include "utility/debug.h"
#include "TembooAccount.h" // Contains Temboo account information
#include "WifiDetails.h" //contains wifi details

Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed but DI


int numRuns = 1;   // Execution count, so this doesn't run forever
int maxRuns = 10;   // Maximum number of times the Choreo should be executed

void setup() {
  Serial.begin(115200);
  
  // For debugging, wait until the serial console is connected.
  delay(4000);
  while(!Serial);

  // Initialize the module
Serial.println(F("\nInitialising the CC3000 ..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Unable to initialise the CC3000! Check your wiring?"));
    while(1);
  }

  Serial.println("OK");
  
   // Delete any old connection data on the module
  Serial.println(F("\nDeleting old connection profiles"));
  if (!cc3000.deleteProfiles()) {
    Serial.println(F("Failed!"));
    while(1);
  }

 
 // Attempt to connect to an access point
  char *ssid = WIFI_SSID;             /* Max 32 chars */
  Serial.print(F("\nAttempting to connect to ")); Serial.println(ssid);
  
  /* NOTE: Secure connections are not available in 'Tiny' mode! */
  if (!cc3000.connectToAP(WIFI_SSID, WPA_PASSWORD, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  Serial.println("Setup complete.\n");
}

void loop() {
  if (numRuns <= maxRuns) {
    Serial.println("Running AppendRow - Run #" + String(numRuns++));

    TembooChoreo AppendRowChoreo(client);

    // Invoke the Temboo client
    AppendRowChoreo.begin();

    // Set Temboo account credentials
    AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
    AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set profile to use for execution
    AppendRowChoreo.setProfile("GoogleDrive");

    // Set Choreo inputs
    String PasswordValue = "ftrpxebhyqfsrsvx";
    AppendRowChoreo.addInput("Password", PasswordValue);
    String SheetNameValue = "Data";
    AppendRowChoreo.addInput("SheetName", SheetNameValue);
    String RowDataValue = "123";
    AppendRowChoreo.addInput("RowData", RowDataValue);
    String SpreadsheetTitleValue = "Temperature";
    AppendRowChoreo.addInput("SpreadsheetTitle", SpreadsheetTitleValue);

    // Identify the Choreo to run
    AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");

    // Run the Choreo; when results are available, print them to serial
    AppendRowChoreo.run();

    while(AppendRowChoreo.available()) {
      char c = AppendRowChoreo.read();
      Serial.print(c);
    }
    AppendRowChoreo.close();
  }

  Serial.println("\nWaiting...\n");
  delay(30000); // wait 30 seconds between AppendRow calls
}
