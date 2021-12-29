#include <Arduino_FreeRTOS.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Stewitter.h>

#define Trig  2
#define Echo  3
#define RST_PIN 9
#define SDA_PIN 10
 
#define UID1 "XX XX XX XX"  // キーのUID

MFRC522 mfrc522(SDA_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

int dist_flag = 0;
int light_flag = 0;
int RFID_flag = 0;
int sub_flag = 0;

int Light_value_before = 0; 
int Light_value_now = 0;
float now_cm;
float before_cm;

unsigned long Q1_nowtime = 0;
unsigned long Q2_nowtime = 0;
unsigned long Q3_nowtime = 0;
unsigned long Q4_nowtime = 0;

// 時間計測開始のフラグ 0の時に一回行い，規定時間の経過後に0に戻す
int Q1_Timer_Flag = 0; // dist_flag = 0 && light_flag = 1
int Q2_Timer_Flag = 0; // dist_flag = 1 && light_flag = 0
int Q3_Timer_Flag = 0; // dist_flag = 0 && light_flag = 0
int Q4_Timer_Flag = 0; // dist_flag = 1 && light_flag = 1

// define tasks
void TaskLight( void *pvParameters );
void TaskDist( void *pvParameters );
void TaskRFID( void *pvParameters );
void Check_state( void *pvParameters );

void setup() {
  Serial.begin(9600);
  
  while (!Serial) {
    ;
  }

  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskLight
    ,  "Light"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  4  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

    xTaskCreate(
    TaskDist
    ,  "Dist"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

    xTaskCreate(
    TaskRFID
    ,  "RFID"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
    
    xTaskCreate(
    Check_state
    ,  "Check"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskLight(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  for (;;) // A Task shall never return or exit.
  {
    Light_value_now = analogRead(0);
    
    if(Light_value_now - Light_value_before >= 80 && Light_value_before != 0){
      Serial.println("Leave Light");
      light_flag = 1;
    }else if(Light_value_before - Light_value_now >= 80){
      Serial.println("Come Back Light");
      light_flag = 0;
    }
    
    Light_value_before = Light_value_now;
    Serial.println(light_flag);
    vTaskDelay(4000 / portTICK_PERIOD_MS);
  }
}

void TaskDist(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(Trig, LOW);       
    delayMicroseconds(2);
    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig, LOW);
    
    now_cm = pulseIn(Echo, HIGH) / 58.0;
    now_cm = (int(now_cm * 100.0)) / 100.0;
    
    
    if(before_cm - now_cm >= 80){
      if(dist_flag == 1){
        Serial.println("Check IN Distance");
        dist_flag = 0;
      }else{
        Serial.println("Check OUT Distance");
        dist_flag = 1;
      }
    }
    
    before_cm = now_cm;
    
    Serial.println(dist_flag);
    vTaskDelay(4000 / portTICK_PERIOD_MS);
  }
}

void TaskRFID(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  for (;;) // A Task shall never return or exit.
  {
      if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        String strBuf[mfrc522.uid.size];
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        strBuf[i] =  String(mfrc522.uid.uidByte[i], HEX);  // (E)using a constant integer
        if(strBuf[i].length() == 1){  // 1桁の場合は先頭に0を追加
          strBuf[i] = "0" + strBuf[i];
        }
      }
   
      String strUID = strBuf[0] + " " + strBuf[1] + " " + strBuf[2] + " " + strBuf[3];
      if ( strUID.equalsIgnoreCase(UID1)){
        RFID_flag = 1;
        sub_flag = 1;
      }
    }else{
      if(sub_flag == 0) RFID_flag = 0;
      else sub_flag = 0;
    }
    Serial.println(RFID_flag);
    vTaskDelay(4000 / portTICK_PERIOD_MS);
  }
}

void Check_state(void *pvParameters)  // This is a task.
{
  (void) pvParameters;


  for (;;) // A Task shall never return or exit.
  {
    if(dist_flag == 0 && light_flag == 1){ //Q1
       Q2_Timer_Flag = 0;
       Q3_Timer_Flag = 0;
       Q4_Timer_Flag = 0;

       if(Q1_Timer_Flag == 0){
        Q1_nowtime = millis() + 10000;
        Q1_Timer_Flag = 1;
       }
       if((long)(Q1_nowtime - millis()) < 0 ){
        Serial.write(1);
        Q1_Timer_Flag = 0;
       }
    }

     else if(dist_flag == 1 && light_flag == 0){ //Q2
       Q1_Timer_Flag = 0;
       Q3_Timer_Flag = 0;
       Q4_Timer_Flag = 0;

       if(Q2_Timer_Flag == 0){
        Q2_nowtime = millis() + 10000;
        Q2_Timer_Flag = 1;
       }
       if( (long)(Q2_nowtime - millis()) < 0 ){
        Serial.write(2);
        Q2_Timer_Flag = 0;
        if(RFID_flag == 1) Serial.write(5);
       }
    }

    else if(dist_flag == 0 && light_flag == 0){ //Q3→在宅中
       Q1_Timer_Flag = 0;
       Q2_Timer_Flag = 0;
       Q4_Timer_Flag = 0;
      
       if(Q3_Timer_Flag == 0){
        Q3_nowtime = millis() + 10000;
        Q3_Timer_Flag = 1;
       }
       Serial.println( (long)(Q3_nowtime - millis()));
       if( (long)(Q3_nowtime - millis()) < 0 ){
        if(RFID_flag == 0) Serial.write(4);
        Q3_Timer_Flag = 0;
       }
    }

    else if(dist_flag == 1 && light_flag == 1){ //Q4→退出中
       Q1_Timer_Flag = 0;
       Q2_Timer_Flag = 0;
       Q3_Timer_Flag = 0;

       if(Q4_Timer_Flag == 0){
        Q4_nowtime = millis() + 10000;
        Q4_Timer_Flag = 1;
       }
       Serial.println( (long)(Q4_nowtime - millis()));
       if( (long)(Q4_nowtime - millis()) < 0 ){
        if(RFID_flag == 1) Serial.write(5);
        Q4_Timer_Flag = 0;
       }
    }
    vTaskDelay(4000 / portTICK_PERIOD_MS);
  }
}
