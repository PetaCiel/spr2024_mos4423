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
      Serial.println("cam_error");
      while(1);
    }
    small.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
    uint16_t* buf = (uint16_t*)small.getImgBuff();
    float* input_buffer = input.data();
    for (int i = 0; i < width * height; ++i, ++buf) {
    input_buffer[i] = (float)(((*buf & 0x07E0) >> 5) << 2);
    }
    //0xABCD and 0x07E0 = 0x03C0
    //0b 0000 0111 1110 0000
    dnnrt.inputVariable(input, 0);
    dnnrt.forward();
    DNNVariable output = dnnrt.outputVariable(0);  

    Serial.print("Result : ");
    for(int i=0;i<10;i++){
      Serial.print(i);
      Serial.print("=");
      Serial.print(output[i],2);
      if (i<9) Serial.print(",");
    }
    Serial.println();
  
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("Loading network model");
  while(!SD.begin()) { Serial.println("Insert SD Card"); }
  sleep(1);
  File nnbfile = SD.open("network_num.nnb");
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
}