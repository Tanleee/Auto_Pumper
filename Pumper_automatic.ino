#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>
#define trig_Pin 12
#define echo_Pin 11
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int travel_time;
//high/depth of container(tank)
float high = 0;
//high of water at this time
float current;
//distace from sensor to water surface
float distance = 0;
//cm/s
float Velocity = 0;
#define Red_Pin 7
#define Blue_Pin 9
#define Green_Pin 8
#define Pumper_Pin 6
#define Sele_but 5
#define Exit_but 4
#define Buzzer_Pin 2
#define Read_Pin A3  //read from Potentiometer
void Deter_dis();
int dt = 1;  //time measure velocity
void setup() {
  // put your setup code here, to run once:
  pinMode(trig_Pin, OUTPUT);
  pinMode(echo_Pin, INPUT);
  pinMode(Red_Pin, OUTPUT);
  pinMode(Blue_Pin, OUTPUT);
  pinMode(Green_Pin, OUTPUT);
  pinMode(Pumper_Pin, OUTPUT);
  pinMode(Sele_but, INPUT);
  pinMode(Exit_but, INPUT);
  digitalWrite(Sele_but, HIGH);
  digitalWrite(Exit_but, HIGH);
  pinMode(Buzzer_Pin, OUTPUT);
  pinMode(Read_Pin, INPUT);
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("SSD1036 fail!");
    while (1) {}
  }
  display.clearDisplay();
  display.setTextSize(2);
  // 14 high, 10 hori, 2 space
  display.setTextColor(SSD1306_WHITE);
  //display.setCursor(30, 20);-> 30 for horizontal distance- distance from text to left 30px
  //20 for vertical distance- same
  display.setCursor(0, 0);
  display.println("Determine");
  display.display();
  for (int i = 0; i < 100; i++) {
    if (i == 33 || i == 66 || i == 99) {
      display.print('.');
      display.display();
    }
    Deter_dis();
    high += distance / 100;
  }
  Deter_dis();
  int dis = distance;
  digitalWrite(Pumper_Pin, HIGH);
  delay(dt);
  digitalWrite(Pumper_Pin, LOW);
  Deter_dis();
  Velocity = (dis - distance) / dt;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("DONE!");
  display.println(high);
  display.println(Velocity);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
}

void move_text(const char *text) {
  for (int i = 0; i < 92; i++) {
    display.setCursor(i, 20);
    display.print(text);
    delay(10);
    display.display();
    display.clearDisplay();
  }
  display.clearDisplay();
  for (int i = 92; i >= 0; i--) {
    display.setCursor(i, 20);
    display.print(text);
    delay(10);
    display.display();
    display.clearDisplay();
  }
  display.clearDisplay();
}
//Default
float Val = 50;
void Check_state(float percent) {
  extern bool sign, sign_M, allow;
  float Val1 = 0.3 * Val;
  float Val2 = 0.9 * Val;
  if (allow) {
    display.println("Level: ");
  }
  if (percent < Val1) {
    digitalWrite(Red_Pin, HIGH);
    digitalWrite(Green_Pin, LOW);
    digitalWrite(Blue_Pin, LOW);
    if (allow) {
      display.println("Low");
    }
    if (!sign && !sign_M) {
      digitalWrite(Pumper_Pin, HIGH);
    }
  } else if (percent >= Val1 && percent < Val2) {
    digitalWrite(Red_Pin, LOW);
    digitalWrite(Green_Pin, HIGH);
    digitalWrite(Blue_Pin, LOW);
    if (allow) {
      display.println("Medium");
    }
  } else if (percent >= Val2) {
    digitalWrite(Red_Pin, LOW);
    digitalWrite(Green_Pin, LOW);
    digitalWrite(Blue_Pin, HIGH);
    if (allow) {
      display.println("High");
    }
    if ((percent > Val && !sign) && !sign_M) {
      digitalWrite(Pumper_Pin, LOW);
    }
  }
}
float percent = 0;
void Find_Mid_dis();
void Deter_dis() {
  digitalWrite(trig_Pin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trig_Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_Pin, LOW);
  // Read pulse duration
  travel_time = pulseIn(echo_Pin, HIGH);
  distance = travel_time * 0.034 / 2;
  delay(25);
}
void Read_But();
//Default mode
void Emergency();
void def_Mod(bool appear = 1) {
  // Emergency();
  extern bool allow;
  Deter_dis();
  // Serial.print(distance);
  // Serial.print("------");
  // Serial.println(travel_time);
  current = high - distance;
  percent = (current / high) * 100;
  if (allow) {
    display.setCursor(0, 0);
    display.println("Process: ");
    display.print(percent);
    display.println(" %");
  }
  Check_state(percent);
  if (appear && allow) {
    display.display();
  }
  extern bool allow;
  if (allow) {
    display.clearDisplay();
  }
}
//Saving mode
//First push -> 1(ON), second->0(OFF)
bool Read_But(uint8_t button, bool *count, bool *oldVal, bool *newVal) {
  *newVal = digitalRead(button);
  if (*newVal && !*oldVal) {
    *count = !*count;
  }
  *oldVal = *newVal;
  return *count;
}
//Value of  Potentiometer voltage
int Value;
int Convert_Val(int amount_func) {
  Value = analogRead(Read_Pin);
  float space = 1024. / amount_func;
  for (uint8_t i = 1; i <= amount_func; i++) {
    if (Value >= space * (i - 1) && Value < space * i) {
      return i;
    }
  }
}
//max is 10 character in same row
void Find_Mid_dis(const char *c, uint8_t vertical = (SCREEN_HEIGHT / 2) - 7, const char *colour = "WHITE") {
  uint8_t horizontal;
  int i = 0;
  while (c[i] != NULL) {
    ++i;
  }
  horizontal = (SCREEN_WIDTH - i * 12) / 2;
  if (colour == "BLACK") {
    display.setTextColor(BLACK);
  }
  display.setCursor(horizontal, vertical);
  display.println(c);
  display.setTextColor(WHITE);
}
bool Read(int);
bool mark = 0;
bool oldVal_S = 1, oldVal_E = 1, newVal_S, newVal_E;
void Saving() {
  // Emergency();
  static uint8_t order_but = 1;
  order_but = Convert_Val(3);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Select: ");
  display.setCursor(20, 18);
  display.print("60 %");
  display.setCursor(20, 34);
  display.print("70 %");
  display.setCursor(20, 50);
  display.print("80 %");
  switch (order_but) {
    case 1:
      display.drawTriangle(0, 18, 0, 29, 11, 24, WHITE);
      break;
    case 2:
      display.drawTriangle(0, 34, 0, 45, 11, 40, WHITE);
      break;
    case 3:
      display.drawTriangle(0, 50, 0, 61, 11, 56, WHITE);
      break;
  }
  display.display();
  // Serial.println(state);
  if (Read(Sele_but)) {
    switch (order_but) {
      case 1:
        Val = 60;
        break;
      case 2:
        Val = 70;
        break;
      case 3:
        Val = 80;
        break;
    }
    display.clearDisplay();
    while (1) {
      Find_Mid_dis("Success!");
      display.display();
      if (Read(Exit_but)) {
        mark = 1;
        delay(800);
        break;
      }
    }
  }
  display.clearDisplay();
}
//Maintenance mode
//interval is duration you want to repeat (minutes)
//time_off is duration pumper off (minutes)
int INTERVAL = 1, TIME_OFF = 1;
unsigned long predict = INTERVAL * 60000;  //sizeof(unsigned long)/60000=72000
bool sign = 0;                             //warning default function stop/non-stop control pumper
void Recur(bool repeat, uint8_t Pin, int interval = INTERVAL, int time_off = TIME_OFF) {
  INTERVAL = interval;
  TIME_OFF = time_off;
  extern bool sign_M;
  if (millis() > (predict + TIME_OFF * 60000)) {
    predict = predict + TIME_OFF * 60000 + INTERVAL * 60000;
    sign = 0;
  }
  if (!repeat) {
    sign = 0;
    return;
  }
  // Serial.print(millis());
  // Serial.print('-');
  // Serial.println(predict);
  if (millis() >= predict && millis() <= predict + TIME_OFF * 60000) {
    if (!sign_M) {
      digitalWrite(Pumper_Pin, LOW);
      sign = 1;
    } else {
      sign = 0;
    }
  }
}
bool state = 0;
void Maintenance() {
  // Emergency();
  static bool count = 0, oldVal = 0, newVal;
  display.clearDisplay();
  if (Read_But(Sele_but, &count, &oldVal, &newVal)) {
    display.drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 30, WHITE);
    Find_Mid_dis("ON");
    state = 1;
  } else {
    display.drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 30, WHITE);
    Find_Mid_dis("OFF");
    state = 0;
  }
  display.display();
}
//Emergency mode
//Sent message to screen time, date it error and type of error
void Emergency() {
  static int pre_dis, previous_time = millis();
  int time_check = 5000;
  Deter_dis();
  //Pumper doesn't work but water level increase
  if (!digitalRead(Pumper_Pin)) {
    if (millis() - previous_time > time_check && distance > pre_dis) {
      while (1) {
        display.clearDisplay();
        Find_Mid_dis("ERROR!");
        display.display();
        digitalWrite(Buzzer_Pin, HIGH);
        delay(1000);
        digitalWrite(Buzzer_Pin, LOW);
        delay(500);
      }
    }
  }
  //Pumper works but water level decrease (user uses water while pumper works)?
  else if (digitalRead(Pumper_Pin) && millis() - previous_time > time_check) {
    float predict = Velocity * time_check;
    if (!(0.99 * predict <= distance - pre_dis && 1.1 * predict >= distance - pre_dis)) {
      display.clearDisplay();
      digitalWrite(Pumper_Pin, LOW);
      while (1) {
        display.clearDisplay();
        Find_Mid_dis("ERROR!");
        display.display();
        digitalWrite(Buzzer_Pin, HIGH);
        delay(1000);
        digitalWrite(Buzzer_Pin, LOW);
        delay(500);
      }
    }
  }
  if (millis() - previous_time > time_check) {
    previous_time = millis();
    pre_dis = distance;
  }
}
//Manual mode
void Dis_info(int x, int y, int Value) {
  display.setCursor(x, y);
  if (Value < 10) {
    display.print(0);
    display.setCursor((x + 12), y);
    display.print(Value);
  } else {
    display.print(Value);
  }
  display.setCursor((x + 22), y);
}
bool reckon = 0;  //count times save value
void Fix_space(float per_conv, uint8_t x, uint8_t y) {
  static uint8_t xs, ys;
  if (reckon) {
    uint8_t start, end;
    if (x >= xs) {
      start = xs + 1;
      end = x - 1;
    } else {
      start = x + 1;
      end = xs - 1;
    }
    for (int i = start; i <= end; i++) {
      uint8_t y1;
      if ((M_PI / 2) <= per_conv && per_conv < (3 * M_PI) / 2) {
        y1 = 32 + sqrt(square(30) - square(i - 64));
      } else {
        y1 = 32 - sqrt(square(30) - square(i - 64));
      }
      display.drawLine(64, 32, i, y1, WHITE);
    }
  }
  xs = x;
  ys = y;
  reckon = !reckon;
}
void Erase(float percent) {
  uint8_t x, y;
  float per_conv = (percent * (3.6) * M_PI) / 180;
  if (0 <= per_conv && per_conv < M_PI / 2) {
    y = 32 - 30 * sin((M_PI / 2) - per_conv);
    x = 64 + 30 * cos((M_PI / 2) - per_conv);
  } else if ((M_PI / 2) <= per_conv && per_conv < M_PI) {
    y = 32 + 30 * sin(per_conv - (M_PI / 2));
    x = 64 + 30 * cos(per_conv - (M_PI / 2));
  } else if (per_conv >= M_PI && per_conv < (3 * M_PI) / 2) {
    y = 32 + 30 * sin((3 * M_PI / 2) - per_conv);
    x = 64 - 30 * cos((3 * M_PI / 2) - per_conv);
  } else {
    y = 32 - 30 * sin(per_conv - (3 * M_PI / 2));
    x = 64 - 30 * cos(per_conv - (3 * M_PI / 2));
  }
  display.drawLine(64, 32, x, y, WHITE);
  Fix_space(per_conv, x, y);
}
//In this function the program won't warn user about error of system
bool sign_M = 0, allow = 1, detect = 0;  //sign_M: allocate permission to control pumper;allow: permit delete/not delete screen;detect: if it detect in main option of manual and exit button is pushed then exit mode.
void Manual() {
  // Emergency();
  static bool count = 0, oldVal = 0, newVal, check_point = 0, signal = 0, confirm = 0, work = 0, monitor = 0;  //monitor Exit_but
  static uint8_t order, opt = 1;
  static uint8_t hour = 0, minute = 0, second = 0;
  if (Read(Sele_but)) {
    check_point = 1;
    if ((!confirm && signal) && (order - 1)) {
      confirm = 1;
    }
  }
  if (allow) {
    display.clearDisplay();
  }
  if (!signal) {
    order = Convert_Val(2);
  }
  //external mode
  bool outside = 0;
  if (!check_point) {
    outside = 1;
    sign_M = 0;
    signal = 0;
    reckon = 0;
    Find_Mid_dis("Handset", 11);
    Find_Mid_dis("Timer", 38);
    switch (order) {
      case 1:
        display.drawRect(0, 9, 128, 18, WHITE);
        break;
      case 2:
        display.drawRect(0, 36, 128, 18, WHITE);
        break;
    }
  } else {  //internal mode
    signal = 1;
    if (!(order - 1)) {
      sign_M = 1;
    }
    switch (order) {
      case 1:
        if (Read_But(Sele_but, &count, &oldVal, &newVal)) {
          display.drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 30, WHITE);
          Find_Mid_dis("ON");
          digitalWrite(Pumper_Pin, HIGH);
        } else {
          display.drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 30, WHITE);
          Find_Mid_dis("OFF");
          digitalWrite(Pumper_Pin, LOW);
        }
        break;
      case 2:
        if (!confirm) {
          Find_Mid_dis("Pumper ?", 0);
          Find_Mid_dis("ON", 18);
          Find_Mid_dis("OFF", 34);
          if (!(Convert_Val(2) - 1)) {
            display.drawTriangle(10, 18, 10, 32, 22, 25, WHITE);
            work = 1;
          } else {
            display.drawTriangle(10, 34, 10, 48, 22, 41, WHITE);
            work = 0;
          }
        } else {
          static bool count = 0, oldVal = 0, newVal, choose = 0, times = 0;  //times: first it reach ->draw circle , second times it doesn't
          float percent = 0;                                                 //choose: choose second or hour or minute
          static unsigned long timeline;
          if (monitor) {
            opt = 1;
            choose = 0;
            monitor = !monitor;
            times = 0;
            second = minute = hour = 0;
          }
          if (!(opt == 4 && choose)) {
            Find_Mid_dis("Interval ?", 0);
            choose = Read_But(Sele_but, &count, &oldVal, &newVal);
            if (!choose) {
              opt = Convert_Val(4);
            }
            switch (opt) {
              case 1:
                display.drawLine(17, 40, 39, 40, WHITE);
                if (choose) {
                  second = Convert_Val(59);
                }
                break;
              case 2:
                display.drawLine(53, 40, 75, 40, WHITE);
                if (choose) {
                  minute = Convert_Val(59);
                }
                break;
              case 3:
                display.drawLine(89, 40, 111, 40, WHITE);
                if (choose) {
                  hour = Convert_Val(23);
                }
                break;
              case 4:
                if (second != 0 || hour != 0 || minute != 0) {
                  display.drawLine(78, 63, 127, 63, WHITE);
                }
                break;
            }
            Dis_info(17, 25, second);
            display.print(':');
            Dis_info(53, 25, minute);
            display.print(':');
            Dis_info(89, 25, hour);
            display.setCursor(78, 47);
            display.print("Next");
          } else {
            sign_M = 1;
            if (!times) {
              times = 1;
              timeline = millis();
              Serial.println("Reach!");
            } else {
              allow = 0;
              float lead_time = ((float)second + (float)hour * 60 * 60 + (float)minute * 60) * 1000;  //saving memory
              percent = ((millis() - timeline) * 100) / lead_time;
              if (percent <= 100) {
                digitalWrite(Pumper_Pin, work);
                Erase(percent);
              } else {
                Find_Mid_dis("DONE", (SCREEN_HEIGHT / 2) - 7, "BLACK");
                sign_M = 0;
              }
            }
          }
        }
        break;
    }
  }
  display.display();
  if (Read(Exit_but)) {
    if (outside) {
      detect = 1;
    }
    check_point = 0;
    confirm = 0;
    monitor = 1;
    allow = 1;
    return;
  }
  //error exit this function
}
//pick 1 time it reach on and then it become off
bool Read(int Pin) {
  bool *oldVal, *newVal;
  if (Pin == Sele_but) {
    oldVal = &oldVal_S;
    newVal = &newVal_S;
  } else {
    oldVal = &oldVal_E;
    newVal = &newVal_E;
  }
  *newVal = digitalRead(Pin);
  if (*oldVal == 0 && *newVal == 1) {
    *oldVal = 1;
    return 1;
  }
  *oldVal = *newVal;
  return 0;
}
//quantity function
uint8_t quan_func = 4;

void loop() {
  // put your main code here, to run repeatedly :
  detect = 0;
  void (*function[quan_func])(void) = { def_Mod, Saving, Maintenance, Manual };
  const char *func_name[quan_func] = { "Default", "Saving", "Upkeep", "Manual" };
  uint8_t position = Convert_Val(quan_func);
  display.clearDisplay();
  Find_Mid_dis(func_name[position - 1]);
  display.display();
  def_Mod(0);
  Recur(state, Pumper_Pin);
  if (Read(Sele_but)) {
    for (int i = 1; i <= quan_func; i++) {
      if (position == i) {
        while (!Read(Exit_but)) {
          def_Mod(0);
          function[i - 1]();
          if (mark) {
            mark = !mark;
            break;
          }
          Recur(state, Pumper_Pin);
          if (detect) {
            break;
          }
        }
      }
    }
  }
}
