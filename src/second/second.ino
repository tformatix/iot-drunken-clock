#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <SPI.h>
#include <deprecated.h>
#include <require_cpp11.h>

#include "arduino_secrets.h"

#define BLYNK_PRINT Serial

#include <BlynkSimpleWiFiShield101.h>
#include <TimeLib.h>
#include <WiFi101.h>
#include <WidgetRTC.h>

#include "drunk_song.h"
#include "sober_song.h"
#include "song_player/song_player.h"

#define SS_PIN 7
#define RST_PIN 6

enum struct UserState {
    Sober = 0,
    Drunk = 1
};

#define ALARM_OFF 0
#define ALARM_ON 1

#define NO_ALARM "TIME_NOT_SET"

#define USER_TAG "B9 2C 5A 63"

constexpr auto BUZZER_PIN = 0;

char auth[] = BLYNK_AUTH_TOKEN;

MFRC522 mfrc522(SS_PIN, RST_PIN);

constexpr size_t melodyLength = sizeof(melody) / sizeof(melody[0]);
constexpr size_t drunkMelodyLength = sizeof(melodyDrunk) / sizeof(melodyDrunk[0]);

Melody soberSongBuffer[melodyLength] = { Melody() };
Melody drunkSongBuffer[drunkMelodyLength] = { Melody() };
auto soberSong = Song::create<melodyLength>(melody, wholenote, soberSongBuffer);
auto drunkSong = Song::create<drunkMelodyLength>(melodyDrunk, wholenoteDrunk, drunkSongBuffer);

auto songPlayer = SongPlayer(BUZZER_PIN);

// real-time clock
BlynkTimer timer;
WidgetRTC rtc;

auto userState = UserState::Sober;
int alarmState = ALARM_ON;
int Buzzer = 0;
String userTag = USER_TAG;

time_t currentTime;
time_t alarmTime;

const int buzzerAlarm = 1;
bool isRinging = false;

void setup() {
    // Initiate a serial communication
    Serial.begin(9600);
    // while (!Serial);

    // Initiate  SPI bus
    SPI.begin();

    // Initiate MFRC522
    mfrc522.PCD_Init();

    // init blynk
    Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD);
    Blynk.syncVirtual(V0, V1, V2);

    Serial.println();

    // Display digital clock every 10 seconds
    timer.setInterval(1000L, changeTime);
}

void loop() {
    Blynk.run();
    timer.run();

    if (isRinging) {
        // check if a rfid tag was scanned
        if (readRFID() == userTag) {
            Serial.println("User deactivated clock");
            Serial.println();
            Blynk.virtualWrite(V0, 0);  // reset alcohol
            songPlayer.stop();    
            isRinging = false;
        } else {
            songPlayer.update();
            Serial.println("unknown TAG :/");
        }
    }
}

BLYNK_WRITE(V0) {
    userState = static_cast<UserState>(param.asInt());
    Serial.println("Drunk set: " + String(param.asInt()));
}

BLYNK_WRITE(V1) {
    // time changed (add  minutes)
    alarmTime = (time_t) param.asInt() + 5*60;
    Serial.println("Alarm set: " + timeToString(alarmTime));
}

BLYNK_WRITE(V2) {
    alarmState = param.asInt();
    Serial.println("Alarm on: " + String(alarmState));
}

BLYNK_CONNECTED() {
    // Synchronize time on connection
    rtc.begin();
}

String timeToString(time_t time) {
    int seconds = numberOfSeconds(time);
    String secondsPad = seconds < 10 ? "0" : "";

    int minutes = numberOfMinutes(time);
    String minutesPad = minutes < 10 ? "0" : "";

    int hours = numberOfHours(time);
    String hoursPad = hours < 10 ? "0" : "";

    return String(hoursPad + hours + ":" + minutesPad + minutes + ":" + secondsPad + seconds);
}

void changeTime() {
    currentTime = now();
    if (!isRinging && timeToString(currentTime) == timeToString(alarmTime) && alarmState == ALARM_ON) {
    // if (!isRinging) {
        isRinging = true;
        // start alarm
        if (userState == UserState::Drunk) {
            songPlayer.setSong(&drunkSong);
        } else {
            songPlayer.setSong(&soberSong);
        }
        songPlayer.start();
    }
}

String readRFID() {
    String tag = "";

    // check if a rfid tag was scanned
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        // read UID (user id) from nfc tag
        Serial.print("read UID tag:");
        String content = "";
        byte letter;

        for (byte i = 0; i < mfrc522.uid.size; i++) {
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX);
            content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
            content.concat(String(mfrc522.uid.uidByte[i], HEX));
        }

        Serial.println();
        Serial.print("Message: ");
        content.toUpperCase();

        tag = content.substring(1);
    }

    return tag;
}

// Digital clock display of the time
String clockDisplay() {
    return String(hour()) + ":" + minute() + ":" + second();
}
