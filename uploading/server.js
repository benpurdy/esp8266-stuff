const express     = require("express");
const app         = express();
const server      = require("http").createServer(app);
const fs          = require("fs");


app.put("/chunk", function(req, res, next) {
  var startTime = Date.now();
  var body = '';

  var outFile = fs.openSync("./output.txt", 'w');
  var rxBytes = 0;
  var totalBytes = parseInt(req.headers["content-length"]);


  req.on('data', function(data) {
    
    fs.writeSync(outFile, data.toString());
    rxBytes += data.length;

    var seconds =  (Date.now() - startTime) * 0.001;

    console.log( rxBytes + "/" + totalBytes + ", " + seconds.toFixed(2) + " seconds");
   });


  req.on('end', function() {
    console.log("Done receiving data..");
    fs.close(outFile);
    res.end();
  });

  req.on('error', function(err){
    console.log("ERROR:",err);
  });

  console.log("A request is happening...");
});


server.listen(9000);

console.log("Server is listening for uploads on port 9000");
