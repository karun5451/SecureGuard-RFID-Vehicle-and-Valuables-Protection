#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include <ESP8266WebServer.h>

extern ESP8266WebServer server;
extern String eventLog;

void handleEventLog();
void handleConfig();
void handleSaveConfig();
void handleHome();
void toggleLockState();
void handleToggleLock();
String getTimeStamp();
void addToEventLog(String event);




#endif // SERVER_FUNCTIONS_H
