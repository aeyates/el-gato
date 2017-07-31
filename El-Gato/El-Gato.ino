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
FatReader root;   // This holds the information for the volumes root directory
FatReader file;
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

enum state {PURRING, MEOWING, GREETING, GROWLING};
state currentState;
enum lightMode {OFF, DIM, BRIGHT};
lightMode currentLightMode;
int purringCounter = 0;
int greetingCounter = 0;
int growlingCounter = 0;

//////////////////////////////////// SETUP
void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps for debugging  
  Serial.println(F("\nHello Kitty"));  // say we woke up!
  Serial.println(FreeRam());

  SdReader card;
  if (!card.init()) {         //play with 8 MHz spi (default faster!)  
    Serial.println(F("Card init. failed!"));  // Something went wrong, lets print out why
  }
  
  // enable optimize read
  card.partialBlockRead(true);
  
  // This card has a FAT partition on 1
  FatVolume vol;
  vol.init(card, 1); 
  
  // Try to open the root directory
  if (!root.openRoot(vol)) {
    Serial.println(F("Can't open root dir!"));      // Something went wrong,
  }

  // Initialize random seed for a different result each time
  randomSeed(analogRead(3));
  
  // Start by purring
  state currentState = PURRING;

  forcePlay(PSTR("purr1.WAV"));
  // Start with lights off
  pinMode(8, OUTPUT); 
  pinMode(9, OUTPUT);
  switchLight(OFF);
  
  delay(2000); // Let this go for a few seconds before looping

}

//////////////////////////////////// LOOP
void loop() {
 
  int distance = 1000;

  // Meowing has solid dim eyes, other states alternate between dim and off
  if (currentState != MEOWING) {
    if (currentLightMode == OFF) {
      switchLight(DIM);
    } else {
      switchLight(OFF);
    }
  }

  // Is the sound still playing? If not replay.
  if (currentState != GREETING and currentState != GROWLING) {
    playAgainIfComplete();    
  }
  // Is anything in front of me? How far?
  distance = checkApproach();
  if (distance > 230 and currentState != PURRING) {
    // Don't go back to purring right away - increment the counter
    purringCounter++;
    if (purringCounter >= 5) {
      greetingCounter = 0;
      growlingCounter = 0;
      currentState = PURRING;
      switchLight(OFF);
      purringCounter = 0;
      forcePlay(PSTR("purr1.WAV"));
    }
  }
  else if (distance > 72 and distance <= 230 and currentState != MEOWING and currentState != GREETING) {
    purringCounter = 0;
    greetingCounter = 0;
    growlingCounter = 0;
    currentState = MEOWING;
    // Brighten the eyes; begin meowing. Dim eyes after a couple of seconds and leave them that way until state changes
    switchLight(BRIGHT);
    forcePlay(PSTR("meow3.WAV"));
    delay(2000);
    switchLight(DIM);
  } else if (distance < 30) {
    Serial.println(growlingCounter);
    currentState = GROWLING;
    purringCounter = 0;
    greetingCounter = 0;
    switchLight(DIM);
    if (growlingCounter == 0) {
       randomMouth();
       switchLight(BRIGHT);
       playcomplete(PSTR("lion.WAV"));
       growlingCounter++;
    } else {
       if (growlingCounter >= 3) {
          switchLight(BRIGHT);
          playcomplete(PSTR("scat.WAV"));
          growlingCounter = 1;
       } else {
          growlingCounter++;      
       }
    }
  } else if (distance <= 72 and currentState != GREETING) {
    purringCounter = 0;
    growlingCounter = 0;
    currentState = GREETING;
    // Keep the eyes bright while kitty is greeting
    switchLight(BRIGHT);
    Serial.println(F("Greetings...Allow me to introduce myself."));
    playcomplete(PSTR("hello2.WAV"));
  } else if (currentState == GREETING) {
    greetingCounter++;
    if (greetingCounter >= 5) {
      greetingCounter = 0;
      // Play one of the other random greetings
      randomGreeting(distance);
    }
  }

  // Delay 2 seconds before checking again
  delay(2000);

  
}

/////////////////////////////////// HELPERS

// This will pass back the distance of an object in front of the range finder
int checkApproach() {
  int distance = analogRead(0)/2; // used to have to divide by 2 but not anymore for some reason

  Serial.print(F("There is something "));
  Serial.print(distance);
  Serial.print(F(" inches away\n"));
  return distance;
}

void randomGreeting(int distance) {

  if (distance > 72) {
    playcomplete(PSTR("fraidy.WAV"));
  } else {
    int number = random(6);
    switchLight(BRIGHT);
    Serial.println(F("Playing a random greeting."));
    
    if (number == 0) {
      playcomplete(PSTR("skin.WAV"));    
    } else if (number == 1) {
      playcomplete(PSTR("tongue.WAV"));        
    } else if (number == 2) {
      playcomplete(PSTR("pajamas.WAV"));            
    } else if (number == 3) {
      playcomplete(PSTR("catnap.WAV"));            
    } else if (number == 4) {
      playcomplete(PSTR("catsmew.WAV"));            
    } else if (number == 5) {
      playcomplete(PSTR("furry.WAV"));            
    } 
  }
}

void randomMouth() {
    int number = random(5);
    if (number == 0) {
      playcomplete(PSTR("bite.WAV"));
    } else if (number == 1) {
      playcomplete(PSTR("pie.WAV"));                             
    } else if (number == 2) {
      playcomplete(PSTR("chronic.WAV"));            
    } else if (number == 3) {
      playcomplete(PSTR("nine.WAV"));            
    } else if (number == 4) {
      playcomplete(PSTR("cool.WAV"));            
    }
}

