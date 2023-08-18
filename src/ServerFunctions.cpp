#include <Arduino.h>
#include <EEPROM.h>
#include "ServerFunctions.h"
#include "config.h"
#include "board.h"
#include "Alerts.h"

String eventLog = "";

void addToEventLog(String event) {
  eventLog += getTimeStamp() + " - " + event + "\n";
  Serial.println(eventLog);
}

String getTimeStamp() {
  unsigned long currentMillis = millis() - startTime;

  unsigned long seconds = currentMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;

  char timeStamp[20];
  snprintf(timeStamp, sizeof(timeStamp), "%ld-%02ld:%02ld:%02ld",
           days, hours % 24, minutes % 60, seconds % 60);

  return String(timeStamp);
}

void toggleLockState() {
  locked = !locked;
  if(!locked){
    digitalWrite(alarmPin, LOW); // Turn off alarm when toggling lock state to unlock
  }
  addToEventLog(locked ? "Vehicle Locked" : "Vehicle Unlocked");
  if (locked) {
    playLockSound();
  } else {
    playUnlockSound();
  }
  
  // Update locked value in EEPROM
  EEPROM.write(4, locked);
  EEPROM.commit();
}

void handleToggleLock() {
  locked = !locked;
  toggleLockState();
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "Redirecting to /");
}

void handleEventLog() {
  String html = "<html><body>";
  html += "<h1>Event Log</h1>";
  html += "<pre class=\"event-log\">" + eventLog + "</pre>";
  html += "<br>";
  html += "<a href=\"/\">Back to Control Panel</a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleConfig() {
  String configHtml = "<html><head><title>Configuration</title>";
  configHtml += "<style>";
  configHtml += "body { font-family: Arial, sans-serif; }";
  configHtml += "h1 { color: #333; }";
  configHtml += "form { margin-bottom: 20px; }";
  configHtml += "label { display: inline-block; width: 180px; }";
  configHtml += "input[type='number'], input[type='text'], input[type='password'] { width: 200px; }";
  configHtml += "select { width: 200px; }";
  configHtml += ".button { background-color: #4CAF50; border: none; color: white; padding: 10px 20px; ";
  configHtml += "text-align: center; text-decoration: none; display: inline-block; font-size: 16px; ";
  configHtml += "margin: 4px 2px; cursor: pointer; }";
  configHtml += "</style>";
  configHtml += "</head><body>";
  configHtml += "<h1>Configuration Page</h1>";

  // Configure mode of operation
  configHtml += "<form action=\"/saveconfig\" method=\"post\">";
  configHtml += "<label for=\"mode\">Mode:</label>";
  configHtml += "<select name=\"mode\">";
  configHtml += "<option value=\"vehicle\">Vehicle</option>";
  configHtml += "<option value=\"valuable\">Valuable Items</option>";
  configHtml += "</select>";
  configHtml += "<input type=\"submit\" class=\"button\" value=\"Save\">";
  configHtml += "</form>";

  // Display appropriate configuration forms based on selected mode
  configHtml += "<form action=\"/saveconfig\" method=\"post\">";
  configHtml += "<h2>Threshold Values</h2>";
  configHtml += "<label for=\"accelThreshold\">Acceleration Threshold:</label>";
  configHtml += "<input type=\"number\" name=\"accelThreshold\" value=\"" + String(accelThreshold) + "\">";
  configHtml += "<br>";
  configHtml += "<label for=\"vibrationThreshold\">Vibration Threshold:</label>";
  configHtml += "<input type=\"number\" name=\"vibrationThreshold\" value=\"" + String(vibrationThreshold) + "\">";
  configHtml += "<br>";
  configHtml += "<label for=\"gyroRotationThreshold\">Gyro Rotation Threshold:</label>";
  configHtml += "<input type=\"number\" name=\"gyroRotationThreshold\" value=\"" + String(gyroRotationThreshold) + "\">";
  configHtml += "<br>";
  configHtml += "<label for=\"temperatureThreshold\">Temperature Threshold:</label>";
  configHtml += "<input type=\"number\" name=\"temperatureThreshold\" value=\"" + String(temperatureThreshold) + "\">";
  configHtml += "<br>";
  configHtml += "<input type=\"submit\" class=\"button\" value=\"Save\">";
  configHtml += "</form>";

  configHtml += "<br><a href=\"/\">Back to Control Panel</a>";
  configHtml += "</body></html>";

  server.send(200, "text/html", configHtml);
}


void handleSaveConfig() {
  if (server.method() == HTTP_POST) {
    if (server.hasArg("register_card")) {
        requested_to_register = true;
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "Card registration request sent. Please follow the instructions on the device screen.");
    }

    if (server.hasArg("mode")) {
      String selectedMode = server.arg("mode");
      if (selectedMode == "vehicle") {
        // Set thresholds for vehicle mode
        accelThreshold = 200;
        vibrationThreshold = 10;
        gyroRotationThreshold = 10;
        temperatureThreshold = 50.0;
      } else if (selectedMode == "valuable") {
        // Set thresholds for valuable items mode
        accelThreshold = 50;
        vibrationThreshold = 5;
        gyroRotationThreshold = 5;
        temperatureThreshold = 40.0;
      }
    if (server.hasArg("accelThreshold")) {
      accelThreshold = server.arg("accelThreshold").toInt();
      EEPROM.write(0, accelThreshold);
    }
    if (server.hasArg("vibrationThreshold")) {
      vibrationThreshold = server.arg("vibrationThreshold").toInt();
      EEPROM.write(1, vibrationThreshold);
    }
    if (server.hasArg("gyroRotationThreshold")) {
      gyroRotationThreshold = server.arg("gyroRotationThreshold").toInt();
      EEPROM.write(2, gyroRotationThreshold);
    }
    if (server.hasArg("temperatureThreshold")) {
      temperatureThreshold = server.arg("temperatureThreshold").toFloat();
      EEPROM.write(3, temperatureThreshold);
    }
    if (server.hasArg("apSSID")) {
      String newApSSID = server.arg("apSSID");
      newApSSID.toCharArray(storedApSSID, sizeof(storedApSSID));
      EEPROM.put(10, storedApSSID);
    }
    if (server.hasArg("apPassword")) {
      String newApPassword = server.arg("apPassword");
      newApPassword.toCharArray(storedApPassword, sizeof(storedApPassword));
      EEPROM.put(50, storedApPassword);
    }
    if (server.hasArg("customDeviceName")) {
      String newCustomDeviceName = server.arg("customDeviceName");
      newCustomDeviceName.toCharArray(storedCustomDeviceName, sizeof(storedCustomDeviceName));
      EEPROM.put(100, storedCustomDeviceName);
    }

    EEPROM.commit();
  }
  server.sendHeader("Location", "/config", true);
  server.send(302, "text/plain", "Redirecting to /config");
}
}


void handleHome() {
    String html = R"(
    <!DOCTYPE html>
    <html>
    <head>
    <title>Scooter Control Center</title>
    <style>
        body {
        font-family: Arial, sans-serif;
        }
        h1 {
        color: #333;
        }
        .button {
        background-color: #4CAF50;
        border: none;
        color: white;
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 16px;
        margin: 4px 2px;
        cursor: pointer;
        }
        .lock-status {
        font-size: 24px;
        margin-bottom: 20px;
        }
        .temperature {
        font-size: 20px;
        margin-bottom: 10px;
        }
        .event-log {
        font-size: 14px;
        white-space: pre-line;
        border: 1px solid #ddd;
        padding: 10px;
        background-color: #f9f9f9;
        }
        .dropdown {
        position: relative;
        display: inline-block;
        }
        .dropdown-content {
        display: none;
        position: absolute;
        background-color: #f9f9f9;
        min-width: 160px;
        box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);
        z-index: 1;
        }
        .dropdown:hover .dropdown-content {
        display: block;
        }
    </style>
    </head>
    <body>
    <h1>Welcome to Your Scooter Control Center</h1>
    
    <h2>Quick Links</h2>
    <div class="dropdown">
        <button class="button">Menu</button>
        <div class="dropdown-content">
        <a href="/">Dashboard</a>
        <a href="/eventlog">Event Log</a>
        <a href="/config">Configuration</a>
        <a href="/about">About</a>
        </div>
    </div>
    
    <h2>Dashboard</h2>
    <div class="lock-status">Lock Status: <span id="lock-status">Unknown</span></div>
    <div class="temperature">Current Temperature: <span id="current-temperature">N/A</span> &#8451;</div>
    <h3>Latest Event Log:</h3>
    <pre class="event-log">%EVENT_LOG%</pre>
    
    <form action="/togglelock" method="get">
        <button class="button" type="submit" id="toggle-lock-button">Toggle Lock</button>
    </form>
    
    <script>
        function updateLockStatus() {
        const lockStatusSpan = document.getElementById("lock-status");
        const toggleLockButton = document.getElementById("toggle-lock-button");
        lockStatusSpan.textContent = %LOCK_STATUS% === 1 ? "Locked" : "Unlocked";
        toggleLockButton.textContent = %LOCK_STATUS% === 1 ? "Unlock" : "Lock";
        }

        function updateTemperature() {
        const temperatureSpan = document.getElementById("current-temperature");
        temperatureSpan.textContent = %CURRENT_TEMPERATURE%;
        }

        function updateEventLog() {
        const eventLogPre = document.querySelector(".event-log");
        eventLogPre.textContent = `%EVENT_LOG%`;
        }

        updateLockStatus();
        updateTemperature();
        updateEventLog();
    </script>
    </body>
    </html>
    )";
    html.replace("%LOCK_STATUS%", locked ? "1" : "0");
    html.replace("%CURRENT_TEMPERATURE%", String(temperatureC));

    // Strip eventLog of newline characters and get the latest event

    String latestEvent = eventLog;
    latestEvent.replace("\n", "<br>");
    size_t lastNewlineIndex = latestEvent.lastIndexOf("<br>");
    if (lastNewlineIndex != -1) {
        latestEvent = latestEvent.substring(lastNewlineIndex + 4); // +4 to remove the "<br>" tag
    }

    // Remove the timestamp from the latest event
    size_t timestampSeparator = latestEvent.indexOf(" - ");
    if (timestampSeparator != -1) {
        latestEvent = latestEvent.substring(timestampSeparator + 3); // +3 to remove the " - " separator and space
    }

    html.replace("%EVENT_LOG%", latestEvent);

    server.send(200, "text/html", html);
}



void handleAbout() {
  String aboutHtml = "<html><head><title>About</title></head><body>";
  aboutHtml += "<h1>About This Project</h1>";
  aboutHtml += "<p>VATA Monitoring Device roject is designed to monitor and safegaurd you vehicle/ valubles when you are away. It provides features such as locking/unlocking, event logging, and configuration settings.</p>";
  aboutHtml += "<p>Developed by Karun Nunavath &copy; 2023.</p>";
  aboutHtml += "<br><a href=\"/\">Back to Control Panel</a>";
  aboutHtml += "</body></html>";

  server.send(200, "text/html", aboutHtml);
}


