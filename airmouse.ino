#include <Wire.h>
#include <BleMouse.h>
#include <MPU6050_tockn.h>

#define LEFTBUTTON 4
#define RIGHTBUTTON 5

#define SDA_PIN 8
#define SCL_PIN 9

#define SPEED 1

BleMouse bleMouse;
MPU6050 mpu(Wire);

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);

  pinMode(LEFTBUTTON, INPUT_PULLUP);
  pinMode(RIGHTBUTTON, INPUT_PULLUP);

  bleMouse.begin();

  Serial.println("Calibrating MPU...");
  mpu.begin();

  Serial.println("Keep sensor still...");
  delay(1000);

  mpu.calcGyroOffsets(true);

  Serial.println("Ready!");
}

void loop() {
  mpu.update();

  if (bleMouse.isConnected()) {

    Serial.print("L=");
    Serial.print(digitalRead(LEFTBUTTON));
    Serial.print(" R=");
    Serial.println(digitalRead(RIGHTBUTTON));

    float xRaw = -mpu.getGyroZ();
    float yRaw = -mpu.getGyroX();

    // scale down movement
    int xMove = xRaw / 4;
    int yMove = -yRaw / 4;

    // dead-zone
    if (abs(xMove) < 3) xMove = 0;
    if (abs(yMove) < 3) yMove = 0;

    bleMouse.move(xMove, yMove);

    static bool leftPressed = false;
    static unsigned long lastClickTime = 0;
    static bool dragging = false;

if (!digitalRead(LEFTBUTTON)) {

  if (!leftPressed) {
    leftPressed = true;

    unsigned long currentTime = millis();

    // Double-click if pressed again within 300ms
    if (currentTime - lastClickTime < 300) {
      bleMouse.click(MOUSE_LEFT);
      delay(100);
      bleMouse.click(MOUSE_LEFT);
      lastClickTime = 0;
    }
    else {
      lastClickTime = currentTime;

      // begin drag
      bleMouse.press(MOUSE_LEFT);
      dragging = true;
    }
  }
}
else {

  if (leftPressed) {
    leftPressed = false;

    if (dragging) {
      bleMouse.release(MOUSE_LEFT);
      dragging = false;
    }
  }
}

    // Right button = right click
static bool rightPressed = false;

if (!digitalRead(RIGHTBUTTON)) {
  if (!rightPressed) {

    bleMouse.click(MOUSE_RIGHT);

    rightPressed = true;
  }
}
else {
  rightPressed = false;
}

  }

  delay(10);
}
