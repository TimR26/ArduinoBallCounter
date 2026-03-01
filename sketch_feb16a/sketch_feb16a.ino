int switchState = 0;
int switchState2 = 0;
int running = false;
int running2 = false;
int count = 0;
int currentDigit = 0;

int ShouldRest = 0;

int hasStarted = 0;
int isStarting = 0;

int32_t recordedTime = 0;

int canScore = 1;

int continuousScore = 0;

int latch = 0;

const uint8_t SEG_MASKS_0_TO_9_INV[10] = {
  0b11000000,  // 0
  0b11111001,  // 1
  0b10100100,  // 2
  0b10110000,  // 3
  0b10011001,  // 4
  0b10010010,  // 5
  0b10000010,  // 6
  0b11111000,  // 7
  0b10000000,  // 8
  0b10010000   // 9
};


void setup() {
  // put your setup code here, to run once:
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(2, INPUT);

  pinMode(12, INPUT);

  pinMode(7, OUTPUT);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);


  Serial.begin(2000000);
}

void loop() {
  switchState = digitalRead(2);
  switchState2 = digitalRead(3);
  ShouldRest = digitalRead(12);

  if (hasStarted == 0) {
    if (count > 5) {
      digitalWrite(7, 1);
      recordedTime = millis();
      count = 0;
      hasStarted = 1;
    }
    if (isStarting == 1) {
      count = (millis() - recordedTime) / 1000;
      // Serial.println(count);
    }
    if ((switchState == 0 || switchState2 == 0) && isStarting == 0) {

      isStarting = 1;
      digitalWrite(7, 1);
      delay(500);
      digitalWrite(7, 0);
      recordedTime = millis();
      delay(1);
      Serial.println(recordedTime);
    }

    if (ShouldRest == 1 && latch == 0) {
      if(continuousScore == 1) {
        continuousScore = 0;
        digitalWrite(7, 0);
      } else {
        continuousScore = 1;
        digitalWrite(7, 1);
      }
      latch = 1;
    } 
    else if (ShouldRest == 0){
      latch = 0;
    }

    // Serial.println(String(count) + " Test " + String(millis() - recordedTime));

  } else {

    if (continuousScore == 1) {
      if (switchState == 0) {
        if (running == false) {
          count = count + 1;
          running = true;
        }
      } else {
        running = false;
      }

      if (switchState2 == 0) {
        if (running2 == false) {
          count = count + 1;
          running2 = true;
        }
      } else {
        running2 = false;
      }
    } else {

      if ((((millis() - recordedTime) / 1000) % 10) < 5) {
        if (switchState == 0) {
          if (running == false) {
            count = count + 1;
            running = true;
          }
        } else {
          running = false;
        }

        if (switchState2 == 0) {
          if (running2 == false) {
            count = count + 1;
            running2 = true;
          }
        } else {
          running2 = false;
        }
        digitalWrite(7, 1);
      } else {
        digitalWrite(7, 0);
      }
    }



    if (ShouldRest == 1) {
      digitalWrite(7, 0);
      switchState = 0;
      switchState2 = 0;
      running = false;
      running2 = false;
      count = 0;

      ShouldRest = 0;

      hasStarted = 0;
      isStarting = 0;

      recordedTime = 0;

      canScore = 1;
      continuousScore = 0;
      while (digitalRead(12) == 1) {}
    }
  }



  if (count < 0) count = 0;
  if (count > 999) count = 999;

  uint8_t d0 = count % 10;
  uint8_t d1 = (count / 10) % 10;
  uint8_t d2 = (count / 100) % 10;
  uint8_t digits[3] = { d0, d1, d2 };


  // --- digit select mask (active LOW for PNP) ---
  // uint8_t digitMask = 0b00000000;   // all off
  uint8_t digitMask = (1 << (currentDigit));  // turn one digit on
  // --- segment data ---
  uint8_t segData = SEG_MASKS_0_TO_9_INV[digits[currentDigit]];
  // --- combine into 16 bits ---
  uint16_t combined =
    ((uint16_t)digitMask << 8) | segData;
  shiftOut16(combined, 8, 10, 9);
  // move to next digit
  currentDigit++;
  if (currentDigit >= 3) currentDigit = 0;



  delay(2);
}


void shiftOut16(uint16_t data16, int dataPin, int clockPin, int latchPin) {
  digitalWrite(latchPin, LOW);

  // Send high byte first (goes to second 595)
  shiftOut(dataPin, clockPin, MSBFIRST, (data16 >> 8) & 0xFF);

  // Send low byte (goes to first 595)
  shiftOut(dataPin, clockPin, MSBFIRST, data16 & 0xFF);

  digitalWrite(latchPin, HIGH);
}
