/* This sketch will listen on the serial port and will upload binary data to an HTTP server. Data is buffered to keep the ESP8266 
 * transmit buffer from overflowing. Essentially, the ESP8266 will only take data from the serial port in small chunks, and will 
 * explicitly "ask" for subsequent chunks only when ready.
 * 
 * The ESP8266 will wait until it sees the following command on the serial port:
 *  
 * SND=#####\n
 * 
 * #### is the total number of bytes to be uploaded, for example:
 * 
 * SND=1276394\n
 * 
 * Upon recieving this command, the ESP8266 will attempt to connect to the server and initiate an HTTP PUT.
 * Once the conneciton is established and the HTTP headers have been sent, the ESP8266 will respond over the serial 
 * connection with the string "READY\n" to indicate that it's ready for the first chunk of data (chunks are specified by 
 * chunkSize, 2kb seems stable after some testing).
 * 
 * The provider of data (the other end of the serial connection) should then send the number of bytes specified by chunkSize.
 * These bytes will be buffered until the chunk is fully recieved. Then the state will switch to SENDING and the ESP8266 will 
 * transmit the buffered bytes to the server. Once complete, it will send "NEXT #####\n" over the serial connection indicating that
 * it's ready for the next chunk of data (##### indicates the total number of bytes remaining, just for debugging purposes).
 * 
 * This cycle will continue until all of the bytes have been transmitted, then the state will switch back to WAITING
 * 
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;

#define WAITING   1
#define SENDING   2
#define BUFFERING 3

int state = WAITING;

int messageSize = 20;
long bytesRemaining = 0;

char* serverAddress   = "192.168.1.177";
int serverPort        = 9000;
const long chunkSize  = 2048;

byte buffer[chunkSize];
int bufferInputIndex = 0;
int bytesToBuffer = 0;

WiFiClient wifiClient;

void setup() {

    Serial.begin(115200);

    Serial.println();
    Serial.println();
    Serial.println();

    for(uint8_t t = 4; t > 0; t--) {
        Serial.print("Waiting a bit...\n");
        Serial.flush();
        delay(1000);
    }

    // PUT YOUR WIFI INFO IN HERE:
    WiFiMulti.addAP("ssid", "password");
}

void loop() {

    if((WiFiMulti.run() == WL_CONNECTED)) {


        if(state == WAITING) {
          
          bool startSend = false;
          
          while(Serial.available() > 0) {
            
            String command = Serial.readStringUntil('\n');

            String firstPart = command.substring(0, 4);
            if(firstPart.equals("SND=")) {
              String byteCount = command.substring(4, command.length());
              Serial.println("Swtiching to buffering mode.");
              
              bytesRemaining = byteCount.toInt();

              Serial.print("Expecting ");
              Serial.print(bytesRemaining);
              Serial.print(" bytes\n");

              state = BUFFERING;
              bytesToBuffer = (chunkSize > bytesRemaining) ? bytesRemaining : chunkSize;
              
              Serial.print("Staring HTTP..\n");
              
              if (!wifiClient.connect(serverAddress, serverPort)) {
                
                Serial.println("connection failed");
                state = WAITING;
                
              } else {
    
                Serial.print("HTTP Putting ");
                Serial.print(bytesRemaining);
                Serial.println(" bytes.\n");

                // Start the HTTP PUT.
                // TODO: use the actual server address in the Host header..
                wifiClient.print("PUT /chunk HTTP/1.1\r\nHost: localhost\r\nContent-type: text/html\r\nConnection: close\r\n\Content-length: ");
                wifiClient.print(bytesRemaining);
                wifiClient.print("\r\n\r\n");

                // Notify the serial connection to start sending data.
                Serial.print("READY\n");
                        
                break;
              }
            }
          }
        }
        

        if(state == BUFFERING) {
           while(Serial.available() > 0) {
             
             buffer[bufferInputIndex++] = Serial.read();
             
             bytesToBuffer--;
             bytesRemaining--;

             // Once the buffer is full, switch to the SENDING state.
             if(bytesToBuffer <= 0) {
              state = SENDING;
              break;
             }
           }
        }
        

        if(state == SENDING) {
          
          wifiClient.write(buffer, bufferInputIndex);
          bufferInputIndex = 0;

          // Check to see if there's more data to send.
          if(bytesRemaining <= 0) {
          
            Serial.print("DONE SENDING!\n");

            // Close the connection to the server and return to the WAITING state.
            wifiClient.stop();
            state = WAITING;
          } else {

            // Determing the next chunk size
            bytesToBuffer = (chunkSize > bytesRemaining) ? bytesRemaining : chunkSize;
            state = BUFFERING;

            // Ask for the next chunk.
            Serial.print("NEXT ");
            Serial.print(bytesToBuffer);
            Serial.print("\n");
          }
        }


    } else {
      Serial.print("No wifi connection yet..");
      delay(1000);
    }
}

