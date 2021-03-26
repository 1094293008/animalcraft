#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
//常量 
const char name[24][5]={"　","穴","阱","■","阱","穴","　","　","象","狮","虎","豹","狗","狼","猫","鼠","象","狮","虎","豹","狗","狼","猫","鼠"};//棋子名称
const int RANK_TOP = 3;//起始行
const int RANK_BOTTOM = 9;//终点行
const int FILE_LEFT = 3;//起始列
const int FILE_RIGHT = 11;//终点列 
const int LIMIT_DEPTH = 100;//最大深度
//以下为棋子编号，0-6对应7种棋子
const int PIECE_ELEPHANT = 0;
const int PIECE_LION = 1;
const int PIECE_TIGER = 2;
const int PIECE_LEOPARD = 3;
const int PIECE_WOLF = 4;
const int PIECE_DOG = 5;
const int PIECE_CAT = 6;
const int PIECE_MOUSE = 7;
//其它 
const int MAX_GEN_MOVES = 35; // 最大的生成走法数
const int MAX_MOVES = 1000;     // 最大的历史走法数
const int MATE_VALUE = 10000;  // 最高分值，即将死的分值
const int BAN_VALUE = MATE_VALUE - 100; // 长将判负的分值，低于该值将不写入置换表
const int WIN_VALUE = MATE_VALUE - 200; // 搜索出胜负的分值界限，超出此值就说明已经搜索出杀棋了
const int NULL_MARGIN = 400;   // 空步裁剪的子力边界
const int NULL_DEPTH = 2;      // 空步裁剪的裁剪深度
const int HASH_SIZE = 1 << 25; // 置换表大小
const int HASH_ALPHA = 1;      // ALPHA节点的置换表项
const int HASH_BETA = 2;       // BETA节点的置换表项
const int HASH_PV = 3;         // PV节点的置换表项
const int RANDOM_MASK = 3;     // 随机性分值

int t2=1000,depth=99999999,t,t3;//时间控制和深度控制
bool fenxi=0,player[2],turn,ranghu=0;

// 判断棋子是否在棋盘中的数组
static const char ccInBoard[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// 判断棋子是否在九宫的数组
static const char ccInFort[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 4, 0, 0, 3, 3, 3, 0, 0, 2, 0, 0, 0, 0,
  0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 2, 5, 0, 0, 0, 0,
  0, 0, 0, 4, 0, 0, 3, 3, 3, 0, 0, 2, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const bool ccCanJump[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// 步长
static const char ccDelta[4] = {-16, -1, 1, 16};
// 跳河步长
static const char ccJumpDelta[4] = {-48,-4,4,48};

// 棋盘初始设置
static int cucpcStartup[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0,10, 0, 8, 0, 0, 0,23, 0,17, 0, 0, 0, 0,
  0, 0, 0, 0,14, 0, 0, 0, 0, 0,20, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,13, 0, 0, 0,19, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,11, 0, 0, 0,21, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0,12, 0, 0, 0, 0, 0,22, 0, 0, 0, 0, 0,
  0, 0, 0, 9, 0,15, 0, 0, 0,16, 0,18, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
/*
// 子力位置价值表
static int cucvlPiecePos[8] = {100,120,70,50,30,20,10,5
};*/
// 子力位置价值表
static int cucvlPiecePos[8][256] = {
  { // 帅(将)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 72,77,73,72,74,74,74,73,71, 0, 0, 0, 0,
  0, 0, 0, 75,78,79, 0, 0, 0,75,78,76, 0, 0, 0, 0,
  0, 0, 0, 71,77,78, 0, 0, 0,76,83,85, 0, 0, 0, 0,
  0, 0, 0,  0,79,74,73,74,76,79,85, 0, 0, 0, 0, 0,
  0, 0, 0, 71,77,78, 0, 0, 0,76,83,85, 0, 0, 0, 0,
  0, 0, 0, 75,78,79, 0, 0, 0,75,78,76, 0, 0, 0, 0,
  0, 0, 0, 72,77,73,72,74,74,74,73,71, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },
   { // 帅(将)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0,103,108,116,117,120,122,125,116,113, 0, 0, 0, 0,
  0, 0, 0,105,111,122,  0,  0,  0,128,133,131, 0, 0, 0, 0,
  0, 0, 0, 99,114,122,  0,  0,  0,129,138,142, 0, 0, 0, 0,
  0, 0, 0,  0,101,118,120,121,122,134,142,  0, 0, 0, 0, 0,
  0, 0, 0, 99,114,122,  0,  0,  0,129,138,142, 0, 0, 0, 0,
  0, 0, 0,105,111,122,  0,  0,  0,128,133,131, 0, 0, 0, 0,
  0, 0, 0,103,108,116,117,120,122,125,116,113, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },
   { // 帅(将)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0,70,73,74,76,77,78,78,75,73, 0, 0, 0, 0,
  0, 0, 0,70,72,72, 0, 0, 0,78,79,80, 0, 0, 0, 0,
  0, 0, 0,66,72,72, 0, 0, 0,79,87,88, 0, 0, 0, 0,
  0, 0, 0, 0,68,71,75,76,77,83,88, 0, 0, 0, 0, 0,
  0, 0, 0,66,72,72, 0, 0, 0,79,87,88, 0, 0, 0, 0,
  0, 0, 0,70,72,72, 0, 0, 0,78,79,80, 0, 0, 0, 0,
  0, 0, 0,70,73,74,76,77,78,78,75,73, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },
   { // 帅(将)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0,37,38,39,39,39,39,41,40,38, 0, 0, 0, 0,
  0, 0, 0,36,40,38, 0, 0, 0,42,42,43, 0, 0, 0, 0,
  0, 0, 0,36,40,38, 0, 0, 0,42,48,50, 0, 0, 0, 0,
  0, 0, 0, 0,38,38,39,39,40,43,50, 0, 0, 0, 0, 0,
  0, 0, 0,36,40,38, 0, 0, 0,42,48,50, 0, 0, 0, 0,
  0, 0, 0,36,40,38, 0, 0, 0,42,42,43, 0, 0, 0, 0,
  0, 0, 0,37,38,39,39,39,39,41,40,38, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },
   { // 帅(将)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0,16,17,17,17,18,18,19,18,18, 0, 0, 0, 0,
  0, 0, 0,17,18,17, 0, 0, 0,21,23,24, 0, 0, 0, 0,
  0, 0, 0,15,18,17, 0, 0, 0,21,24,26, 0, 0, 0, 0,
  0, 0, 0, 0,15,17,18,19,19,22,25, 0, 0, 0, 0, 0,
  0, 0, 0,15,18,17, 0, 0, 0,21,24,26, 0, 0, 0, 0,
  0, 0, 0,17,18,17, 0, 0, 0,21,23,24, 0, 0, 0, 0,
  0, 0, 0,16,17,17,17,18,18,19,18,18, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },
   { // 帅(将)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 6, 6, 6, 6, 6, 7, 7, 6, 5, 0, 0, 0, 0,
  0, 0, 0, 5, 6, 6, 0, 0, 0, 8, 9, 9, 0, 0, 0, 0,
  0, 0, 0, 5, 7, 6, 0, 0, 0, 9,10,11, 0, 0, 0, 0,
  0, 0, 0, 0, 5, 6, 6, 6, 6, 8,11, 0, 0, 0, 0, 0,
  0, 0, 0, 5, 7, 6, 0, 0, 0, 9,10,11, 0, 0, 0, 0,
  0, 0, 0, 5, 6, 6, 0, 0, 0, 8, 9, 9, 0, 0, 0, 0,
  0, 0, 0, 6, 6, 6, 6, 6, 7, 7, 6, 5, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },
   { // 帅(将)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 4, 4, 5, 5, 5, 5, 5, 4, 4, 0, 0, 0, 0,
  0, 0, 0, 4, 5, 5, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0,
  0, 0, 0, 3, 5, 5, 0, 0, 0, 5, 6, 7, 0, 0, 0, 0,
  0, 0, 0, 0, 5, 5, 5, 5, 5, 6, 7, 0, 0, 0, 0, 0,
  0, 0, 0, 3, 5, 5, 0, 0, 0, 5, 6, 7, 0, 0, 0, 0,
  0, 0, 0, 4, 5, 5, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0,
  0, 0, 0, 4, 4, 5, 5, 5, 5, 5, 4, 4, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },
   { // 帅(将)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 7, 7, 8, 8, 9,10, 8, 7, 6, 0, 0, 0, 0,
  0, 0, 0, 5, 7, 7, 9,11,10, 8, 8, 8, 0, 0, 0, 0,
  0, 0, 0, 4, 7, 7, 9,11,10, 8, 9,12, 0, 0, 0, 0,
  0, 0, 0, 0, 5, 7, 8, 8, 8, 8,12, 0, 0, 0, 0, 0,
  0, 0, 0, 4, 7, 7, 9,11,10, 8, 9,12, 0, 0, 0, 0,
  0, 0, 0, 5, 7, 7, 9,11,10, 8, 8, 8, 0, 0, 0, 0,
  0, 0, 0, 7, 7, 8, 8, 9,10, 8, 7, 6, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },
};


// 判断棋子是否在棋盘中
inline bool IN_BOARD(int sq) {
  return ccInBoard[sq] != 0;
}

// 获得格子的横坐标
inline int RANK_Y(int sq) {
  return sq >> 4;
}

// 获得格子的纵坐标
inline int FILE_X(int sq) {
  return sq & 15;
}

// 根据纵坐标和横坐标获得格子
inline int COORD_XY(int x, int y) {
  return x + (y << 4);
}

//
inline int PIECE_NAME(int pc) {
  return (pc&7);
}


// 翻转格子
inline int SQUARE_FLIP(int sq) {
  return 254 - sq;
}

// 纵坐标水平镜像
inline int FILE_FLIP(int x) {
  return 14 - x;
}

// 横坐标垂直镜像
inline int RANK_FLIP(int y) {
  return 15 - y;
}

// 格子水平镜像
inline int MIRROR_SQUARE(int sq) {
  return COORD_XY(FILE_FLIP(FILE_X(sq)), RANK_Y(sq));
}

// 格子水平镜像
inline int SQUARE_FORWARD(int sq, int sd) {
  return sq - 16 + (sd << 5);
}

// 是否在河中
inline bool INRIVER(int sq) {
  return ccInFort[sq]==3;
}
// 是否在兽穴中 
inline bool INSHOUXUE(int sq,int tag) {
  if(tag==8) return ccInFort[sq]==1;
  return ccInFort[sq]==5;
}
// 是否在陷阱中 
inline bool INXIANJING(int sq,int tag) {
  if(tag==8) return ccInFort[sq]==2;
  return ccInFort[sq]==4;
}

// 是否已过河
inline bool AWAY_HALF(int sq, int sd) {
  return (sq & 0x80) == (sd << 7);
}

// 是否在河的同一边
inline bool SAME_HALF(int sqSrc, int sqDst) {
  return !((sqSrc ^ sqDst) & 0x80);
}

// 是否在同一行
inline bool SAME_RANK(int sqSrc, int sqDst) {
  return !((sqSrc ^ sqDst) & 0xf0);
}

// 是否在同一列
inline bool SAME_FILE(int sqSrc, int sqDst) {
  return !((sqSrc ^ sqDst) & 0x0f);
}

// 获得红黑标记(红子是8，黑子是16)
inline int SIDE_TAG(int sd) {
  return 8 + (sd << 3);
}

// 获得对方红黑标记
inline int OPP_SIDE_TAG(int sd) {
  return 16 - (sd << 3);
}

// 获得走法的起点
inline int SRC(int mv) {
  return mv & 255;
}

// 获得走法的终点
inline int DST(int mv) {
  return mv >> 8;
}

// 根据起点和终点获得走法
inline int MOVE(int sqSrc, int sqDst) {
  return sqSrc + sqDst * 256;
}

// 走法水平镜像
inline int MIRROR_MOVE(int mv) {
  return MOVE(MIRROR_SQUARE(SRC(mv)), MIRROR_SQUARE(DST(mv)));
  } 

// 走法水平镜像
inline int BLACK(int sq) {
	int x=FILE_X(sq),y=RANK_Y(sq);
	return COORD_XY(14-x,y);
  } 

// 历史走法信息(占4字节)
struct MoveStruct {
  int wmv,ucpcCaptured,wpc;

  void Set(int mv, int pcCaptured,int pc) {
    wmv = mv;
    ucpcCaptured = pcCaptured;
    wpc=pc;
  }
}; // mvs

// RC4密码流生成器
struct RC4Struct {
  int s[256];
  int x, y;

  void InitZero(void);   // 用空密钥初始化密码流生成器
  int Nextint(void) {  // 生成密码流的下一个字节
    int uc;
    x = (x + 1) & 255;
    y = (y + s[x]) & 255;
    uc = s[x];
    s[x] = s[y];
    s[y] = uc;
    return s[(s[x] + s[y]) & 255];
  }
  int NextLong(void) { // 生成密码流的下四个字节
    int uc0, uc1, uc2, uc3;
    uc0 = Nextint();
    uc1 = Nextint();
    uc2 = Nextint();
    uc3 = Nextint();
    return uc0 + (uc1 << 8) + (uc2 << 16) + (uc3 << 24);
  }
};

// 用空密钥初始化密码流生成器
void RC4Struct::InitZero(void) {
  int i, j;
  int uc;

  x = y = j = 0;
  for (i = 0; i < 256; i ++) {
    s[i] = i;
  }
  for (i = 0; i < 256; i ++) {
    j = (j + s[i]) & 255;
    uc = s[i];
    s[i] = s[j];
    s[j] = uc;
  }
}

// Zobrist结构
struct ZobristStruct {
  int dwKey, dwLock0, dwLock1;

  void InitZero(void) {                 // 用零填充Zobrist
    dwKey = dwLock0 = dwLock1 = 0;
  }
  void InitRC4(RC4Struct &rc4) {        // 用密码流填充Zobrist
    dwKey = rc4.NextLong();
    dwLock0 = rc4.NextLong();
    dwLock1 = rc4.NextLong();
  }
  void Xor(const ZobristStruct &zobr) { // 执行XOR操作
    dwKey ^= zobr.dwKey;
    dwLock0 ^= zobr.dwLock0;
    dwLock1 ^= zobr.dwLock1;
  }
  void Xor(const ZobristStruct &zobr1, const ZobristStruct &zobr2) {
    dwKey ^= zobr1.dwKey ^ zobr2.dwKey;
    dwLock0 ^= zobr1.dwLock0 ^ zobr2.dwLock0;
    dwLock1 ^= zobr1.dwLock1 ^ zobr2.dwLock1;
  }
};

// Zobrist表
static struct {
  ZobristStruct Player;
  ZobristStruct Table[16][256];
} Zobrist;

// 初始化Zobrist表
static void InitZobrist(void) {
  int i, j;
  RC4Struct rc4;

  rc4.InitZero();
  Zobrist.Player.InitRC4(rc4);
  for (i = 0; i < 16; i ++) {
    for (j = 0; j < 256; j ++) {
      Zobrist.Table[i][j].InitRC4(rc4);
    }
  }
}
// 局面结构
struct PositionStruct {
  bool sdPlayer;                   // 轮到谁走，0=红方，1=黑方
  int ucpcSquares[256];            // 棋盘上的棋子
  int vlWhite, vlBlack;            // 红、黑双方的子力价值
  int nDistance, nMoveNum;         // 距离根节点的步数，历史走法数
  MoveStruct mvsList[MAX_MOVES];   // 历史走法信息列表
  ZobristStruct zobr;              // Zobrist
  bool CanJump(int src,int dst)
  {
  	  if(PIECE_NAME(ucpcSquares[src])==PIECE_LION||PIECE_NAME(ucpcSquares[src])==PIECE_TIGER)
  	  {
  	  	if(!ccCanJump[src]||!ccCanJump[dst]) return 0;
  	  	  for(int i=0;i<=3;i++)
  	  	  {
  	  	  	  if(dst-src==ccJumpDelta[i])
  	  	  	  {
		  	  	  	  	for(int j=src+ccDelta[i];j!=dst&&IN_BOARD(j);j+=ccDelta[i])
		  	  	  	  	{
  	  	  	  		        if(ucpcSquares[j]==PIECE_MOUSE+24-(ucpcSquares[src]-PIECE_NAME(ucpcSquares[src]))||!INRIVER(j)) return 0;
						}
						return 1;
				}
		  }
		  return 0;
	  }
  	  return 0;
  }
  bool CanMove(int src,int dst)
  {
  	  if(PIECE_NAME(ucpcSquares[src])==PIECE_MOUSE)
  	  {
  	  	  for(int i=0;i<=3;i++)
  	  	  {
  	  	  	  if(dst-src==ccDelta[i])
  	  	  	  {
					return 1;
				}
		  }
		  return 0;
		}
  	  	if(INRIVER(dst)) return 0;
  	  	  for(int i=0;i<=3;i++)
  	  	  {
  	  	  	  if(dst-src==ccDelta[i])
  	  	  	  {
					return 1;
				}
		  }
		  return 0;
  }
  bool CanEat(int src,int dst)
  {
  	int as=PIECE_NAME(ucpcSquares[src]),bs=PIECE_NAME(ucpcSquares[dst]);
  	if(INXIANJING(dst,ucpcSquares[dst]-bs)) return 1;
  	  if(as==PIECE_MOUSE&&bs==PIECE_ELEPHANT)
  	  {
  	  	  if(INRIVER(src)&&!INRIVER(dst)) return 0;
  	  	  return 1;
	}
  	  if(as==PIECE_ELEPHANT&&bs==PIECE_MOUSE) return 0;
  	  return as<=bs;
  }
  void ClearBoard(void) {         // 清空棋盘
    sdPlayer = vlWhite = vlBlack = nDistance = 0;
    memset(ucpcSquares, 0, 256);
    zobr.InitZero();
  }
  void SetIrrev(void) {           // 清空(初始化)历史走法信息
    mvsList[0].Set(0, 0,0);
    nMoveNum = 1;
  }
  void Startup(void);             // 初始化棋盘
  void ChangeSide(void) {         // 交换走子方
    sdPlayer = 1 - sdPlayer;
    zobr.Xor(Zobrist.Player);
  }
  void AddPiece(int sq, int pc) { // 在棋盘上放一枚棋子
    ucpcSquares[sq] = pc;
    // 红方加分，黑方(注意"cucvlPiecePos"取值要颠倒)减分
    if (pc < 16) {
      vlWhite += cucvlPiecePos[pc - 8][sq];
      zobr.Xor(Zobrist.Table[pc - 8][sq]);
    } else {
      vlBlack += cucvlPiecePos[pc - 16][BLACK(sq)];
      zobr.Xor(Zobrist.Table[pc - 8][sq]);
    }
  }
  void DelPiece(int sq, int pc) { // 从棋盘上拿走一枚棋子
    ucpcSquares[sq] = 0;
    // 红方减分，黑方(注意"cucvlPiecePos"取值要颠倒)加分
    if (pc < 16) {
      vlWhite -= cucvlPiecePos[pc - 8][sq];
      zobr.Xor(Zobrist.Table[pc - 8][sq]);
    } else {
      vlBlack -= cucvlPiecePos[pc - 16][BLACK(sq)];
      zobr.Xor(Zobrist.Table[pc - 8][sq]);
    }
  }
  int Evaluate(void) const {      // 局面评价函数
    return (!sdPlayer ? vlWhite - vlBlack : vlBlack - vlWhite);
  }
  bool Captured(void) const {     // 上一步是否吃子
    return mvsList[nMoveNum - 1].ucpcCaptured != 0;
  }
  int MovePiece(int mv);                      // 搬一步棋的棋子
  void UndoMovePiece(int mv, int pcCaptured); // 撤消搬一步棋的棋子
  bool MakeMove(int mv);                      // 走一步棋
  void UndoMakeMove(void) {                   // 撤消走一步棋
    nDistance --;
    nMoveNum --;
    ChangeSide();
    UndoMovePiece(mvsList[nMoveNum].wmv, mvsList[nMoveNum].ucpcCaptured);
  }
  void NullMove(void) {                       // 走一步空步
    ChangeSide();
    mvsList[nMoveNum].Set(0, 0,0);
    nMoveNum ++;
    nDistance ++;
  }
  void UndoNullMove(void) {                   // 撤消走一步空步
    nDistance --;
    nMoveNum --;
    ChangeSide();
  }
  // 生成所有走法，如果"bCapture"为"1"则只生成吃子走法
  int GenerateMoves(int *mvs, bool bCapture = 0);
  bool LegalMove(int mv);               // 判断走法是否合理
  bool IsMate(void);
  bool RepWuLai(void) const;
  bool RepWuSong(void) const;
  bool RepStatus(void) const;
  bool NullOkay(void) const {                 // 判断是否允许空步裁剪
    return (sdPlayer == 0 ? vlWhite : vlBlack) > NULL_MARGIN;
  }
  void Mirror(PositionStruct &posMirror) const; // 对局面镜像
};

// 初始化棋盘
void PositionStruct::Startup(void) {
  int sq, pc;
  ClearBoard();
	memset(ucpcSquares,0,sizeof(ucpcSquares));
  for (sq = 0; sq < 256; sq ++) {
  	if(IN_BOARD(sq))
  	{
	    pc = cucpcStartup[sq];
	    if (pc != 0) {
	      AddPiece(sq, pc);
	    }
	}
  }
  SetIrrev();
}

// 搬一步棋的棋子
int PositionStruct::MovePiece(int mv) {
  int sqSrc, sqDst, pc, pcCaptured;
  sqSrc = SRC(mv);
  sqDst = DST(mv);
  pcCaptured = ucpcSquares[sqDst];
  if (pcCaptured != 0) {
    DelPiece(sqDst, pcCaptured);
  }
  pc = ucpcSquares[sqSrc];
  DelPiece(sqSrc, pc);
  AddPiece(sqDst, pc);
  return pcCaptured;
}

// 撤消搬一步棋的棋子
void PositionStruct::UndoMovePiece(int mv, int pcCaptured) {
  int sqSrc, sqDst, pc;
  sqSrc = SRC(mv);
  sqDst = DST(mv);
  pc = ucpcSquares[sqDst];
  DelPiece(sqDst, pc);
  AddPiece(sqSrc, pc);
  if (pcCaptured != 0) {
    AddPiece(sqDst, pcCaptured);
  }
}

// 走一步棋
bool PositionStruct::MakeMove(int mv) {
  int pcCaptured;

  pcCaptured = MovePiece(mv);
  ChangeSide();
  mvsList[nMoveNum].Set(mv, pcCaptured,ucpcSquares[DST(mv)]);
  nMoveNum ++;
  nDistance ++;
  return 1;
}
// "GenerateMoves"参数
const bool GEN_CAPTURE = 1;

// 生成所有走法，如果"bCapture"为"1"则只生成吃子走法
int PositionStruct::GenerateMoves(int *mvs, bool bCapture){
  int i, j, nGenMoves, nDelta, sqSrc, sqDst;
  int pcSelfSide, pcOppSide, pcSrc, pcDst;
  // 生成所有走法，需要经过以下几个步骤：

  nGenMoves = 0;
  pcSelfSide = SIDE_TAG(sdPlayer);
  pcOppSide = OPP_SIDE_TAG(sdPlayer);
  for (sqSrc = 0; sqSrc < 256; sqSrc ++) {
  	if(!IN_BOARD(sqSrc)) continue;
    // 1. 找到一个本方棋子，再做以下判断：
    pcSrc = ucpcSquares[sqSrc];
    if ((pcSrc & pcSelfSide) == 0) {
      continue;
    }
    // 2. 根据棋子确定走法
    for(int delta=0;delta<=3;delta++)
    {
    	sqDst=sqSrc+ccDelta[delta];
    	if(!IN_BOARD(sqDst)||INSHOUXUE(sqDst,pcSelfSide)) continue;
    	pcDst = ucpcSquares[sqDst];
    	if(!CanMove(sqSrc,sqDst))continue;
    	if(!ucpcSquares[sqDst]&&bCapture) continue;
        if (ucpcSquares[sqDst] ? ((pcDst & pcOppSide) != 0  && CanEat(sqSrc,sqDst) ): (pcDst & pcSelfSide) == 0) {
          mvs[nGenMoves] = MOVE(sqSrc, sqDst);
          nGenMoves ++;
        }
	}
	for(int delta=0;delta<=3;delta++)
	{
    	sqDst=sqSrc+ccJumpDelta[delta];
    	if(!IN_BOARD(sqDst)||INSHOUXUE(sqDst,pcSelfSide)) continue;
    	pcDst = ucpcSquares[sqDst];
    	if(!CanJump(sqSrc,sqDst))continue;
    	if(!ucpcSquares[sqDst]&&bCapture) continue;
        if (ucpcSquares[sqDst] ?  ((pcDst & pcOppSide) != 0  && CanEat(sqSrc,sqDst) ): (pcDst & pcSelfSide) == 0) {
          mvs[nGenMoves] = MOVE(sqSrc, sqDst);
          nGenMoves ++;
        }
	}
  }
  return nGenMoves;
}

// 判断走法是否合理
bool PositionStruct::LegalMove(int mv){
  int sqSrc, sqDst, sqPin;
  int pcSelfSide,pcOppSide, pcSrc, pcDst, nDelta;
  // 判断走法是否合法，需要经过以下的判断过程：

  // 1. 判断起始格是否有自己的棋子
  sqSrc = SRC(mv);
  pcSrc = ucpcSquares[sqSrc];
  pcSelfSide = SIDE_TAG(sdPlayer);
  pcOppSide = OPP_SIDE_TAG(sdPlayer);
  if ((pcSrc & pcSelfSide) == 0) {
    return 0;
  }

  // 2. 判断目标格是否有自己的棋子
  sqDst = DST(mv);
  pcDst = ucpcSquares[sqDst];
  if ((pcDst & pcSelfSide) != 0) {
    return 0;
  }
  
  // 2. 根据棋子确定走法
    for(int delta=0;delta<=3;delta++)
    {
    	int sqDst2=sqSrc+ccDelta[delta];
    	if(!IN_BOARD(sqDst)||INSHOUXUE(sqDst,pcSelfSide)||sqDst2!=sqDst) continue;
    	pcDst = ucpcSquares[sqDst];
    	if(!CanMove(sqSrc,sqDst))continue;
        if (ucpcSquares[sqDst] ? ((pcDst & pcOppSide) != 0  && CanEat(sqSrc,sqDst) ): (pcDst & pcSelfSide) == 0) {
        	return 1;
        }
	}
    for(int delta=0;delta<=3;delta++)
    {
    	int sqDst2=sqSrc+ccJumpDelta[delta];
    	if(!IN_BOARD(sqDst)||INSHOUXUE(sqDst,pcSelfSide)||sqDst2!=sqDst) continue;
    	pcDst = ucpcSquares[sqDst];
    	if(!CanJump(sqSrc,sqDst))continue;
        if (ucpcSquares[sqDst] ? ((pcDst & pcOppSide) != 0  && CanEat(sqSrc,sqDst) ): (pcDst & pcSelfSide) == 0) {
        	return 1;
        }
	}
	return 0;
}

// 判断是否被杀
bool PositionStruct::IsMate(void) {
	if(((ucpcSquares[99]>=8&&ucpcSquares[99]<=23)&&!sdPlayer)||((ucpcSquares[107]>=8&&ucpcSquares[107]<=23)&&sdPlayer)) return 1;
	return 0;
}

/*
// 检测重复局面
bool PositionStruct::RepStatus(void) const {
	if(player[!sdPlayer]==0) return 0;
	int count[24];
	memset(count,0,sizeof(count));
	for(int i=nMoveNum-1;i>=0;i-=2)
	{
		count[mvsList[i].wpc]++;
		if(count[mvsList[i].wpc]>=7) return 1;
		if(i==0||i==1) return 0;
		if(mvsList[i].wpc!=mvsList[i-2].wpc) return 0;
	}
	return 0;
}*/
bool PositionStruct::RepWuLai(void) const {
	int count[24][256];
	memset(count,0,sizeof(count));
	for(int i=nMoveNum-3;i>=nMoveNum-16&&i>=0;i-=2)
	{
		if(INXIANJING(DST(mvsList[i].wmv),16)||INXIANJING(DST(mvsList[i].wmv),8))
		{
			return 0;
		}
		count[mvsList[i].wpc][DST(mvsList[i].wmv)]++;
		if(count[mvsList[i].wpc][DST(mvsList[i].wmv)]>=3&&DST(mvsList[i].wmv)==DST(mvsList[nMoveNum].wmv))
		{
			return 1;
		}
	}
	return 0;
}
bool PositionStruct::RepWuSong(void) const {
	int animal=mvsList[nMoveNum-1].wpc,dst=DST(mvsList[nMoveNum-1].wmv),count[6],qigenum=0;
	memset(count,0,sizeof(count));
	for(int i=nMoveNum-3;i>=nMoveNum-36&&i>=0;i-=2)
	{
		if(INXIANJING(DST(mvsList[i].wmv),16)||INXIANJING(DST(mvsList[i].wmv),8))
		{
			return 0;
		}
		if(mvsList[i].wpc!=animal) return 0;
		bool rep=0;
		for(int j=0;j<qigenum;j++)
		{
			if(DST(mvsList[i].wmv)==count[j])
			{
				rep=1;
				break;
			}
		}
		if(rep==0) count[qigenum++]=DST(mvsList[i].wmv);
		if(qigenum>5) return 0;
	}
	for(int j=0;j<qigenum;j++)
	{
		if(dst==count[j])
		{
			return 1;
		}
	}
	return 0;
}
bool PositionStruct::RepStatus(void) const {
	if(RepWuLai()) return 1;
	if(RepWuSong()) return 1;
	return 0;
}


static PositionStruct pos; // 局面实例

// 与图形界面有关的全局变量
static struct {
  int mvLast;                       // 选中的格子，上一步棋
  bool bFlipped,bGameOver;                // 是否翻转棋盘，是否游戏结束(不让继续玩下去)
  int sqSelected;
} Xqwl;

// 初始化棋局
static void Startup(void) {
  pos.Startup();
  Xqwl.sqSelected = Xqwl.mvLast = 0;
  Xqwl.bGameOver = 0;
}
inline void printfenxi(int depth,clock_t time,int now,int total,int score,char mv[])//输出分析 
{
	if(t-t3>=250||now==1) printf("%d %.2lf %d/%d %d %s\r",depth,0.001*time,now,total,score,mv);
}
/*
以下为核心计算功能!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/ 

// 置换表项结构
struct HashItem {
  int ucDepth, ucFlag;
  short svl;
  int wmv, wReserved;
  int dwLock0, dwLock1;
};
// 与搜索有关的全局变量
static struct {
  int mvResult;                  // 电脑走的棋
  int nHistoryTable[65536];      // 历史表
  int mvKillers[LIMIT_DEPTH][2]; // 杀手走法表
  HashItem HashTable[HASH_SIZE]; // 置换表
} Search;
// 提取置换表项
static int ProbeHash(int vlAlpha, int vlBeta, int nDepth, int &mv) {
  bool bMate; // 杀棋标志：如果是杀棋，那么不需要满足深度条件
  HashItem hsh;

  hsh = Search.HashTable[pos.zobr.dwKey & (HASH_SIZE - 1)];
  if (hsh.dwLock0 != pos.zobr.dwLock0 || hsh.dwLock1 != pos.zobr.dwLock1) {
    mv = 0;
    return -MATE_VALUE;
  }
  mv = hsh.wmv;
  bMate = 0;
  if (hsh.svl > WIN_VALUE) {
    if (hsh.svl < BAN_VALUE) {
      return -MATE_VALUE; // 可能导致搜索的不稳定性，立刻退出，但最佳着法可能拿到
    }
    hsh.svl -= pos.nDistance;
    bMate = 1;
  } else if (hsh.svl < -WIN_VALUE) {
    if (hsh.svl > -BAN_VALUE) {
      return -MATE_VALUE; // 同上
    }
    hsh.svl += pos.nDistance;
    bMate = 1;
  }
  if (hsh.ucDepth >= nDepth || bMate) {
    if (hsh.ucFlag == HASH_BETA) {
      return (hsh.svl >= vlBeta ? hsh.svl : -MATE_VALUE);
    } else if (hsh.ucFlag == HASH_ALPHA) {
      return (hsh.svl <= vlAlpha ? hsh.svl : -MATE_VALUE);
    }
    return hsh.svl;
  }
  return -MATE_VALUE;
};

// 保存置换表项
static void RecordHash(int nFlag, int vl, int nDepth, int mv) {
  HashItem hsh;
  hsh = Search.HashTable[pos.zobr.dwKey & (HASH_SIZE - 1)];
  if (hsh.ucDepth > nDepth) {
    return;
  }
  hsh.ucFlag = nFlag;
  hsh.ucDepth = nDepth;
  if (vl > WIN_VALUE) {
    if (mv == 0 && vl <= BAN_VALUE) {
      return; // 可能导致搜索的不稳定性，并且没有最佳着法，立刻退出
    }
    hsh.svl = vl + pos.nDistance;
  } else if (vl < -WIN_VALUE) {
    if (mv == 0 && vl >= -BAN_VALUE) {
      return; // 同上
    }
    hsh.svl = vl - pos.nDistance;
  } else {
    hsh.svl = vl;
  }
  hsh.wmv = mv;
  hsh.dwLock0 = pos.zobr.dwLock0;
  hsh.dwLock1 = pos.zobr.dwLock1;
  Search.HashTable[pos.zobr.dwKey & (HASH_SIZE - 1)] = hsh;
};

// MVV/LVA每种子力的价值
static int cucMvvLva[24] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  15,24,13,7,4,3,2,3,
  15,24,13,7,4,3,2,3
};


// 求MVV/LVA值
inline int MvvLva(int mv) {
  return (cucMvvLva[pos.ucpcSquares[DST(mv)]] << 3) - cucMvvLva[pos.ucpcSquares[SRC(mv)]];
}
// "qsort"按MVV/LVA值排序的比较函数
static int CompareMvvLva(const void *lpmv1, const void *lpmv2) {
  return MvvLva(*(int *) lpmv2) - MvvLva(*(int *) lpmv1);
}

// "qsort"按历史表排序的比较函数
static int CompareHistory(const void *lpmv1, const void *lpmv2) {
  return Search.nHistoryTable[*(int *) lpmv2] - Search.nHistoryTable[*(int *) lpmv1];
}
// 走法排序阶段
const int PHASE_HASH = 0;
const int PHASE_KILLER_1 = 1;
const int PHASE_KILLER_2 = 2;
const int PHASE_GEN_MOVES = 3;
const int PHASE_REST = 4;

// 走法排序结构
struct SortStruct {
  int mvHash, mvKiller1, mvKiller2; // 置换表走法和两个杀手走法
  int nPhase, nIndex, nGenMoves;    // 当前阶段，当前采用第几个走法，总共有几个走法
  int mvs[MAX_GEN_MOVES];           // 所有的走法

  void Init(int mvHash_) { // 初始化，设定置换表走法和两个杀手走法
    mvHash = mvHash_;
    mvKiller1 = Search.mvKillers[pos.nDistance][0];
    mvKiller2 = Search.mvKillers[pos.nDistance][1];
    nPhase = PHASE_HASH;
  }
  int Next(void); // 得到下一个走法
};

// 得到下一个走法
inline int SortStruct::Next(void) {
  int mv;
  switch (nPhase) {
  // "nPhase"表示着法启发的若干阶段，依次为：

  // 0. 置换表着法启发，完成后立即进入下一阶段；
  case PHASE_HASH:
    nPhase = PHASE_KILLER_1;
    if (mvHash != 0) {
      return mvHash;
    }
    // 技巧：这里没有"break"，表示"switch"的上一个"case"执行完后紧接着做下一个"case"，下同

  // 1. 杀手着法启发(第一个杀手着法)，完成后立即进入下一阶段；
  case PHASE_KILLER_1:
    nPhase = PHASE_KILLER_2;
    if (mvKiller1 != mvHash && mvKiller1 != 0 && pos.LegalMove(mvKiller1)) {
      return mvKiller1;
    }

  // 2. 杀手着法启发(第二个杀手着法)，完成后立即进入下一阶段；
  case PHASE_KILLER_2:
    nPhase = PHASE_GEN_MOVES;
    if (mvKiller2 != mvHash && mvKiller2 != 0 && pos.LegalMove(mvKiller2)) {
      return mvKiller2;
    }

  // 3. 生成所有着法，完成后立即进入下一阶段；
  case PHASE_GEN_MOVES:
    nPhase = PHASE_REST;
    nGenMoves = pos.GenerateMoves(mvs);
    qsort(mvs, nGenMoves, sizeof(int), CompareHistory);
    nIndex = 0;

  // 4. 对剩余着法做历史表启发；
  case PHASE_REST:
    while (nIndex < nGenMoves) {
      mv = mvs[nIndex];
      nIndex ++;
      if (mv != mvHash && mv != mvKiller1 && mv != mvKiller2 && pos.LegalMove(mv)) {
        return mv;
      }
    }
  // 5. 没有着法了，返回零。
  default:
    return 0;
  }
}
// 对最佳走法的处理
inline void SetBestMove(int mv, int nDepth) {
  int *lpmvKillers;
  Search.nHistoryTable[mv] += nDepth * nDepth;
  lpmvKillers = Search.mvKillers[pos.nDistance];
  if (lpmvKillers[0] != mv) {
    lpmvKillers[1] = lpmvKillers[0];
    lpmvKillers[0] = mv;
  }
}

// 静态(Quiescence)搜索过程
static int SearchQuiesc(int vlAlpha, int vlBeta) {
  int i, nGenMoves;
  int vl, vlBest;
  int mvs[MAX_GEN_MOVES];
  // 一个静态搜索分为以下几个阶段

  
  // 2. 到达极限深度就返回局面评价
  if (pos.nDistance == LIMIT_DEPTH) {
    return pos.Evaluate();
  }

  // 3. 初始化最佳值
  vlBest = -MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)

 {

    // 5. 如果不被将军，先做局面评价
    vl = pos.Evaluate();
    if (vl > vlBest) {
      vlBest = vl;
      if (vl >= vlBeta) {
        return vl;
      }
      if (vl > vlAlpha) {
        vlAlpha = vl;
      }
    }


    // 6. 如果局面评价没有截断，再生成吃子走法
    nGenMoves = pos.GenerateMoves(mvs, GEN_CAPTURE);
    qsort(mvs, nGenMoves, sizeof(int), CompareMvvLva);
  }

  // 7. 逐一走这些走法，并进行递归
  for (i = 0; i < nGenMoves; i ++) {
    if (pos.MakeMove(mvs[i])) {
    	if(pos.RepStatus())
    	{
    		pos.UndoMakeMove();
			continue;
		}
      vl = -SearchQuiesc(-vlBeta, -vlAlpha);
      pos.UndoMakeMove();

      // 8. 进行Alpha-Beta大小判断和截断
      if (vl > vlBest) {    // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
        vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
        if (vl >= vlBeta) { // 找到一个Beta走法
          return vl;        // Beta截断
        }
        if (vl > vlAlpha) { // 找到一个PV走法
          vlAlpha = vl;     // 缩小Alpha-Beta边界
        }
      }
    }
  }

  // 9. 所有走法都搜索完了，返回最佳值
  return vlBest == -MATE_VALUE ? pos.nDistance - MATE_VALUE : vlBest;
}
long long node=0;
// "SearchFull"的参数
const bool NO_NULL = 1;
// 零窗口完全搜索例程
static int SearchCut(int vlBeta, int nDepth, bool bNoNull = false) {
  int nNewDepth, vlBest=-MATE_VALUE, vl;
  int mvHash, mv, mvEvade;
  SortStruct MoveSort;
  // 完全搜索例程包括以下几个步骤：

  if(pos.IsMate()) return -MATE_VALUE;
  // 1. 在叶子结点处调用静态搜索；
  if (nDepth <= 0) {
    return SearchQuiesc(vlBeta - 1, vlBeta);
  }

  // 3. 置换裁剪；
  vl = ProbeHash(vlBeta - 1, vlBeta, nDepth, mvHash);
  if (vl > -MATE_VALUE) {
    return vl;
  }

  // 6. 尝试空着裁剪；
  if (!bNoNull && pos.NullOkay()) {
    pos.NullMove();
    vl = -SearchCut(1 - vlBeta, nDepth - NULL_DEPTH - 1, NO_NULL);
    pos.UndoNullMove();

    if (vl >= vlBeta) {
    	return vl;
    }
  }
MoveSort.Init(mvHash);

  // 8. 按照"MoveSortStruct::NextFull()"例程的着法顺序逐一搜索；
  while ((mv = MoveSort.Next()) != 0) {
    if (pos.MakeMove(mv)) {

    	if(pos.RepStatus())
    	{
    		pos.UndoMakeMove();
			continue;
		}
      // 9. 尝试选择性延伸；
      nNewDepth = nDepth - 1;

      // 10. 零窗口搜索；
      vl = -SearchCut(1 - vlBeta, nNewDepth);
      pos.UndoMakeMove();

      // 11. 截断判定；
      if (vl > vlBest) {
        vlBest = vl;
        if (vl >= vlBeta) {
          RecordHash(HASH_BETA, vlBest, nDepth, mv);
          SetBestMove(mv, nDepth);
          return vlBest;
        }
      }
    }
  }

  // 12. 不截断措施。
  if (vlBest == -MATE_VALUE) {
    return pos.nDistance - MATE_VALUE;
  } else {
    return vlBest;
  }
}
// 超出边界(Fail-Soft)的Alpha-Beta搜索过程
static int SearchFull(int vlAlpha, int vlBeta, int nDepth, bool bNoNull = 0) {
	node++;
  int nHashFlag, vl, vlBest;
  int mv, mvBest, mvHash, nNewDepth;
  SortStruct Sort;
  // 一个Alpha-Beta完全搜索分为以下几个阶段
  // 1. 到达水平线，则调用静态搜索(注意：由于空步裁剪，深度可能小于零)
  if(pos.IsMate()) return -MATE_VALUE;
  if (nDepth <= 0) {
   return SearchQuiesc(vlAlpha, vlBeta);
  }
   // 1-2. 到达极限深度就返回局面评价
  if (pos.nDistance == LIMIT_DEPTH) {
    return pos.Evaluate();
  }
  // 1-3. 尝试置换表裁剪，并得到置换表走法
  vl = ProbeHash(vlAlpha, vlBeta, nDepth, mvHash);
  if (vl > -MATE_VALUE) {
    return vl;
  }

  // 1-4. 尝试空步裁剪(根节点的Beta值是"MATE_VALUE"，所以不可能发生空步裁剪)
  if (!bNoNull && pos.NullOkay()) {
    pos.NullMove();
    vl = -SearchFull(-vlBeta, 1 - vlBeta, nDepth - NULL_DEPTH - 1, NO_NULL);
    pos.UndoNullMove();
    if (vl >= vlBeta) {
      return vl;
    }
  }

  // 2. 初始化最佳值和最佳走法
  nHashFlag = HASH_ALPHA;
  vlBest = -MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)
  mvBest = 0;           // 这样可以知道，是否搜索到了Beta走法或PV走法，以便保存到历史表

  // 3. 初始化走法排序结构
  Sort.Init(mvHash);
  
	
  // 4. 逐一走这些走法，并进行递归
  while ((mv = Sort.Next()) != 0) {
    if (pos.MakeMove(mv)) {
    	if(pos.RepStatus())
    	{
    		pos.UndoMakeMove();
			continue;
		}
      // 将军延伸
      nNewDepth = nDepth-1;
      // PVS
      if (vlBest == -MATE_VALUE) {
        vl = -SearchFull(-vlBeta, -vlAlpha, nNewDepth);
      } else {
        vl = -SearchCut(-vlAlpha, nNewDepth);
        if (vl > vlAlpha && vl < vlBeta) {
          vl = -SearchFull(-vlBeta, -vlAlpha, nNewDepth);
        }
      }
      pos.UndoMakeMove();
      // 5. 进行Alpha-Beta大小判断和截断
      if (vl > vlBest) {    // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
        vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
        if (vl >= vlBeta) { // 找到一个Beta走法
          nHashFlag = HASH_BETA;
          mvBest = mv;      // Beta走法要保存到历史表
          break;            // Beta截断
        }
        if (vl > vlAlpha) { // 找到一个PV走法
          nHashFlag = HASH_PV;
          mvBest = mv;      // PV走法要保存到历史表
          vlAlpha = vl;     // 缩小Alpha-Beta边界
        }
      }
    }
  }

  // 5. 所有走法都搜索完了，把最佳走法(不能是Alpha走法)保存到历史表，返回最佳值
  if (vlBest == -MATE_VALUE) {
    // 如果是杀棋，就根据杀棋步数给出评价
    return pos.nDistance - MATE_VALUE;
  }
  // 记录到置换表
  RecordHash(nHashFlag, vlBest, nDepth, mvBest);
  if (mvBest != 0) {
    // 如果不是Alpha走法，就将最佳走法保存到历史表
    SetBestMove(mvBest, nDepth);
  }
  return vlBest;
}

inline void DrawBoard(void) {
	system("cls");
  int x, y, xx, yy, sq, pc;
  if(Xqwl.bFlipped)
  {
  for (y = RANK_BOTTOM; y >= RANK_TOP; y --)
  {
	printf("%2d",y-2);
    for (x = FILE_RIGHT; x >= FILE_LEFT;x --)
	{
		sq=COORD_XY(x, y);
     	if(pos.ucpcSquares[sq]) printf("%s",name[pos.ucpcSquares[sq]]);
     	else printf("%s",name[ccInFort[sq]]);
    }
    printf("\n");
  }
  printf("   9 8 7 6 5 4 3 2 1\n");
  }
  else
  {
  for (y = RANK_TOP; y <= RANK_BOTTOM; y ++)
  {
	printf("%2d",y-2);
    for (x = FILE_LEFT; x <= FILE_RIGHT;x ++)
	{
		sq=COORD_XY(x, y);
     	if(pos.ucpcSquares[sq]) printf("%s",name[pos.ucpcSquares[sq]]);
     	else printf("%s",name[ccInFort[sq]]);
    }
    printf("\n");
  }
  printf("   1 2 3 4 5 6 7 8 9\n");
  }
  if(ranghu) printf("第%d回合\n",(pos.nMoveNum+1)/2);
}
int nGenMoves,mvs[MAX_GEN_MOVES];
// 根节点的Alpha-Beta搜索过程
static int SearchRoot(int nDepth) {
  int vl, vlBest, mv, nNewDepth,i;
  int alpha=-MATE_VALUE,beta=MATE_VALUE;
  i=0;
  SortStruct Sort;

  vlBest = -MATE_VALUE;
  Sort.Init(Search.mvResult);
  while ((mv = Sort.Next()) != 0) {
    if (pos.MakeMove(mv)) {
    	if(pos.RepStatus())
    	{
    		pos.UndoMakeMove();
			continue;
		}
		i++;
      nNewDepth =nDepth-1;
		
      if (vlBest == -MATE_VALUE) {
        vl = -SearchFull(-MATE_VALUE, MATE_VALUE, nNewDepth, NO_NULL);
      } else {
        vl = -SearchCut(-vlBest, nNewDepth);
        if (vl > vlBest) {
          vl = -SearchFull(-MATE_VALUE, -vlBest, nNewDepth, NO_NULL);
        }
      }
      pos.UndoMakeMove();
      if (vl > vlBest) {
        vlBest = vl;
	    Search.mvResult = mv;
        if (vlBest > -WIN_VALUE && vlBest < WIN_VALUE) {
          vlBest += (rand() & RANDOM_MASK) - (rand() & RANDOM_MASK);
        }
      }
    }
    

    if(fenxi)
	{
		char aa[15];
		strcpy(aa,name[pos.ucpcSquares[SRC(mv)]]); 
		switch(DST(mv)-SRC(mv))
		{
			case 1: strcat(aa,"→");break;
			case 4: strcat(aa,"→");break;
			case -1: strcat(aa,"←");break;
			case -4: strcat(aa,"←");break;
			case -16: strcat(aa,"↑");break;
			case -48: strcat(aa,"↑");break;
			case 16: strcat(aa,"↓");break;
			case 48: strcat(aa,"↓");
		}
		printfenxi(nDepth,t-t3,i,nGenMoves,vlBest,aa);
	}
  	t = clock(); 
  	if(t-t3>t2) break;
  }
  RecordHash(HASH_PV, vlBest, nDepth, Search.mvResult);
  SetBestMove(Search.mvResult, nDepth);
  return vlBest;
}
// 迭代加深搜索过程
static int SearchMain(void) {
  int i, vl;

  // 初始化
  memset(Search.mvKillers, 0, LIMIT_DEPTH * 2 * sizeof(int)); // 清空杀手走法表
  memset(Search.nHistoryTable, 0, 65536 * sizeof(int));       // 清空历史表
  memset(Search.HashTable, 0, HASH_SIZE * sizeof(HashItem));  // 清空置换表
  
  
  t3 = t = clock();       // 初始化定时器
  pos.nDistance = 0; // 初始步数

  

  // 检查是否只有唯一走法
  vl = 0;
  nGenMoves = pos.GenerateMoves(mvs);
  for (i = 0; i < nGenMoves; i ++) {
    if (pos.MakeMove(mvs[i])) {
      pos.UndoMakeMove();
      Search.mvResult = mvs[i];
      vl ++;
    }
  }
  for(i=0;i<nGenMoves;i++)
  {
  	if(!pos.LegalMove(mvs[i])) nGenMoves--;
  }
  if (vl == 1) {
    return Search.mvResult;
  }
  

  // 迭代加深过程
  for (i = 1; i <= depth; i ++) {
	t=clock();
    vl = SearchRoot(i);
    // 搜索到杀棋，就终止搜索
    if (vl > WIN_VALUE || vl < -WIN_VALUE) {
      break;
    }
    // 超过n秒，就终止搜索
    if (t - t3 > t2) {
      break;
    }
  }
  return Search.mvResult;
}
// 电脑回应一步棋
static int ResponseMove(void) {
	srand((int) time(NULL));
	InitZobrist();
  // 电脑走一步棋
  SearchMain();
  Xqwl.mvLast=Search.mvResult;
  pos.MakeMove(Xqwl.mvLast);
  
  if (pos.IsMate()) {
				DrawBoard();
    // 如果分出胜负，那么播放胜负的声音，并且弹出不带声音的提示框
          printf("请再接再厉！\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
	Xqwl.bGameOver = 1;
	return 2;
  }else {
  	
        	if(pos.RepStatus())
        	{
          printf("祝贺你取得胜利！\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
				DrawBoard();
          Xqwl.bGameOver = 1;
          return 2;
			}
  return 0;
  }
}
bool can=1;
// 点击格子事件处理
static int ClickSquare(int sq) {
  int pc, mv;
  //sq = Xqwl.bFlipped ? SQUARE_FLIP(sq) : sq;
  pc = pos.ucpcSquares[sq];

  if ((pc & SIDE_TAG(pos.sdPlayer)) != 0)
  {
    // 如果点击自己的子，那么直接选中该子
    Xqwl.sqSelected = sq;
    return 5;
  }
  else if (Xqwl.sqSelected != 0 && !Xqwl.bGameOver)
  {
    mv = MOVE(Xqwl.sqSelected, sq);
    if (pos.LegalMove(mv)) {
      if (pos.MakeMove(mv)) {
      	can=1;
        Xqwl.mvLast = mv;
        Xqwl.sqSelected = 0;
        if (pos.IsMate()) {
				DrawBoard();
          printf("祝贺你取得胜利！\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
          Xqwl.bGameOver = 1;
          return 1;
        } else {
        	if(pos.RepStatus())
        	{
          printf("请再接再厉！\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
				DrawBoard();
          Xqwl.bGameOver = 1;
          return 2;
			}
          return 0;
          //ResponseMove();
        }
      }
      return 5;
    }
    return 5;
    // 如果根本就不符合走法(例如马不走日字)，那么程序不予理会
  }
}
static int WndProc(void)
{
	int x,y;
	scanf("%d %d",&x,&y);
  	if (x+2 >= FILE_LEFT && x+2 <= FILE_RIGHT && y+2 >= RANK_TOP && y+2 <= RANK_BOTTOM)
	{
  		int a=ClickSquare(COORD_XY(x+2, y+2));
  		if(a!=5) return a;
  		else return 0;
  	}
  	return 0;
}
int main()
{
	Xqwl.bFlipped =0;
	char a='1';
	while(a!='E'&&a!='e')
	{
		system("cls");
		system("title 野兽争霸斗兽棋AI(V1.31) QQ:403809264");
		printf("野兽争霸斗兽棋AI V1.31\n");
		if(fenxi) printf("请选择功能：\nA  我当红棋\nB  我当黑棋\nC  双人对战\nD  电脑对战\nE  退出\nF  设置电脑时间(%d毫秒)\nG  设置计算深度(%d层)\nH  隐藏分析\n",t2,depth);
		else printf("请选择功能：\nA  我当红棋\nB  我当黑棋\nC  双人对战\nD  电脑对战\nE  退出\nF  设置电脑时间(%d毫秒)\nG  设置计算深度(%d层)\nH  显示分析\n",t2,depth);
		if(Xqwl.bFlipped) printf("I  翻转棋盘(目前红方在左)\n");
		else printf("I  翻转棋盘(目前蓝方在左)\n");
		if(cucpcStartup[155]) printf("J  开启让虎攻防模式\n");
		else printf("J  关闭让虎攻防模式\n");
		scanf("%c",&a);
		if(a=='A'||a=='a'||a=='B'||a=='b'||a=='C'||a=='c'||a=='D'||a=='d')
		{
			if(a=='A'||a=='a')
			{
				player[0]=0;
				player[1]=1;
			}
			else if(a=='B'||a=='b')
			{
				player[0]=1;
				player[1]=0;
			}
			else if(a=='C'||a=='c')
			{
				player[0]=0;
				player[1]=0;
			}
			else if(a=='D'||a=='d')
			{
				player[0]=1;
				player[1]=1;
			}
			turn=1;
			Startup();
			while(!Xqwl.bGameOver)
			{
				turn=!turn;
				DrawBoard();
				if(player[turn])
				{
					if(Xqwl.bGameOver)
					{
						break;
					}
					ResponseMove();
				}
				else
				{
					if(Xqwl.bGameOver)
					{
						break;
					}
					can=0;
					while(!can)
					{
						WndProc();
					}
				}
				if(Xqwl.bGameOver)
				{
					break;
				}
				int mv=Xqwl.mvLast;
			}
			system("pause>nul");
		}
		else if(a=='F'||a=='f')
		{
			printf("输入电脑运算时间(毫秒)(0为无限制)：");
			scanf("%d",&t2);
			if(t2<=0) t2=99999999;
		}
		else if(a=='G'||a=='g')
		{
			printf("输入电脑运算深度(层数)(0为无限制)：");
			scanf("%d",&depth);
			if(depth<=0) depth=99999999;
		}
		else if(a=='H'||a=='h')
		{
			fenxi=!fenxi;
		}
		else if(a=='I'||a=='i')
		{
			Xqwl.bFlipped=!Xqwl.bFlipped;
		}
		else if(a=='J'||a=='j')
		{
			cucpcStartup[155]=18-cucpcStartup[155];
			ranghu=!ranghu;
		}
	}
	return 0;
}
