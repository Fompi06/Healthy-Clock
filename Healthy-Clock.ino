#include <GyverTM1637.h>                              // подключаем стороннюю библиотеку для удобства работы
#include "SIMPCODE.h"                                 // подключаем мою личною библиотеку для удобства

#define TIMER_ENABLE

#define ReminderWork 60                                // время, спустя которое вам напомнят об отдыхе
#define RepeatedReminderWork 20                        // время, спустя которое вам напомнят об отдыхе, если вы уже работаете сверхурочно
#define ReminderRelax 20                               // время, спустя которое вам напомнят о работе
#define RepeatedReminderRelax 10                       // время, спустя которое вам напомнят о работе, если вы уже отдыхаете слишком долго
#define randomAlertTime 30                             // значение "случайности" напоминания об отдыхе

#define RED 2                                          // задаем пины
#define GREEN 3
#define BUZ 10
#define DIO 13
#define CLK 12
#define POW 7
#define SWCH 9

GyverTM1637 disp(CLK, DIO);
byte numberSegments[10] = {
  0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
  0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111,
};

uint32_t Timer1;
uint32_t Timer2;
uint32_t Timer3;
uint32_t Timer4;
uint32_t Timer5;

void setup() {
  // put your setup code here, to run once:
  for (int i = 2; i <= 6; i++) pinMode(i, OUTPUT);
  pinMode(BUZ, OUTPUT);
  pinMode(POW, INPUT_PULLUP);
  pinMode(SWCH, INPUT_PULLUP);
  Serial.begin(9600);
  randomSeed(analogRead(0));                             // в качестве рандомизатора времени "опроса" человека, работает он или нет, возьмем значения с аналогового пина
  disp.brightness(5);                                    // яркость, 0 - 7 (минимум - максимум)
}

void workMode()
{
  byte randomTime = random(10, 50);
  byte randomAlert = random(0, randomAlertTime);
  digitalWrite(GREEN, 0);                                // отключаем все светодиоды
  digitalWrite(RED, 0);
  static int a, b, c, d;                                 // добавляем к переменным static, чтобы при выходе из функции значения не сбрасывались
  int f = 0;                                             // данная перемена является счетчиком минут
  disp.displayByte(numberSegments[a], numberSegments[b], numberSegments[c], numberSegments[d]);
  bool missed = false;                                   // добавляем переменную, которая будет хранить: true - если человек пропустил отдых, false - если человек не пропускал отдых
  int f0 = 0;
  delay(500);                                            // ждем 500 мс, чтобы избежать последствий от дребезга кнопки
  Timer1 = millis();                                   // сброс таймера
  while (1)
  {
    digitalWrite(RED, 0);
    noTone(BUZ);
    bool flag = 1;                                       // добавляем переменную типа bool, чтобы включать и отключать точку
    disp.point(flag);
    disp.displayByte(numberSegments[a], numberSegments[b], numberSegments[c], numberSegments[d]);
    Timer2 = millis();                                   // сброс таймера
    Timer5 = millis();                                   // сброс таймера
    while (millis() - Timer1 <= 60000)
    {
      if (!digitalRead(SWCH)) relaxMode();  // переходим в режим отдыха
      if (millis() - Timer2 >= 500) {
        flag = !flag;
        disp.point(flag);
        Timer2 = millis();                              // сброс таймера
      }
      if (missed && (millis() - Timer5 >= 1000)) {
        digitalWrite(RED, !digitalRead(RED));
        Timer5 = millis();
      }
    }
    d++; f++;
    Timer1 = millis();                                   // сброс таймера
    if (d == 10) {
      d = 0;
      c++;
    }
    if (c == 6) {
      c = 0;
      b++;
    }
    if (b == 10) {
      b = 0;
      a++;
    }
    if (a == 2) a = 0;                                  // чтобы часы выглядели правильно, при каждом переключении проверяем, нет ли у нас 60 минут или 24 часов
    bool flag1 = 1;
    if ((missed && f - f0 >= RepeatedReminderWork) || (!missed && f >= ReminderWork + randomAlert))
    {
      disp.displayByte(numberSegments[a], numberSegments[b], numberSegments[c], numberSegments[d]);
      digitalWrite(RED, 1);

      for (int i = 0; i <= 10; i++)                     // 10 раз повторяем манипуляции с зуммером
      {
        Timer4 = millis();
        while (millis () - Timer4 < 1000)               // ждем одну секунду, постоянно проверяя, нажата ли кнопка
        {
          if (!digitalRead(POW)) relaxMode();
        }
        flag1 = !flag1;
        switch (flag1) {
          case 0:
            tone(BUZ, 900);
            break;
          case 1:
            noTone(BUZ);
            break;
        }
      }
      missed = 1;
      f0 = f;
    }
    
    if (f == randomTime)                               // если наш счетчик совпал со значением переменной, начинаем "опрашивать" человека
    {
      bool checkup = 0;                                // создаем переменную: true - человек нажал на кнопку, false - человек не нажал на кнопку
      for (int i = 0; i <= 20; i++)                    // повторяем наш цикл 20 раз, чтобы человек успел нажать на кнопку, если ему это нужно
      {
        tone(BUZ, 700);                                // подаем сигнал оповещения с помощью пищалки
        digitalWrite(RED, 1);                          // включаем светодиод
        Timer1 = millis();
        while (millis() - Timer1 <= 500) {             // ждем 500 мс, постоянно опрашивая кнопку
          if (!digitalRead(POW)) {
            checkup = 1;                               // если человек нажал на кнопку, присвоить переменной true и досрочно выйти из цикла
            break;
          }
        }
        noTone(BUZ);
        digitalWrite(RED, 0);
        Timer1 = millis();
        while (millis() - Timer1 <= 250) {
          if (!digitalRead(POW)) {
            checkup = 1;
            break;
          }
        }
      }
      if (!checkup) {
        disp.displayByte(_i, _d, _l, _e);              // выводим на экран о том, что кнопка не нажата, и работа стоит
        delay(1000);
        relaxMode();                                   // если кнопку так и никто не нажал, отправляем таймер на режим отдыха =)
      }
      else randomTime = random(10, 50);                // а если нажал, то снова задаем ему случайное время, через которое мы его опросим
    }
    
  }
}


void relaxMode()
{
  digitalWrite(GREEN, 0);           // отключаем все светодиоды
  digitalWrite(RED, 0);
  static int a, b, c, d;            // добавляем к переменным static, чтобы при выходе из функции значения не сбрасывались
  int f = 0;                        // данная перемена является счетчиком минут
  disp.displayByte(numberSegments[a], numberSegments[b], numberSegments[c], numberSegments[d]);
  bool missed = false;              // добавляем переменную, которая будет хранить: true - если человек пропустил отдых, false - если человек не пропускал отдых
  int f0 = 0;
  delay(500);                       // ждем 500 мс, чтобы избежать последствий от дребезга кнопки
  Timer1 = millis();                                   // сброс таймера
  while (1)
  {
    noTone(BUZ);
    bool flag = 1;                  // добавляем переменную типа bool, чтобы включать и отключать точку
    disp.point(flag);
    disp.displayByte(numberSegments[a], numberSegments[b], numberSegments[c], numberSegments[d]);
    Timer2 = millis();              // сброс таймера
    Timer5 = millis();              // сброс таймера
    while (millis() - Timer1 <= 60000)
    {
      if (!digitalRead(SWCH)) workMode();  // переходим в режим отдыха
      if (millis() - Timer2 >= 1000) {     // делаем задержку в два раза больше, дабы отличать режимы
        flag = !flag;
        disp.point(flag);
        Timer2 = millis();          // сброс таймера
      }
      if (missed && (millis() - Timer5 >= 1000)) {
        digitalWrite(GREEN, !digitalRead(GREEN));
        Timer5 = millis();
      }
    }
    d++; f++;
    Timer1 = millis();                                   // сброс таймера
    if (d == 10) {
      d = 0;
      c++;
    }
    if (c == 6) {
      c = 0;
      b++;
    }
    if (b == 10) {
      b = 0;
      a++;
    }
    if (a == 2) a = 0;
    bool flag1 = 1;
    if ((missed && f - f0 >= RepeatedReminderRelax) || (!missed && f >= ReminderRelax))
    {
      disp.displayByte(numberSegments[a], numberSegments[b], numberSegments[c], numberSegments[d]);
      digitalWrite(GREEN, 1);

      for (int i = 0; i <= 10; i++)        // 10 раз повторяем манипуляции с зуммером
      {
        Timer4 = millis();
        while (millis() - Timer4 < 1500)   // ждем полторы секунды, постоянно проверяя, нажата ли кнопка
        {
          if (!digitalRead(POW)) workMode();
        }
        flag1 = !flag1;
        switch (flag1) {
          case 0:
            tone(BUZ, 900);
            break;
          case 1:
            noTone(BUZ);
            break;
        }
      }
      missed = 1;
      f0 = f;
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  workMode();
}
