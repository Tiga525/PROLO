//Based on BLE client example

//サーバーが停止するとエラーで回復できない
/////////////////////////////////////////////
#define PWMCONTROL_PIN 25

#define LEDC_CHANNEL 1
#define LEDC_BASE_FREQ 50
#define LEDC_TIMER_10_BIT 10

int minimum = 26;  // (26/1024)*20ms ≒ 0.5 ms  (-90°)
int maximum = 123; // (123/1024)*20ms ≒ 2.4 ms (+90°)
int n = minimum;

//////////////////////////////////////////////////////

#include "BLEDevice.h"

#include "Wire.h"

#include "ESP32Servo.h"

#include "esp_system.h"

#define SERVICE_UUID        "e3344d55-d8b4-44f7-afa0-2813b39048cf"

#define CHARACTERISTIC_UUID "66315508-1570-47db-b6c6-84a952b860d4"

#define SERVER_NAME         "esp32devc"


// Published values for SG90 servos; adjust if needed


int servoPin = 15;

static BLEUUID  serviceUUID(SERVICE_UUID);

static BLEUUID  charUUID(CHARACTERISTIC_UUID);

static BLEAddress *pServerAddress;

static boolean doConnect = false;

static boolean connected = false;

static BLERemoteCharacteristic* pRemoteCharacteristic;

int flag = 0; //フラグを立てる

volatile int16_t axRaw_i = 0, ayRaw_i = 0 , azRaw_i = 0 ; //初期設定加速度

volatile int16_t axRaw, ayRaw, azRaw ;

volatile int mal = 0;

int val = 0;

int aaa;

int bbb;

static void notifyCallback(

  BLERemoteCharacteristic* pBLERemoteCharacteristic,

  uint8_t* pData,

  size_t length,

  bool isNotify) {

//    Serial.print("Notify callback for characteristic ");

//    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());

//    Serial.print(" of data length ");

//    Serial.println(length);

}

 

 

bool connectToServer(BLEAddress pAddress) {

//    Serial.print("Forming a connection to ");

//    Serial.println(pAddress.toString().c_str());

    BLEClient*  pClient  = BLEDevice::createClient();

    pClient->connect(pAddress);

    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);

//    Serial.print(" - Connected to server :");

//    Serial.println(serviceUUID.toString().c_str());

   if (pRemoteService == nullptr) {

//      Serial.print("Failed to find our service UUID: ");

//      Serial.println(serviceUUID.toString().c_str());

     return false;

    }

    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);

//    Serial.print(" - Found our characteristic UUID: ");

//    Serial.println(charUUID.toString().c_str());

    if (pRemoteCharacteristic == nullptr) {

//      Serial.print("Failed to find our characteristic UUID: ");

      return false;

    }

//    Serial.println(" - Found our characteristic");

    pRemoteCharacteristic->registerForNotify(notifyCallback);

}

 

 

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {

  void onResult(BLEAdvertisedDevice advertisedDevice) {

//    Serial.print("BLE Advertised Device found: ");

//    Serial.println(advertisedDevice.toString().c_str());//Name: esp32devc, Address: 30:ae:a4:02:a3:fe, txPower: -21

//    Serial.println(advertisedDevice.getName().c_str());//esp32devc

    

    if(advertisedDevice.getName()==SERVER_NAME){ //

//      Serial.println(advertisedDevice.getAddress().toString().c_str());

      advertisedDevice.getScan()->stop();

      pServerAddress = new BLEAddress(advertisedDevice.getAddress());

      doConnect = true;

    }

  }

};

 

 

void setup() {

  

  Serial.begin(115200);

// Wire.begin(21,22);

//  Serial.println("BLE_DataClient180130");

  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();

//  Serial.println("getScan");

  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());

//  Serial.println("setAdvertisedDeviceCallbacks");

  pBLEScan->setActiveScan(true);

  pBLEScan->start(10);

//  Serial.println("");

//  Serial.println("End of setup");



  Wire.beginTransmission(0x68);

  Wire.write(0x6B);

  Wire.write(0x00);

  Wire.endTransmission();

  Wire.beginTransmission(0x68);

  Wire.write(0x1C);

  Wire.write(0x10);

  Wire.endTransmission();

  Wire.beginTransmission(0x68);

  Wire.write(0x1B);

  Wire.write(0x08);

  Wire.endTransmission();

  Wire.beginTransmission(0x68);

  Wire.write(0x1A);

  Wire.write(0x05);

  Wire.endTransmission();

 

  ledcSetup(0, 50, 10);  // 0ch 50 Hz 10bit resolution

  ledcAttachPin(15, 0); // 15pin, 0ch

  ledcSetup(1, 50, 10);
  ledcAttachPin(25, 1);

  

 

}

 

 

void loop() {

 

 

  if (doConnect == true) {

    if (connectToServer(*pServerAddress)) {

//      Serial.println("We are now connected to the BLE Server.");

      connected = true;

    } else {

//      Serial.println("We have failed to connect to the server; there is nothin more we will do.");

      connected = false;

    }

    doConnect = false;

  }

  if (connected) {

//    digitalWrite(5, HIGH);

    std::string value = pRemoteCharacteristic->readValue();

    String strVal=value.c_str();

    int strNum=strVal.toInt();

    Serial.println(strNum);

    digitalWrite(4,HIGH);

//    acceleration();

    digitalWrite(4,LOW);

//    

//    if(strNum <= 20 && aaa == 0){

      if(strNum == 1){
  


       aaa = 1;


       



        } else if(strNum == 0){

//        digitalWrite(16,LOW); //ロック施錠

//        digitalWrite(17,HIGH);

        aaa = 0;

        bbb = 0;


     

       } else if(strNum == 2){

        aaa = 2; 

        }



 

 satsuei();   

 //satsuei2();



  } else{

//    Serial.println("Not connected");

    doConnect = true;

     delay(100);

  }

//  delay(1000);

//  digitalWrite(5, LOW);

 }



void acceleration() {

  int axRaw_sum = 0;

  int ayRaw_sum = 0;

  int azRaw_sum = 0;

  

 for(int i=0;i<100;i++){

  axRaw = Wire.read() << 8 | Wire.read();

  ayRaw = Wire.read() << 8 | Wire.read();

  azRaw = Wire.read() << 8 | Wire.read();

  axRaw_sum += axRaw - axRaw_i;

  ayRaw_sum += ayRaw - ayRaw_i;

  azRaw_sum += azRaw - azRaw_i;

   delay(10);

 }

  axRaw = axRaw_sum / 100;

  ayRaw = ayRaw_sum / 100;

  azRaw = azRaw_sum / 100;

    

//  Serial.print("accel_X = ");

//  Serial.print(axRaw); Serial.print(" , ");

//  Serial.print("accel_Y = ");

//  Serial.print(ayRaw); Serial.print(" , ");

//  Serial.print("accel_Z = ");

//  Serial.print(azRaw); Serial.print(" , "); 

    

  if(abs(axRaw) <= 20 || abs(ayRaw) <= 20){

    mal = 1;

//    Serial.println(mal);

    } else{

      mal = 0;

      }

}



void satsuei(){

  if(aaa == 1 && val == 1 && bbb == 0){ 

  val = 0;

  bbb = 1;


 ledcWrite(0,52);

  delay(3000);

 ledcWrite(0,76);

  ledcWrite(1,32);

  //delay(3000);

 //ledcWrite(1,76);

 

  //Serial.println("Right");

  

  } else if(aaa == 1 && val == 0 && bbb == 0){

  val = 1;

  bbb = 1;

 

  ledcWrite(0,100);

  delay(3000); 

  ledcWrite(0,76);

  ledcWrite(1,76);

  //delay(3000);

 //ledcWrite(1,76);

         


   } 




 }

   void satsuei2(){

  if(aaa == 1 && val == 1 && bbb == 0){ 
    
//////////////////////////////////////////////////////////////////////////////////////////////////////
  
  ledcWrite(LEDC_CHANNEL, 100);
  
  delay(3000);

  ledcWrite(LEDC_CHANNEL,76);
  

//////////////////////////////////////////////////////////////////////////////////////////////////////  
  } else if(aaa == 1 && val == 0 && bbb == 0){
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////        


   } 




 }  


    
