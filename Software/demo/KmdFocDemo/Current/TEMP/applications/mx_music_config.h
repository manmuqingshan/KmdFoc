#ifndef _MX_MUSIC_CONFIG_H
#define _MX_MUSIC_CONFIG_H

//不抄曲谱了，真实难抄
//定义时值单位,决定演奏的速度 ms为单位 2000为佳
//#define TT 2697
#define TT 2900

////定义低音音名C
//#define L1 262
//#define L2 294
//#define L3 330
//#define L4 349
//#define L5 392
//#define L6 440
//#define L7 494

////定义中音音名C
//#define M1 523
//#define M2 587
//#define M3 659
//#define M4 698
//#define M5 784
//#define M6 880
//#define M7 988

////定义高音音名C
//#define H1 1047
//#define H2 1175
//#define H3 1319
//#define H4 1397
//#define H5 1568
//#define H6 1760
//#define H7 1976

//定义低音音名D
#define L1 294
#define L2 330
#define L3 370
#define L4 392
#define L5 440
#define L6 494
#define L7 554

//定义中音音名D
#define M1 587
#define M2 659
#define M3 740
#define M4 784
#define M5 880
#define M6 988
#define M7 1109

//定义高音音名D
#define H1 1175
#define H2 1319
#define H3 1480
#define H4 1568
#define H5 1760
#define H6 1976
#define H7 2217

////定义低音音名E
//#define L1 330
//#define L2 370
//#define L3 415
//#define L4 440
//#define L5 494
//#define L6 554
//#define L65 587
//#define L7 622

////定义中音音名E
//#define M1 659
//#define M2 740
//#define M3 831
//#define M4 880
//#define M5 988
//#define M6 1109
//#define M7 1245

////定义高音音名E
//#define H1 1319
//#define H2 1480
//#define H3 1661
//#define H4 1760
//#define H5 1976

////定义低音音名F (单位是Hz)
//#define L1 349
//#define L2 392
//#define L3 440
//#define L4 466
//#define L5 523
//#define L6 587
//#define L7 659

////定义中音音名F
//#define M1 698
//#define M2 784
//#define M3 880
//#define M4 932
//#define M5 1047
//#define M6 1175
//#define M7 1319

////定义高音音名F
//#define H1 1397
//#define H2 1568
//#define H3 1760
//#define H4 1865

typedef struct
{
    short mName; //音名
    short mTime; //时值,全音符,二分音符,四分音符
} tNote;
//Always with me  (一拍674.15)*4=2697 F调
#define Always_with_me_length 256
const tNote Always_with_me[Always_with_me_length]=
{
    {0,TT/2},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M1,TT/8},{M5,TT/4+TT/8},{M3,TT/8},
    {M2,TT/4},{M5,TT/4},{M2,TT/4},{M1,TT/8},{L6,TT/8},{M3,TT/4+TT/8},{M1,TT/8},
    {L7,TT/2},{M1,TT/8},{L7,TT/8},{L6,TT/4},{L7,TT/4},{M1,TT/8},{M2,TT/8},
    {L5,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/8},{M4,TT/4},{M4,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
    {M2,TT/2},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M1,TT/8},{M5,TT/4+TT/8},{M3,TT/8},
    {M2,TT/4},{M5,TT/4},{M2,TT/4},{M1,TT/8},{L6,TT/8},{L6,TT/4},{L7,TT/8},{M1,TT/8},
    {L5,TT/2},{0,TT/8},{L5,TT/8},{L6,TT/4},{L7,TT/4},{M1,TT/8},{M2,TT/8},//question
    {L5,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/8},{M4,TT/4},{M4,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M1,TT/2+TT/4},

    {0,TT/2},{M3,TT/8},{M4,TT/8},{M5,TT/4},{M5,TT/4},{M5,TT/4},{M5,TT/4},{M5,TT/8},{M6,TT/8},{M5,TT/8},{M4,TT/8},
    {M3,TT/4},{M3,TT/4},{M3,TT/4},{M3,TT/4},{M3,TT/8},{M4,TT/8},{M3,TT/8},{M2,TT/8},
    {M1,TT/4},{M1,TT/4},{M1,TT/8},{L7,TT/8},{L6,TT/4},{L7,TT/4},{L7,TT/8},{M1,TT/8},
    {M2,TT/4},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/2},{M3,TT/8},{M4,TT/8},
    {M5,TT/4},{M5,TT/4},{M5,TT/4},{M5,TT/4},{M5,TT/8},{M6,TT/8},{M5,TT/8},{M4,TT/8},
    {M3,TT/4},{M3,TT/4},{M3,TT/4},{M3,TT/8},{M4,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L7,TT/8},
    {L6,TT/4},{L6,TT/8},{L7,TT/8},{M1,TT/8},{M2,TT/8},{L5,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/8},
    {M2,TT/4+TT/8},{M2,TT/8},{M2,TT/8},{M1,TT/8},{M1,TT/2+TT/4},

    {0,TT/2},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M1,TT/8},{M5,TT/4+TT/8},{M3,TT/8},
    {M2,TT/4},{M5,TT/4},{M2,TT/4},{M1,TT/8},{L6,TT/8},{M3,TT/4+TT/8},{M1,TT/8},
    {L7,TT/2},{M1,TT/8},{L7,TT/8},{L6,TT/4},{L7,TT/4},{M1,TT/8},{M2,TT/8},
    {L5,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/8},{M4,TT/4},{M4,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},
    {M2,TT/2},{M1,TT/8},{M2,TT/8},{M3,TT/8},{M1,TT/8},{M5,TT/4+TT/8},{M3,TT/8},
    {M2,TT/4},{M5,TT/4},{M2,TT/4},{M1,TT/8},{L6,TT/8},{L6,TT/4},{L7,TT/8},{M1,TT/8},
    {L5,TT/2},{0,TT/8},{L5,TT/8},{L6,TT/4},{L7,TT/4},{M1,TT/8},{M2,TT/8},//question
    {L5,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/8},{M4,TT/4},{M4,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{M1,TT/2+TT/4},

    {0,TT/2},{M3,TT/8},{M4,TT/8},{M5,TT/4},{M5,TT/4},{M5,TT/4},{M5,TT/4},{M5,TT/8},{M6,TT/8},{M5,TT/8},{M4,TT/8},
    {M3,TT/4},{M3,TT/4},{M3,TT/4},{M3,TT/4},{M3,TT/8},{M4,TT/8},{M3,TT/8},{M2,TT/8},
    {M1,TT/4},{M1,TT/4},{M1,TT/8},{L7,TT/8},{L6,TT/4},{L7,TT/4},{L7,TT/8},{M1,TT/8},
    {M2,TT/4},{M2,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M2,TT/2},{M3,TT/8},{M4,TT/8},
    {M5,TT/4},{M5,TT/4},{M5,TT/4},{M5,TT/4},{M5,TT/8},{M6,TT/8},{M5,TT/8},{M4,TT/8},
    {M3,TT/4},{M3,TT/4},{M3,TT/4},{M3,TT/8},{M4,TT/8},{M3,TT/8},{M2,TT/8},{M1,TT/8},{L7,TT/8},
    {L6,TT/4},{L6,TT/8},{L7,TT/8},{M1,TT/8},{M2,TT/8},{L5,TT/4},{M1,TT/4},{M2,TT/8},{M3,TT/8},
    {M2,TT/4+TT/8},{M2,TT/8},{M2,TT/8},{M1,TT/8},{M1,TT/2+TT/4},
};

#define HaoYunLai_Length 156
const tNote HaoYunLai[HaoYunLai_Length]=
{
    {M6,TT/4},{H3,TT/8+TT/16},{H2,TT/16},{H2,TT/4},{H1,TT/8},{M6,TT/8},
    {M5,TT/4},{H1,TT/8+TT/16},{H2,TT/16},{M6,TT/4},{0,TT/4},
    {M6,TT/4},{H2,TT/4},{H1,TT/4},{M6,TT/8},{M5,TT/8},
    {M2,TT/4},{M5,TT/8},{M6,TT/8},{M5,TT/8},{M3,TT/4},{0,TT/8},
    //好运来祝你好运来，好运带来了喜和爱
    {M3,TT/4},{M6,TT/8+TT/16},{M5,TT/16},{M6,TT/4},{M6,TT/8},{M5,TT/8},
    {M6,TT/4},{H2,TT/8+TT/16},{H1,TT/16},{H2,TT/4},{0,TT/4},
    {H1,TT/8+TT/16},{H1,TT/16},{H1,TT/8},{H2,TT/8},{H3,TT/8},{H3,TT/8},{H2,TT/8},{H1,TT/8},
    {M5,TT/4},{H1,TT/8+TT/16},{M6,TT/16}, {M6,TT/2+TT/4},{0,TT/4},
    //好运来我们好运来，迎着好运兴旺发达通四海


    {M6,TT/8+TT/16},{M6,TT/16},{H1,TT/8},{H1,TT/8},{M6,TT/4},{0,TT/8},{M6,TT/8},
    {M5,TT/8},{M3,TT/8},{M5,TT/8},{H1,TT/8},{M6,TT/4},{0,TT/4},
    //叠个千纸鹤，再系个红腰带
    {M6,TT/8},{H1,TT/8},{H1,TT/8+TT/16},{H1,TT/16},{H1,TT/8},{M6,TT/8},{M5,TT/4},
    {M6,TT/8},{M5,TT/8},{M2,TT/8},{M5,TT/8},{M3,TT/4},{0,TT/8},{M2,TT/8},
    //愿善良的人们天天好运来
    {M3,TT/8},{M2,TT/8},{M1,TT/8},{M3,TT/8},{M2,TT/4},{0,TT/8},{M3,TT/8},
    {M6,TT/8},{M5,TT/8},{M3,TT/8},{M6,TT/8},{M6,TT/8},{M5,TT/4},{0,TT/8},
    //你勤劳生活美，你健康春常在
    {M6,TT/8},{H1,TT/8},{H1,TT/8+TT/16},{M6,TT/16},{H2,TT/8},{H2,TT/8},{H2,TT/8},{H1,TT/8},
    {M6,TT/4},{M5,TT/8},{H1,TT/8},{M6,TT/2+TT/4},{0,TT/4},
    //你一生的忙碌为了笑逐颜开

    {M6,TT/4},{H3,TT/8+TT/16},{H2,TT/16},{H2,TT/4},{H1,TT/8},{M6,TT/8},
    {M5,TT/4},{H1,TT/8+TT/16},{H2,TT/16},{M6,TT/4},{0,TT/4},
    {M6,TT/4},{H2,TT/4},{H1,TT/4},{M6,TT/8},{M5,TT/8},
    {M2,TT/4},{M5,TT/8},{M6,TT/8},{M5,TT/8},{M3,TT/4},{0,TT/8},
    //好运来祝你好运来，好运带来了喜和爱
    {M3,TT/4},{M6,TT/8+TT/16},{M5,TT/16},{M6,TT/4},{M6,TT/8},{M5,TT/8},
    {M6,TT/4},{H2,TT/8+TT/16},{H1,TT/16},{H2,TT/4},{0,TT/4},
    {H1,TT/8+TT/16},{H1,TT/16},{H1,TT/8},{H2,TT/8},{H3,TT/8},{H3,TT/8},{H2,TT/8},{H1,TT/8},
    {M5,TT/4},{H1,TT/8+TT/16},{M6,TT/16}, {M6,TT/2+TT/4},{0,TT/4},
    //好运来我们好运来，迎着好运兴旺发达通四海
    {H2,TT/4+TT/2},{0,TT/4},{H3,TT/4+TT/2},{0,TT/4},{M6,TT/4+TT/2},
    {0,TT/4},{M5,TT/8},{M5,TT/8},{M6,TT/4},{0,TT/4},
    //通四海，好运来
};
//秦时明月 画风 有点勉强  D调
#define HuaFeng_Length 357
const tNote HuaFeng[HuaFeng_Length]=
{
    {0,TT/8},{H3,TT/8},{H5,TT/8},{H3,TT/16},{H2,TT/16},{H2,TT/4},
    {0,TT/8},{H2,TT/8},{H5,TT/8},{H2,TT/16},{H1,TT/16},{H1,TT/4},
    {0,TT/8},{H3,TT/8},{H5,TT/8},{H3,TT/16},{H2,TT/16},{H2,TT/4},{H1,TT/8},{M6,TT/16},{H1,TT/16},
    {H1,TT/1},
    {0,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/16},{M2,TT/16},{M2,TT/8},
    {0,TT/8},{M2,TT/8},{M1,TT/8},{M2,TT/8},{M5,TT/8},{M2,TT/16},{M1,TT/16},{0,TT/8},{M5,TT/8},
    {M6,TT/8},{H1,TT/8},{M6,TT/16},{M5,TT/16},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},
    {M5,TT/8},{M3,TT/8},{M3,TT/2},{0,TT/4},
    {0,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/16},{M2,TT/16},{M2,TT/8},
    {0,TT/8},{M2,TT/8},{M1,TT/8},{M2,TT/8},{M5,TT/8},{M2,TT/16},{M1,TT/16},{0,TT/8},{M5,TT/8},
    {M6,TT/8},{M5,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/8},{M2,TT/16},{M1,TT/16},{M1,TT/8},
    {M1,TT/2},{0,TT/4},{0,TT/4},
    {0,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/16},{M2,TT/16},{M2,TT/8},
    {0,TT/8},{M2,TT/8},{M1,TT/8},{M2,TT/8},{M5,TT/8},{M2,TT/8},{M1,TT/8},{0,TT/8},{M5,TT/8},
    {M6,TT/8},{H1,TT/8},{M6,TT/16},{M5,TT/16},{M5,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M6,TT/16},{M5,TT/16},
    {M3,TT/8},{M3,TT/8},{M3,TT/2},{0,TT/4},
    {0,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},{M6,TT/16},{M3,TT/16},{M3,TT/16},{M2,TT/8},{M2,TT/16},
    {0,TT/8},{M2,TT/8},{M1,TT/8},{M2,TT/8},{M5,TT/8},{M2,TT/16},{M1,TT/16},{0,TT/8},{M5,TT/8},
    {M3,TT/4},{M1,TT/4},{L6,TT/8},{M2,TT/8},{M1,TT/8},{M1,TT/8},
    {M1,TT/2},{0,TT/8},{M6,TT/8},{M3,TT/8},{H2,TT/16},{H1,TT/16},
    {H1,TT/8},{H2,TT/8},{H3,TT/8},{H2,TT/8},{0,TT/8},{M5,TT/8},{H2,TT/8},{H1,TT/8},
    {M7,TT/8},{H1,TT/8},{H2,TT/8},{H1,TT/16},{H1,TT/16},{H1,TT/8},{M5,TT/16},{M6,TT/8},{H1,TT/16},
    {H1,TT/4},{0,TT/16},{M5,TT/16},{M6,TT/16},{H1,TT/16},{H3,TT/8},{H2,TT/16},{H1,TT/16},{H1,TT/8},{H3,TT/8},
    {H3,TT/2},{0,TT/8},{M6,TT/8},{H3,TT/8},{H2,TT/16},{H1,TT/16},
    {H1,TT/8},{H2,TT/8},{H3,TT/8},{H2,TT/8},{0,TT/8},{H5,TT/8},{H2,TT/8},{H1,TT/8},
    {H2,TT/8},{H3,TT/8},{H2,TT/8},{H1,TT/16},{H1,TT/16},{H1,TT/4},{M5,TT/16},{M6,TT/8},{H1,TT/16},
    {H1,TT/4},{0,TT/16},{M5,TT/16},{M5,TT/16},{H3,TT/16},{H2,TT/16},{H1,TT/16},{H1,TT/8},{M6,TT/8},{H1,TT/8},
    {H1,TT/2},{0,TT/8},{M6,TT/8},{H3,TT/8},{H2,TT/16},{H1,TT/16},
    {H1,TT/8},{H2,TT/8},{H3,TT/8},{H2,TT/8},{0,TT/8},{M5,TT/8},{H2,TT/8},{H1,TT/8},
    {M7,TT/8},{H1,TT/8},{H2,TT/8},{H1,TT/16},{H1,TT/16},{H1,TT/4},{M5,TT/16},{M6,TT/8},{H1,TT/16},
    {H1,TT/4},{0,TT/16},{M5,TT/16},{M6,TT/16},{H1,TT/16},{H3,TT/8},{H2,TT/16},{H1,TT/16},{H1,TT/8},{H3,TT/8},
    {H3,TT/2},{0,TT/8},{M6,TT/8},{H3,TT/8},{H2,TT/16},{H1,TT/16},
    {H1,TT/8},{8,TT/8},{H3,TT/8},{H2,TT/8},{0,TT/8},{H5,TT/8},{H2,TT/16},{H1,TT/16},{H1,TT/8},
    {H2,TT/8},{H3,TT/8},{H2,TT/8},{H1,TT/16},{H1,TT/16},{H1,TT/4},{M5,TT/16},{M6,TT/8},{M1,TT/16},
    {H1,TT/4},{0,TT/16},{M5,TT/16},{M6,TT/16},{H3,TT/16},{H2,TT/16},{H1,TT/16},{H1,TT/8},{M6,TT/8},{H2,TT/8},
    {H1,TT/2},{0,TT/4},{0,TT/4},
    {0,TT/8},{H3,TT/8},{H5,TT/8},{H3,TT/16},{H2,TT/16},{H2,TT/2},
    {0,TT/8},{H2,TT/8},{H5,TT/8},{H2,TT/16},{H1,TT/16},{H1,TT/2},
    {0,TT/8},{H3,TT/8},{H5,TT/8},{H3,TT/16},{H2,TT/16},{H2,TT/4},{H3,TT/8},{H5,TT/16},{H3,TT/16},
    {3,TT/2},{H1,TT/16},{H2,TT/16},{H3,TT/16},{H5,TT/16},{H3,TT/16},{H2,TT/16},{H1,TT/16},{H6,TT/16},
    {H3,TT/4},{H6,TT/4},{H2,TT/4},{H3,TT/4},{H2,TT/4},
    {H2,TT/8},{H3,TT/8},{H5,TT/8},{H1,TT/8},{H1,TT/2},
    {M6,TT/8},{H5,TT/4},{H3,TT/8},{H1,TT/8},{H2,TT/8},{M6,TT/8},{H2,TT/16},{H1,TT/16},
    {H1,TT/1},
    {H1,TT/2},{0,TT/4},{0,TT/4},
    {0,TT/8},{M3,TT/8},{M2,TT/8},{M3,TT/8},{M5,TT/8},{M3,TT/16},{M2,TT/16},{M2,TT/4},
    {0,TT/8},{M2,TT/8},{M1,TT/8},{M2,TT/8},{M5,TT/8},{M2,TT/16},{M1,TT/16},{0,TT/8},{M5,TT/8},
    {M6,TT/8},{M5,TT/8},{M5,TT/8},{M3,TT/8},{M3,TT/8},{M2,TT/4},{M2,TT/8},
    {M2,TT/2},{0,TT/4},{M2,TT/8},{M1,TT/16},{M1,TT/16},
    {M1,TT/4},{M1,TT/2},{0,TT/4},
};

#endif
