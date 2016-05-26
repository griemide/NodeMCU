## Simple Ping sketch for ESP8266 

Library ESP8266Ping granted by dancol90 at [github](https://github.com/dancol90/ESP8266Ping)

### enhanced sketc and modified library

modified library and keywords.txt found here

ESP8266Ping.h:
```C++
    // bool ping(IPAddress dest,   byte count = 5);
    // bool ping(const char* host, byte count = 5);
    bool remoteIP(IPAddress dest,   byte count = 5);
    bool remoteHOST(const char* host, byte count = 5);
````
ESP8266Ping.cpp:
```C++
// bool PingClass::ping(IPAddress dest, byte count) {
bool PingClass::remoteIP(IPAddress dest, byte count) {

// bool PingClass::ping(const char* host, byte count) {
bool PingClass::remoteHOST(const char* host, byte count) {
    IPAddress remote_addr;

    if (WiFi.hostByName(host, remote_addr))
        // return ping(remote_addr, count);
        return remoteIP(remote_addr, count);
````
keywords.txt:
```C++
#######################################
# Methods and Functions (KEYWORD2)
#######################################

ping	KEYWORD2
remoteIP	KEYWORD2
remoteHOST	KEYWORD2
````


Serial: <br/>
![COM5](COM5 debugging.GIF)
