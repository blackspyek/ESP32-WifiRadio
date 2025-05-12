#include "StationServer.h"
#include <WiFi.h>
#include <Preferences.h>
#include "RadioTft.h"
#include "RadioController.h"

WiFiServer server(1337);
void saveRadioURL(int radioStationNumber, const String& name, const String& url) {
    prefs.begin("radio", false);
  
    String key_name = "station" + String(radioStationNumber) + "_name";
    String key_url  = "station" + String(radioStationNumber) + "_url";
  
    prefs.putString(key_name.c_str(), name);
    prefs.putString(key_url.c_str(), url);
  
    prefs.end();
    menuItems[radioStationNumber].name = name;
    menuItems[radioStationNumber].url  = url;

    if (radioStationNumber == getLastRadioStationFromFlash()) {
        currentRadioStation.item.name = name;
        currentRadioStation.item.url  = url;
    }
}

void handleRequest(WiFiClient client) {
    String request = client.readStringUntil('\r');
    client.readStringUntil('\n'); 
  
    if (request.startsWith("GET /?")) {
      int station = -1;
      String name, url;
  
      int sIndex = request.indexOf("station=");
      if (sIndex != -1) station = request.substring(sIndex + 8).toInt();
  
      int nameIndex = request.indexOf("name=");
      if (nameIndex != -1) {
        int amp = request.indexOf('&', nameIndex);
        name = request.substring(nameIndex + 5, amp);
        name.replace('+', ' ');
      }
  
      int urlIndex = request.indexOf("url=");
      if (urlIndex != -1) {
        int space = request.indexOf(' ', urlIndex);
        url = request.substring(urlIndex + 4, space);
      }
  
      if (station >= 0 && station < 3) {
        saveRadioURL(station, name, urlDecode(url));
        Serial.printf("Zapisano: [%d] %s - %s\n", station, name.c_str(), url.c_str());
      }
      client.println("HTTP/1.1 303 See Other");
      client.println("Location: /");
      client.println("Connection: close");
      client.println();
      client.stop();
      return;
    }
  
    // HTML odpowiedź
    String html = "<html><body><h2>Radio Config</h2>";
    for (int i = 0; i < 3; i++) {
      html += "<form method='GET'>";
      html += "<input type='hidden' name='station' value='" + String(i) + "'>";
      html += "Name: <input type='text' name='name' value='" + menuItems[i].name + "'><br>";
      html += "URL: <input type='text' name='url' value='" + menuItems[i].url + "'><br>";
      html += "<input type='submit' value='Update Station " + String(i + 1) + "'>";
      html += "</form><hr>";
    }
    html += "</body></html>";
  
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(html);
    client.stop();
}

String urlDecode(const String &input) {
    String decoded = "";
    char temp[] = "0x00";
    unsigned int len = input.length();
    unsigned int i = 0;
  
    while (i < len) {
      if (input[i] == '%') {
        if (i + 2 < len) {
          temp[2] = input[i + 1];
          temp[3] = input[i + 2];
          decoded += (char) strtol(temp, NULL, 16);
          i += 3;
        } else {
          break;
        }
      } else if (input[i] == '+') {
        decoded += ' ';
        i++;
      } else {
        decoded += input[i++];
      }
    }
    return decoded;
  }