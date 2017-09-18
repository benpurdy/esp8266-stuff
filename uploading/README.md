## Uploading Large Files with the ESP8266


This is an example of a solution for uploading large files via the ESP8266.


### Contents

* `chunked-upload/chunked-upload.ino` - the arduino sketch to run on the ESP8266
* `send-test.js` - a node app to act as the serial data provider (the thing needing to send a large file).
* `server.js` - a node app to recieve the large fila via HTTP PUT
* `big1.txt` - a file to be sent.


### Installation

1. Clone the repository
2. `npm install`


### Running the example

1. Edit the sketch to specify the correct server address and port, and your wifi info (ssid, password).
2. Upload the arduino sketch to your ESP8266.
3. Edit `send-test.js` to specify the correct serial port for your ESP8266.
4. Start express (to receive the file) `node server.js` and leave it running in a terminal.
5. In a separate terminal session, run `node send-test.js`

Once the upload is complete, look at `output.txt` and it should be a copy of `big1.txt`