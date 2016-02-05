

/*****************
 * ATEM 1 ME Uplink Control 0.2
 * Kontrolliert das ATEM Television Studio per PS2-Tastatur (at 9600 baud)
 *
 * by Achim Barczok
 */
/*****************
 * ursprünglich für das ATEM Television Studio entwickelt
 * ab 0.3 für ATEM 1 M/E
 */



// Including libraries: 
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>

//#include <MemoryFree.h>
#include <Streaming.h>


// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0E, 0xC8, 0x27 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(10, 22, 38, 85);              // <= SETUP!  IP address of the Arduino


// Include ATEM library and make an instance:
// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.
#include <ATEM.h>
ATEM AtemSwitcher(IPAddress(10, 22, 38, 192), 5641);  // <= SETUP (the IP address of the ATEM switcher)

// Include PS2-Library, setze PINs für Tastatur; im Studio bitte auf 8 und 3 lassen.

#include <PS2Keyboard.h>

const int DataPin = 8;
const int IRQpin =  3;

PS2Keyboard keyboard;

  int i = 1;

void setup() { 

 
  // Keyboard einstellen
  delay(1000);
  keyboard.begin(DataPin, IRQpin);
  Serial.begin(9600);
  Serial.println("Keyboard Test:");  
  
  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(9600);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  
  
  // give the Ethernet shield a second to initialize:
  delay(1000);
  
  // Initialize a connection to the switcher:
  AtemSwitcher.serialOutput(true);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher.connect();
  
  // give the Ethernet shield a second to initialize:
  delay(1000);

//  SmartView.begin(IPAddress(10, 22, 38, 194));	 // <= SETUP (the IP address of the SmartView)
//  SmartView.connect();
//  SmartView.serialOutput(true);
// }

  // Shows free memory:  
//  Serial << F("freeMemory()=") << freeMemory() << "\n";

    
}

void loop() {

  // Tastenbelegung
  // Stills 1-9 in Mediaplayer 1 laden
  char mp1Src[] = {'z','x','c','v','b','n','m',','};  
  
  // Camera Program 1-10
  char proGo[] = {'1','2','3','4','5','6','7','8','9','0'};
  
  // Camera Preview 1-10
  char preGo[] = {'q','w','e','r','t','y','u','i','o','p'};

  // Upstream Key 1 (Bild-in-Bild) Auswahl
  char up1Src[] = {'a','s','d','f','g','h','j','k','l',';','\'','\\'}; 
  
  // Program Cut + Auto
  char proCut = PS2_BACKSPACE;
  char proAuto = PS2_ENTER;

  // DownstreamKey1 Buttons
  // char dsk1Tie = PS2_LCTRL;
  // char dsk1On = PS2_LALT;
  char dsk1Auto = ' ';



  // Transition Keys für Upstream und Preview

  char up1On = PS2_HOME;
  char up2On = PS2_PAGEUP;
  char bkgPre = PS2_DELETE;
  char up1Pre = PS2_END;
  char up2Pre = PS2_PAGEDOWN;

  // Clip 1 in Media-Player 1 einfügen Clip rein, Play und Donwstream1 Key on Air
  char introOn = '\/';     

  
  // Check for packets, respond to them etc. Keeping the connection alive!
  // VERY important that this function is called all the time - otherwise connection might be lost because packets from the switcher is
  // overlooked and not responded to.
    AtemSwitcher.runLoop();
    
    

  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
    Serial << F("Connection to ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher.connect();
  }  

  // If you fancy to make delays in your sketches, ALWAYS do it using the AtemSwitcher delay function - this will wait while calling ru
  // Loop() checking for packets and thus keeping the connection up.
    AtemSwitcher.delay(0);


  // If you monitor the serial output, you should see a lot of "ACK, rpID: xxxx" and then every 5 seconds this message:
  // Serial << F("End of normal loop() - still kicking?\n");

  // Ist das Keyboard angeschlossen?
  if (keyboard.available()) {
  
    // Key einlesen!
    // Serial.println("Keyboard found");
    char c = keyboard.read();
    Serial.print(c);
    
    bool checkStatus = 0;    

    // Tastatur auslesen und interpretieren


    int i;
    int j;
    boolean belegt = false;

    for (i = 0; i<9; i = i + 1) {
      if (c==mp1Src[i]) {
        Serial.print("Src");
        Serial.print(i+1);
        Serial.print(" in MP1");
        AtemSwitcher.mediaPlayerSelectSource(1,false,i+1);      
        belegt = true;
      }
    }

    for (i = 0; i<10; i = i + 1) {
      if (c==proGo[i]) {
        Serial.print("Program = ");
        Serial.print(i+1);
        AtemSwitcher.changeProgramInput(i+1); 
        belegt = true;
      }
    }


    for (i = 0; i<10; i = i + 1) {
      if (c==preGo[i]) {
        Serial.print("Preview = ");
        Serial.print(i+1);
        AtemSwitcher.changePreviewInput(i+1); 
        belegt = true;
      }
    }

    for (i = 0; i<12; i = i + 1) {
      if (c==up1Src[i]) {
        Serial.print("Bild in Bild Src = ");
        Serial.print(i+1);
        AtemSwitcher.changeUpstreamKeyFillSource(1,i+1); 
        belegt = true;
      }
    }
    
   if (c==dsk1Auto)
       { 
       Serial.print("DownstreamKeyer1 Auto");
       AtemSwitcher.doAutoDownstreamKeyer(1);
       belegt = true;
       }
   else if (c==proCut)
        { 
        Serial.print("Cut");
        AtemSwitcher.doCut();
        belegt = true;
        }
    else if (c==proAuto)
        { 
        Serial.print("Auto");
        AtemSwitcher.doAuto(); 
        belegt = true;
        } 
    else if (c==up1On)
        { 
        Serial.print("Key1 geswitched");
        belegt = true;
        if (AtemSwitcher.getUpstreamKeyerStatus(1)== true)
           {AtemSwitcher.changeUpstreamKeyOn(1,0);} else
           {AtemSwitcher.changeUpstreamKeyOn(1,1);}
        } 
    else if (c==up2On)
        { 
        Serial.print("Key2 geswitched");
        belegt = true;
        if (AtemSwitcher.getUpstreamKeyerStatus(2)== true)
           {AtemSwitcher.changeUpstreamKeyOn(2,0);} else
           {AtemSwitcher.changeUpstreamKeyOn(2,1);}
        }
    else if (c==bkgPre)
        { 
        Serial.print("Bkg Preview geswitched");
        belegt = true;
        if (AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(0)== true)
           {AtemSwitcher.changeUpstreamKeyNextTransition(0,0);} else
           {AtemSwitcher.changeUpstreamKeyNextTransition(0,1);}       
        }  
    else if (c==up1Pre)
        { 
        Serial.print("key1 Preview geswitched");
        belegt = true;
        if (AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1)== true)
           {AtemSwitcher.changeUpstreamKeyNextTransition(1,0);} else
           {AtemSwitcher.changeUpstreamKeyNextTransition(1,1);}       
        }   
    else if (c==up2Pre)
        { 
        Serial.print("key2 Preview geswitched");
        belegt = true;
        if (AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(2)== true)
           {AtemSwitcher.changeUpstreamKeyNextTransition(2,0);} else
           {AtemSwitcher.changeUpstreamKeyNextTransition(2,1);}       
        }  
    else if (c==introOn)
        {
          Serial.print("Medien-Intro gestartet");
          belegt = true;
          AtemSwitcher.mediaPlayerSelectSource(1,true,1);
          AtemSwitcher.mediaPlayerClipStart(1);  
          AtemSwitcher.changeDownstreamKeyOn(1,true);    
        }    

    if (belegt == false) {
      Serial.print("Diese Taste ist nicht belegt: ");    
      Serial.print(c);  
    }

  }

}

