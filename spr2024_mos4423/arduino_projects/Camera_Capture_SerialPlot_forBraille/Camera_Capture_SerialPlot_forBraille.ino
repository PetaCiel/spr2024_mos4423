/*
 * board :Spresense Reference Board v3.2.0
 * Core  :MainCore(default)
 * Debug :Disabled(default)
 * Memory:1536KB
 */

#include <Camera.h>
#include <SDHCI.h>
SDClass SD;

const int start_x = 48; //始点x
const int start_y =  8; //始点y
const int width = 56;   //small幅x
const int height =56;   //small幅y
const int mag = 4;      //拡大率

int img_dat;      //本来のデータはこちらに格納
int img_thr=140;  //0~225までのモノクロ閾値定義

const int cycle_time=200;//単位はms, 200ms





typedef struct {
  int in;
  int out[3];
}b_dat;
b_dat br_d={0,{0,0,0}};
typedef struct {
  int n;
  int x;
  int y;
  int w;
  int h;
}coordinate_t;

coordinate_t coor_bit={0,0,0,width,height};
coordinate_t coor_br ={0,0,0,(width/2), (height/4)};

void braille(b_dat *br_d){
  int data[3]={0b11100010,0b10100000,0b10000000};
  int riu=(br_d->in)>>8;
  int rid=(br_d->in)&0b11111111;
  int kotei=(rid&0b0000111111);
  int hendo=((riu&0b11)<<2)+((rid&11000000)>>6);
  br_d->out[0]=data[0];
  br_d->out[1]=data[1]+hendo;
  br_d->out[2]=data[2]+kotei;
}
void COOR_CONV_NtoXY(coordinate_t *data){
  data->x=data->n % data->w;
  data->y=data->n / data->w;
}
void COOR_CONV_XYtoN(coordinate_t *data){
  data->n=(data->y*data->w)+data->x;
}

bool loop_flag = false;

void CamCB(CamImage img) {
  //Serial.println("CamCB_test");
  int br_arr[(width/2)*(height/4)]={0};
  if (img.isAvailable() && loop_flag) {
    CamImage small;
    CamErr err=img.clipAndResizeImageByHW(small , start_x , start_y , mag*width+start_x-1 , mag*height+start_y-1 , width , height);
  //CamErr err=img.clipAndResizeImageByHW(small , 48 , 8 , 271 , 231 , 56 , 56);
    if (err != CAM_ERR_SUCCESS){
      Serial.println("cam_error");
      while(1);
    }
    uint8_t* gray = (uint8_t*)malloc(width*height);
    uint16_t* imgbuff = (uint16_t*)small.getImgBuff();
    for (int i = 0; i < width*height; ++i) {
      img_dat = (uint8_t)(((imgbuff[i] & 0xf000) >> 8) | ((imgbuff[i] & 0x00f0) >> 4));
      if(img_dat>img_thr) gray[i]=1;
      else gray[i]=0;
      coor_bit.n=i;
      COOR_CONV_NtoXY(&coor_bit);
      coor_br.x=(coor_bit.x/2);
      coor_br.y=(coor_bit.y/4);
      COOR_CONV_XYtoN(&coor_br);

      if(((coor_bit.y%4)==0)&&((coor_bit.x%2)==0)) br_arr[coor_br.n]+=(0x01*gray[i]); if(((coor_bit.y%4)==0)&&((coor_bit.x%2)==1)) br_arr[coor_br.n]+=(0x08*gray[i]);
      if(((coor_bit.y%4)==1)&&((coor_bit.x%2)==0)) br_arr[coor_br.n]+=(0x02*gray[i]); if(((coor_bit.y%4)==1)&&((coor_bit.x%2)==1)) br_arr[coor_br.n]+=(0x10*gray[i]);
      if(((coor_bit.y%4)==2)&&((coor_bit.x%2)==0)) br_arr[coor_br.n]+=(0x04*gray[i]); if(((coor_bit.y%4)==2)&&((coor_bit.x%2)==1)) br_arr[coor_br.n]+=(0x20*gray[i]);
      if(((coor_bit.y%4)==3)&&((coor_bit.x%2)==0)) br_arr[coor_br.n]+=(0x40*gray[i]); if(((coor_bit.y%4)==3)&&((coor_bit.x%2)==1)) br_arr[coor_br.n]+=(0x80*gray[i]);
    }
    for(int i=0;i<((width/2)*(height/4));i++){
      br_d.in=br_arr[i];
      braille(&br_d);
      Serial.write(br_d.out[0]);
      Serial.write(br_d.out[1]);
      Serial.write(br_d.out[2]);
      if((i%(width/2))==((width/2)-1)) Serial.print("\n");
    }
    free(gray);
    loop_flag = false;
  }
}





void setup() {
  Serial.begin(115200);
  theCamera.begin();
  theCamera.startStreaming(true, CamCB);
  //Serial.println("capture");
}

void loop() {
  loop_flag=true;
  delay(cycle_time);
}
