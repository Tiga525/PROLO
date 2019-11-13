//https://dotstud.io/blog/nefrybt-ble-bluetooth-peripheral/
//HardwareSerial Serial1(2);
 
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>

#define SENSOR_ADRS   0x40              // GP2Y0E03のI2Cアドレス
#define DISTANCE_ADRS 0x5E
 
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
int value = 0;
const int analogInPin = 4;
int sensorValue = 0;
int  ans;
 
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

const int led_pin = 32;
const int vol_pin = 35;

int vol_value = 0;

int count = 0;
int On = 0;

 
 
#define SERVICE_UUID        "e3344d55-d8b4-44f7-afa0-2813b39048cf"
#define CHARACTERISTIC_UUID "66315508-1570-47db-b6c6-84a952b860d4"
#define SERVER_NAME         "esp32devc"
 
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
 
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};
 
void setup() {
  Serial.begin(115200);
  
//  Serial1.begin(115200);
  Wire.begin(21,22);
  pinMode(vol_pin,INPUT);
  pinMode(32, OUTPUT);
  //pinMode(vol_pin, INPUT);
  digitalWrite( led_pin, HIGH );
  delay(100);
  
  Serial.println("BLE_DataServer180130");
  BLEDevice::init(SERVER_NAME);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
 
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
  delay(500);

  
  

  
}
 
void loop() {
 
  if (deviceConnected) {
//    Serial.printf("*** NOTIFY: %d ***\n", value);
    char buffer[10];
//    sensorValue = analogRead(analogInPin);
//    value=map(sensorValue, 0, 4095, 0, 1100);
//　  sprintf(buffer, "%d", sensorValue);

    //a_distance();
     button();
    
    value = ans;
//    Serial.write(value);
    sprintf(buffer, "%d", value);
    Serial.printf(buffer);
    pCharacteristic->setValue(buffer);
    pCharacteristic->notify();
//    digitalWrite(5, HIGH);   // turn the LED on (HIGH is the voltage level)
//    delay(30);                       // wait for a second
//    digitalWrite(5, LOW);    // turn the LED off by making the voltage LOW
    //value++;
  }
  delay(100);
  
}

void a_distance() {

     byte c[2] ;

     Wire.beginTransmission(SENSOR_ADRS) ;        // 通信の開始処理
     Wire.write(DISTANCE_ADRS) ;                  // 距離値を格納したテーブルのアドレスを指定する
     ans = Wire.endTransmission() ;               // データの送信と終了処理
     if (ans == 0) {
          ans = Wire.requestFrom(SENSOR_ADRS,2) ; // GP2Y0E03にデータ送信要求をだし、2バイト受信する
          c[0] = Wire.read()  ;                   // データの11-4ビット目を読み出す
          c[1] = Wire.read()  ;                   // データの 3-0ビット目を読み出す
          ans = ((c[0]*16+c[1]) / 16) / 4 ;       // 取り出した値から距離(cm)を計算する
          Serial.print(ans) ;                     // シリアルモニターに表示させる
          Serial.println("cm") ;
     } else {
         Serial.print("ERROR NO.=") ;            // GP2Y0E03と通信出来ない
          Serial.println(ans) ;
     }
     delay(500) ;                                 // 500ms後に繰り返す
  }

void b_distance() {
  ans = analogRead(A11);
  Serial.println(ans);
}

void button(){

  
  
  if(digitalRead(13) == HIGH){
    ans = 1;
  } else{
    ans = 0;
  }


  vol_value = analogRead( vol_pin );

  //delay(100);

  if(vol_value < 500 && On == 0){
    ans = 1;
    count ++;

    if (count == 5){
      count = 0;
      On = 1;
      }
    }

   else if(vol_value > 500 && On == 1){
    ans = 1;
    count ++;

    if (count == 5){
      count = 0;
      On = 0;
      }
    }



  else{
    ans = 0;
    }


  Serial.println( vol_value );
}
