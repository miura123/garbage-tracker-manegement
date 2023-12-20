/*
 *  number_recognition.ino - Number recognition sketch with graphics
 *  Copyright 2022 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
// マルチコア用
#ifdef SUBCORE
#error "Core selection is wrong!!"
#endif
#include <MP.h>
int subcore = 1; /* Communication with SubCore1 */
#include <Camera.h>
#include "Adafruit_ILI9341.h"
#include <DNNRT.h>
#include <SDHCI.h>
#define TFT_DC  9
#define TFT_CS  10
Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC);
#define OFFSET_X  (104)
#define OFFSET_Y  (0)
#define CLIP_WIDTH (112)
#define CLIP_HEIGHT (224)
#define DNN_WIDTH  (28)
#define DNN_HEIGHT  (28)
SDClass SD;
DNNRT dnnrt;
DNNVariable input(DNN_WIDTH*DNN_HEIGHT);
// ボタン用ピンの定義
// 学習キットを使用している場合はボタン番号を取り扱い説明書で確認してください
#define BUTTON 0
// 推論結果保存用のカウント変数
int cnt = 0;
// 推論結果の精度平均用の変数
int acc_avg = 0;
// ボタン押下時に呼ばれる割り込み関数
bool bButtonPressed = false;
void changeState() {
  // MPLog("tokudadesu\n");
  bButtonPressed = true;
}
// #include <MP.h>
// int subcore = 1;
// モーター関連
#include <Servo.h>
static Servo s_servo_1; /**< Servo object */
static Servo s_servo_2; /**< Servo object */

int serial_data;

void CamCB(CamImage img) {

  if (!img.isAvailable()) {
    Serial.println("Image is not available. Try again");
    return;
  }
  // カメラ画像の切り抜きと縮小
  CamImage small;
  CamErr err = img.clipAndResizeImageByHW(small
                     , OFFSET_X, OFFSET_Y
                     , OFFSET_X + CLIP_WIDTH -1
                     , OFFSET_Y + CLIP_HEIGHT -1
                     , DNN_WIDTH, DNN_HEIGHT);
  if (!small.isAvailable()){
    putStringOnLcd("Clip and Reize Error:" + String(err), ILI9341_RED);
    return;
  }
  // 認識用モノクロ画像を設定
  uint16_t* imgbuf = (uint16_t*)small.getImgBuff();
  float *dnnbuf = input.data();
  for (int n = 0; n < DNN_HEIGHT*DNN_WIDTH; ++n) {
    dnnbuf[n] = (float)(((imgbuf[n] & 0xf000) >> 8) 
                      | ((imgbuf[n] & 0x00f0) >> 4))/255.;
  }
  
  if (bButtonPressed) {
    for (int i = 0; i < 10; i++){
      // 推論の実行
      dnnrt.inputVariable(input, 0);
      dnnrt.forward();
      DNNVariable output = dnnrt.outputVariable(0);
      int index = output.maxIndex();
      if (index == 0 | index == 1){
        cnt += index;
        acc_avg += output[index];
      }
    }
    acc_avg = acc_avg / 10;
    if (cnt >= 8){
      s_servo_1.write(0);
      delay(5000);
      s_servo_1.write(-80);
      serial_data = 0;
      Serial.write("0\n");
    }
    else {
      s_servo_2.write(0);
      delay(5000);
      s_servo_2.write(-80);
      Serial.write("1\n");
      serial_data = 1;
    }
    cnt = 0;
    bButtonPressed = false;
  }
  Serial.println(serial_data);
  delay(10000);
}
void setup() {   
  Serial.begin(115200);
  MP.begin(subcore);
  // SDカードの挿入待ち
  while (!SD.begin()) { 
    putStringOnLcd("Insert SD card", ILI9341_RED); 
  }
  // SDカードにある学習済モデルの読み込み
  File nnbfile = SD.open("model.nnb");
  // 学習済モデルでDNNRTを開始
  int ret = dnnrt.begin(nnbfile);
  if (ret < 0) {
    putStringOnLcd("dnnrt.begin failed" + String(ret), ILI9341_RED);
    return;
  }
  ret = MP.begin(1);
  display.begin();
  display.setRotation(3);
  theCamera.begin();
  theCamera.startStreaming(true, CamCB);
  attachInterrupt(digitalPinToInterrupt(BUTTON), changeState, CHANGE);
  s_servo_1.attach(PIN_D02);
  s_servo_2.attach(PIN_D03);
  s_servo_1.write(-80);
  s_servo_2.write(-80);
}
void loop() { 
}