## Uploading Large Files with the ESP8266

Arduino sketch is in the `chunked-upload` folder.


### Contents

* `chunked-upload/chunked-upload.ino` - the arduino sketch to run on the ESP8266
* `send-test.js` - a node app to act as the data provider (the thing needing to send a large file)
* `server.js` - a node app to recieve the large fila via HTTP PUT


### Installation

1. Clone the repository
2. `npm install`


### Running the example

1. Edit the sketch to specify the correct server address and port, and your wifi info (ssid, password).
2. Upload the arduino sketch to your ESP8266.
3. Edit `send-test.js` to specify the correct serial port for your ESP8266.
4. Start express (to receive the file) `node server.js` and leave it running in a terminal.
5. In a separate terminal session, run `node send-test.js`
