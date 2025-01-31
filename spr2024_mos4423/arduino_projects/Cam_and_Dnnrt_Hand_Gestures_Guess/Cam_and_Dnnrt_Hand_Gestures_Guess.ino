/*
 * board :Spresense Reference Board v3.2.0
 * Core  :MainCore(default)
 * Debug :Disabled(default)
 * Memory:"1024"KB
 */

#include <Camera.h>
#include <DNNRT.h>
#include <File.h>
#include <SDHCI.h>
SDClass SD;

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
    guess_flag=output.maxIndex();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Loading network model");
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
  Serial.print("\t\t\t\t推測結果 = ");
  switch(guess_flag){
    case 0: Serial.print("グー\n"); break;
    case 1: Serial.print("チョキ\n"); break;
    case 2: Serial.print("パー\n"); break;
    case 3: Serial.print("ウィー\n"); break;
  };
  delay(10);
}