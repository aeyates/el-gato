/*
 * The Cat of Truth
 * -- Purr/glow while waiting for approach
 * -- Meow/flash when approached
 * -- Growl when hit
 * -- Hello when close approach
 */
#include <WaveHC.h>
#include <WaveUtil.h>

// TODO: Holding these globally is expensive. Not sure how else to do it
SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader file;
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time
int sonarPin = 0; //pin connected to analog out on maxsonar sensor

enum state {PURRING, MEOWING, GREETING, HISSING};
state currentState;
boolean lightOn=false;
int i = 0;
int purringCounter = 0;

/*
 * Define macro to put error messages in flash memory
 */
#define error(msg) error_P(PSTR(msg))

//////////////////////////////////// SETUP
void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps for debugging  
  putstring_nl("\nHello Kitty");  // say we woke up!
  putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
  Serial.println(FreeRam());

  // Get a random seed and the led pin for the stuff we're faking
  randomSeed(analogRead(3));

  if (!card.init()) {         //play with 8 MHz spi (default faster!)  
    error("Card init. failed!");  // Something went wrong, lets print out why
  }
  
  // enable optimize read
  card.partialBlockRead(true);
  
  // This card has a FAT partition on 1
  vol.init(card, 1); 
  
  // Try to open the root directory
  if (!root.openRoot(vol)) {
    error("Can't open root dir!");      // Something went wrong,
  }
  
  // Whew! We got past the tough parts.
  putstring_nl("Files found (* = fragmented):");

  // Print out all of the files in all the directories.
  //root.ls(LS_R | LS_FLAG_FRAGMENTED);
  pinMode(8, OUTPUT);

  // Start by purring with lights on and dim
  state currentState = PURRING;

  putstring("Purr...Kitty is waiting for something to happen.");
  Serial.println();
  forcePlay(PSTR("purr1.WAV"));
  toggleLight();
  delay(3000); // Let this go for a few seconds before looping

}

//////////////////////////////////// LOOP
void loop() {
  i++;
  // Don't loop more than 4 times without stopping for input
  if (i%8 == 0) {
    while (Serial.read() >= 0) {}
    if (lightOn) {
      toggleLight();
    }
    putstring_nl("\ntype any character to start");
    while (Serial.read() < 0) {}
  }

  int distance = 1000;
  long hit = false;

  // Is the sound still playing? If not replay.
  playAgainIfComplete();
  // Is anything in front of me? How far?
  distance = checkApproach();
  // Have I been hit? If so, fuck you buddy.
  hit = checkForHit();
  // TODO: Is there a hand in my mouth? Hey, mister, watch yourself.
  // TODO: Hiss should stop if no longer being hit.
  // TODO: Voice should not repeat itself
  if (hit and currentState != HISSING) {
    purringCounter = 0;
    currentState = HISSING;
    putstring("Hiss...Who the fuck do you think you are, buddy?\n");
    forcePlay(PSTR("screech2.WAV"));
    flash(100, 3);
  } 
  else if (distance > 36 and currentState != PURRING) {
    // Don't go back to purring right away - increment the counter
    purringCounter++;
    if (purringCounter >= 5) {
      currentState = PURRING;
      putstring("Purr...I haven't seen anyone in awhile. Back to waiting.\n");
      purringCounter = 0;
      forcePlay(PSTR("purr1.WAV"));
    }
  }
  else if (distance > 10 and distance <= 36 and currentState != MEOWING) {
    purringCounter = 0;
    currentState = MEOWING;
    putstring("Meow...I sensed an approach but you're not too close to me. I'll meow to bring you in.\n");
    forcePlay(PSTR("meow.WAV"));
  } 
  else if (distance <= 10 and currentState != GREETING) {
    purringCounter = 0;
    currentState = GREETING;
    putstring("Greetings...Allow me to introduce myself.\n");
    forcePlay(PSTR("hello.WAV"));
    if (!lightOn) {
      toggleLight();
    }
  }

  // Delay 2 seconds before checking again
  delay(2000);

  
}

/////////////////////////////////// HELPERS
/*
 * print error message and halt
 */
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}
/*
 * print error message and halt if SD I/O error, great for debugging!
 */
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}

// TODO: This will pass back the distance of an object in front of the range finder
int checkApproach() {
  int distance = analogRead(sonarPin) /2; // reads the maxsonar sensor and divides the value by 2

  //long distance = random(25);
  Serial.print("There is something ");
  Serial.print(distance);
  Serial.print(" inches away\n");
  return distance;
}

// TODO: This will check the accelerometer
boolean checkForHit() {
   long hit = random(5);
   if (hit == 0) {
    Serial.println("You just hit me!");
    return true;  
   }
   return false;
}

