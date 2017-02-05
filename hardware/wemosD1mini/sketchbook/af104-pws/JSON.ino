/*
 * SKETCH:
 *  JSON.ino  (JSON decoding)
 * 
 *  (c) 2017, Michael Gries
 *  Creation: 2017-02-05 (based on JsonHttpClientEsp8266.ino)
 *  Modified: 2017-02-05 (editorial changes)
 * 
 * PREREQUISITES:
 *   none
 *  
 * LINKS:
 *   see Arduino JSON library https://github.com/bblanchon/ArduinoJson
 *   see used example http://gries.spdns.de/data/get/Aussentemperatur
 *  
 * ISSUES:
 *   none
 *   
 */

#include <ArduinoJson.h>

WiFiClient JSONclient;

bool JSONconnect2host(const char* hostName); 
bool JSONsendRequestG(const char* host, const char* resource);
bool skipResponseHead();
void readReponseContent(char* content, size_t maxSize);
bool parseUserData(char* content, struct UserData* userData);
void JSONdisconnect();

const unsigned long HTTP_TIMEOUT = 2000;  // max respone time from server
const size_t MAX_CONTENT_SIZE = 512;       // max size of the HTTP response

const char* JSONserver = "gries.spdns.de";                 // server's address
const char* resource = "/data/get/Aussentemperatur";   // http resource

// The type of data that we want to extract from the page
struct UserData {
  char key[20];
  char value[8];
};

void JSONshowUserData(const struct UserData* userData) 
{
  Serial.print(__func__); Serial.print(": Key = ");
  Serial.println(userData->key);
  Serial.print(__func__); Serial.print(": Value = ");
  Serial.println(userData->value);
  String tempAF104(userData->value); //convert char[] to String
  PWStempAF104 = tempAF104;
  PWStempValid = true;
}

void GetAF104tempData(void)
{
  PWStempValid = false; // reset while starting query
  if (JSONconnect2host(JSONserver)) {
    if (JSONsendRequestG(JSONserver, resource) && skipResponseHead()) {
      char response[MAX_CONTENT_SIZE];
      readReponseContent(response, sizeof(response));
      // Issues: response contains lengt og JSON message. 
      // I.e. this information habe to be masked
      response[0]=' '; // mask length byte 1
      response[1]=' '; // mask length byte 2
      
      //DEBUG: 
      //char* responsetest = "{\"value\":\" -1.1\",\"key\":\"Aussentemperatur\",\"response\":\"get\"}";
      //Serial.print("Response: "); Serial.println(responsetest);
      //Serial.print("Response: "); Serial.println(response);

      UserData userData;
      //if (parseUserData(responsetest, &userData)) { // DEBUG only
      if (parseUserData(response, &userData)) {
        JSONshowUserData(&userData);
      }
    }
    JSONdisconnect();
  }
}

// Open connection to the HTTP server
bool JSONconnect2host(const char* hostName) 
{
  Serial.print(__func__); Serial.print(": connect to ");
  Serial.print(hostName); Serial.print(" ... ");

  bool ok = JSONclient.connect(hostName, 80);

  Serial.println(ok ? "connected" : "Connection Failed!");
  return ok;
}

// Send the HTTP GET request to the server
bool JSONsendRequestG(const char* host, const char* resource) 
{
  Serial.print(__func__); Serial.print(": GET ");
  Serial.println(resource);

  JSONclient.print("GET ");
  JSONclient.print(resource);
  JSONclient.println(" HTTP/1.1");
  JSONclient.print("Host: ");
  JSONclient.println(JSONserver);
  JSONclient.println("Connection: close");
  JSONclient.println();

  return true;
}

// Skip HTTP headers so that we are at the beginning of the response's body
bool skipResponseHead() 
{
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";
  JSONclient.setTimeout(HTTP_TIMEOUT);
  bool ok = JSONclient.find(endOfHeaders);
  if (!ok) {
    Serial.print(__func__); Serial.println("No response or invalid response!");
  }
  return ok;
}

// Read the body of the response from the HTTP server
void readReponseContent(char* content, size_t maxSize) 
{
  size_t length = JSONclient.readBytes(content, maxSize);
  //DEBUG: 
  //Serial.print("Length: "); Serial.println(length);
  content[length] = 0;
  //DEBUG: 
  //Serial.print("Content: ");  Serial.println(content);  Serial.println("EOF");
}

/*
 * 
 * {
 * "value":" -1.1",
 * "key":"Aussentemperatur",
 * "response":"get"
 * }
 * 
 */

bool parseUserData(char* content, struct UserData* userData)
{
  // Compute optimal size of the JSON buffer according to what we need to parse.
  // This is only required if you use StaticJsonBuffer.

  const size_t BUFFER_SIZE =
      JSON_OBJECT_SIZE(3);     // the root object has 3 elements
      //+ JSON_OBJECT_SIZE(5)  // the "address" object has 5 elements

  // Allocate a temporary memory pool on the stack
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  // If the memory pool is too big for the stack, use this instead:
  // DynamicJsonBuffer jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(content);

  if (!root.success()) {
    Serial.print(__func__); Serial.println("JSON parsing failed!");
    return false;
  }

  // Here were copy the strings we're interested in
  strcpy(userData->value, root["value"]);
  strcpy(userData->key, root["key"]);
  // It's not mandatory to make a copy, you could just use the pointers
  // Since, they are pointing inside the "content" buffer, so you need to make
  // sure it's still in memory when you read the string

  return true;
}

// Close the connection with the HTTP server
void JSONdisconnect()
{
  //Serial.println("Disconnect"); // DEBUG only
  JSONclient.stop();
}
