void toggleLight() {
  if (lightOn) {
    lightOn = false;
    digitalWrite(8, LOW);    // turn the LED off by making the voltage LOW  
  }
  else {
    lightOn = true;
    digitalWrite(8, HIGH); // turn the LED on (HIGH is the voltage level)
  }
}

void flash(long time, int repeat) {
  for (int n=0; n<repeat; n++) {
    toggleLight();
    delay(time);
    toggleLight();
    delay(time);
  }
}

