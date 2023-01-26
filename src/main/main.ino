#define BLYNK_PRINT Serial

#define DRUNK_THRESHOLD 850

#include "arduino_secrets.h"
#include <SPI.h>
#include <WiFi101.h>
#include <BlynkSimpleWiFiShield101.h>
#include <LiquidCrystal.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

#include "drunk_song.h"
#include "sober_song.h"
#include "song_player/song_player.h"

char auth[] = BLYNK_AUTH_TOKEN;
LiquidCrystal lcd(12, 11, 2, 3, 4, 5);

constexpr size_t melodyLength = sizeof(melody) / sizeof(melody[0]);
constexpr size_t drunkMelodyLength = sizeof(melodyDrunk) / sizeof(melodyDrunk[0]);

Melody soberSongBuffer[melodyLength] = { Melody() };
Melody drunkSongBuffer[drunkMelodyLength] = { Melody() };
auto soberSong = Song::create<melodyLength>(melody, wholenote, soberSongBuffer);
auto drunkSong = Song::create<drunkMelodyLength>(melodyDrunk, wholenoteDrunk, drunkSongBuffer);

auto songPlayer = SongPlayer(1);

BlynkTimer timer;
WidgetRTC rtc;

String currentTime;
String alarmTime;

bool isDrunk = false;
bool isActivated = false;
bool isRinging = false;

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

BLYNK_WRITE(V0)
{
  // drunk changed
  isDrunk = param.asInt() > 0;
  Serial.println("isDrunk: " + String(isDrunk));

  lcd.setCursor(9, 0);
  lcd.print(isDrunk ? "(drunk)" : "       ");
}

BLYNK_WRITE(V1)
{
  // time changed
  Blynk.virtualWrite(V0, 0); // reset alcohol

  lcd.setCursor(0, 1);
  alarmTime = timeToString((time_t) param.asInt());
  Serial.println("alarmTime: " + alarmTime);

  lcd.print(alarmTime);
}

BLYNK_WRITE(V2)
{
  // activated changed
  isActivated = param.asInt() > 0;
  Serial.println("isActivated: " + String(isActivated));

  lcd.setCursor(11, 1);
  lcd.print(isActivated ? " (on)" : "(off)");
}

void setup() {
  // Debug console
  Serial.begin(9600);
  // while (!Serial) {}

  Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD);
  Blynk.syncVirtual(V0, V1, V2);

  pinMode(0, INPUT); // button

  analogWrite(A3, LOW);  // brightness of LCD to max
  lcd.begin(16, 2);      // set up the LCD's number of columns and rows

  // Display digital clock every 10 seconds
  timer.setInterval(1000L, changeTime);
}

void loop() {
  Blynk.run();
  timer.run();

  int alcohol = analogRead(A1);
  if(isActivated && alcohol > DRUNK_THRESHOLD) {
    Serial.println("alcohol detected");
    Blynk.virtualWrite(V0, 1); // set drunk

    lcd.setCursor(9, 0);
    lcd.print("(drunk)");
  }

  if(isRinging){
    // read the state of the pushbutton value
    byte buttonState = digitalRead(0);
    if (buttonState == LOW) {
      // turn off ringing
      songPlayer.stop();    
      isRinging = false;
    } else {
      songPlayer.update();
    }
  }
}

// Digital clock display of the time
void changeTime() {
  lcd.setCursor(0, 0);  // line 1 is the second row (counting begins with 0)
  currentTime = timeToString(now());

  if(isActivated && currentTime == alarmTime)  {
    // alarm should ring
    Serial.println("alarm");
    isRinging = true;
    if(isDrunk) {
      // drunk melody
      songPlayer.setSong(&drunkSong);
    } else {
      // normal melody
      songPlayer.setSong(&soberSong);
    }
    songPlayer.start();
  }
  
  lcd.print(currentTime);  // send message to LCD
}

String timeToString(time_t time){
  int seconds = numberOfSeconds(time);
  String secondsPad = seconds < 10 ? "0" : "";
  int minutes = numberOfMinutes(time);
  String minutesPad = minutes < 10 ? "0" : "";
  int hours = numberOfHours(time);
  String hoursPad = hours < 10 ? "0" : "";

  return String(hoursPad + hours + ":" + minutesPad + minutes + ":" + secondsPad + seconds);
}