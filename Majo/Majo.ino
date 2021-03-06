
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <title>Colobot</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style>

        html {
          font-family: Arial, Helvetica, sans-serif;
          text-align: center;
        }

        input[type=number], select {
          width: 100%;
          padding: 12px 20px;
          margin: 8px 0;
          display: inline-block;
          border: 1px solid #ccc;
          border-radius: 4px;
          box-sizing: border-box;
        }

        input[type=submit] {
          width: 100%;
          background-color: #4CAF50;
          color: white;
          padding: 14px 20px;
          margin: 8px 0;
          border: none;
          border-radius: 4px;
          cursor: pointer;
        }

        input[type=submit]:hover {
          background-color: #45a049;
        }

        #canvasContainer{
          height: 400px;
          width: 400px;
          background-color: #18191A; 
          position:relative;
          left: calc(50vw - 200px);
        }

        #canvas {
          width:400px;
          height:400px;
        }

    </style>

    <script>
      var gateway = `ws://${window.location.hostname}/ws`;
      var websocket;
      
      window.addEventListener('load', onLoad);

      var age = 0;
      var num = 0;

      var colors = [
        ["Red"    ,"Pula"   ,"#FF0000"  ,1  ,"red-english"    ,"red-tagalog"    ,"red-correct"],
        ["Blue"   ,"Asul"   ,"#0000FF"  ,2  ,"blue-english"   ,"blue-tagalog"   ,"blue-correct"],
        ["Yellow" ,"Dilaw"  ,"#FFFF00"  ,3  ,"yellow-english" ,"yellow-tagalog" ,"yellow-correct"],
        ["Green"  ,"Berde"  ,"#00FF00"  ,4  ,"green-english"  ,"green-tagalog"  ,"green-correct"],
        ["Violet" ,"Ube"   ,"#9400D3"  ,5  ,"violet-english" ,"violet-tagalog" ,"violet-correct"],
        ["Orange" ,"Dalandan"  ,"#FF7F00"  ,6  ,"orange-english" ,"orange-tagalog" ,"orange-correct"],
        ["Pink"   ,"Rosas"  ,"#FFC0CB"  ,7  ,"pink-english"   ,"pink-tagalog"   ,"pink-correct"]];



      var questions = [4,6,7];
      var currentQuestion = 1;
      var language = 0; //bool
      var answerFound = 0;//bool
      
      function onLoad(event) {

        websocket = new WebSocket(gateway);

        websocket.onopen    = onOpen;
        websocket.onclose   = onClose;
        websocket.onmessage = onMessage; // <-- add this line

      }

      function onOpen(event) {
        console.log('Connection opened');
      }
      function onClose(event) {
        console.log('Connection closed');
        alert('Socket got disconnected please refresh the page');
        window.location.href = window.location.href;
      }
      function onMessage(event) {

        var recDatas = JSON.parse(event.data)
        //console.log(recDatas)

        var c = document.getElementById("canvas");
        var ctx = c.getContext("2d");


        if(answerFound == 0){
          ctx.clearRect(0, 0, canvas.width, canvas.height);
          for(var z =0; z< recDatas.blocks.length; z++){
              for(var i =0; i<colors.length; i++){
                  if(recDatas.blocks[z].signature === colors[i][3]){
                      ctx.beginPath();
                      ctx.rect(recDatas.blocks[z].x, recDatas.blocks[z].y, recDatas.blocks[z].width, recDatas.blocks[z].height);
                      ctx.fillStyle   = colors[i][2]
                      ctx.strokeStyle = colors[i][2]
                      ctx.fill();
  
                      if(recDatas.blocks[z].signature === questions[currentQuestion]){
                          answerFound = 1;
                          websocket.send('Stop Streaming')
                          websocket.send(colors[questions[currentQuestion]][6]);
                          document.getElementById('questionColor').innerHTML    =  'WINNER!';
                          document.getElementById('questionColor').style.color  =  '#FFD700';
                      }
  
                      break;
                  }
              }
          }
        }
      }

      function checkFirstPageInputs(){

          age = document.getElementById("userAge").value
          num = document.getElementById("userNum").value


          if ( age === 3 || age === 4 ){
            websocket.send('Disable Movement');
          }
          else{
            alert('You can try using the remote to control the robot :)')
            websocket.send('Enable Movement');
          }

          document.getElementById('page1').style.display='none';
          document.getElementById('page2').style.display='block';

          startGame();
      }

      function startGame(){

          while(questions.length < num){
              var r = Math.floor(Math.random() * colors.length);
              if(questions.indexOf(r) === -1)
                  questions.push(r);
          }
          console.log(questions);

          displayQuestions();
          websocket.send('Start Streaming');
      }

      function displayQuestions(){

        if(language == 1)
            websocket.send(colors[questions[currentQuestion]][5]);
        else
            websocket.send(colors[questions[currentQuestion]][4]);
            
        document.getElementById('questionColor').innerHTML    =  colors[questions[currentQuestion]][language];
        document.getElementById('questionColor').style.color  =  colors[questions[currentQuestion]][2];
      }

      function filipinize(){


        if(language == 1){
          document.getElementById('languageButton').value    =  'Filipino';
          language = 0;
        }
        else{
          document.getElementById('languageButton').value    =  'English';
          language = 1;
        }
        
        displayQuestions();
      }

      function next(){

        if(answerFound==0){
          alert('You have to find the color ' + colors[questions[currentQuestion]][language] + ' first');
          return;
        }
          
        if(questions.length > currentQuestion +1){
          answerFound = 0;
          websocket.send('Start Streaming');
          currentQuestion++;
        }
        else{
          websocket.send('congrats');
          alert("Congratulations! You have found all the colors! Click 'OK' to play again")
          window.location.href = window.location.href;
          return;
        }
        displayQuestions();
      }



    </script>
  </head>



  <body>
    <div id = "page1">
      <h2 class="display-1">Colobot</h2>

      <form action="javascript:;" onsubmit="checkFirstPageInputs()">

        <label for="userAge">Age:</label>
        <input type="number" id="userAge" name="age" min="3" max="6" required placeholder="Enter your age" oninvalid="this.setCustomValidity('Ages 3 to 6 only')" oninput="this.setCustomValidity('')">

        <label for="userNum">Number of Colors:</label>
        <input type="number" id="userNum" name="num" min="1" max="7" required placeholder="Enter a number from 1 - 7" oninvalid="this.setCustomValidity('Only numbers from 1 - 7')" oninput="this.setCustomValidity('')">

        <input type="submit">
      </form>

    </div>

    <div id = "page2" style="display: none;">
      <h2 class="display-1">Colobot</h2>
      <div class="row">
        <div class="col-sm">
          <h3>Pixymon Camera</h3>
          <div id="canvasContainer">
            <canvas id="canvas" width=400 height=400/>
          </div>
          </br>
          </br>
          <H4>Find me the color <b><p id="questionColor"></p></b></H4>
          </br>
          </br>
          <H4>That's correct! You found the color <p id="questionColor"></p> </H4>
          <input class="btn btn-primary col-3" onclick="filipinize()" type="submit" id="languageButton" value="Filipino">
          <input class="btn btn-success col-4" onclick="next()"       type="submit" id="nextcolor"      value="Next Color">
        </div>
      </div>
    </div>
  </body>

</html>
)rawliteral";










#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include <Pixy2SPI_SS.h>



bool streamState = false;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

Pixy2SPI_SS pixy;
int minimumBlockAge    = 50;
int minimumBlockHeight = 10;
int minimumBlockWidth  = 10;

SoftwareSerial mySoftwareSerial(33, 32); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

//File number
const int background1     = 20;
const int congrats        = 9;

const int blueCorrect     = 21;
const int blueEnglish     = 22;
const int blueTagalog     = 23;

const int greenCorrect    = 1;
const int greenEnglish    = 2;
const int greenTagalog    = 3;

const int orangeCorrect   = 4;
const int orangeEnglish   = 5;
const int orangeTagalog   = 6;

const int pinkCorrect     = 7;
const int pinkEnglish     = 8;
const int pinkTagalog     = 9;

const int redCorrect      = 11;
const int redEnglish      = 12;
const int redTagalog      = 13;

const int violetCorrect   = 14;
const int violetEnglish   = 15;
const int violetTagalog   = 16;

const int yellowCorrect   = 17;
const int yellowEnglish   = 18;
const int yellowTagalog   = 19;



void notifyClients() {

    bool dataIndicator = false;
    String jsonString = "{ \"blocks\": [ ";
    
    for (int i=0; i<pixy.ccc.numBlocks; i++)
    {
      if (pixy.ccc.blocks[i].m_age > minimumBlockAge && pixy.ccc.blocks[i].m_height > minimumBlockHeight && pixy.ccc.blocks[i].m_width > minimumBlockWidth) {   // check for object that are in the camera view for longer 50 frames atleast
        dataIndicator = true;
        jsonString +=     "{ \"x\": "           +  String(pixy.ccc.blocks[i].m_x) + 
                          ", \"y\": "           +  String(pixy.ccc.blocks[i].m_y) + 
                          ", \"height\": "      +  String(pixy.ccc.blocks[i].m_height) + 
                          ", \"width\": "       +  String(pixy.ccc.blocks[i].m_width) + 
                          ", \"age\": "         +  String(pixy.ccc.blocks[i].m_age)  + 
                          ", \"signature\": "   +  String(pixy.ccc.blocks[i].m_signature) +
                          " },";
      }
    }

  jsonString.remove(jsonString.length() - 1);
  jsonString += " ]";

  jsonString += " }";

  if(dataIndicator)
    ws.textAll(jsonString);
  
}

void playMusic(int trackNo){
  myDFPlayer.play(trackNo); 
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    
    Serial.print("Data Recieved: ");
    Serial.println((char*)data);

    //========================================================//
//    if (strcmp((char*)data, "Enable Movement") == 0) {
//      Serial.println("Movement ENABLED");
//      digitalWrite(ledPin,true);
//      return;
//    }
//    if (strcmp((char*)data, "Disable Movement") == 0) {
//      Serial.println("Movement DISABLED");
//      digitalWrite(ledPin,false);
//      return;
//    }

    if (strcmp((char*)data, "Start Streaming") == 0) {
      Serial.println("Stream started");
      streamState = true;
      return;
    }

    if (strcmp((char*)data, "Stop Streaming") == 0) {
      Serial.println("Stream stoped");
      streamState = false;
      return;
    }

    if (strcmp((char*)data, "Reset") == 0) {
      Serial.println("Resetting");
      streamState = false;
      return;
    }
    //========================================================//
    if (strcmp((char*)data, "red-english") == 0) {
      Serial.println("Playing red-english");
      myDFPlayer.pause();
      playMusic(redEnglish);
      return;
    }
    if (strcmp((char*)data, "red-tagalog") == 0) {
      Serial.println("Playing red-tagalog");
      myDFPlayer.pause();
      playMusic(redTagalog);
      return;
    }
    if (strcmp((char*)data, "red-correct") == 0) {
      Serial.println("Playing red-correct");
      myDFPlayer.pause();
      playMusic(redCorrect);
      return;
    }
    //========================================================//
    if (strcmp((char*)data, "blue-english") == 0) {
      Serial.println("Playing blue-english");
      myDFPlayer.pause();
      playMusic(blueEnglish);
      return;
    }
    if (strcmp((char*)data, "blue-tagalog") == 0) {
      Serial.println("Playing blue-tagalog");
      myDFPlayer.pause();
      playMusic(blueTagalog);
      return;
    }
    if (strcmp((char*)data, "blue-correct") == 0) {
      Serial.println("Playing blue-correct");
      myDFPlayer.pause();
      playMusic(blueCorrect);
      return;
    }
    //========================================================//
    if (strcmp((char*)data, "yellow-english") == 0) {
      Serial.println("Playing yellow-english");
      myDFPlayer.pause();
      playMusic(yellowEnglish);
      return;
    }
    if (strcmp((char*)data, "yellow-tagalog") == 0) {
      Serial.println("Playing yellow-tagalog");
      myDFPlayer.pause();
      playMusic(yellowTagalog);
      return;
    }
    if (strcmp((char*)data, "yellow-correct") == 0) {
      Serial.println("Playing yellow-correct");
      myDFPlayer.pause();
      playMusic(yellowCorrect);
      return;
    }
    //========================================================//
    if (strcmp((char*)data, "green-english") == 0) {
      Serial.println("Playing green-english");
      myDFPlayer.pause();
      playMusic(greenEnglish);
      return;
    }
    if (strcmp((char*)data, "green-tagalog") == 0) {
      Serial.println("Playing green-tagalog");
      myDFPlayer.pause();
      playMusic(greenTagalog);
      return;
    }
    if (strcmp((char*)data, "green-correct") == 0) {
      Serial.println("Playing green-correct");
      myDFPlayer.pause();
      playMusic(greenCorrect);
      return;
    }
    //========================================================//
    if (strcmp((char*)data, "violet-english") == 0) {
      Serial.println("Playing violet-english");
      myDFPlayer.pause();
      playMusic(violetEnglish);
      return;
    }
    if (strcmp((char*)data, "violet-tagalog") == 0) {
      Serial.println("Playing violet-tagalog");
      myDFPlayer.pause();
      playMusic(violetTagalog);
      return;
    }
    if (strcmp((char*)data, "violet-correct") == 0) {
      Serial.println("Playing violet-correct");
      myDFPlayer.pause();
      playMusic(violetCorrect);
      return;
    }
    //========================================================//
    if (strcmp((char*)data, "orange-english") == 0) {
      Serial.println("Playing orange-english");
      myDFPlayer.pause();
      playMusic(orangeEnglish);
      return;
    }
    if (strcmp((char*)data, "orange-tagalog") == 0) {
      Serial.println("Playing orange-tagalog");
      myDFPlayer.pause();
      playMusic(orangeTagalog);
      return;
    }
    if (strcmp((char*)data, "orange-correct") == 0) {
      Serial.println("Playing orange-correct");
      myDFPlayer.pause();
      playMusic(orangeCorrect);
      return;
    }
    //========================================================//
    if (strcmp((char*)data, "pink-english") == 0) {
      Serial.println("Playing pink-english");
      myDFPlayer.pause();
      playMusic(pinkEnglish);
      return;
    }
    if (strcmp((char*)data, "pink-tagalog") == 0) {
      Serial.println("Playing pink-tagalog");
      myDFPlayer.pause();
      playMusic(pinkTagalog);
      return;
    }
    if (strcmp((char*)data, "pink-correct") == 0) {
      Serial.println("Playing pink-correct");
      myDFPlayer.pause();
      playMusic(pinkCorrect);
      return;
    }
    //========================================================//
    if (strcmp((char*)data, "congrats") == 0) {
      Serial.println("congrats");
      myDFPlayer.pause();
      playMusic(congrats);
      return;
    }
    
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
              
  switch (type) {
//    case WS_EVT_CONNECT:
//      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
//      break;
//    case WS_EVT_DISCONNECT:
//      Serial.printf("WebSocket client #%u disconnected\n", client->id());
//      streamState = false;
//      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
  }
}




void setup(){
  
  // Serial port for debugging purposes
  Serial.begin(115200);


  mySoftwareSerial.begin(9600);
  Serial.println("DFRobot DFPlayer Mini Demo");
  Serial.println("Initializing DFPlayer ... (May take 3~5 seconds)");

  if (!myDFPlayer.begin(mySoftwareSerial, true, false)) {  //Use softwareSerial to communicate with mp3.
    Serial.println("Unable to begin:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
  }
  Serial.println("DFPlayer Mini online.");



//  //via softAPIIP
  const char* ssid     = "ESP32-Access-Point";
  const char* password = "123456789";
  Serial.print("Setting AP (Access Point)???");
  WiFi.softAP(ssid, password,1,0,1);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);



  ws.onEvent(onEvent);
  server.addHandler(&ws);

  // Route for root / web page

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  pixy.init();
  server.begin();

  myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  myDFPlayer.outputSetting(true, 15);
  myDFPlayer.enableLoop(); //enable loop.
  myDFPlayer.loop(background1);     //Play the first mp3
}

int period = 100; // in millisecond
unsigned long timeLastDataSent = 0;

void loop() {
  ws.cleanupClients();

  if (myDFPlayer.available()) {
    if( myDFPlayer.readType() == DFPlayerPlayFinished){
      Serial.print("Finished Playing Track: ");
      Serial.println(myDFPlayer.read());
      myDFPlayer.loop(background1);
    }
  }

  
  if(streamState && ((millis() - timeLastDataSent) > period) ){
      timeLastDataSent = millis();
      pixy.ccc.getBlocks();
      if (pixy.ccc.numBlocks)
        notifyClients();
  }

  
}
