//Developed by Alp Karademir -  2019

// Arduino Based Bluetooth Controlled Car with Fingerprint Module with RTOS
// Used module : FMP10 Fingerprint Module

//Libraries
#include <Adafruit_Fingerprint.h>
#include <Arduino_FreeRTOS.h>

//------------------------------------//

#define mySerial Serial1 

#define sol_ileri 5
#define sol_geri 3
#define sol_hiz 7

#define sag_ileri 4
#define sag_geri 6
#define sag_hiz 2

#define redled 8
#define greenled 9

int motorlar_hiz = 120;

//------------------------------------//

int personID = 0; 

bool acikMi = false;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);
void vTask4(void *pvParameters);
//------------------------------------//



void setup() {


  Serial.begin(9600); 

  xTaskCreate(  vTask1,
                (const portCHAR *)"Task 1",
                1024,
                NULL,
                1,
                NULL);
  xTaskCreate(  vTask2,
                (const portCHAR *)"Task 2",
                1024,
                NULL,
                2,
                NULL);
  xTaskCreate(  vTask3,
                (const portCHAR *)"Task 3",
                1024,
                NULL,
                3,
                NULL);
  xTaskCreate(  vTask4,
                (const portCHAR *)"Task 4",
                1024,
                NULL,
                4,
                NULL);

pinMode(8, OUTPUT);
pinMode(9, OUTPUT);

digitalWrite(8, HIGH);

    pinMode(sol_ileri, OUTPUT);
    pinMode(sag_ileri, OUTPUT);
    pinMode(sol_geri, OUTPUT);
    pinMode(sag_geri, OUTPUT);
    pinMode(sol_hiz, OUTPUT);
    pinMode(sag_hiz, OUTPUT);

  Serial.println("\n\nAdafruit finger detect test");

  finger.begin(57600);
  delay(5);




}


void loop() {

  
}

void vTask1(void *pvParameters) 
{
  (void) pvParameters;
  const char *pcTaskName = "Araç Kilidi Açma Görevi";
  for (;;)
  {

    if (personID == 1 ) { 
      Serial.println("Araç Kilidi Açıldı");
      acikMi = true;
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
      Serial.println(pcTaskName);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS); 
  }
}


void vTask2(void *pvParameters)
{
  (void) pvParameters;
  const char *pcTaskName = "Araç Kilidi Kapatma Görevi";
  vTaskDelay(200 / portTICK_PERIOD_MS);  
  for (;;)
  {


    if (personID == 2) { 
      Serial.println("Araç Kilidi Kapandı");
      acikMi = false;
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      Serial.println(pcTaskName);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS); 
  }
}

void vTask3(void *pvParameters) 
{
  (void) pvParameters;
  const char *pcTaskName = "Parmak İzi Okuma Görevi ";
  vTaskDelay(400 / portTICK_PERIOD_MS); 
  for (;;)
  {
    personID =  getFingerprintIDez();
    vTaskDelay(50 / portTICK_PERIOD_MS); 
  }
}


void vTask4(void *pvParameters)
{
  (void) pvParameters;
  const char *pcTaskName = "Bluetooth Aktif/DeAktif Görevi";
  vTaskDelay(600 / portTICK_PERIOD_MS); 
  for (;;)
  {
    if (acikMi == true) {
      if (Serial.available()) {
        char data = Serial.read();

        if (data == 'F')
        {
          vTaskDelay(10 / portTICK_PERIOD_MS);
          while (data == 'F') {

            if (Serial.available()) {
              break;
            }
            ileri();
          }
          vTaskDelay(10 / portTICK_PERIOD_MS);
          Serial.println("İleri");
          dur();

        }
        else if (data == 'B')
        {

          vTaskDelay(10 / portTICK_PERIOD_MS);
          while (data == 'B') {

            if (Serial.available()) {
              break;
            }
            geri();
            vTaskDelay(10 / portTICK_PERIOD_MS);
            Serial.println("Geri");
            dur();
          }
        }
        else if (data == 'L')
        {

          vTaskDelay(10 / portTICK_PERIOD_MS);
          while (data == 'L') {

            if (Serial.available()) {
              break;
            }
            sol();
            Serial.println("sol");
            vTaskDelay(10 / portTICK_PERIOD_MS);
            dur();
          }
        }
        else if (data == 'R')
        {

          vTaskDelay(10 / portTICK_PERIOD_MS);
          while (data == 'R') {

            if (Serial.available()) {
              break;
            }
            sag();
            vTaskDelay(10 / portTICK_PERIOD_MS);
            Serial.println("sag");
            dur();
          }
        }
      }
    }
    else { 
      Serial.println("Parmak Kapalı Bluetooth");
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); 
  }
}








int getFingerprintIDez() { 
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  if (finger.fingerID == 1 && acikMi == false) {
    acikMi = true;
    return 1;
  } else if (finger.fingerID == 1 && acikMi == true) {
    acikMi = false;
    return 2;
  } else {
    return -1;
  }

}


void geri()
{
  digitalWrite(sol_ileri, 0);
  digitalWrite(sag_ileri, 0);
  digitalWrite(sol_geri, 1);
  digitalWrite(sag_geri, 1);
  analogWrite(sol_hiz, motorlar_hiz);
  analogWrite(sag_hiz, motorlar_hiz);
}

void ileri()
{
  digitalWrite(sol_ileri, 1);
  digitalWrite(sag_ileri, 1);
  digitalWrite(sol_geri, 0);
  digitalWrite(sag_geri, 0);
  analogWrite(sol_hiz, motorlar_hiz);
  analogWrite(sag_hiz, motorlar_hiz);
}

void sag()
{
  digitalWrite(sol_ileri, 1);
  digitalWrite(sag_ileri, 0);
  digitalWrite(sol_geri, 1);
  digitalWrite(sag_geri, 0);
  analogWrite(sol_hiz, motorlar_hiz);
  analogWrite(sag_hiz, motorlar_hiz);
}

void sol()
{
  digitalWrite(sol_ileri, 0);
  digitalWrite(sag_ileri, 1);
  digitalWrite(sol_geri, 0);
  digitalWrite(sag_geri, 1);
  analogWrite(sol_hiz, motorlar_hiz);
  analogWrite(sag_hiz, motorlar_hiz);
}
void dur()
{
  digitalWrite(sol_ileri, 0);
  digitalWrite(sag_ileri, 0);
  digitalWrite(sol_geri, 0);
  digitalWrite(sag_geri, 0);
  analogWrite(sol_hiz, 0);
  analogWrite(sag_hiz, 0);
}
