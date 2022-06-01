
#define led_tower 1
#define led_1 2
#define led_2 3
#define led_3 4
#define led_4 5
#define led_5 6
#define led_R 7
#define led_G 8
#define led_B 9

#define SERVO_PIN 10
#define pump1_pin 11
#define pump2_pin 12

#define SW 13             // кнопка энкодера
#define CLK 14            // энкодер А0
#define DT 15             // энкодер А1
#define BTN1_PIN 16        // кнопка старт (BTN_PIN --- КНОПКА --- GND) A2
#define BTN2_PIN 17      // кнопка ручной налив (BTN_PIN --- КНОПКА --- GND) A3
// 18 sda A4
// 19 scl A5


//энкодер 
#include "GyverEncoder.h"
Encoder enc1(CLK, DT, SW);

//lcd питание, GND к GND, дата идет через одноименные пины
#include "LCD_1602_RUS.h"
LCD_1602_RUS lcd(0x27, 16, 2);
       
//кнопки
#include "GyverButton.h"
GButton butt1(BTN1_PIN); 
GButton butt2(BTN2_PIN);

//серво
#include <VarSpeedServo.h>
VarSpeedServo servo;

long pump_time = 58000;
int  start_pump_pos=0;
int  inc_pump_pos=45;
boolean mode_menu=0; // 0 работа наливатора, 1 настройка
byte mode_setings = 0; // 0 тип, 1 кол-вло стаканов, 2 кол-во основного  мл, 3 кол-во второго 
boolean mode_save=0; //флаг для сохранения
boolean mode_save_yes=1; //флаг подтверждения сохранения
boolean butt2_mode=0; //для второй кнопки прямого налива, 0 - лить первым, 1 - лить вторым

byte set_type=2;
int set_4el=2;
int set_ml1=50;
int set_ml2=100;

void setup() {
 
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print(L"Привет,");
  lcd.setCursor(6, 1);
  lcd.print(L"Алкаш");
 
  butt1.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt1.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt1.setType(HIGH_PULL);
  butt1.setDirection(NORM_OPEN);
  butt1.setTickMode(AUTO);

  butt2.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt2.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt2.setType(HIGH_PULL);
  butt2.setDirection(NORM_OPEN);
  butt2.setTickMode(AUTO);
 
  enc1.setTickMode(AUTO);

  servo.attach(SERVO_PIN);
  servo.write(90);

  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  pinMode(led_3, OUTPUT);
  pinMode(led_4, OUTPUT);
  pinMode(led_5, OUTPUT);
  pinMode(led_tower, OUTPUT);
  pinMode(led_R, OUTPUT);
  pinMode(led_G, OUTPUT);
  pinMode(led_B, OUTPUT);
  
  digitalWrite(led_1, LOW);
  digitalWrite(led_2, LOW);
  digitalWrite(led_3, LOW);
  digitalWrite(led_4, LOW);
  digitalWrite(led_5, LOW);
  digitalWrite(led_tower, HIGH);
  digitalWrite(led_R, LOW);
  digitalWrite(led_G, LOW);
  digitalWrite(led_B, HIGH);

  pinMode(pump1_pin, OUTPUT);
  pinMode(pump2_pin, OUTPUT);
  delay(1000); 
  digitalWrite(pump1_pin, LOW);
  digitalWrite(pump2_pin, LOW);
  main_LCD();
  led_4el ();
  
}

void loop() {
 
 //========================================== настройка ================================
  if (enc1.isHolded()) // вход в настройку
  {
      if (!mode_menu)
     {
     mode_menu=1;
     digitalWrite(led_R, HIGH);
     digitalWrite(led_B, LOW);
     lcd.clear();
     lcd.setCursor(3, 0); lcd.print(L"Настройка");
     delay (1000);
     menu_LCD ();
     }
     else if (mode_menu)
     {
     mode_save=1;
     mode_save_yes=1;
     lcd.clear();
     lcd.setCursor(3, 0); lcd.print(L"Сохраняем?");
     lcd.setCursor(3, 1); lcd.print(L"Да<   Нет");
     }
    }
 //========================= цикл настройки ====================================
  if  (mode_menu)
    {     
        if (enc1.isLeft()) 
        {
          if ((mode_setings == 0)&&(!mode_save))
            {set_type=1;
            menu_LCD ();
            }
          if ((mode_setings == 1)&&(!mode_save))
            {set_4el=set_4el-1;
            if (set_4el==0) set_4el=1;
            led_4el ();
            menu_LCD ();
            }
          if ((mode_setings == 2)&&(!mode_save))
            {set_ml1=set_ml1-5;
            if (set_ml1<5) set_ml1=5;
            menu_LCD ();
            }
          if ((mode_setings == 3)&&(!mode_save)&&(set_type==2))
            {set_ml2=set_ml2-10;
            if (set_ml2<10) set_ml2=10;
            menu_LCD ();
            }
          if (mode_save)
            { mode_save_yes=1;
              lcd.clear();
              lcd.setCursor(3, 0); lcd.print(L"Сохраняем?");
              lcd.setCursor(3, 1); lcd.print(L"Да<   Нет");}
        }
     if (enc1.isRight()) 
        {
          if ((mode_setings == 0)&&(!mode_save))
            {set_type=2;
             menu_LCD ();
            }
         if ((mode_setings == 1)&&(!mode_save))
            {set_4el=set_4el+1;
            if (set_4el>5) set_4el=5;
            led_4el ();
            menu_LCD ();
            }
          if ((mode_setings == 2)&&(!mode_save))
            {set_ml1=set_ml1+5;
            if (set_ml1>500) set_ml1=500;
            menu_LCD ();
            }
          if ((mode_setings == 3)&&(!mode_save)&&(set_type==2))
            {set_ml2=set_ml2+10;
            if (set_ml2>500) set_ml2=500;
            menu_LCD ();
            }
          if (mode_save)
            { mode_save_yes=0;
              lcd.clear();
              lcd.setCursor(3, 0); lcd.print(L"Сохраняем?");
              lcd.setCursor(3, 1); lcd.print(L"Да   >Нет");}
        }
  if (enc1.isClick()) 
  {  
       if (!mode_save)
       {
           lcd.setCursor(14, 1); lcd.print(L"OK");
           delay (1000);
             mode_setings = mode_setings+1;
             if (mode_setings == 4) mode_setings = 0;
             if ((mode_setings == 3)&&(set_type ==1)) mode_setings = 0;
             menu_LCD ();
        }
  else if (mode_save)
  {
    if (mode_save_yes)
    {
      lcd.setCursor(14, 1); lcd.print(L"OK");
      delay (1000);
     mode_save=0;
     mode_menu=0;
     mode_setings=0;
     digitalWrite(led_B, HIGH);
     digitalWrite(led_R, LOW);
     main_LCD ();
    }
    else if (!mode_save_yes)
               {
                lcd.setCursor(14, 1); lcd.print(L"OK");
                delay (1000); 
                mode_save=0;
                mode_save_yes=0;
                menu_LCD ();
               }
    }  
   }
  }
//====================== конец цикла настройки ====================================

//====================== отрабока кнопок налива ===================================

 if (butt2.isDouble() && !mode_menu) //выбор режима налива
  {  butt2_mode=!butt2_mode;
    lcd.clear();
    if (butt2_mode) {lcd.setCursor(3, 0); lcd.print(L"Готов лить"); lcd.setCursor(4, 1); lcd.print(L"второго");}
   else if (!butt2_mode) {lcd.setCursor(3, 0); lcd.print(L"Готов лить"); lcd.setCursor(4, 1); lcd.print(L"первого");}
    delay (1000);
    main_LCD();}
 
 if (butt1.isSingle() && !mode_menu) //главная кнопка налива
{
  pumping ();
  main_LCD();
}

if (butt2.isStep() && !butt2_mode && !mode_menu)   //кнопка прямого налива, льем первой помпой
 {
  digitalWrite(led_G, HIGH);
  lcd.clear();
    lcd.setCursor(5, 0); lcd.print(L"Наливаю");
    lcd.setCursor(5, 1); lcd.print(L"Первого");
    led_pump_4el ();
    digitalWrite(pump1_pin, HIGH);
 }
   if (butt2.isStep() && butt2_mode && !mode_menu)   //кнопка прямого налива, льем второй помпой
 {
    digitalWrite(led_R, HIGH);
    lcd.clear();
    lcd.setCursor(5, 0); lcd.print(L"Наливаю");
    lcd.setCursor(5, 1); lcd.print(L"Второго");
    led_pump_4el ();
    digitalWrite(pump2_pin, HIGH);
 }
   if (butt2.isRelease()&& !mode_menu)
  { digitalWrite(led_G, LOW);
    digitalWrite(led_R, LOW);
    digitalWrite(pump1_pin, LOW);
    digitalWrite(pump2_pin, LOW);
    led_4el ();
    main_LCD();
  }
  


//======================== конец цикла loop =======================================
}


void pumping ()
{
   digitalWrite(led_G, HIGH);
   digitalWrite(led_B, LOW);
   delay (300);
  int pos = start_pump_pos;
  
  for (int i = 1; i<=set_4el; i++)
  {
    servo.write(pos, 100);
    delay(1000);
    lcd.clear();
     lcd.setCursor(0, 0); lcd.print(L"стакан");
    lcd.setCursor(7, 0); lcd.print(i);
    lcd.setCursor(1, 1); lcd.print(L"из бут1");
    lcd.setCursor(9, 1); lcd.print(set_ml1);
    lcd.setCursor(13, 1); lcd.print(L"ml");
    int pump1_time_value=map(set_ml1, 0, 500, 0, pump_time);
    digitalWrite(pump1_pin, HIGH);
    delay(pump1_time_value);
    digitalWrite(pump1_pin, LOW);
    
    if (set_type==2)
    {
        lcd.clear();
    lcd.setCursor(8, 0); lcd.print(L"стакан");
    lcd.setCursor(15, 0); lcd.print(i);
    lcd.setCursor(1, 1); lcd.print(L"из бут2");
    lcd.setCursor(9, 1); lcd.print(set_ml2);
    lcd.setCursor(13, 1); lcd.print(L"ml");
    int pump2_time_value=map(set_ml2, 0, 500, 0, pump_time);
    digitalWrite(pump2_pin, HIGH);
    delay(pump2_time_value);
    digitalWrite(pump2_pin, LOW);
    }
    delay (1000);
    if (set_4el==4&i==2)
    pos=pos+inc_pump_pos+inc_pump_pos;
    else pos=pos+inc_pump_pos;
  } 
  servo.write(90, 100);
  digitalWrite(led_B, HIGH);
   digitalWrite(led_G, LOW);
  }


void main_LCD ()
{      
     lcd.clear();
     if (set_type==2)
     {
     lcd.setCursor(0, 0); lcd.print(L"тип чел мл1 мл2");
     lcd.setCursor(0, 1); lcd.print(L" 2");
     lcd.setCursor(5, 1); lcd.print(set_4el);
     lcd.setCursor(8, 1); lcd.print(set_ml1);
     lcd.setCursor(12, 1); lcd.print(set_ml2);
     }
     if (set_type==1)
     {
     lcd.setCursor(1, 0); lcd.print(L"тип  чел  мл1");
     lcd.setCursor(1, 1); lcd.print(L" 1");
     lcd.setCursor(7, 1); lcd.print(set_4el);
     lcd.setCursor(11, 1); lcd.print(set_ml1);
     }     
}

void led_4el ()
{
  if (set_4el==1)
  {
  start_pump_pos=90;
  inc_pump_pos=0;
  digitalWrite(led_1, LOW);
  digitalWrite(led_2, LOW);
  digitalWrite(led_3, HIGH);
  digitalWrite(led_4, LOW);
  digitalWrite(led_5, LOW);
  }
  if (set_4el==2)
  {
  start_pump_pos=45;
  inc_pump_pos=90;
  digitalWrite(led_1, LOW);
  digitalWrite(led_2, HIGH);
  digitalWrite(led_3, LOW);
  digitalWrite(led_4, HIGH);
  digitalWrite(led_5, LOW);
  }
  if (set_4el==3)
  {
  start_pump_pos=45;
  inc_pump_pos=45;
  digitalWrite(led_1, LOW);
  digitalWrite(led_2, HIGH);
  digitalWrite(led_3, HIGH);
  digitalWrite(led_4, HIGH);
  digitalWrite(led_5, LOW);
  }
   if (set_4el==4)
  {
  start_pump_pos=0;
  inc_pump_pos=45;
  digitalWrite(led_1, HIGH);
  digitalWrite(led_2, HIGH);
  digitalWrite(led_3, LOW);
  digitalWrite(led_4, HIGH);
  digitalWrite(led_5, HIGH);
  }
    if (set_4el==5)
  {
  start_pump_pos=0;
  inc_pump_pos=45;
  digitalWrite(led_1, HIGH);
  digitalWrite(led_2, HIGH);
  digitalWrite(led_3, HIGH);
  digitalWrite(led_4, HIGH);
  digitalWrite(led_5, HIGH);
  }

}

void menu_LCD ()
{
    lcd.clear();
    if (mode_setings == 0)
            {
            lcd.setCursor(3, 0); lcd.print(L"Тип напитка");
                if (set_type==1)
                {lcd.setCursor(5, 1); lcd.print(L"Чистяк");}
                else if (set_type==2)
                {lcd.setCursor(4, 1); lcd.print(L"Коктейль");}
            }
         if (mode_setings == 1)
            {
            lcd.setCursor(1, 0); lcd.print(L"Кол-во человек");
            lcd.setCursor(7, 1); lcd.print(set_4el);
            }
          if (mode_setings == 2)
            {
            lcd.setCursor(1, 0); lcd.print(L"Сколько первого");
            lcd.setCursor(7, 1); lcd.print(set_ml1);
            }
          if ((mode_setings == 3)&&(set_type==2))
            {
             lcd.setCursor(1, 0); lcd.print(L"Сколько второго");
             lcd.setCursor(7, 1); lcd.print(set_ml2);
            }
}

void  led_pump_4el ()
{
   digitalWrite(led_1, LOW);
  digitalWrite(led_2, LOW);
  digitalWrite(led_3, HIGH);
  digitalWrite(led_4, LOW);
  digitalWrite(led_5, LOW);
}
