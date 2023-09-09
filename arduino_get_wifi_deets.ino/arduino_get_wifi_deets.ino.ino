#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");
}

void loop() {
  Serial.println("scan start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) Serial.println("no networks found");
  else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) { // Print SSID and RSSI for each network found
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(WiFi.SSID(i));
    Serial.print(" (");
    Serial.print(WiFi.RSSI(i));
    Serial.print(") -> ");
    
    char *encryptionTypeText[] = { "OPEN", "WEP", "WPA_PSK", "WPA2_PSK", "WPA_WPA2_PSK", "WPA2_ENTERPRISE", "OTHER" };
    int k = WiFi.encryptionType(i);
    if ( k < WIFI_AUTH_OPEN || k > WIFI_AUTH_WPA2_ENTERPRISE ) k = WIFI_AUTH_WPA2_ENTERPRISE + 1;
    Serial.println(encryptionTypeText[k]);
    delay(10);
    }
  }

  Serial.println("");
  delay(5000); // Wait a bit before scanning again
}
