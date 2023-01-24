# The Drunken Clock
IoT project by Tobias Fischer, Serhat Öner, Florentin Posch, Abdelrahaman Shehata und Michael Zauner at FH Hagenberg.

[![IMAGE_ALT](https://img.youtube.com/vi/XtBdFjt5PR8/0.jpg)](https://www.youtube.com/watch?v=XtBdFjt5PR8)</br>
https://www.youtube.com/watch?v=XtBdFjt5PR8

## Problem
Heavy and all-nighters are often the reason for oversleeping. Oversleeping in lectures is often a big problem, because an absence also means missing the subject matter. Especially in a student's situation, this can have serious disadvantages. Our personal experience with oversleeping and our frustration with a deep sleep caused by a high alcohol consumption gave us the idea to find a definitive solution to this problem.

## Solution
Our project has two alarm clocks. For the first alarm clock, an alarm (time) must be set. This works via the Blynk app. The first alarm clock has an alcohol sensor that tests whether the user is intoxicated. This measures the level of alcohol gases in the air. If he is alcoholized, the melody is changed on both alarm clocks. When the first alarm rings, the user has to press a button to make it stop ringing. 
Five minutes after the first alarm rings, the second alarm goes off. This can only be turned off by holding the chip or card over the scanner. The identification takes place by the serial number of the chip.

## Components
- one Buzzer per alarm clock
- Main Alarm Clock (Arduino MKR1000)
    - Alcohol Sensor (MQ 3)
    - LCD
    - Button
- Second Alarm Clock (Arduino MKR1000)
    - RFID-Modul (RC522)

## Circuit Diagrams
### Main
![IMAGE_ALT](https://raw.githubusercontent.com/tformatix/iot-drunken-clock/main/docs/circuit_diagram/Main_Part_Steckplatine.png)

### Second
![IMAGE_ALT](https://raw.githubusercontent.com/tformatix/iot-drunken-clock/main/docs/circuit_diagram/Second_Part_Steckplatine.png)