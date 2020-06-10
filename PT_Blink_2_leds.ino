#include <pt.h> // ProtoThread Library
#include <clock.h> // Library for the CLOCK_SECOND constant
#include <timer.h> // Libery for the timer

#define LEDPIN1 13 // Definition of the led pin 1

// Struct that keep a protothread instance
static struct pt pts_led; // Instance of protothread for led 1
static struct pt pts_web; // Instance of protothread for led 2

// Timer that will controle the time for the
// protothread instance
static struct timer t1; // Timer for the led 1

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xAA, 0xCA, 0xFE, 0x17, 0xEE, 0xEE };
IPAddress ip(10,8,8,33);

void setup(){
  // Change pinMode of led 1 and 2
  pinMode(LEDPIN1, OUTPUT);
  pinMode(LEDPIN2, OUTPUT);
  
  // Init the instances of protothreads
  PT_INIT(&pt1);
  PT_INIT(&pt2);
  
  // Set the time for each timer
  timer_set(&t1, 0.3145678*CLOCK_SECOND); // 1000ms for the led 1
  timer_set(&t2, 0.13*CLOCK_SECOND); // 900ms for the led 2
  
}

// Declaration of the Protothread
static PT_THREAD(pt_WebServer(struct pt *pt, byte[] *m, IPAddress *ip)){
  // This protothread have three parameters
  // The first one is the instance of protothread
  // The second one is the timer, to check if this timer is expired
  // The third one is the led pin, for the blink functions
  PT_BEGIN(pt); // Begin the protothread
  // Initialize the Ethernet server library
  // with the IP address and port you want to use 
  // (port 80 is default for HTTP):
  EthernetServer server(80);

  void webserver() {
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
      // an http request ends with a blank line
      boolean currentLineIsBlank = true;
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.write(c);
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank) {
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            // add a meta refresh tag, so the browser pulls again every 5 seconds:
            client.println("<meta http-equiv=\"refresh\" content=\"5\">");
            // output the value of each analog input pin
            for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
              int sensorReading = analogRead(analogChannel);
              client.print("analog input ");
              client.print(analogChannel);
              client.print(" is ");
              client.print(sensorReading);
              client.println("<br />");       
            }
            client.println("</html>");
            break;
          }
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } 
          else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      // give the web browser time to receive the data
      delay(1);
      // close the connection:
      client.stop();
    }
  }
    
  PT_END(pt); // End of the protothread
}


// Declaration of the Protothread
static PT_THREAD(pt_blinkLed(struct pt *pt, struct timer *t, char ledpin)){
  // This protothread have three parameters
  // The first one is the instance of protothread
  // The second one is the timer, to check if this timer is expired
  // The third one is the led pin, for the blink functions
  PT_BEGIN(pt); // Begin the protothread
  
  // while loop, like loop() of arduino
  while(1){
    // wait until the timer expire
    PT_WAIT_UNTIL(pt, timer_expired(t));
    // write HIGH to the led pin
    digitalWrite(ledpin, HIGH);
    // reset te timer to expire later
    timer_reset(t);
    // wait again the the timer expire
    PT_WAIT_UNTIL(pt, timer_expired(t));
    // write LOW to the led pin
    digitalWrite(ledpin, LOW);
    // reset the timer and start the loop again
    timer_reset(t);
  }
  
  PT_END(pt); // End of the protothread
}

void loop(){
  // The call of the protothread for led 1
  // loop that the instance is different from the second call
  // the timer is different too, as the led pin
  pt_blinkLed(&pts_led, &t1, LEDPIN1);
  pt_WebServer(&pts_web, &mac, &ip);
  
  // The call of protothread functions should be periodically
  // the system won't call it automatically
}
