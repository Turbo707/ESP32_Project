//---------- ESP32 PIN CONNECTIONS ----------//
/*  

     /-------- NOTE --------/
    #ALL ARE CONNECTED TO 5V

     /---- LCD ----/
    VCC → 5V
    GND → GND
    SDA → 21
    SCL → 22
     
     /---- Keypad Wiring ----/
    Rows    → 13, 12, 14, 27
    Columns → 26, 25, 33, 32

     /---- LEDs + RGB LED ----/
    Single LEDs → THROUGH 220Ω → 5,16,17,18,19
    RGB LED     → THROUGH 220Ω → 23(R),15(G),2(B)

     /---- LDR ----/
    DO → 34

     /---- Passive Buzzer ----/
    + → 4

     /---- WiFi and Time ----/
    # WiFi connects automatically via code (ssid + wifiPass)
    # Time uses NTP (pool.ntp.org) with IST +5:30 (india)

*/

#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <time.h>
#include <EEPROM.h>

/* ---------- LCD ---------- */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* ---------- EEPROM ---------- */
#define EEPROM_SIZE 4
#define HS_ADDR 0

/* ---------- PINS ---------- */
#define LDR_PIN 34
#define BUZZER 4
#define R 23
#define G 15
#define B 2

int leds[5] = {5, 16, 17, 18, 19};

/* ---------- WIFI ---------- */
const char* ssid = "YOUR_WIFI_NAME";
const char* wifiPass = "WIFI_PASSWORD";

/* ---------- KEYPAD ---------- */
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/* ---------- SECURITY ---------- */
String password = "A707";
String entered = "";
bool unlocked = false;

/* ---------- DINO CHARS ---------- */
byte dinoRun1[8] = {B00100,B01110,B00100,B01100,B00100,B00110,B01001,B00000};
byte dinoRun2[8] = {B00100,B01110,B00100,B00110,B00100,B01100,B10010,B00000};
byte dinoJump[8] = {B00100,B01110,B00100,B00100,B00100,B00100,B00100,B00000};

/* ---------- SETUP ---------- */
void setup() {
  lcd.init();
  lcd.backlight();
  EEPROM.begin(EEPROM_SIZE);

  for (int i = 0; i < 5; i++) pinMode(leds[i], OUTPUT);
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LDR_PIN, INPUT);

  lcd.print("Enter Password");
}

/* ---------- LOOP ---------- */
void loop() {
  char key = keypad.getKey();
  if (!key) return;

  if (!unlocked) {
    handlePassword(key);
    return;
  }

  if (key == 'A') showTime();
  if (key == 'B') dinoGame();
  if (key == 'C') showLight();
  if (key == 'D') ledRgbMenu();
  if (key == '#') logout();
}

/* ---------- PASSWORD ---------- */
void handlePassword(char key) {
  if (key == '#') {
    if (entered == password) {
      unlocked = true;
      entered = "";
      lcd.clear();
      lcd.print("Access Granted");
      delay(1000);
      showMenu();
    } else {
      entered = "";
      lcd.clear();
      lcd.print("Wrong Password");
      delay(1500);
      lcd.clear();
      lcd.print("Enter Password");
    }
  } else if (key == '*') {
    entered = "";
    lcd.clear();
    lcd.print("Enter Password");
  } else {
    entered += key;
    lcd.setCursor(0,1);
    lcd.print(entered);
  }
}

/* ---------- MENU ---------- */
void showMenu() {
  lcd.clear();
  lcd.print("A Time B Game");
  lcd.setCursor(0,1);
  lcd.print("C Light D LED");
}

/* ---------- LOGOUT ---------- */
void logout() {
  unlocked = false;
  entered = "";
  lcd.clear();
  lcd.print("Logged Out");
  delay(1500);
  lcd.clear();
  lcd.print("Enter Password");
}

/* ---------- TIME ---------- */
void showTime() {
  lcd.clear();
  lcd.print("Connecting...");
  WiFi.begin(ssid, wifiPass);
  while (WiFi.status() != WL_CONNECTED) delay(300);

  configTime(19800, 0, "pool.ntp.org");

  bool blink = true;
  const char* days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

  while (true) {
    struct tm t;
    if (!getLocalTime(&t)) continue;

    int light = analogRead(LDR_PIN);
    if (light < 1500) lcd.noBacklight();
    else lcd.backlight();

    lcd.clear();

    int hr = t.tm_hour % 12;
    if (hr == 0) hr = 12;

    char c = blink ? ':' : ' ';
    blink = !blink;

    lcd.setCursor(0,0);
    lcd.printf("%02d%c%02d%c%02d %s",
               hr, c, t.tm_min, c, t.tm_sec,
               t.tm_hour >= 12 ? "PM" : "AM");

    lcd.setCursor(0,1);
    lcd.printf("%s %02d/%02d/%04d",
               days[t.tm_wday],
               t.tm_mday, t.tm_mon + 1,
               t.tm_year + 1900);

    delay(500);
    if (keypad.getKey() == '*') break;
  }
  lcd.backlight();
  showMenu();
}

/* ---------- LDR ---------- */
void showLight() {
  while (true) {
    lcd.clear();
    lcd.print("Light:");
    lcd.setCursor(0,1);
    lcd.print(analogRead(LDR_PIN));
    delay(500);
    if (keypad.getKey() == '*') break;
  }
  showMenu();
}

/* ---------- LED + RGB ---------- */
void ledRgbMenu() {
  lcd.clear();
  lcd.print("1-5 LED");
  lcd.setCursor(0,1);
  lcd.print("6R 7G 8B *");

  while (true) {
    char k = keypad.getKey();
    if (!k) continue;

    if (k >= '1' && k <= '5')
      digitalWrite(leds[k - '1'], !digitalRead(leds[k - '1']));

    if (k == '6') digitalWrite(R, !digitalRead(R));
    if (k == '7') digitalWrite(G, !digitalRead(G));
    if (k == '8') digitalWrite(B, !digitalRead(B));
    if (k == '*') break;
  }
  showMenu();
}

/* ---------- DINO GAME ---------- */
void dinoGame() {
  int dinoY = 1, jumpTime = 0;
  int obs1 = 15, obs2 = 22;
  int score = 0, highScore;
  int speedDelay = 250;
  bool frame = false;

  highScore = EEPROM.readInt(HS_ADDR);

  lcd.createChar(0, dinoRun1);
  lcd.createChar(1, dinoRun2);
  lcd.createChar(2, dinoJump);

  lcd.clear();
  lcd.print("DINO GAME");
  delay(1000);

  while (true) {
    lcd.clear();
    char k = keypad.getKey();

    if (k == '#') break;

    if (k == '*' && dinoY == 1) {
      dinoY = 0;
      jumpTime = 3;
      tone(BUZZER, 1000, 80);
    }

    if (jumpTime > 0) jumpTime--;
    else dinoY = 1;

    frame = !frame;

    lcd.setCursor(1, dinoY);
    lcd.write(dinoY == 0 ? byte(2) : (frame ? byte(0) : byte(1)));

    lcd.setCursor(obs1,1); lcd.print("|");
    lcd.setCursor(obs2,1); lcd.print("|");

    if ((obs1 == 1 || obs2 == 1) && dinoY == 1) {
      tone(BUZZER, 200, 400);
      if (score > highScore) {
        EEPROM.writeInt(HS_ADDR, score);
        EEPROM.commit();
      }
      lcd.clear();
      lcd.print("GAME OVER");
      delay(2000);
      break;
    }

    obs1--; obs2--;

    if (obs1 < 0) { obs1 = 15; score++; }
    if (obs2 < 0) obs2 = 22;

    digitalWrite(R, score % 3 == 0);
    digitalWrite(G, score % 3 == 1);
    digitalWrite(B, score % 3 == 2);

    lcd.setCursor(10,0);
    lcd.print(score);

    delay(speedDelay > 80 ? speedDelay-- : 80);
  }
  showMenu();
}
