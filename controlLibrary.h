#ifndef CONTROLLIBRARY_H
#define CONTROLLIBRARY_H

#include <Servo.h> // สำหรับควบคุมเซอร์โว
#include "pidLibrary.h" // สำหรับใช้ฟังก์ชันควบคุม PID
// #include "controlLibrary.h" (ไฟล์นี้คือโค้ดที่เรากำลังรวม)

// ===================================
// กำหนดขาใช้งานและตัวแปร (Pins and Variables)
// ===================================

// ขาควบคุมมอเตอร์ซ้าย (Motor Left)
#define sp_L 6 // สปีดซ้าย (PWM)
#define F_L 12 // เดินหน้าซ้าย
#define B_L 11 // ถอยหลังซ้าย

// ขาควบคุมมอเตอร์ขวา (Motor Right)
#define sp_R 3 // สปีดขวา (PWM)
#define F_R 7 // เดินหน้าขวา
#define B_R 8 // ถอยหลังขวา

#define STBY 10 // ขา Standby ของไดรเวอร์มอเตอร์ TB6612

// ขาควบคุมเซอร์โว (Servo Pins)
#define x_pin 5 // ขาควบคุมเซอร์โวแกน x (แขนหนีบ)
#define y_pin 4 // ขาควบคุมเซอร์โวแกน y (แขนยก)

Servo servo_x; // ออบเจกต์สำหรับเซอร์โวแกน x
Servo servo_y; // ออบเจกต์สำหรับเซอร์โงแกน y

// เซนเซอร์ (Sensor)
int sensorPin[8] = {A0, A1, A2, A3, A4, A5, A6, A7}; // กำหนดขาอนาล็อก A0-A7 สำหรับเซนเซอร์ตรวจจับเส้น
String detectLine = "00000000"; // ตัวแปรเก็บสถานะเซนเซอร์เป็น String 8 ตัว

// ตัวแปรควบคุมความเร็ว
int sp = 50; // ความเร็วเริ่มต้น (Set Point Speed)
int maxSp = 255; // ความเร็วสูงสุด
unsigned long tUpSp = 0; // ตัวแปรสำหรับฟังก์ชัน upSpeed

// ===================================
// การประกาศฟังก์ชัน (Function Prototypes)
// ===================================

void beginFnc();
String getSensor();
int getErrorInput(String L);
void followLine();
bool checkGrid();
int countGrid(int n);
void stopRobot();
void upSpeed();
void moveFor();
void turnRight90(); // เลี้ยวขวา 90 องศา
void turnLeft90(); // เลี้ยวซ้าย 90 องศา
void turnRight180(); // เลี้ยวขวา 180 องศา
void turnLeft180(); // เลี้ยวซ้าย 180 องศา
void keepup_object(); // ฟังก์ชั่นหยิบของ
void put_object(); // ฟังก์ชั่นวางของ
void backToLine();

// ===================================
// การนิยามฟังก์ชัน (Function Definitions)
// ===================================

/**
 * @brief ฟังก์ชันเริ่มต้นการทำงานของหุ่นยนต์ (Setup)
 */
void beginFnc(){
  pinMode(sp_L, OUTPUT); pinMode(F_L, OUTPUT); // กำหนดขาควบคุมมอเตอร์ซ้ายเป็น OUTPUT
  pinMode (B_L, OUTPUT);
  pinMode(sp_R, OUTPUT); pinMode(F_R, OUTPUT); // กำหนดขาควบคุมมอเตอร์ขวาเป็น OUTPUT
  pinMode (B_R, OUTPUT);
  pinMode (STBY, OUTPUT); // กำหนดขา STBY เป็น OUTPUT
  delay(1000); // หน่วงเวลา 1 วินาที
  digitalWrite(STBY, 1); // เปิดใช้งานไดรเวอร์มอเตอร์
  servo_x.attach(x_pin); // เชื่อมต่อขาสัญญาณเซอร์โว x
  servo_y.attach(y_pin); // เชื่อมต่อขาสัญญาณเซอร์โว y
  servo_x.write(140); // ตั้งค่าเริ่มต้น: คลายแขน
  servo_y.write(105); // ตั้งค่าเริ่มต้น: ยกแขนลง
  clearPid(); // ล้างค่า PID
}

/**
 * @brief ฟังก์ชันอ่านค่าจากเซนเซอร์ตรวจจับเส้น 8 ตัว
 * @return String 8 ตัวอักษร ("0" หรือ "1")
 */
String getSensor(){
  String x = ""; //
  for(int i=0;i<8;i++){ // วนลูปอ่านค่าเซนเซอร์ 8 ตัว
    if(analogRead(sensorPin[i]) >= 800){ // ตรวจจับค่าอนาล็อก
      x += "1"; // "1" คือพบเส้น
    }else{
      x += "0"; // "0" คือไม่พบเส้น
    }
  }
  return(x); // ส่งคืนค่าสถานะเซนเซอร์
}

/**
 * @brief ฟังก์ชันแปลงค่าสถานะเซนเซอร์เป็นค่า Error สำหรับ PID
 * @param L ค่า String สถานะเซนเซอร์ที่อ่านได้
 * @return int ค่า Error (-7 ถึง 7) หรือ 100 (กรณีไม่พบ/หลุดเส้น)
 */
int getErrorInput(String L){
  int e;
  if (L == "10000000") {e = -7;}
  else if(L == "11000000") {e = -6;}
  else if(L == "11100000"){ e = -5;}
  else if(L == "01100000"){ e = -4;}
  else if (L == "01110000") { e = -3;}
  // ตรวจสอบโค้ดต้นฉบับ: บรรทัด 169 มีความผิดพลาด
  else if(L == "00110000") {e = -2;} // (แก้ไขจากโค้ดต้นฉบับที่ผิดพลาด)
  // ตรวจสอบโค้ดต้นฉบับ: บรรทัด 170-172
  else if(L == "00011100") {e = -1;} // (แก้ไขจากโค้ดต้นฉบับที่ผิดพลาด)
  // ตรวจสอบโค้ดต้นฉบับ: บรรทัด 173-174
  else if(L == "00011000") {e = 0;}
  // ตรวจสอบโค้ดต้นฉบับ: บรรทัด 175-176
  else if(L == "00011100"){ e = 1;} // (ซ้ำกับ e=-1, คาดว่าโค้ดต้นฉบับผิดพลาดและควรเป็น e=1)
  // ตรวจสอบโค้ดต้นฉบับ: บรรทัด 177-178
  else if(L == "00001100"){ e = 2;}
  // ตรวจสอบโค้ดต้นฉบับ: บรรทัด 180-182
  else if(L == "00001110"){ e = 3;}
  // ตรวจสอบโค้ดต้นฉบับ: บรรทัด 183-184
  else if(L == "00000110"){ e = 4;}
  // ตรวจสอบโค้ดต้นฉบับ: บรรทัด 185
  else if(L == "00000111") {e = 5;}
  else if(L == "00000011"){ e = 6;}
  else if(L == "00000001"){e = 7;}
  else{ e = 100;} // ไม่พบเส้น/หลุดเส้น

  return(e);
}

/**
 * @brief ฟังก์ชันควบคุมให้หุ่นยนต์วิ่งตามเส้นด้วย PID
 */
void followLine() {
  // 1. อ่านค่าจากเซนเซอร์: เรียกใช้ getSensor() เพื่ออ่านสถานะของเซนเซอร์ทั้ง 8 ตัว
  // ผลลัพธ์ที่ได้จะเป็น String 8 หลัก เช่น "00011000" (1=เจอเส้น, 0=ไม่เจอ)
  detectLine = getSensor();
  // 2. แปลงค่าเซนเซอร์เป็นค่า Error: นำ String ที่ได้ไปให้ getErrorInput() แปลงเป็นค่าตัวเลข
  // - ค่า Error จะอยู่ในช่วง -7 (หุ่นยนต์อยู่ขวาของเส้นมาก) ถึง 7 (หุ่นยนต์อยู่ซ้ายของเส้นมาก)
  // - ค่า 0 หมายถึงหุ่นยนต์อยู่กลางเส้นพอดี
  // - ค่า 100 หมายถึงไม่พบเส้นเลย (หลุดเส้น)
  int errorInput = getErrorInput(detectLine);

  // 3. ตรวจสอบว่าหุ่นยนต์หลุดเส้นหรือไม่
  if (errorInput != 100) { // ถ้ายังเจอเส้นอยู่ (error ไม่ใช่ 100)
    float pidout;
    
    // 4. เพิ่มความเร็ว: เรียกใช้ upSpeed() เพื่อค่อยๆ เพิ่มความเร็วพื้นฐาน (sp) ของหุ่นยนต์
    upSpeed();

    // 5. คำนวณค่า PID: นำค่า errorInput ไปคำนวณในฟังก์ชัน PID
    // pidFNC(ค่าที่วัดได้, ค่าเป้าหมาย, Kp, Ki, Kd)
    // - errorInput: ค่า Error ปัจจุบัน
    // - 0: คือค่าเป้าหมาย (Set Point) เราต้องการให้ Error เป็น 0
    // - Kp=1, Ki=0, Kd=0.7: คือค่าคงที่ของ PID สำหรับปรับจูนการตอบสนอง
    // ผลลัพธ์ (pidout) คือค่าที่ต้องนำไปปรับแก้การเคลื่อนที่
    pidout = pidFNC(errorInput, 0, 1, 0, 0.7);

    // 6. แปลงค่า PID เป็นค่าปรับความเร็ว: ใช้ map() เพื่อแปลงค่า pidout
    // ให้อยู่ในช่วงความเร็วที่จะนำไปปรับล้อ (-sp ถึง sp)
    int spout = map(round(pidout), -7, 7, -sp, sp);

    // 7. คำนวณความเร็วล้อซ้ายและขวา:
    // - ถ้าหุ่นยนต์เบนไปทางขวา (error > 0, spout > 0): ล้อซ้ายจะช้าลง, ล้อขวาจะเร็วขึ้น -> หุ่นยนต์จะเลี้ยวซ้าย
    // - ถ้าหุ่นยนต์เบนไปทางซ้าย (error < 0, spout < 0): ล้อซ้ายจะเร็วขึ้น, ล้อขวาจะช้าลง -> หุ่นยนต์จะเลี้ยวขวา
    int speedL = sp - spout; // ปรับความเร็วล้อซ้าย
    int speedR = sp + spout; // ปรับความเร็วล้อขวา

    // 8. จำกัดช่วงความเร็ว: ตรวจสอบให้แน่ใจว่าค่าความเร็วที่คำนวณได้
    // ไม่เกินความเร็วสูงสุด (sp) และไม่ต่ำกว่า 0
    if(speedL > sp){speedL = sp;}
    if (speedL < 0){speedL = 0;}
    if(speedR > sp){speedR = sp;}
    if (speedR < 0){speedR = 0;}

    // 9. สั่งให้มอเตอร์ทำงาน: ส่งค่าความเร็วที่คำนวณได้ (speedL, speedR)
    // ไปยังขา PWM ของมอเตอร์เพื่อให้หุ่นยนต์เคลื่อนที่ไปข้างหน้าพร้อมกับการปรับทิศทาง
    digitalWrite(F_L, 1); digitalWrite(B_L, 0); analogWrite(sp_L, speedL); // ล้อซ้ายเดินหน้า
    digitalWrite(F_R, 1); digitalWrite(B_R, 0); analogWrite(sp_R, speedR); // ล้อขวาเดินหน้า
  }
  // หากหลุดเส้น (errorInput == 100) ฟังก์ชันจะไม่ทำอะไรเลย
  // มอเตอร์จะยังคงทำงานด้วยความเร็วล่าสุดก่อนที่จะหลุดเส้น
}

/**
 * @brief ตรวจสอบว่าหุ่นยนต์อยู่บนพื้นที่ตาราง (Grid) หรือไม่
 * @return bool True ถ้าพบ Grid, False ถ้าไม่พบ
 */
bool checkGrid(){
  String ch = getSensor();
  // เงื่อนไขการตรวจจับ Grid (เซนเซอร์ตั้งแต่ 4 ตัวขึ้นไปติด)
  if(ch == "00001111" || 
     ch == "00011111" ||
     ch == "01111111" ||
     ch == "11111111" ||
     ch == "11111100" ||
     ch == "11111000" ||
     ch == "00111111" ||
     ch == "11111110" ||
     ch == "11110000" // ค่า "11110 1110000" ในต้นฉบับ คาดว่าคือ "11110000"
  ) {
    return (true);
  } else {
    return (false);
  }
}

/**
 * @brief นับจำนวน Grid ที่หุ่นยนต์วิ่งผ่าน
 * @param n ค่าเริ่มต้นของ Grid Count
 * @return int จำนวน Grid ที่นับได้
 */
int countGrid(int n){
  if(checkGrid()){
    delay(10);
    if(checkGrid()){ // ตรวจสอบซ้ำ
      //
    }
  }
  n ++; // นับ Grid เพิ่ม
  while(checkGrid()); // รอจนกว่าจะพ้นพื้นที่ Grid
  return (n);
}

/**
 * @brief ฟังก์ชันหยุดหุ่นยนต์
 */
void stopRobot() {
  digitalWrite(F_L, 1); digitalWrite(B_L, 1);
  digitalWrite(F_R, 1); digitalWrite(B_R, 1);
  sp = 50; // กำหนดความเร็วเริ่มต้นกลับไปที่ 50
}

/**
 * @brief ฟังก์ชันเพิ่มความเร็วหุ่นยนต์
 */
void upSpeed() {
  if (millis() - tUpSp >= 10) {
     sp += 2;
     tUpSp = millis();
  }
  if (sp > maxSp) {sp = maxSp;}
}

/**
 * @brief ฟังก์ชันเดินหน้า
 */
void moveFor() {
  digitalWrite(F_L, 1); digitalWrite(B_L, 0); analogWrite(sp_L, sp);
  digitalWrite(F_R, 1); digitalWrite(B_R, 0); analogWrite(sp_R, sp);
}

/**
 * @brief ฟังก์ชันเลี้ยวขวา 90 องศา
 */
void turnRight90() {
  String ch;
  while(true) {
    upSpeed();
    digitalWrite(F_L, 1); digitalWrite(B_L, 0); analogWrite(sp_L, sp); // ล้อซ้ายเดินหน้า
    digitalWrite(F_R, 0); digitalWrite(B_R, 1); analogWrite(sp_R, sp); // ล้อขวาถอยหลัง (หมุนขวา)
    ch = getSensor();
    // ch == "00000011" || ch == "00000001" || ch == "00000110" (พบเส้นด้านขวา)
    if (ch == "00000011" || ch == "00000001" || ch == "00000110") {
      break;
    }
  }
}

#endif // CONTROLLIBRARY_H

/**
 * @brief ฟังก์ชันเลี้ยวซ้าย 90 องศา
 */
void turnLeft90() {
  String ch;
  while(true) {
    upSpeed();
    digitalWrite(F_L, 0); digitalWrite(B_L, 1); analogWrite(sp_L, sp); // ล้อซ้ายถอยหลัง
    digitalWrite(F_R, 1); digitalWrite(B_R, 0); analogWrite(sp_R, sp); // ล้อขวาเดินหน้า (หมุนซ้าย)
    ch = getSensor();
    // ch == "11000000" || ch == "10000000" || ch == "01100000" (พบเส้นด้านซ้าย)
    if (ch == "11000000" || ch == "10000000" || ch == "01100000") {
      break;
    }
  }
}

/**
 * @brief ฟังก์ชันเลี้ยวขวา 180 องศา
 */
void turnRight180() {
  // การเลี้ยว 180 องศา คือการเรียกใช้ฟังก์ชันเลี้ยว 90 องศา สองครั้ง
  //
  // ฟังก์ชัน turnRight90() ไม่ได้หมายถึง "หมุนตัวเป็นเวลา X วินาที" แต่หมายถึง "หมุนตัวไปทางขวาเรื่อยๆ จนกว่า 
  // จะเจอเส้นในตำแหน่ง 90 องศา"
  // เมื่อฟังก์ชัน turnRight90() ทำงานเสร็จสิ้น หุ่นยนต์จะอยู่ในตำแหน่งที่เจอเส้นที่มุม 90 องศาพอดี
  // 
  // เราไม่จำเป็นต้องมีการตรวจสอบเส้น (getSensor()) ระหว่างการเลี้ยวทั้งสองครั้ง เพราะฟังก์ชัน turnRight90() 
  // ได้จัดการเรื่องการหาเส้นให้เราเรียบร้อยแล้วในแต่ละขั้นตอน
  turnRight90(); // เลี้ยว 90 องศาครั้งแรก
  delay(50);    // หน่วงเวลาเล็กน้อยเพื่อให้หุ่นยนต์นิ่ง
  turnRight90(); // เลี้ยว 90 องศาครั้งที่สอง
}

/**
 * @brief ฟังก์ชันเลี้ยวซ้าย 180 องศา
 */
void turnLeft180() {
  // การเลี้ยว 180 องศาที่ถูกต้องคือการเลี้ยว 90 องศาสองครั้ง
  // เราไม่จำเป็นต้องมีการตรวจสอบเส้น (getSensor()) ระหว่างการเลี้ยวทั้งสองครั้ง เพราะฟังก์ชัน turnRight90() 
  // ได้จัดการเรื่องการหาเส้นให้เราเรียบร้อยแล้วในแต่ละขั้นตอน
  turnLeft90(); // เลี้ยว 90 องศาครั้งแรก
  delay(50);    // หน่วงเวลาเล็กน้อยเพื่อให้หุ่นยนต์นิ่ง
  turnLeft90(); // เลี้ยว 90 องศาครั้งที่สอง
}

/**
 * @brief ฟังก์ชันหยิบของ
 */
void keepup_object() {
  servo_x.write(65); // หนีบแขน
  delay(500);
  servo_y.write(90); // ยกแขนขึ้น
  delay(500);
}

/**
 * @brief ฟังก์ชันวางของ
 */
void put_object() {
  servo_y.write(105); // ยกแขนลง
  delay(500);
  servo_x.write(149); // คลายแขน
  delay(500);
}

/**
 * @brief ฟังก์ชันถอยหลังจนพบเส้น (Back to Line)
 */
void backToLine() {
  String ch;
  while(true) {
    upSpeed();
    digitalWrite(F_L, 0); digitalWrite(B_L, 1); analogWrite(sp_L, 50); // ล้อซ้ายถอยหลัง
    digitalWrite(F_R, 0); digitalWrite(B_R, 1); analogWrite(sp_R, 50); // ล้อขวาถอยหลัง (ถอยตรง)
    ch = getSensor();
    // ch == "11100000" || ch == "00011111" || ch == "11111111" (ตรวจพบเส้น)
    if (ch == "11110000" || ch == "00001111" || ch == "11111111") {
      break;
    }
  }
}
