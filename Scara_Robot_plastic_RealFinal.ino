#include <Servo.h>
#define EN 8
#define X_DIR 5
#define Y_DIR 6
#define Z_DIR 7
#define W_DIR 13
#define X_STP 2
#define Y_STP 3
#define Z_STP 4
#define W_STP 12
#define X_photo 9
#define Y_photo 10
#define Z_photo 11
#define W_photo A3
#define X_photo_init A2        
#define Switch1 A1
#define servoPin A0

Servo myservo; 
        
//serial_num받아오는 변수
int serial_num=0;
int serial_box_num=0;
int mode_select_flag = 0;
void setup ()
{
  //connect(영준수정)
  Serial.begin(9600);
  Serial.setTimeout(50);
  /////////limit switch///////////
   pinMode(X_photo, INPUT); 
   pinMode(Y_photo, INPUT); 
   pinMode(Z_photo, INPUT);
   pinMode(W_photo, INPUT);
   pinMode(Switch1, INPUT); 
   ////////step motor//////////
   pinMode (X_DIR, OUTPUT); pinMode (X_STP, OUTPUT);
   pinMode (Y_DIR, OUTPUT); pinMode (Y_STP, OUTPUT);
   pinMode (Z_DIR, OUTPUT); pinMode (Z_STP, OUTPUT);
   pinMode (W_DIR, OUTPUT); pinMode (W_STP, OUTPUT);
   pinMode (EN, OUTPUT);
   digitalWrite (EN, LOW);
   ////////servo motor
   myservo.attach(servoPin, 500, 2500);
}
void loop ()
{
     //serial 받아오기
     serial_num = Serial.read();
        //스위치 값 받아오기
     if(serial_num == 10)
     {
        mode_select_flag = 1;
     }
     else if(serial_num ==20)
     {
        mode_select_flag = 2;
     }
     switch (mode_select_flag)
     {
      case 1:
        AutoMode();
        mode_select_flag=0;
        break;
      case 2:
        if(serial_num == 11)
        {
          x_cw_run();
        }
        else if(serial_num == 12)
        {
          x_ccw_run();
        }
        else if(serial_num == 21)
        {
          y_cw_run();
        }
        else if(serial_num == 22)
        {
          y_ccw_run();
        }
        else if(serial_num == 31)
        {
          z_up_run();
        }
        else if(serial_num == 32)
        {
          z_down_run();
        }
        else if(serial_num == 41)
        {
          w_cw_run();
        }
        else if(serial_num == 42)
        {
          w_ccw_run();
        }
        else if(serial_num == 51)
        {
          gripper_open();
        }
        else if(serial_num == 52)
        {
          gripper_close();
        }
     }
}
////////////////////////////////오토모드///////////////////////////////
//집고오는 형식
void AutoMode() //Z축 포토인터럽트 X축 포토인터럽트 하나 추가가 됐을 때 플로우
{
  To_init();
  To_Waitplace_From_init();
  To_DarkBox_From_Waitplace_in();
  To_Waitplace_From_DarkBox();
  delay(2000);
  Serial.println(5);
  delay(2000);
  To_DarkBox_From_Waitplace_out();
  serial_box_num = Serial.read();
  switch (serial_box_num)
  {
    case 1:
      To_Box1_From_DarkBox();
      break;
    case 2:
      To_Box2_From_DarkBox();
      break;
    case 3:
      To_Box3_From_DarkBox();
      break;
    }
}
////////////////////////오토모드 도와주는 함수 To_(도착)_From_(출발)///////////////////
void To_Waitplace_From_init()
{
  x_ccw100();
  gripper_open();
  w_ccw20();
  delay(1000);
  down();
  delay(1000);
  gripper_close();
  delay(1000);
  To_DarkBox_Z();
}
void To_DarkBox_From_init()
{
  To_DarkBox_Z();
  To_DarkBox_Y();
  To_DarkBox_X();
  To_DarkBox_W();
  gripper_open();
  delay(1000);
  down();
  delay(1000);
  gripper_close();
  delay(1000);
  To_DarkBox_Z();
}
void To_Waitplace_From_DarkBox()
{
  x_cw40();
}
void To_DarkBox_From_Waitplace_in()
{
  To_DarkBox_Z();
  To_DarkBox_Y();
  To_DarkBox_X();
  To_DarkBox_W();
  down();
  delay(1000);
  gripper_open();
  delay(1000);
  To_DarkBox_Z();
}
void To_DarkBox_From_Waitplace_out()
{
  To_DarkBox_Z();
  To_DarkBox_Y();
  To_DarkBox_X();
  To_DarkBox_W();
  gripper_open();
  delay(1000);
  down();
  delay(1000);
  gripper_close();
  delay(1000);
  To_DarkBox_Z();
}
void To_Box1_From_DarkBox()
{
  x_cw100();
  y_cw80();
  gripper_open();
}
void To_Box2_From_DarkBox()
{
  x_cw90();
  y_cw80();
  gripper_open();
}
void To_Box3_From_DarkBox()
{
  x_cw80();
  y_clockwise();   //영준이가 바꿨따
  gripper_open();
}
void To_init()
{
  To_DarkBox_Z();
  To_Xinit();
  To_DarkBox_Y();
  To_DarkBox_W();
  gripper_open();
}
//PHOTO
//포토인터럽트 감지시 멈추는 함수
void To_Xinit()
{ 
  digitalWrite (EN, LOW);
  digitalWrite (X_DIR, true);
  delay (50);
  while(1)
  {
    if(analogRead(X_photo_init) <500)
    {
      break;
    }
     digitalWrite (X_STP, HIGH);
     delayMicroseconds (1500);
     digitalWrite (X_STP, LOW);
     delayMicroseconds (1500);
  }
}
void To_DarkBox_X()
{ 
  digitalWrite (EN, LOW);
  digitalWrite (X_DIR, false);
  delay (50);
  while(1)
  {
    if(digitalRead(X_photo) == LOW)
    {
      break;
    }
     digitalWrite (X_STP, HIGH);
     delayMicroseconds (1500);
     digitalWrite (X_STP, LOW);
     delayMicroseconds (1500);
  }
}
void To_DarkBox_Y()
{
  digitalWrite (EN, LOW);
  digitalWrite (Y_DIR, false);
  delay (50);

  while(1)
  {
    if(digitalRead(Y_photo) == LOW)
    {
      break;
    }
     digitalWrite (Y_STP, HIGH);
     delayMicroseconds (1500);
     digitalWrite (Y_STP, LOW);
     delayMicroseconds (1500);
  }
}
void To_DarkBox_Z()
{
  digitalWrite (EN, LOW);
  digitalWrite (Z_DIR, true);
  delay (50);

  while(1)
  {
    if(digitalRead(Z_photo) == HIGH) //걸릴때가 high!!
    {
      break;
    }
     digitalWrite (Z_STP, HIGH);
     delayMicroseconds (1500);
     digitalWrite (Z_STP, LOW);
     delayMicroseconds (1500);
  }
}
void To_DarkBox_W()
{
  digitalWrite (EN, LOW);
  digitalWrite (W_DIR, false);
  delay (50);

  while(1)
  {
    if(digitalRead(W_photo) == HIGH)
    {
      break;
    }
     digitalWrite (W_STP, HIGH);
     delayMicroseconds (1500);
     digitalWrite (W_STP, LOW);
     delayMicroseconds (1500);
  }
}
///////////////////////////시리얼통신/////////////////////////////
///////////////////////////수동 조작/////////////////////////////
//수동조작 : 버튼누를 때 동작 안누르면 동작X
void x_cw_run()
{
   digitalWrite(EN, LOW); //스텝모터 활성화
   digitalWrite(X_DIR, true);  // 방향: 역회전
   delay(50);
   while (1) //회전 시작
   {
      serial_num=Serial.read();
      if (digitalRead(X_photo_init)  == LOW)
      {
         digitalWrite(EN, HIGH);
         break;
      }//센서 접촉시
      else if (serial_num == 4) //버튼 뗄때
      {
         digitalWrite(EN, HIGH);
         break;
      }// 멈춤
      else
      {
         digitalWrite(X_STP, HIGH);
         delayMicroseconds(1500);
         digitalWrite(X_STP, LOW);
         delayMicroseconds(1500);
      }// 작동
   }//while
}// x_cw_run
void x_ccw_run()
{
   digitalWrite(EN, LOW); //스텝모터 활성화
   digitalWrite(X_DIR, false);  // 방향: 역회전
   delay(50);
   while (1) //회전 시작
   {
      serial_num=Serial.read();
      if (digitalRead(X_photo) == LOW)
      {
         digitalWrite(EN, HIGH);
         break;
      }
      else if (serial_num == 4) //버튼 뗄때
      {
         digitalWrite(EN, HIGH);
         break;
      }
      else
      {
         digitalWrite(X_STP, HIGH);
         delayMicroseconds(1500);
         digitalWrite(X_STP, LOW);
         delayMicroseconds(1500);
      }
   }
}
void y_cw_run()
{
  digitalWrite (EN, LOW); //스텝모터 활성화
  digitalWrite (Y_DIR, true);  // 방향: 정회전
  delay (50);
  while(1) //회전 시작
  {
     serial_num=Serial.read(); 
      if (digitalRead(Y_photo) == LOW)
      {
         digitalWrite(EN, HIGH);
         break;
      }//센서 접촉시
      else if (serial_num == 4) //버튼 뗄때
      {
         digitalWrite(EN, HIGH);
         break;
      }// 멈춤
      else
      {
         digitalWrite(Y_STP, HIGH);
         delayMicroseconds(1500);
         digitalWrite(Y_STP, LOW);
         delayMicroseconds(1500);
      }// 작동
  }
}
void y_ccw_run()
{
   digitalWrite (EN, LOW); //스텝모터 활성화
   digitalWrite (Y_DIR, false);  // 방향: 역회전
   delay (50);
   while (1) //회전 시작
   {
      serial_num=Serial.read();
      if (digitalRead(Y_photo) == LOW)
      {
         digitalWrite(EN, HIGH);
         break;
      }
      else if (serial_num == 4)//버튼 뗄때
      {
         digitalWrite(EN, HIGH); //모든 스텝모터 비활성화
         break;
      }
      else
      {
         digitalWrite(Y_STP, HIGH);
         delayMicroseconds(1500);
         digitalWrite(Y_STP, LOW);
         delayMicroseconds(1500);
      }
   }
}
void z_up_run()
{
  digitalWrite (EN, LOW); //스텝모터 활성화
  digitalWrite (Z_DIR, true);  // 방향: 정회전
  delay (50);
  while(1) //회전 시작
  {
     serial_num=Serial.read();     
      if (digitalRead(Z_photo) == HIGH)
      {
         digitalWrite(EN, HIGH);
         break;
      }//센서 접촉시
      else if (serial_num == 4) //버튼 뗄때
      {
         digitalWrite(EN, HIGH);
         break;
      }// 멈춤
      else
      {
         digitalWrite(Z_STP, HIGH);
         delayMicroseconds(1500);
         digitalWrite(Z_STP, LOW);
         delayMicroseconds(1500);
      }// 작동
  }
}
void z_down_run()
{
  digitalWrite (EN, LOW); //스텝모터 활성화
  digitalWrite (Z_DIR, false);  // 방향: 역회전
  delay (50);
  while(1) //회전 시작
  {
     serial_num=Serial.read();
     if (digitalRead(Z_photo) == HIGH)
      {
         digitalWrite(EN, HIGH);
         break;
      }//센서 접촉시
      else if (serial_num == 4) //버튼 뗄때
      {
         digitalWrite(EN, HIGH);
         break;
      }// 멈춤
      else
      {
         digitalWrite(Z_STP, HIGH);
         delayMicroseconds(1500);
         digitalWrite(Z_STP, LOW);
         delayMicroseconds(1500);
      }// 작동

  }
}
void w_cw_run()
{
  digitalWrite (EN, LOW); //스텝모터 활성화
  digitalWrite (W_DIR, false);  // 방향: 정회전
  delay (50);
  while(1) //회전 시작
  {
     serial_num=Serial.read();
     if (digitalRead(W_photo) == HIGH)
      {
         digitalWrite(EN, HIGH);
         break;
      }//센서 접촉시
      else if (serial_num == 4) //버튼 뗄때
      {
         digitalWrite(EN, HIGH);
         break;
      }// 멈춤
      else
      {
         digitalWrite(W_STP, HIGH);
         delayMicroseconds(1500);
         digitalWrite(W_STP, LOW);
         delayMicroseconds(1500);
      }// 작동
  }
}
void w_ccw_run()
{
  digitalWrite (EN, LOW); //스텝모터 활성화
  digitalWrite (W_DIR, true);  // 방향: 정회전
  delay (50);
  while(1) //회전 시작
  {
     serial_num=Serial.read();
     if (digitalRead(W_photo) == HIGH)
      {
         digitalWrite(EN, HIGH);
         break;
      }//센서 접촉시
      else if (serial_num == 4) //버튼 뗄때
      {
         digitalWrite(EN, HIGH);  
         break;
      }// 멈춤
      else
      {
         digitalWrite(W_STP, HIGH);
         delayMicroseconds(1500);
         digitalWrite(W_STP, LOW);
         delayMicroseconds(1500);
      }// 작동
  }
}
/////////////////그리퍼조작//////////////////////
void gripper_open()
{
     myservo.write(50);
}

void gripper_close()
{
    myservo.write(160);
}
////////////////스텝모터 동작함수//////////////////////////////
void step (boolean dir, byte dirPin, byte stepperPin, int steps)
{
  digitalWrite (EN, LOW);
  digitalWrite (dirPin, dir);
  delay (50);
  
  for (int i = 0; i<steps; i++)
  {
      digitalWrite (stepperPin, HIGH);
      delayMicroseconds (1500);
      digitalWrite (stepperPin, LOW);
      delayMicroseconds (1500);
  }
}
/////////////////////x, y, z동작함수////////////////////////
void y_clockwise()
{
  step(true, Y_DIR, Y_STP, 450);
}

void x_cw40() // y축 90도 이동 [시계방향]
{
  step(true, X_DIR, X_STP, 800);
}
void x_ccw40() // y축 90도 이동 [반시계방향]
{
  step(false, X_DIR, X_STP, 800);
}
void x_cw80() // x축 90도 이동 [시계방향]
{
  step(true, X_DIR, X_STP, 880);
}
void x_ccw80() // x축 90도 이동 [반시계방향]
{
  step(false, X_DIR, X_STP, 1200);
}
void x_cw90() // x축 90도 이동 [시계방향]
{
  step(true, X_DIR, X_STP, 1398);
}
void x_ccw90() // x축 90도 이동 [반시계방향]
{
  step(false, X_DIR, X_STP, 1400);
}
void x_cw100() // x축 90도 이동 [시계방향]
{
  step(true, X_DIR, X_STP, 1700);
}
void x_ccw100() // x축 90도 이동 [반시계방향]
{
  step(false, X_DIR, X_STP, 1700);
}
void x_ccw180() // x축 180도 이동 [반시계방향]
{
  step(false, X_DIR, X_STP, 2500);
}
void y_cw80() // y축 90도 이동 [시계방향]
{
  step(true, Y_DIR, Y_STP, 600);
}
void y_ccw80() // y축 90도 이동 [반시계방향]
{
  step(false, Y_DIR, Y_STP, 600);
}
void y_cw90() // y축 90도 이동 [시계방향]
{
  step(true, Y_DIR, Y_STP, 700);
}
void y_ccw90() // y축 90도 이동 [반시계방향]
{
  step(false, Y_DIR, Y_STP, 700);
}
void up() // z축 90도 이동 [시계방향]
{
  step(true, Z_DIR, Z_STP, 800);
}
void down() // z축 90도 이동 [반시계방향]
{
  step(false, Z_DIR, Z_STP, 1500);
}
void w_cw90() // y축 90도 이동 [시계방향]
{
  step(false, W_DIR, W_STP, 700);
}
void w_ccw90() // y축 90도 이동 [반시계방향]
{
  step(true, W_DIR, W_STP, 700);
}
void w_cw20() // y축 90도 이동 [시계방향]
{
  step(false, W_DIR, W_STP, 350);
}
void w_ccw20() // y축 90도 이동 [반시계방향]
{
  step(true, W_DIR, W_STP, 163);
}
