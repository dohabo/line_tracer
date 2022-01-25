#include <SoftwareSerial.h> // 블루투스
#include <Servo.h> // 서보모터
#include <MsTimer2.h> // 타이머2 인터럽트

// joystick pins
#define VRX (A0)
#define VRY (A1)

// joystick sensor values
#define VR_MIN_VALUE (0)
#define VR_MAX_VALUE (1023)
#define VR_MID_VALUE (512)
#define VR_MID_UPPER (564)
#define VR_MID_LOWER (460)

char getData = '1'; //블루토스 통신 수신 값 받는 함수

SoftwareSerial BTSerial (11, 12); // 블루투스 핀 설정

// 조이스틱 값 변환 함수
int adjustJoystickValue(int value) {
  value = constrain(value, VR_MIN_VALUE, VR_MAX_VALUE);
  if (VR_MID_LOWER <= value && value <= VR_MID_UPPER) 
    return VR_MID_VALUE;
  else
    return value;
}

// 조이스틱 핀 및 설정 값
const int SW4_pin = 7;           // 블루투스 자동/수동 버튼 핀(1=>자동,0=>수동)
const int SW1_pin = 10;          // 차량 라인트레이서 모드 변경 버튼
const int SW3_pin = 9;           // 차량 정지 버튼(수동 모드 전환)

boolean buttonState;            //현재 버튼 상태
boolean lastButtonState = HIGH; //마지막 버튼 상태
long lastDebounceTime = 0;      //마지막 버튼 디바운스 타임
long debounceDelay = 30;        //디바운스 딜레이
int BT2State = 1;                   // 조이스틱 블루투스 상태값(1=> 사용 못함, 0 => 조이스틱 사용)


  // 물건 감지용 적외선
int sensor = 6; // 적외선 센서 핀
int count = 0; // 적외선 감지 카운트 변수

  // 로봇 핀
Servo Servo1;
Servo Servo2;
Servo Servo3;
Servo Servo4;


// 물건 감지용 적외선 센서 작동 함수
void flash(){
  int data = digitalRead(sensor);
  if(data == LOW){
    count++;
    BTSerial.write("c"); // 차량 출발
    delay(300);
  }
}

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
//  Serial2.begin(9600);
  pinMode(SW4_pin, INPUT_PULLUP);//버튼을 pull up 입력으로 설정
  pinMode(SW1_pin, INPUT_PULLUP);//버튼을 pull up 입력으로 설정
  pinMode(SW3_pin, INPUT_PULLUP);//버튼을 pull up 입력으로 설정
  pinMode(sensor,INPUT);
  MsTimer2::set(1, flash);
  MsTimer2::start();
  Servo1.attach(2);
  Servo2.attach(3);
  Servo3.attach(4);
  Servo4.attach(5);
  }

void loop() {
  //////블루투스 송, 수신
  if(BTSerial.available()){
    getData = BTSerial.read();
    }
//  
////  //SW4 녹색 버튼 채터링 /초기값 :1, 조이스틱 블루투스 상태값(1=> 사용 못함, 0 => 조이스틱 사용)
//  int reading = digitalRead(SW4_pin);
//  if(reading != lastButtonState){
//    lastDebounceTime = millis();
//  }
//
//  if((millis() - lastDebounceTime) > debounceDelay){
//    if(reading != buttonState){
//      buttonState = reading;
//      if(buttonState == LOW){
//      BT2State =! BT2State;
//      BTSerial.write('0');
//      //if(digitalRead(SW1_pin) == LOW){
//        //BTSerial.write('a');
//      //}
//      }
//    }
//  }
////  //SW1 빨간색 버튼 채터링(차량 정지, 조이스틱 모드)
//// if(digitalRead(SW1_pin) == LOW){
////  BTSerial.write('1');
////  Serial.print("SW1_Pin");
//// }  
////  
//////  //SW3 파란색 버튼 채터링(차량 라인트레이서 모드)
////   if(digitalRead(SW3_pin) == LOW){
////  BTSerial.write('0');
////    Serial.print("SW3_Pin");
//// }
//
  // 블루투스로 'r'수신 시 로봇 작동
 if(getData == 'r'){
    robotStart();
    BTSerial.write("d");
    getData = '1';
 }

// //조이스틱 모드 전환 시 작동됨(녹색 버튼 누를 시)
//  if(BT2State == 0){
//    joystick();
//  }
//  lastButtonState = reading; // 녹색버튼 1, 0 상태값 변수(조이스틱 사용모드 or NONE)
}

//조이스틱 함수
void joystick(){
// read joystick values
  int vrx = analogRead(VRX);
  int vry = analogRead(VRY);

  // ignore 10% error around 512 since it not accurate
  vrx = adjustJoystickValue(vrx);
  vry = adjustJoystickValue(vry);

  // send data over BLE
  BTSerial.print(vrx);  
  BTSerial.print(",");
  BTSerial.println(vry);
  Serial.println(vrx,vry);
}

  // A구역에서 차량에 물건 적재하는 로봇 동작
void robotStart(){
  Servo1.write(60);
  Servo2.write(30);
  Servo3.write(30);
  Servo4.write(90);
  delay(500);
  // put your main code here, to run repeatedly:
  // Servo1 구동
  for(int i = 60; i<=120; i++){
    Servo1.write(i);
    delay(50);
  }
  delay(1000);

  // Servo2 구동
  for(int i = 30;i<=90;i++){
    Servo2.write(i);
    delay(50);
  }
delay(1000);

  // Servo3 구동
  for(int i = 30; i>=0;i--){
    Servo3.write(i);
    delay(50);
  }
  for(int i = 90; i<=150;i++){
    Servo4.write(i);
    delay(50);
  }
  delay(1000);

  for(int i = 90; i>=30;i--){
    Servo2.write(i);
    delay(50);
  }
  delay(1000);
  for(int i = 120; i>=20; i--){
    Servo1.write(i);
    delay(50);
  }
  delay(1000);
  // Servo2 구동
  for(int i = 30;i<=90;i++){
    Servo2.write(i);
    delay(50);
  }
  for(int i = 150; i>=90;i--){
    Servo4.write(i);
    delay(50);
  }
  delay(1000);
  for(int i = 90; i>=30;i--){
    Servo2.write(i);
    delay(50);
  }
  count = 0;
  delay(2000);
}

void robotStart2(){
Serial.println("로봇작동시작");
  Servo1.write(60);
  Servo2.write(30);
  Servo3.write(30);
  Servo4.write(90);
  delay(500);
  // put your main code here, to run repeatedly:
  // Servo1 구동
  delay(1000);
  for(int i = 120; i>=20; i--){
    Servo1.write(i);
    delay(50);
  }
    delay(1000);
  // Servo2 구동
  for(int i = 30;i<=90;i++){
    Servo2.write(i);
    Serial.print("Servo2:");
    Serial.println(i);
    delay(50);
  }
  delay(1000);
  // Servo3 구동
  for(int i = 30; i>=0;i--){
    Servo3.write(i);
    Serial.print("Servo3:");
    Serial.println(i);
    delay(50);
  }
  delay(1000);
  for(int i = 90; i<=150;i++){
    Servo4.write(i);
    delay(50);
  }
  delay(1000);
    // Servo2 구동
    for(int i = 90; i>=30;i--){
    Servo2.write(i);
    Serial.print("Servo2:");
    Serial.println(i);
    delay(50);
  }
  delay(1000);
  for(int i = 20; i<=120; i++){
    Servo1.write(i);
    delay(50);
  }
  delay(1000);
 for(int i = 30;i<=90;i++){
    Servo2.write(i);
    Serial.print("Servo2:");
    Serial.println(i);
    delay(50);
  }
delay(1000);
   for(int i = 150; i>=90;i--){
    Servo4.write(i);
    delay(50);
  }

  delay(1000);
    // Servo2 구동
    for(int i = 90; i>=30;i--){
    Servo2.write(i);
    Serial.print("Servo2:");
    Serial.println(i);
    delay(50);
  }
  delay(1000);
}
