## Uploading Large Files with the ESP8266

Arduino sketch is in the `chunked-upload` folder.

### Installation

1. Clone the repository
2. `npm install`


### Running the example

1. Start express (to receive the file) `node server.js`
2. Edit the sketch to specify the correct server address and port, and your wifi info (ssid, password).
3. Upload the arduino sketch to your ESP8266.
4. Edit `send-test.js` to specify the correct serial port for your ESP8266.
5. Run `node send-test.js`
