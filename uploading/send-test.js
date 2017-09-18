const fs          = require("fs");

var bigFileData = fs.readFileSync("./big1.txt");

var SerialPort = require("serialport");

const Readline = SerialPort.parsers.Readline;
const parser = new Readline();


var serial = new SerialPort(
  "/dev/cu.usbserial-AL034FT4", {
    baudRate: 115200
  }, 
  function(err) {
    console.log("Open.", err);
    if(!err){
      
    }
  }
);


var writing = false;
var offset = 0;
var chunkSize = 2048;


function sendChunk() {

  if(writing) {

    var end = offset + chunkSize;

    if(end > bigFileData.length) {
      end = bigFileData.length;
      console.log("Writing last chunk.");
      writing = false;
    }
    
    var out = bigFileData.slice(offset, end);
    
    console.log("Sending chunk, " + out.length + " bytes, from=" + offset + " to " + end);
    
    serial.write(out);

    offset += out.length;
  }

}

serial.pipe(parser);

parser.on('data', function(line) {

  console.log("SERIAL: " + line);

  if(line == "READY") {
    console.log("Sending buffer, bytes=" + bigFileData.length);
    writing = true;
    sendChunk();  
  } else if(line.indexOf("NEXT") === 0) {
    sendChunk();
  }
});


// Start sending after 5 seconds.
setTimeout(function() {
   console.log("Starting, data size=" + bigFileData.length);
   serial.write("SND=" + bigFileData.length + "\n");
}, 5000);
