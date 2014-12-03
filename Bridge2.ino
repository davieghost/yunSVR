/*
  Arduino Yún Bridge example

 This example for the Arduino Yún shows how to use the
 Bridge library to access the digital and analog pins
 on the board through REST calls. It demonstrates how
 you can create your own API when using REST style
 calls through the browser.

 Possible commands created in this shetch:

 * "/arduino/digital/13"     -> digitalRead(13)
 * "/arduino/digital/13/1"   -> digitalWrite(13, HIGH)
 * "/arduino/analog/2/123"   -> analogWrite(2, 123)
 * "/arduino/analog/2"       -> analogRead(2)
 * "/arduino/mode/13/input"  -> pinMode(13, INPUT)
 * "/arduino/mode/13/output" -> pinMode(13, OUTPUT)

 This example code is part of the public domain

 http://arduino.cc/en/Tutorial/Bridge

 */

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

// Listen on default port 5555, the webserver on the Yún
// will forward there all the HTTP requests for us.
YunServer server;

//////////////// MOTOR CONSTANTS ////////////////
       
// Half-bridge enable pins                               
const int EN1=9;
const int EN2=6;  

// Motor 1 control pins
const int MC1=A5;   
const int MC2=A4;    

// Motor 2 control pins
const int MC3=A3;    
const int MC4=A2;     

//////////////////////////////////////////////////

//////////////// MOTOR FUNCTIONS /////////////////

void left_forward(int rate)
{
  Serial.print(F("left forward: ")); Serial.println(rate);

  digitalWrite(EN1, LOW);
  digitalWrite(MC1, HIGH);
  digitalWrite(MC2, LOW);
  analogWrite(EN1, rate);
}

void right_forward(int rate)
{
  Serial.print(F("right forward: ")); Serial.println(rate);

  digitalWrite(EN2, LOW);
  digitalWrite(MC3, HIGH);
  digitalWrite(MC4, LOW);
  analogWrite(EN2, rate);
}

void left_reverse(int rate)
{
  Serial.print(F("left reverse: ")); Serial.println(rate);

  digitalWrite(EN1, LOW);
  digitalWrite(MC1, LOW);
  digitalWrite(MC2, HIGH);
  analogWrite(EN1, rate);
}

void right_reverse(int rate)
{
  Serial.print(F("right reverse: ")); Serial.println(rate);

  digitalWrite(EN2, LOW);
  digitalWrite(MC3, LOW);
  digitalWrite(MC4, HIGH);
  analogWrite(EN2, rate);
}

void left_brake()
{	
  Serial.println(F("left brake"));
  
  digitalWrite(EN1, LOW);
  digitalWrite(MC1, LOW);
  digitalWrite(MC2, LOW);
  analogWrite(EN1, HIGH);
}

void right_brake()
{	
  Serial.println(F("right brake"));

  digitalWrite(EN2, LOW);
  digitalWrite(MC3, LOW);
  digitalWrite(MC4, LOW);
  analogWrite(EN2, HIGH);
}

///////////// END MOTOR FUNCTIONS /////////////////

void setup() {
  // Bridge startup
//  pinMode(13, OUTPUT);
//  digitalWrite(13, LOW);
  Bridge.begin();
//  digitalWrite(13, HIGH);*/
  
  /////////////////////// MOTOR SETUP /////////////////////////
  
  pinMode(EN1, OUTPUT);
  pinMode(MC1, OUTPUT);
  pinMode(MC2, OUTPUT);
  left_brake();

  pinMode(EN2, OUTPUT);
  pinMode(MC3, OUTPUT);
  pinMode(MC4, OUTPUT);
  right_brake();   
  
  ///////////////////// END MOTOR SETUP ///////////////////////  

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
}

void loop() {
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void process(YunClient client) {
  
  // read the command 
  String command = client.readString();
  
  // decode the command
  int left_dir = command.substring(0,1).toInt();
  int left_speed = command.substring(1,4).toInt();
  int right_dir = command.substring(4,5).toInt();
  int right_speed = command.substring(5,8).toInt();
  
  // set the speed of the left motor
  if (left_speed == 0) 
    left_brake();
  else {
    if (left_dir == 0) 
      left_reverse(left_speed);
    else 
      left_forward(left_speed);
   }
    
   // set the speed of the right motor
  if (right_speed == 0) 
    right_brake();
  else {
    if (right_dir == 0)
      right_reverse(right_speed);
    else 
      right_forward(right_speed);
  }
    
//  client.print("Hello"); //command.substring(0,8));  
  client.print('jsonCallback({"message": "hello"})');
}

/*  // is "digital" command?
  if (command == "digital") {
    digitalCommand(client);
  }

  // is "analog" command?
  if (command == "analog") {
    analogCommand(client);
  }

  // is "mode" command?
  if (command == "mode") {
    modeCommand(client);
  }
}

void digitalCommand(YunClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  }
  else {
    value = digitalRead(pin);
  }

  // Send feedback to client
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.println(value);

  // Update datastore key with the current pin value
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));
}

void analogCommand(YunClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/analog/5/120"
  if (client.read() == '/') {
    // Read value and execute command
    value = client.parseInt();
    analogWrite(pin, value);

    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" set to analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  }
  else {
    // Read analog pin
    value = analogRead(pin);

    // Send feedback to client
    client.print(F("Pin A"));
    client.print(pin);
    client.print(F(" reads analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "A";
    key += pin;
    Bridge.put(key, String(value));
  }
}

void modeCommand(YunClient client) {
  int pin;

  // Read pin number
  pin = client.parseInt();

  // If the next character is not a '/' we have a malformed URL
  if (client.read() != '/') {
    client.println(F("error"));
    return;
  }

  String mode = client.readStringUntil('\r');

  if (mode == "input") {
    pinMode(pin, INPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as INPUT!"));
    return;
  }

  if (mode == "output") {
    pinMode(pin, OUTPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as OUTPUT!"));
    return;
  }

  client.print(F("error: invalid mode "));
  client.print(mode);
}*/


