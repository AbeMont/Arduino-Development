#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <Adafruit_SSD1306.h>

int baseServo = 2;
Servo BaseServo;

int heightClaw = 16;
Servo HeightClaw;

int extendServo = 0;
Servo ExtendServo;

int clawServo = 15;
bool clawopen = false;
Servo ClawServo;

ESP8266WebServer server(80);
Adafruit_SSD1306 display(-1);

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

  <h4>Open/Close Claw</h4>
  <button class="btn" id="clawBtn"> Open/Close </button>

  <h4>Base Controls</h4>
  <input type="range" min="0" max="180" value="90" class="slider" id="myRange">

  <h4>Claw Height Controls</h4>
  <input type="range" min="60" max="120" value="90" class="slider" id="clawHeight">

  <h4>Extend Forward/Back Controls</h4>
  <input type="range" min="40" max="120" value="90" class="slider" id="extend">

</div>

<script>

  var slider = document.getElementById("myRange");
  var clawHeightSlider = document.getElementById("clawHeight");
  var extendSlider = document.getElementById("extend");
  var clawBtn = document.getElementById("clawBtn");

  slider.oninput = function() {
    console.log(this.value);
    pwm_change(this.value);
  }

  clawHeightSlider.oninput = function() {
    console.log(this.value);
    clawHeight_change(this.value);
  }

  extendSlider.oninput = function() {
    console.log(this.value);
    extend_change(this.value);
  }

  clawBtn.onclick = function(){  
    console.log(this.value);
    btn_change();
  }

  function pwm_change(val) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "sliderURL?sliderVal="+val, true);
    xhttp.send();
  }

  function clawHeight_change(val) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "clawHeightURL?clawHeightSliderVal="+val, true);
    xhttp.send();
  }

  function extend_change(val) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "extendURL?extendSliderVal="+val, true);
    xhttp.send();
  }

  function btn_change(val) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "buttonURL", true);
    xhttp.send();
  }

</script>
 
</body>
</html>
)=====";

void showData(String data, String servo)
{
  display.clearDisplay();
  display.setCursor(10, 0);
  display.print("Moving " + servo);
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
  showData(clawHeight_val, "Claw Height Servo");
  HeightClaw.write(clawHeight_val.toInt());
  server.send(200, "text/plain", clawHeight_val);
}

void handle_extend()
{
  String extend_val = server.arg("extendSliderVal");
  Serial.print("slider val: ");
  Serial.println(extend_val);
  showData(extend_val, "Extend Servo");
  ExtendServo.write(extend_val.toInt());
  server.send(200, "text/plain", extend_val);
}

void handle_button(){

  clawopen = !clawopen;
  clawopen ? ClawServo.write(120) : ClawServo.write(145);
  showData(String(clawopen), "Claw");
  server.send(200, "text/plain", String(clawopen));
}

void setup()
{
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
  server.on("/extendURL", handle_extend);
  server.on("/buttonURL", handle_button);

  server.begin();

  // Servos

  BaseServo.attach(baseServo);
  BaseServo.write(90);

  HeightClaw.attach(heightClaw);
  HeightClaw.write(70);

  ExtendServo.attach(extendServo);
  ExtendServo.write(90);

  ClawServo.attach(clawServo);
  ClawServo.write(145);

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