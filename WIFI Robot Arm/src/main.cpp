#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <Adafruit_SSD1306.h>

int baseServo = 2;
Servo BaseServo;

ESP8266WebServer server(80);
Adafruit_SSD1306 display(-1);

int heightClaw = 16;
Servo HeightClaw;

int blueLED = 0;

int yellowLED = 15;

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
  </head>
<body>

<style>
  body {
    margin:0;
    background-color:#000;
    color: Lime;
    font-family: consolas;
  }
  .container {
    text-align:center;
    width: 75%;
    margin: auto;
  }

  .slider {
    width: 100%;
  } 
</style>
 
<h2 style="text-align:center;">Dr. Monty's <br/> Wifi Robot Arm<h2>
 
<div class="container">
  <h3>Base Controls</h3>
  <input type="range" min="0" max="180" value="90" class="slider" id="myRange">
  <h3>CLaw Height Controls</h3>
  <input type="range" min="60" max="120" value="90" class="slider" id="clawHeight">
</div>

<script>

  var slider = document.getElementById("myRange");
  var clawHeightSlider = document.getElementById("clawHeight");

  slider.oninput = function() {
    console.log(this.value);
    pwm_change(this.value);
  }

  function pwm_change(val) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "sliderURL?sliderVal="+val, true);
    xhttp.send();
  }

  clawHeightSlider.oninput = function() {
    console.log(this.value);
    clawHeight_change(this.value);
  }

  function clawHeight_change(val) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "clawHeightURL?clawHeightSliderVal="+val, true);
    xhttp.send();
  }

</script>
 
</body>
</html>
)=====";

void showData(String data, String servo)
{
  display.setCursor(10, 0);
  display.println("Moving " + servo);
  display.setCursor(10, 10);
  display.println(data += " degress");
  display.display();
  display.clearDisplay();
}

void handle_baseServo ()
{
  String pwm_val = server.arg("sliderVal"); // reading from slider on html pagae
  Serial.print("slider val: ");
  Serial.println(pwm_val);
  showData(pwm_val, "Base Servo");
  BaseServo.write(pwm_val.toInt());
  server.send(200, "text/plain", pwm_val); // handling the webpage update

}

void handle_clawHeight(){
  String clawHeight_val = server.arg("clawHeightSliderVal");
  Serial.print("slider val: ");
  Serial.println(clawHeight_val);
  showData(clawHeight_val, "Claw height Servo");
  HeightClaw.write(clawHeight_val.toInt());
  server.send(200, "text/plain", clawHeight_val);
}

void setup(){
  Serial.begin(9600);

  WiFi.begin("Prettyfly-2.4G", "29912991");

  //////////////////////////////////////////////
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  ///////////////////////////////////////////

  server.on("/", []() { server.send_P(200, "text/html", MAIN_page); }); //Which routine to handle at root
  server.on("/sliderURL", handle_baseServo);
  server.on("/clawHeightURL", handle_clawHeight);

  server.begin();

  // Servos

  BaseServo.attach(baseServo);
  BaseServo.write(90);

  HeightClaw.attach(heightClaw);
  HeightClaw.write(70);

  // Display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(0.8);
  display.setTextColor(WHITE);

  display.setCursor(10, 0);
  display.println("Dr.Monty's Factory");
  display.println(WiFi.localIP());

  display.display();
  display.startscrollright(0x00, 0x00);
  display.clearDisplay();
}

void loop()
{
  server.handleClient();

}