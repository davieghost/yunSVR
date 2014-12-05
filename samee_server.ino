// this server runs on the Yun
// it accepts commands via the url string, and sets the speeds
// of the two motors based on the commands

// this has to be uploaded with Arduino IDE version 1.5.8 (or later) 
// to work on the Yun
// the ino file name has to be the same as the folder that contains it

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

// Listen on default port 5555, the webserver on the Yún
// will forward there all the HTTP requests for us.
YunServer server;
       
// Half-bridge enable pins                               
const int EN1 = 9;
const int EN2 = 6;  

// Motor 1 control pins
const int MC1 = A5;   
const int MC2 = A4;    

// Motor 2 control pins
const int MC3 = A3;    
const int MC4 = A2;     

// SETUP: RUNS ONCE AT THE BEGINNING
void setup() {
  Serial.begin(9600);
  Bridge.begin();
  
  // initialize the motor
  pinMode(EN1, OUTPUT);
  pinMode(MC1, OUTPUT);
  pinMode(MC2, OUTPUT);
  left_brake();

  pinMode(EN2, OUTPUT);
  pinMode(MC3, OUTPUT);
  pinMode(MC4, OUTPUT);
  right_brake();    

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
  
  Serial.println(F("Server is listening."));
}

// LOOP: RUNS CONTINUOUSLY AFTER SETUP IS DONE
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

// process a single command
void process(YunClient client) {
  
  // read the command embedded in the url
  String command = client.readString();
  Serial.println();
  Serial.print(F("New command: "));
  Serial.print(command);
  
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
    
  // send back the command for debugging purposes
  String response = "jsonCallback({'message': '" + command.substring(0,8) + "'})"; 
  client.print(response);
}

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

