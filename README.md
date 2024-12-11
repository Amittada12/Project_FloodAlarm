## ขั้นตอนการทำงาน
1.วางแผนการซื้ออุปกรณ์ ดูอุปกรณ์ว่าต้องการใช้อะไรบ้าง ต้องการงบกี่บาท ในที่นี้อุปกรณ์ที่เราต้องใช้ประกอบด้วย 

* Board ESP8266
* Sensor Waterlevel
* Sensor Flowrate

  
![Floodalarm](https://github.com/user-attachments/assets/d566802b-8703-44a5-b649-ff463b77d27b)

  
2.เริ่มโหลดไลบรารีและเริ่มเขียนแต่ละไฟล์ที่เราจะใช้ ในที่นี้ใช้ทั้งหมด 4 ไฟล์ประกอบด้วย FloodAlarm.ino, index.html, style.css, script.js

#### ไลบรารีที่ใช้ประกอบด้วย
* ESP8266WiFi.h
\' http://downloads.arduino.cc/packages/package_renesas_index.json
https://arduino.esp8266.com/stable/package_esp8266com_index.json
https://dl.espressif.com/dl/package_esp32_index.json \'
* ESPAsyncWebServer.h
\' https://github.com/me-no-dev/ESPAsyncWebServer.git  \'
* Arduino_JSON.h
\' https://github.com/arduino-libraries/Arduino_JSON.git \'
* FS.h
\' https://github.com/esp8266/Arduino.git \'
* LittleFS.h
\' https://github.com/lorol/LITTLEFS.git \'


3.เมื่อเขียนโค้ดเสร็จตรวจเช็คความถูกต้องและแก้ไขหากมี error และเริ่มต่อบอร์ดเชื่อมบอร์ดและต่อบอร์ดและต้องตามขาที่เราได้เขียนไว้ในโค้ด 
4.ทดสอบเมื่อรันกับโค้ดและเมื่อไม่มีข้อผิดพลาดลองกับสถานการณ์น้ำท่วมสมมุติ หากหน้าเว็บเซิร์ฟเวอร์และอุปกรณ์ทำงานได้ถูกต้องถือว่าเสร็จสิ้น
