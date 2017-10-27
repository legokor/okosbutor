//source is the 5fb709 commit.

#define redPin  3  //r
#define grnPin  5  //g
#define bluPin  6  //b



byte finR=0;
byte finG=0;
byte finB=0;

byte curR=0;
byte curB=0;
byte curG=0;


byte rgb[3][7][3] = { {{ 0, 0, 0 }, { 255, 255, 255 },{ 255, 0, 0 },{ 0, 255, 0 }, { 0, 0, 255 }, { 255, 255, 0 }, { 80, 80, 80 }}};

/*
                      {{ 0x8d, 0x6e, 0x63 }, { 0xff, 0x57, 0x22 },{ 0xff, 0x6f, 0x00 },{ 0xcd, 0xdc, 0x39 }, { 0x4c, 0xaf, 0x50 }, { 0x00, 0x96, 0x88 }, { 0x7e, 0x57, 0xc2 }},
                      {{255,255,0}, {255,255,0},{250,255,0},{250,127,0},{250,0,0},{250,0,0},{255,0,0}} };

*/
int colorPalette = 1;
int colorNum=0;
