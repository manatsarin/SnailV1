/**
 * @file SnailV1.ino
 * @brief Main sketch for the Snail V1 line-following robot.
 *
 * This file contains the main setup and loop functions. All robot-specific
 * functionality is abstracted into controlLibrary.h.
 */
#include "controlLibrary.h"


/**
 * @brief ฟังก์ชัน setup จะทำงานเพียงครั้งเดียวเมื่อโปรแกรมเริ่มทำงาน
 *        (The setup function runs once when you press reset or power the board)
 *
 * ความซ้ำซ้อนของโค้ด: โค้ด เช่น pinMode(...), servo_x.attach(...), delay(1000) และ clearPid() 
 * ทั้งหมดนี้มีอยู่แล้วในฟังก์ชัน beginFnc() การเก็บโค้ดไว้ในที่เดียว (คือใน controlLibrary.h) 
 * ช่วยให้การแก้ไขและดูแลรักษาโค้ดในอนาคตทำได้ง่ายขึ้นครับ
 * ความชัดเจน: การทำให้ฟังก์ชัน setup() เหลือเพียง beginFnc(); ทำให้เห็นภาพรวมของโปรแกรมได้ง่ายขึ้น 
 * ว่าส่วน setup คือการ "เริ่มต้นการทำงานของหุ่นยนต์" โดยมีรายละเอียดการทำงานอยู่ในไลบรารีที่แยกไว้
 */
void setup() {
  // เรียกใช้ฟังก์ชัน beginFnc() จาก controlLibrary.h เพื่อตั้งค่าเริ่มต้นทั้งหมดของหุ่นยนต์
  // (Calls beginFnc() from controlLibrary.h to initialize all robot components)
  beginFnc();
}

void loop() {
/*
* ภาพรวมการทำงานของ followLine()
* ฟังก์ชันนี้มีหน้าที่อ่านค่าจากเซนเซอร์ตรวจจับเส้น, คำนวณว่าหุ่นยนต์อยู่ห่างจากศูนย์กลางของเส้นมากน้อยแค่ไหน (ค่า Error), 
* จากนั้นใช้ค่า Error นี้ไปคำนวณในระบบควบคุมแบบ PID เพื่อปรับความเร็วของล้อซ้ายและขวาให้หุ่นยนต์วิ่งกลับมาอยู่กลางเส้นเสมอ
* สรุปง่ายๆ คือ followLine() เป็นวงจรการทำงานที่เกิดขึ้นซ้ำๆ อย่างรวดเร็ว (อ่าน -> คำนวณ -> ปรับแก้) 
* เพื่อให้หุ่นยนต์สามารถวิ่งตามเส้นได้อย่างราบรื่นครับ
*/
  followLine();
}