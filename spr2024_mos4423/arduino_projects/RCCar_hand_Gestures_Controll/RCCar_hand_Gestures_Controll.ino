/*
 * board :Spresense Reference Board v3.2.0
 * Core  :MainCore(default)
 * Debug :Disabled(default)
 * Memory:"1024"KB
 */
#include "MotorDriver.h"//ライブラリのinclude
#include <Camera.h>
#include <DNNRT.h>
#include <File.h>
#include <SDHCI.h>
SDClass SD;
MDOUT md;
const int start_x = 48; //始点x
const int start_y =  8; //始点y
const int width = 28;   //small幅x
const int height =28;   //small幅y
const int mag = 8;      //拡大率

DNNRT dnnrt;
DNNVariable input(width * height);

int img_dat;      //本来のデータはこちらに格納
int img_thr=140;  //0~225までのモノクロ閾値定義

int guess_flag=0;//推論値格納

/*
  .setAにはa側のenbl1,enbl2,phaseピンを設定
  .setBにはb側のenbl1,enbl2,phaseピンを設定
  .stbにはスタンバイピンを宣言、宣言した時点でHIGHになっている、宣言のみで特に何も使わない

  stop,forw,revr,brakはそれぞれflagとvalueで構成
  flagはa側とb側を決める
  valueはbrak以外の3つに備わっていて、それぞれphaseを入力する

  stopがストップ、enbl1= LOW,enbl2= LOW
  forwが正回転、  enbl1= LOW,enbl2=HIGH
  revrが逆回転、  enbl1=HIGH,enbl2= LOW
  brakがブレーキ、enbl1=HIGH,enbl2=HIGH

  例えば、速度を100としてa,bに正回転させたかったら
  forw(false,100);
  forw(true,100);
*/

void CamCB(CamImage img) {
  if (img.isAvailable()){
    CamImage small;
    CamErr err=img.clipAndResizeImageByHW(small , start_x , start_y , mag*width+start_x-1 , mag*height+start_y-1 , width , height);
  //CamErr err=img.clipAndResizeImageByHW(small , 48 , 8 , 271 , 231 , 28 , 28);
/*
  // 320×240の画像に対し(48, 8)から(271, 231)までの画像(224×224)を切り出して、1/8した28×28に変換
  // imgが元画像、smallが切り出し後の画像データ
*/
    if (err != CAM_ERR_SUCCESS){
      Serial.println("error");
      while(1);
    }
    small.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
    uint16_t* buf = (uint16_t*)small.getImgBuff();
    float* input_buffer = input.data();
    for (int i = 0; i < width * height; ++i, ++buf) {
      img_dat = (((*buf & 0x07E0) >> 5) << 2);
      if(img_dat>img_thr) input_buffer[i]=0.00f;
      else input_buffer[i]=1.00f;
    }
    dnnrt.inputVariable(input, 0);
    dnnrt.forward();
    DNNVariable output = dnnrt.outputVariable(0);
    guess_flag = output.maxIndex();
  }
}



void setup() {
  Serial.begin(115200);
  Serial.println("Loading network model");
  md.setA(4,3,6);//A側のピン宣言、enbl1,enbl2,phase
  md.setB(10,9,5);//B側のピン宣言、enbl1,enbl2,phase
  md.stby(8);//おまじないとして宣言、ここで定義されたstbyピンをHIGHにする
  md.stop(true,0);//A側速度の初期化
  md.stop(false,0);//B側速度の初期化
  while(!SD.begin()) { Serial.println("Insert SD Card"); }
  sleep(1);
  File nnbfile = SD.open("network_hand.nnb");
  if (!nnbfile) {
    Serial.println("nnb not found");
    while(1);
  }
  Serial.println("Initialize DNNRT");
  int ret = dnnrt.begin(nnbfile);
  if (ret < 0) {
    Serial.println("DNNRT initialize error.");
    while(1);
  }
  theCamera.begin();
  theCamera.startStreaming(true, CamCB);

}

void loop() {
/* 
  case0:グー
  case1:チョキ
  case2:パー
  case3:ウィキ
*/
  //Serial.println(guess_flag);
  if(guess_flag==1){
    md.forw(true,90);
    md.revr(false,90);
  }
  if(guess_flag==2){
    md.forw(false,90);
    md.revr(true,90);
  }
  if(guess_flag==0){
    md.stop(false,90);
    md.stop(true,90);
  }
}
