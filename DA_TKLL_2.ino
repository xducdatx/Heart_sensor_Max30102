/*          KHAI BÁO THƯ VIỆN            */
#include <BluetoothSerial.h>
#include "heartRate.h"
#include "MAX30105.h"
#include <Wire.h>
/*/////////////////////////////////////////////////////////////////////////////////////*/
/*          KHAI BÁO TÊN MODULE             */
MAX30105 particleSensor;
BluetoothSerial ESP_BT;
/*/////////////////////////////////////////////////////////////////////////////////////*/
/*          KHAI BÁO BIẾN            */
bool neg;                           // Cờ negative (neg) để lưu trạng thái cạnh xuống của đồ thị
int count = 0;                     // Biến đếm số Nhịp tim
bool pos = false;                 // Cờ positive (pos) để lưu trạng thái cạnh lên của đồ thị
float delta = 0.0;               // Biến lưu khoảng thời gian để tính nhịp tim, được reset khi >= thời gian người dùng yêu cầu
float last_time = 0.0;          // Biến lưu lại thời gian Millis() cho lần tính nhịp tim trước đó
const int sampleSize = 20;     // Giá trị số lượng Mẫu, NÊN giữ giá trị này
float irValues[sampleSize];   // Mảng giá trị IR gồm sampleSize mẫu
long last_averageValue = 0.0;// Biến lưu lại giá trị averageValue lần trước đó
int countArr = 0;          // Dùng để làm đặt vị trí cho giá trị Ir trong mảng
 /*/////////////////////////////////////////////////////////////////////////////////////*/
/*          HÀM SETUP (CHẠY 1 LẦN): 
      1. Khai báo Serial Monitor 
      2. Khai báo tên Bluetooth của ESP32 ở màn hình điện thoại
      3. Tiến hành khởi động chuẩn kết nối I2C với tốc độ 400k Hz và luôn trong tình trạng hoạt động (while(1)) 
      4. Khởi động & cấu hình cảm biến nhịp tim MAX30102 với tên khai báo theo mặc định
      5. Nhấp nháy đèn LED Đỏ (0x0A) để hiển thị người dùng biết cảm biến đang chạy
      6. Tắt đèn LED Xanh (0)                                                           */
void setup() {
      Serial.begin(115200);
      ESP_BT.begin("ESP32_Control");
      if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
            while (1);
      }
      particleSensor.setup();
      particleSensor.setPulseAmplitudeRed(0x0A); 
      particleSensor.setPulseAmplitudeGreen(0); 
}
/*/////////////////////////////////////////////////////////////////////////////////////*/
/*          HÀM LOOP (CHẠY MÃI MÃI): 
      1. Khai báo Serial Monitor 
      2. Khai báo tên Bluetooth của ESP32 ở màn hình điện thoại
      3. Tiến hành khởi động chuẩn kết nối I2C với tốc độ 400k Hz và luôn trong tình trạng hoạt động (while(1)) 
      4. Khởi động & cấu hình cảm biến nhịp tim MAX30102 với tên khai báo theo mặc định
      5. Nhấp nháy đèn LED Đỏ (0x0A) để hiển thị người dùng biết cảm biến đang chạy
      6. Tắt đèn LED Xanh (0)                                                           */
void loop() {
      float irValue = particleSensor.getIR();
              // Serial.print("IR:");
        // Serial.println(irValue);
        // Serial.print(" ");
      // for (int i = sampleSize - 1; i > 0; i--) {
      //       irValues[i] = irValues[i - 1];
      // }
      // irValues[0] = irValue;
      irValues[countArr % sampleSize] = irValue;
      countArr++;
      long averageValue = 0.0;
      for (int i = 0; i < sampleSize; i++) {
            averageValue += irValues[i];
      }
      averageValue /= sampleSize;
      
      // Serial.println(averageValue);                                          

      if ((pos == false) && (averageValue < irValue)) {
            pos = true; neg = false;
            count +=1;
      }
      else if (irValue < averageValue) {
            pos = false; neg = true;
      }
      const int n = 15000;
      delta = millis() - last_time;
      if (delta >= n) {
            int BPM = count*(60000/n);
            Serial.print("Nhip tim: ");
            Serial.println(BPM);
            ESP_BT.write(BPM); // Xuất giá trị lên ứng dụng di dộng
            count = 0;
            delta = 0.0;
            last_time = millis();
      }
      // last_averageValue = averageValue;
      // delay(50); // NÊN giữ giá trị này
}
/*/////////////////////////////////////////////////////////////////////////////////////*/
/*          CÁC CMT DÙNG TRONG HÀM LOOP (ĐỂ TEST CODE - DEBUG)
      - Hiển thị giá trị IR
        //Serial.println("IR:");
        //Serial.print(irValue);
        //Serial.print(" ");
      - Hiển thị thời gian delta = millis() - last_time
        // Serial.print("Thời gian: ");
        // Serial.println(delta);
      - Hiển thị giá trị averageValue (Cộng thêm 1000 để nâng đồ thị cao lên dễ nhìn)
      //  Serial.println(averageValue+1000);                                            */
/*/////////////////////////////////////////////////////////////////////////////////////*/

