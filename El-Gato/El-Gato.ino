/*
 * The Cat of Truth
 * -- Purr/glow while waiting for approach
 * -- Meow/flash when approached
 * -- Growl when hit
 * -- Hello when close approach
 */
#include <WaveHC.h>
#include <WaveUtil.h>

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader file;   // This object representing the WAV file being played
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time
int sonarPin = 0; //pin connected to analog out on maxsonar sensor

enum state {PURRING, MEOWING, GROWLING, HISSING};
state currentState;
char filename[13];
int i = 0;

/*
 * Define macro to put error messages in flash memory
 */
#define error(msg) error_P(PSTR(msg))

// Function definitions (we define them here, but the code is below)
void play(FatReader &dir);

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
  // Tell the user what we found
  putstring(", type is FAT");
  Serial.println(vol.fatType(), DEC);     // FAT16 or FAT32?
  
  // Try to open the root directory
  if (!root.openRoot(vol)) {
    error("Can't open root dir!");      // Something went wrong,
  }
  
  // Whew! We got past the tough parts.
  putstring_nl("Files found (* = fragmented):");

  // Print out all of the files in all the directories.
  //root.ls(LS_R | LS_FLAG_FRAGMENTED);

  // Start by purring with lights on and dim
  state currentState = PURRING;

  putstring("Purr...Kitty is waiting for something to happen.");
  Serial.println();
  strcpy_P(filename, PSTR("purr1.WAV"));
  playfile(filename);
  softGlow();
  delay(3000); // Let this go for a few seconds before looping

}

//////////////////////////////////// LOOP
void loop() {
  i++;
  // Don't loop more than 4 times without stopping for input
  if (i%4 == 0) {
    while (Serial.read() >= 0) {}
    putstring_nl("\ntype any character to start");
    while (Serial.read() < 0) {}
  }

  int distance = 1000;
  long hit = false;

  // Check sensors
  do {
    // Is the sound still playing? If not replay.
    playIfComplete(filename);
    // Is anything in front of me? How far?
    distance = checkApproach();
    // Have I been hit? If so, fuck you buddy.
    hit = checkForHit();
    // Is there a hand in my mouth? Hey, mister, watch yourself.
    // Delay 2 seconds before checking again
    delay(2000);
  } while (distance > 60 and !hit);

  if (hit and currentState != HISSING) {
    currentState = HISSING;
    putstring("Hiss...Who the fuck do you think you are, buddy?\n");
    strcpy_P(filename, PSTR("hiss1.WAV"));
    playfile(filename);
    flash(200);
  }
  else if (distance > 10 and currentState != MEOWING) {
    currentState = MEOWING;
    putstring("Meow...I sensed an approach but you're not too close to me. I'll meow to bring you in.\n");
    strcpy_P(filename, PSTR("meow2.WAV"));
    playfile(filename);
    flash(2000);
  } 
  else if (distance <= 10 and currentState != GROWLING) {
    currentState = GROWLING;
    putstring("Growl...You're awfully close, buddy.\n");
    strcpy_P(filename, PSTR("growl.WAV"));
    playfile(filename);
    flash(1000);
  }
  
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
   long hit = random(8);
   if (hit == 0) {
    Serial.println("You just hit me!");
    return true;  
   }
   return false;
}

