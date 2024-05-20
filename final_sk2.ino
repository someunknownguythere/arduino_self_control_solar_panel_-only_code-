
//định nghĩa chân pin cắm trên arduino, các giá trị giới hạn và các biến

#define redpin 10
#define greenpin 8
#define bluepin 9
#define inp_but 2
#include <Servo.h>
  Servo Servo_Auto;
    int Auto_pos;
    int Auto_pin = 5;
  Servo Servo_manual;
    int Manual_pin = 3;
    int Manual_pos;
  int Mode;
  int Mode_ButtonState;
  int Mode_Button_prev_State;
  int down_limit = 100;
  int Penalty;
  int Mode_before;


void setup() {
  pinMode(inp_but, INPUT_PULLUP); // sử dựng input_pullup cho tín hiệu ổn định hơn chút, thực ra INPUT cũng được
  pinMode(redpin, OUTPUT);//setup 3 pin rgb
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  Serial.begin(9600); // debug purpose
  Servo_Auto.attach(Auto_pin);//setup cho động cơ servo
  Servo_manual.attach(Manual_pin);
  Mode = 0; //def mode = manual
}

void loop() {
  Mode_button_check(); //kiểm tra input nút bấm
  // kiểm soát nếu ánh sáng quá yếu (đêm) sẽ đưa vào trạng thái nghỉ (ko áp dụng khi đang điều chỉnh tay)
  // mode 0-manual, 1-stop, 2-manual+auto
  if (Mode != 0) {
      if ((analogRead(A0) + analogRead(A1)) <= down_limit) {
      Mode = 1;
      Servo_Auto.write(90);
     Servo_manual.write(90);
      Penalty = 1;
    }else {
      if (Penalty == 1) {
        Penalty = 0;
        Mode = 2;
    }
  }
  }
  choose(Mode);
  // put your main code here, to run repeatedly:
  Led_indicate (redpin, bluepin, greenpin, Mode); // điều khiển led theo mode 0-green, 1-red, 2-blue
  delay(20); //delay buộc phải có để có chuyển động chuẩn

}
void choose (int Mode) { //function điều khiển chọn mode
  
  if (Mode == 0) {
    Mode_Manual();
  } else if (Mode == 1) {
    Mode_stop();
  } else if (Mode == 2) {
    Mode_Auto();
  }
}

  void Mode_Manual(){ // code mode 0
    Servo_Auto.write(Servo_Auto.read() + joystick_var(A2));
    Servo_manual.write(Servo_manual.read() + joystick_var(A3));
    delay(10);
    
  }
  void Mode_Auto(){ //code mode 2
    Auto_pos = Servo_Auto.read();
    Auto_pos = Auto_pos + so_sanh(analogRead(A0), analogRead(A1), 30, 3);
    Servo_manual.write(Servo_manual.read() + joystick_var(A3));
    Servo_Auto.write(Auto_pos);
    delay(10);
  }
  void Mode_stop(){ // code mode 1 (failsafe)
    Servo_Auto.write(Servo_Auto.read());
    Servo_manual.write(Servo_manual.read());
  }



  int so_sanh (int final_1, int final_2, int sai_so, int speed){  // so sánh giá trị 2 quang trở
    if ( abs(final_1 - final_2) <= sai_so ) {
      return 0;
    } else {
      if (final_1 < final_2) {
        return 1*speed;
      }
      if (final_1 > final_2) {
        return -1*speed;
      }
    }
    delay (10);
  }

  int joystick_var (int pin) {  // đọc giá trị joystick
    int val = analogRead(pin);
      val = map(val, 0,1023,-2,2);
      return val;
  }
  int Mode_change (int Mode_rn) { //function điều khiển xoay vòng mode
    if (Mode_rn == 0) {
      Mode_rn = 1;
    }
    else if (Mode_rn == 1) {
      Mode_rn = 2;
    }
    else if (Mode_rn == 2) {
      Mode_rn = 0;
    }
    return Mode_rn;
  }
  void Mode_button_check() { // kiểm thử khi có nút nhấn
        Mode_ButtonState = digitalRead(2);
    if (Mode_ButtonState != Mode_Button_prev_State) {
    if (Mode_ButtonState == LOW ) {
      Serial.println("pressed");
      Mode = Mode_change (Mode);
    }
  }
  Mode_Button_prev_State = Mode_ButtonState;
  }

  void Led_indicate (int red, int blue, int green, int Mode) {  // điều khiển led
    if (Mode == 0) {
      digitalWrite(red, 0);
      digitalWrite(blue, 0);
      digitalWrite(green, 1);
    } else if (Mode == 1) {
      digitalWrite(red, 1);
      digitalWrite(blue, 0);
      digitalWrite(green, 0);
    } else if (Mode == 2) {
      digitalWrite(red, 0);
      digitalWrite(blue, 1);
      digitalWrite(green, 0);
    }
  }
