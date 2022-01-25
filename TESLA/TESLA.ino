#include <MsTimer2.h> // 타이머2 인터럽트
#include <NewPing.h>        // 초음파 센서로 거리값을 쉽게 얻을 수 있는 라이브러리
#include <Servo.h>

// 블루투스
char getData = 1;

// 모터 드라이브 핀
#define EA 3      //***적용
#define EB 8      //***적용
#define M_IN1 4   //***적용
#define M_IN2 5   //***적용
#define M_IN3 9   //***적용
#define M_IN4 10  //***적용

// DC모터
#define MOTOR_MIN_PWM 110
#define MOTOR_MAX_PWM 255
#define MOTOR_DRIVE_PWM 110     //-- 적외선센서 변경사항
#define MOTOR_TURN_DELAY 2000

// 적외선 센서
#define R_Sensor 39    //***적용
#define CR_Sensor 40   //***적용
#define CL_Sensor 41   //***적용
#define L_Sensor 38    //***적용
// 초음파 센서 핀
#define ECHO 31              // 수신부
#define TRIG 30              // 송신부
// 최소, 최대, 지연 (초음파 센서 측정 가능 거리: 20mm ~ 4500mm)
#define SONAR_MIN_DISTANCE 3
#define SONAR_MAX_DISTANCE 200
#define SONAR_DELAY 25
NewPing sonar(TRIG, ECHO, SONAR_MAX_DISTANCE);
long frontDistance = 0;

long frontSensing(int loop)
{
  long duration, distance;
  long avgDistance = 0;

  // 센서가 튀는 현상이 있어서 5번 측정하여 평균값으로 판단
  for (int i = 0; i < loop; i++)
  {
    digitalWrite(TRIG, HIGH);  // trigPin에서 초음파 발생(echoPin도 HIGH)
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
  
    duration = pulseIn(ECHO, HIGH);    // echoPin 이 HIGH를 유지한 시간을 저장 한다.
    distance = ((float)(340 * duration) / 1000) / 2;    
    avgDistance = avgDistance + distance;
    delay(1);
  }
  avgDistance = avgDistance / loop;
  return avgDistance;
}

// LED
int LED_G = 6;         //***적용
int LED_R = 7;         //***적용
volatile int LED_G_State = 1;   // LED초록 상태값 //***적용
volatile int LED_R_State = 0;   // LED빨강 상태값 //***적용

// led 인터럽트 함수
void flash(){
  digitalWrite(LED_G, LED_G_State);
  digitalWrite(LED_R, LED_R_State);
}

// 조이스틱 동작 관련
#define VR_MIN_VALUE (0)
#define VR_MAX_VALUE (1023)
int convertToPWM(int value) {
  return map(value, VR_MIN_VALUE, VR_MAX_VALUE, -MOTOR_MAX_PWM, MOTOR_MAX_PWM);
}

void setup() {
  Serial.begin(9600);
  Serial.print("통신시작");
  // 블루투스
  Serial2.begin(9600);
  Serial1.begin(9600);

  // 핀모드
  pinMode(EA, OUTPUT);
  pinMode(M_IN1, OUTPUT);
  pinMode(M_IN2, OUTPUT); 
  
  pinMode(EB, OUTPUT);
  pinMode(M_IN3, OUTPUT);  
  pinMode(M_IN4, OUTPUT); 

  pinMode(R_Sensor, INPUT);  
  pinMode(CR_Sensor, INPUT);  
  pinMode(CL_Sensor, INPUT);
  pinMode(L_Sensor, INPUT);

  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
  MsTimer2::set(1, flash);
  MsTimer2::start();
}


void loop() {

//  // hm-10 블루투스 연결 시, 값을 getData 변수에 저장(변수: 1- 조이스틱모드 0 - 라인트레이서모드) 
    frontDistance = frontSensing(5);
   if(frontDistance < 100){
    movePattern();
  }

  if(Serial2.available()){
    getData = Serial2.read();
  }
//    int vrx = Serial2.parseInt();
//    int vry = Serial2.parseInt();
//
//    Serial.print("getData:");
//    Serial.print(getData);
//    Serial.print("vrx:");
//    Serial.print(vrx);
//    Serial.print("vry:");
//    Serial.print(vry);
//    
//    
//    int pwmX = convertToPWM(vrx);
//    int pwmL = pwmX;
//    int pwmR = pwmX;
//
//    int pwmY = convertToPWM(vry);
//    pwmL = pwmL + pwmY;
//    pwmR = pwmR - pwmY;
//
//    // happy drive!
//    drive(pwmL, pwmR);

  if(getData == 'c'){
  linetracer();
  }
//  else if(getData == 'b'){
//    if(Serial2.available()){
//    int vrx = Serial2.parseInt();
//    int vry = Serial2.parseInt();
//        
//    // calculate speed of both motors according to the x axis
//    int pwmX = convertToPWM(vrx);
//    int pwmL = pwmX;
//    int pwmR = pwmX;
//  
//    // adjust left and right speed according to the y axis
//    int pwmY = convertToPWM(vry);
//    pwmL = pwmL + pwmY;
//    pwmR = pwmR - pwmY;
//
//    // happy drive!
//    drive(pwmL, pwmR);
//  }
//  }
  else if(getData == 'd'){
    drive(120,120);
      delay(500);
      Serial.print("차량다시출발!");
      getData = 'c';
  }
 }

// 라인 트레이서 작동 함수
void linetracer(){  
  if(digitalRead(CR_Sensor) == HIGH && digitalRead(CL_Sensor) == HIGH && digitalRead(R_Sensor) == HIGH &&digitalRead(L_Sensor) == HIGH){
    LED_G_State = 1;
    LED_R_State = 0;
    drive(0,0);
    Serial2.write('r');
    getData = '1';
  }
  
  else if(digitalRead(CR_Sensor) == HIGH && digitalRead(CL_Sensor) == HIGH){
    //디버깅용
    LED_G_State = 0;
    LED_R_State = 1;
    drive(MOTOR_DRIVE_PWM, MOTOR_DRIVE_PWM);
  }
  else if(digitalRead(CR_Sensor) == LOW && digitalRead(CL_Sensor) == HIGH){
    //디버깅용
    LED_G_State = 0;
    LED_R_State = 1;
    drive(130, 0);
  }
  else if(digitalRead(CR_Sensor) == HIGH && digitalRead(CL_Sensor) == LOW){
    //디버깅용
    LED_G_State = 0;
    LED_R_State = 1;
    drive(0, 130);
  }
  else if(digitalRead(R_Sensor) == HIGH)
  {
    //디버깅용
    LED_G_State = 0;
    LED_R_State = 1;
    drive(-MOTOR_DRIVE_PWM, MOTOR_DRIVE_PWM);
  }
  else if(digitalRead(L_Sensor) == HIGH)
  {
    LED_G_State = 0;
    LED_R_State = 1;
    drive(MOTOR_DRIVE_PWM, -MOTOR_DRIVE_PWM);
  }
}

// 모터 방향 조절 함수
void drive(int pwmA, int pwmB) {
  pwmA = constrain(pwmA, -MOTOR_MAX_PWM, MOTOR_MAX_PWM); // 센서값을 -MAX ~ MAX 사이로 제한
  pwmB = constrain(pwmB, -MOTOR_MAX_PWM, MOTOR_MAX_PWM);

  // A모터 방향
  if (pwmA > 0) {
    digitalWrite(M_IN1, LOW);    // 정방향
    digitalWrite(M_IN2, HIGH);
  } else if (pwmA < 0) {
    digitalWrite(M_IN1, HIGH);     // 역방향
    digitalWrite(M_IN2, LOW);
  } else {
    digitalWrite(M_IN1, LOW);     // 멈춤
    digitalWrite(M_IN2, LOW);
  }

  // B모터 방향
   if (pwmB > 0) {
    digitalWrite(M_IN3, HIGH);     // 정방향
    digitalWrite(M_IN4, LOW);
  } else if (pwmB < 0) {
    digitalWrite(M_IN3, LOW);    // 역방향
    digitalWrite(M_IN4, HIGH);
  } else {
    digitalWrite(M_IN3, LOW);     // 멈춤
    digitalWrite(M_IN4, LOW);
  }

  if (abs(pwmA) < MOTOR_MIN_PWM) pwmA = 0;
  if (abs(pwmB) < MOTOR_MIN_PWM) pwmB = 0;

  analogWrite(EA, abs(pwmA));
  analogWrite(EB, abs(pwmB));
}

void movePattern(){
    drive(0,0);
    delay(1000);
    drive(-120,120);
    delay(600);
    drive(120,120);
    delay(9000);
    drive(120,-120);
    delay(400);
    drive(120,120);
    delay(600);
    drive(120,-120);
    delay(400);
    drive(120,120);
    delay(600);
  }
