
// Switch the lights to the desired mode
void switchLight(lightMode mode) {
  if (mode == OFF) {
    analogWrite(A1, 255);
    analogWrite(A2, 255);
    digitalWrite(8, HIGH);    
    digitalWrite(9, HIGH);    
    currentLightMode = OFF;           
  } else if (mode == DIM) {
    analogWrite(A2, 255);
    analogWrite(A1, 0);     
    digitalWrite(8, LOW);    
    digitalWrite(9, HIGH);    
    currentLightMode = DIM;   
  } else {
    // Bright
    analogWrite(A1, 255);
    analogWrite(A2, 0);    
    digitalWrite(8, HIGH);    
    digitalWrite(9, LOW);    
    currentLightMode = BRIGHT;       
  }  
}

