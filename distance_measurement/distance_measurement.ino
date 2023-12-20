// 赤外線センサー(GP2Y0A21YK)距離取得
#include <MP.h>
void setup() {
     Serial.begin(115200) ;               // 9600bpsでシリアル通信のポートを開きます
     MP.begin();
     
}
 
void loop() {
     Serial.print("No.1:");
     Serial.print(IRread(A0)) ;       // 距離(ln有りで改行、無しで改行なし)
     Serial.println("cm");
     Serial.print("No.2:");
     Serial.print(IRread(A1)) ;       // 距離(ln有りで改行、無しで改行なし)
     Serial.println("cm");
     delay(500) ;                          // 500ms後に繰り返す
}
 
/*カッコ内に指定したアナログピンナンバーのセンサー(GP2Y0A21YK)信号を読み取りセンサーから障害物までの距離をcmで返す関数*/
double IRread(int PinNo) {
     double ans ;
     double distance;
     int i ;
     ans = 0 ;
     for (i=0 ; i < 100 ; i++) {
          ans  = ans + analogRead(PinNo) ;   // 指定のアナログピン(0番端子)から読取ります
     }
     distance = 19501.14*pow(ans/100, -1.256676); //距離
     return distance;
}
