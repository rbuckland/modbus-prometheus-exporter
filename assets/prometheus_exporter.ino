#include <pt.h> // ProtoThread Library
#include <clock.h> // Library for the CLOCK_SECOND constant
#include <timer.h> // Libery for the timer
#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xAA, 0xCA, 0xFE, 0x17, 0xEE, 0xEE };
IPAddress ip(10,8,8,33);
#define LEDPIN 13 // Definition of the led pin that will blink

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


// Struct that keep a protothread instance
static struct pt led_thread;
static struct pt server_thread;

void setup(){

  Ethernet.begin(mac, ip);
  server.begin();

  // Change pinMode of led to OUTPUT
  pinMode(LEDPIN, OUTPUT);
  // Init the instance of led_thread
  PT_INIT(&led_thread);
  PT_INIT(&server_thread);
  // start the Ethernet connection and the server:
}

// Declaration of the Protothread
static PT_THREAD(protothread1(struct pt *pt)){
  static struct timer t; // Struct that will control the time
                         // of the protothread
                         // The static keyword is necessary to
                         // don't lose the data after the thread
                         // is yield
  PT_BEGIN(pt); // The begin of the protothread
                // All variables won't be saved if it isn't static
  
  // while loop, like the loop() of arduino
  while(1){
    webserver();
  }
  
  PT_END(pt); // The end of the protothread
}


// Declaration of the Protothread
static PT_THREAD(protothread2(struct pt *pt)){
  static struct timer t; // Struct that will control the time
                         // of the protothread
                         // The static keyword is necessary to
                         // don't lose the data after the thread
                         // is yield
  PT_BEGIN(pt); // The begin of the protothread
                // All variables won't be saved if it isn't static
  
  // Set the timer to 1 second
  timer_set(&t, 0.3*CLOCK_SECOND);
  
  // while loop, like the loop() of arduino
  while(1){
    // If the timer is expired, then continue the function
    // otherwhise it'll pass the processing to another thread
    PT_WAIT_UNTIL(pt, timer_expired(&t));
    // write HIGH to the led pin
    digitalWrite(LEDPIN, HIGH);
    // reset the timer, to 1 second after the last expired time
    timer_reset(&t);
    // Wait again for the expire of the timer
    PT_WAIT_UNTIL(pt, timer_expired(&t));
    // write LOW to the led pin
    digitalWrite(LEDPIN, LOW);
    // reset the timer and start the loop again
    timer_reset(&t);
  }
  
  PT_END(pt); // The end of the protothread
}

void loop(){
  protothread1(&led_thread); // Call of the protothread is
                      // necessary to make this call
                      // periodically, the system won't
                      // call it automatically
  
  protothread1(&server_thread); // Call of the protothread is
  // All protothreads should be called were
  // Any delay function or function call that keep the
  // process time for a while may disturb the protothreads
  // functionality
}
