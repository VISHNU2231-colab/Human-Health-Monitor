// ======================================================
//              2IN1 HUMAN HEALTH MONITOR
//                 ARDUINO UNO R3
// ======================================================

#define LCD_ADDR 0x3E

#define SDA_PIN A4
#define SCL_PIN A5

#define TEMP_PIN 2
#define PULSE_PIN A0

#define RED_LED 8
#define GREEN_LED 10
#define BUZZER 12


// ======================================================
// TEMPERATURE
// ======================================================

float temperature = 0.0;

float tempReadings[5];

byte tempIndex = 0;
byte tempCount = 0;

unsigned long lastTempRead = 0;


// ======================================================
// PULSE / FINGER
// ======================================================

bool fingerPresent = false;
bool previousFinger = false;

int fingerBase = 0;

unsigned long lastFingerDetected = 0;


// ======================================================
// LCD
// ======================================================

void SDA_HIGH()
{
  pinMode(SDA_PIN, INPUT_PULLUP);
}

void SDA_LOW()
{
  pinMode(SDA_PIN, OUTPUT);
  digitalWrite(SDA_PIN, LOW);
}

void SCL_HIGH()
{
  pinMode(SCL_PIN, INPUT_PULLUP);
}

void SCL_LOW()
{
  pinMode(SCL_PIN, OUTPUT);
  digitalWrite(SCL_PIN, LOW);
}


void i2cStart()
{
  SDA_HIGH();
  SCL_HIGH();

  delayMicroseconds(5);

  SDA_LOW();

  delayMicroseconds(5);

  SCL_LOW();
}


void i2cStop()
{
  SDA_LOW();

  SCL_HIGH();

  delayMicroseconds(5);

  SDA_HIGH();

  delayMicroseconds(5);
}


void i2cWrite(byte data)
{
  for (byte i = 0; i < 8; i++)
  {
    if (data & 0x80)
      SDA_HIGH();
    else
      SDA_LOW();

    SCL_HIGH();

    delayMicroseconds(5);

    SCL_LOW();

    data <<= 1;
  }

  SDA_HIGH();

  SCL_HIGH();

  delayMicroseconds(5);

  SCL_LOW();
}


void lcdCommand(byte cmd)
{
  i2cStart();

  i2cWrite(LCD_ADDR << 1);

  i2cWrite(0x00);

  i2cWrite(cmd);

  i2cStop();

  delay(2);
}


void lcdData(byte data)
{
  i2cStart();

  i2cWrite(LCD_ADDR << 1);

  i2cWrite(0x40);

  i2cWrite(data);

  i2cStop();
}


void lcdPrint(String text)
{
  for (byte i = 0; i < text.length(); i++)
  {
    lcdData(text[i]);
  }
}


void lcdSetCursor(byte col, byte row)
{
  if (row == 0)
    lcdCommand(0x80 + col);
  else
    lcdCommand(0xC0 + col);
}


void lcdClear()
{
  lcdCommand(0x01);
  delay(2);
}


// ======================================================
// DS18B20
// ======================================================

void dsReset()
{
  pinMode(TEMP_PIN, OUTPUT);

  digitalWrite(TEMP_PIN, LOW);

  delayMicroseconds(480);

  pinMode(TEMP_PIN, INPUT_PULLUP);

  delayMicroseconds(70);

  delayMicroseconds(410);
}


void dsWriteBit(byte b)
{
  pinMode(TEMP_PIN, OUTPUT);

  digitalWrite(TEMP_PIN, LOW);

  if (b)
  {
    delayMicroseconds(6);

    pinMode(TEMP_PIN, INPUT_PULLUP);

    delayMicroseconds(64);
  }
  else
  {
    delayMicroseconds(60);

    pinMode(TEMP_PIN, INPUT_PULLUP);

    delayMicroseconds(10);
  }
}


byte dsReadBit()
{
  byte b;

  pinMode(TEMP_PIN, OUTPUT);

  digitalWrite(TEMP_PIN, LOW);

  delayMicroseconds(3);

  pinMode(TEMP_PIN, INPUT_PULLUP);

  delayMicroseconds(10);

  b = digitalRead(TEMP_PIN);

  delayMicroseconds(53);

  return b;
}


void dsWriteByte(byte data)
{
  for (byte i = 0; i < 8; i++)
  {
    dsWriteBit(data & 1);

    data >>= 1;
  }
}


byte dsReadByte()
{
  byte data = 0;

  for (byte i = 0; i < 8; i++)
  {
    data |= (dsReadBit() << i);
  }

  return data;
}


void startTemperature()
{
  dsReset();

  dsWriteByte(0xCC);

  dsWriteByte(0x44);
}


float readTemperature()
{
  dsReset();

  dsWriteByte(0xCC);

  dsWriteByte(0xBE);

  byte lowByte = dsReadByte();

  byte highByte = dsReadByte();

  int16_t raw =
    ((int16_t)highByte << 8) | lowByte;

  return raw / 16.0;
}


// ======================================================
// TEMPERATURE FILTER
// ======================================================

void resetTemperature()
{
  temperature = 0.0;

  tempIndex = 0;

  tempCount = 0;

  for (byte i = 0; i < 5; i++)
  {
    tempReadings[i] = 0;
  }
}


void updateTemperature(float newTemp)
{
  if (newTemp < 20.0 || newTemp > 45.0)
    return;


  tempReadings[tempIndex] = newTemp;

  tempIndex++;

  if (tempIndex >= 5)
    tempIndex = 0;


  if (tempCount < 5)
    tempCount++;


  float total = 0;


  for (byte i = 0; i < tempCount; i++)
  {
    total += tempReadings[i];
  }


  temperature =
    total / tempCount;
}


// ======================================================
// PULSE SENSOR
// ======================================================

float signal = 0;

float baseline = 0;

bool beatDetected = false;

unsigned long lastBeatTime = 0;

unsigned long lastAcceptedBeat = 0;

int bpm = 0;

int bpmValues[5];

byte bpmCount = 0;


void resetPulse()
{
  signal = analogRead(PULSE_PIN);

  baseline = signal;

  beatDetected = false;

  lastBeatTime = 0;

  lastAcceptedBeat = 0;

  bpm = 0;

  bpmCount = 0;


  for (byte i = 0; i < 5; i++)
  {
    bpmValues[i] = 0;
  }
}


void readPulse()
{
  int raw = analogRead(PULSE_PIN);


  signal =
    (signal * 0.90) +
    (raw * 0.10);


  baseline =
    (baseline * 0.995) +
    (signal * 0.005);


  float pulseLevel =
    signal - baseline;


  if (pulseLevel > 35 && !beatDetected)
  {
    unsigned long now = millis();


    if (
      lastAcceptedBeat == 0 ||
      now - lastAcceptedBeat >= 550
    )
    {
      if (lastBeatTime != 0)
      {
        unsigned long interval =
          now - lastBeatTime;


        if (
          interval >= 550 &&
          interval <= 1500
        )
        {
          int newBPM =
            60000 / interval;


          if (
            newBPM >= 40 &&
            newBPM <= 110
          )
          {
            if (bpmCount < 5)
            {
              bpmValues[bpmCount] =
                newBPM;

              bpmCount++;
            }
            else
            {
              for (byte i = 0; i < 4; i++)
              {
                bpmValues[i] =
                  bpmValues[i + 1];
              }

              bpmValues[4] =
                newBPM;
            }


            long total = 0;


            for (byte i = 0; i < bpmCount; i++)
            {
              total += bpmValues[i];
            }


            bpm =
              total / bpmCount;
          }
        }
      }


      lastBeatTime = now;

      lastAcceptedBeat = now;
    }


    beatDetected = true;
  }


  if (pulseLevel < 12)
  {
    beatDetected = false;
  }
}


// ======================================================
// NORMAL OUTPUT
// ======================================================

void normalOutput()
{
  digitalWrite(GREEN_LED, HIGH);

  digitalWrite(RED_LED, LOW);

  digitalWrite(BUZZER, LOW);
}


// ======================================================
// ABNORMAL OUTPUT
// ======================================================

void abnormalOutput()
{
  digitalWrite(GREEN_LED, LOW);

  digitalWrite(RED_LED, HIGH);

  digitalWrite(BUZZER, HIGH);
}


// ======================================================
// CHECK TEMPERATURE
// ======================================================

void checkTemperature()
{
  if (tempCount == 0)
  {
    // No valid reading yet
    normalOutput();
    return;
  }


  if (temperature >= 38.0)
  {
    abnormalOutput();
  }
  else
  {
    normalOutput();
  }
}


// ======================================================
// SETUP
// ======================================================

void setup()
{
  pinMode(SDA_PIN, INPUT_PULLUP);

  pinMode(SCL_PIN, INPUT_PULLUP);

  pinMode(PULSE_PIN, INPUT);


  pinMode(RED_LED, OUTPUT);

  pinMode(GREEN_LED, OUTPUT);

  pinMode(BUZZER, OUTPUT);


  // Start NORMAL state
  // Green ON

  normalOutput();


  // ====================================================
  // PULSE BASELINE
  // ====================================================

  long total = 0;


  for (int i = 0; i < 100; i++)
  {
    total += analogRead(PULSE_PIN);

    delay(5);
  }


  fingerBase =
    total / 100;


  resetTemperature();

  resetPulse();


  // ====================================================
  // LCD INITIALIZATION
  // ====================================================

  delay(50);

  lcdCommand(0x38);
  lcdCommand(0x39);
  lcdCommand(0x14);
  lcdCommand(0x70);
  lcdCommand(0x56);
  lcdCommand(0x6C);

  delay(200);

  lcdCommand(0x38);
  lcdCommand(0x0C);
  lcdCommand(0x01);

  delay(5);


  // ====================================================
  // WELCOME
  // ====================================================

  lcdClear();

  lcdSetCursor(2, 0);

  lcdPrint("2IN1 HEALTH");

  lcdSetCursor(4, 1);

  lcdPrint("MONITOR");

  delay(2500);


  // ====================================================
  // READY
  // ====================================================

  lcdClear();

  lcdSetCursor(3, 0);

  lcdPrint("READY");

  lcdSetCursor(1, 1);

  lcdPrint("PLACE FINGER");

  delay(2000);


  startTemperature();

  lastTempRead = millis();
}


// ======================================================
// LOOP
// ======================================================

void loop()
{
  int rawPulse =
    analogRead(PULSE_PIN);


  // ====================================================
  // FINGER DETECTION
  // ====================================================

  if (abs(rawPulse - fingerBase) > 25)
  {
    fingerPresent = true;

    lastFingerDetected =
      millis();
  }
  else
  {
    if (
      millis() - lastFingerDetected
      > 1500
    )
    {
      fingerPresent = false;
    }
  }


  // ====================================================
  // NEW PERSON
  // ====================================================

  if (
    fingerPresent &&
    !previousFinger
  )
  {
    // Clear old person's temperature

    resetTemperature();

    resetPulse();


    // NORMAL DEFAULT

    normalOutput();


    // Start fresh measurement

    startTemperature();

    lastTempRead =
      millis();
  }


  // ====================================================
  // FINGER PRESENT
  // ====================================================

  if (fingerPresent)
  {
    // Keep normal indicator ON
    // until abnormal temperature is measured

    checkTemperature();


    // Pulse reading

    readPulse();


    // Temperature reading

    if (
      millis() - lastTempRead
      >= 1000
    )
    {
      float newTemp =
        readTemperature();


      updateTemperature(newTemp);


      startTemperature();


      lastTempRead =
        millis();


      // Check latest temperature

      checkTemperature();
    }
  }


  // ====================================================
  // FINGER REMOVED
  // ====================================================

  if (
    !fingerPresent &&
    previousFinger
  )
  {
    // Last measured temperature
    // remains as final value

    checkTemperature();
  }


  // Save state

  previousFinger =
    fingerPresent;


  // ====================================================
  // LCD DISPLAY
  // ====================================================

  static unsigned long displayTime = 0;


  if (
    millis() - displayTime
    >= 500
  )
  {
    lcdClear();


    // ---------------- TEMPERATURE ----------------

    lcdSetCursor(0, 0);

    lcdPrint("T:");


    if (tempCount > 0)
    {
      lcdPrint(
        String(temperature, 1)
      );

      lcdPrint("C");


      float fahrenheit =
        (temperature * 9.0 / 5.0)
        + 32.0;


      lcdPrint(" ");


      lcdPrint(
        String(fahrenheit, 1)
      );

      lcdPrint("F");
    }
    else
    {
      lcdPrint("--.-C ---.-F");
    }


    // ---------------- PULSE ----------------

    lcdSetCursor(0, 1);

    lcdPrint("P:");


    if (
      fingerPresent &&
      bpm > 0
    )
    {
      lcdPrint(
        String(bpm)
      );

      lcdPrint(" BPM");
    }
    else
    {
      lcdPrint("-- BPM");
    }


    displayTime =
      millis();
  }


  delay(10);
}
