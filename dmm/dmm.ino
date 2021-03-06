// Arduino Pro Mini code to control DIGITAL MODUL M:
// a custom digital back for film Leica M bodies based on Canon EOS 350D
#include <Bounce.h>
#include <EEPROM.h>

// we use a 4 position selector knob, mapped to 4 inputs, to select camera ISO
#define ISO_100  0
#define ISO_200  1
#define ISO_400  2
#define ISO_800  3

// TODO: playback and zoom-in 100% while this button is held
#define IN_FAP 0

#define IN_SHUTTER 1
#define BTN_SHUTTER 3
#define BTN_PRERELEASE A4
#define SW_MYSTERY 2

// we drive the amputated 350D buttons from output pins
#define BTN_ISO 8
#define BTN_UP  8
#define BTN_DN  10
#define BTN_SET 9
#define BTN_PLAY 11
#define BTN_ZOOM_IN 12
#define SW_CANON_PWR 12

// we simulate the shutter curtain and mirror position sensors for the Canon
#define SW_SHUTTER_CURTAIN1 A0
#define SW_SHUTTER_CURTAIN2 A1
#define SW_MIRROR_UP A2
#define SW_MIRROR_DOWN A3

#define SW_POWER A5

#define ISO_ADDR 0

#define out_on(pin_no) digitalWrite(pin_no, HIGH);
#define out_off(pin_no) digitalWrite(pin_no, LOW);

int isopos;

int buttons[4]={
  7, 6, 5, 4};
//Bounce *buttons_deb[4];

//Bounce button_fap = Bounce(IN_FAP, 15);

// blink the led once
void blink() {
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
}

// simulate a keypress action
void click(int pin) {
  digitalWrite(pin, HIGH);
  delay(20);
  digitalWrite(pin, LOW);
  delay(20);
}

//zoom in the image to 100% center
void pixelpeep() {
  click(BTN_PLAY);
  click(BTN_ZOOM_IN);
  click(BTN_ZOOM_IN);
  click(BTN_ZOOM_IN);
  click(BTN_ZOOM_IN);
  click(BTN_ZOOM_IN);
  click(BTN_ZOOM_IN);
  click(BTN_ZOOM_IN);
  click(BTN_ZOOM_IN);
}

int getRotaryPos() {
  int i, pos = 0
  ;
  for(i = 7; i > 3; i--) {
    if(digitalRead(i) == LOW)
      pos = i - 3;
  }
  if (i < 0)
    i = 4;
  
  return pos;
}

// ISO selection button sequence
void iso_seq(int pos) {
  int dist, dir;
  click(BTN_ISO);

  dist = pos - isopos;
  dir = (dist >= 0 ? 1 : -1);
  // drive the cursor to required position
  for(; pos != isopos; isopos += dir) {
    click(dir > 0 ? BTN_DN : BTN_UP);
  }
  click(BTN_SET);
  
  EEPROM.write(ISO_ADDR, isopos);
}

// camera firing sequence
void shoot() {

  out_on(BTN_SHUTTER);
  delay(8); // minimum trigger time
  out_off(SW_MIRROR_DOWN);
  delay(24);
  //Set mirror up and fire the 1st curtain
  out_on(SW_MIRROR_UP);
  out_off(BTN_SHUTTER);
  delay(33);
  out_off(SW_SHUTTER_CURTAIN1);
  out_off(BTN_PRERELEASE);
  digitalWrite(13, HIGH); //LED on
  delay(495); // Canon's shutter is set to 1/2 sec
  out_off(SW_SHUTTER_CURTAIN2);
  delay(35);
  out_off(SW_MIRROR_UP);
  delay(33);
  out_on(SW_SHUTTER_CURTAIN1);
  delay(5);
  out_on(SW_SHUTTER_CURTAIN2);
  delay(28);
  out_on(SW_MYSTERY);
  delay(40);
  out_off(SW_MYSTERY);
  delay(1);
  out_on(SW_MIRROR_DOWN);

  digitalWrite(13, LOW); //LED off

}

void setup() {
  int i;
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  EEPROM.write(ISO_ADDR, 0);

  isopos = EEPROM.read(ISO_ADDR);

  // shutter release button
  pinMode(IN_SHUTTER, INPUT_PULLUP);

  // camera circuit power
  out_off(SW_CANON_PWR);
  pinMode(SW_CANON_PWR, OUTPUT);
  out_off(SW_POWER);
  pinMode(SW_POWER, OUTPUT);

  // fap button
  pinMode(IN_FAP, INPUT_PULLUP);
  
  // set up the inputs from ISO knob
  for(i = 0; i < 4; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
    //buttons_deb[i] = new Bounce(buttons[i], 32);
  }

  out_off(BTN_PRERELEASE);
  out_off(BTN_SHUTTER);
  pinMode(BTN_SHUTTER, OUTPUT);
  pinMode(BTN_PRERELEASE, OUTPUT);
  pinMode(SW_MYSTERY, OUTPUT);
  pinMode(BTN_ISO, OUTPUT);
  pinMode(BTN_UP, OUTPUT);
  pinMode(BTN_DN, OUTPUT);
  pinMode(BTN_SET, OUTPUT);

  out_on(SW_SHUTTER_CURTAIN1);
  out_on(SW_SHUTTER_CURTAIN2);
  pinMode(SW_SHUTTER_CURTAIN1, OUTPUT);
  pinMode(SW_SHUTTER_CURTAIN2, OUTPUT);
  out_on(SW_MIRROR_DOWN);
  out_off(SW_MIRROR_UP);
  pinMode(SW_MIRROR_UP, OUTPUT);
  pinMode(SW_MIRROR_DOWN, OUTPUT);

  // delay to let the camera warm up
  out_on(SW_CANON_PWR);
  out_on(SW_POWER);
  delay(250);
  out_off(SW_MYSTERY);

  delay(2000); 
  out_on(BTN_PRERELEASE);
}

void loop() {
  int new_iso;
  
  // fire the shutter if necessary
  if(digitalRead(IN_SHUTTER) == LOW) {
    shoot();
    delay(500);
    out_on(BTN_PRERELEASE);

  }

  new_iso = getRotaryPos();
  if ( new_iso != isopos )
    if( new_iso == 4 && isopos != 3 )
      delay(100);
    else
       iso_seq(new_iso);
       
}

  
