/* 巡回セールスマン問題を解く局所探索法による近似アルゴリズム（2-OPTおよびOR-OPT近傍) */

#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#define N 600         /* 地点数の最大サイズ */
#define ZERO 0.0001   /* 許容誤差幅 */
#define SEED 14       /* 乱数の種 */ 
struct point          /* 構造体pの定義 */
{
  float x;
  float y;
};
int n;                /* 外部変数：点の個数 */
struct point p[N];    /* p[i].xとp[i].yは点iの座標 */

/* 関数の宣言 */
float initial(int *x);
float local(int *init, float lginit, int *lopt);
float length(int i, int j, int *x);
void printtour(int *x, float length);
int rand_from(int min, int max);

main()
/* TSP局所探索法のテストプログラム */
{
  int i, init[N], lopt[N];
  float initlg, bestlg;
  FILE *file;
  file=fopen("locdata1", "r");   /* 入力データの読込 */
  fscanf(file, "%d", &n);
  for(i=0; i<n; i++){
    fscanf(file, "%f", &p[i].x);
    fscanf(file, "%f", &p[i].y);
  }
  printf("n = %d\n", n);
  printf("point i       x[i]       y[i] \n");
  for(i=0; i<n; i++) 
    printf("%3d        %f,  %f\n", i, p[i].x, p[i].y);
  srand(SEED);
  initlg=initial(init);          /* 初期巡回路の生成 */
  printf("initial tour\n");
  printtour(init, initlg);
  bestlg=local(init, initlg, lopt);  /* 局所探索 */
  printf("local optimal tour\n");    /* 結果の出力 */
  printtour(lopt, bestlg);
}

float initial(int *x)
/* 地点数nの初期巡回路をxに作成、その長さが出力される */
{
  int i, j, r, test[N];   /* test[i]は地点iの利用フラグ */
  float lg;
  for(i=0; i<n; i++) test[i]=0;
  j=0;
  for(i=0; i<n; i++){
    r=rand_from(0, n-i-1);  /* 地点jからr個目の空き地点をx[i]とする */
    while(test[j]==1) j=(j+1)%n;
    while(r>0){
      r=r-1; j=(j+1)%n;
      while(test[j]==1) j=(j+1)%n;
    }
    test[j]=1; x[i]=j;
  } 
  lg=0.0;
  for(i=0; i<n; i++) lg=lg+length(i, i+1, x);  /* 巡回路の長さ */
  return(lg);
}

float local(int *init, float initlg, int *lopt)
/* 巡回路initを初期解（その長さinitlg）として、2-OPTおよびOR-OPT近傍に
よる局所探索を適用し、局所最適解loptの巡回路長を出力 */
{
  int i, i0, j, k, h, temp, tm[3];
  float lg, lgtemp, lg0, lg1;
  for(i=0; i<n; i++) lopt[i]=init[i];
  lg=initlg;
  i0=0;
RESTART:                              /* 近傍探索の開始 */
  for(i=i0; i<i0+n; i++){             /* 2-OPT近傍の探索 */
    for(j=i+2; j<i+n-1; j++){         /* lgtempは長さの増分 */
      lgtemp=length(i, j, lopt)+length(i+1, j+1, lopt)-length(i, i+1, lopt)-length(j, j+1, lopt);
      if(lgtemp<-ZERO){               /* 改良解の発見 */
        lg=lg+lgtemp;
        for(k=0; k<(j-i)/2; k++){     /* 改良解の構成 */
          temp=lopt[(i+1+k)%n]; lopt[(i+1+k)%n]=lopt[(j-k)%n]; 
          lopt[(j-k)%n]=temp;
        }
        printf("improved solution by 2-OPT\n");
        goto IMPROVED;
      }
    } 
  } 
  for(i=i0; i<i0+n; i++){           /* Or-OPT近傍の探索 */ 
    for(k=i+1; k<=i+3; k++){
      for(j=k+1; j<i+n-1; j++){     /* lgtempは長さの増分 */
        lg0=length(i, i+1, lopt)+length(j, j+1,lopt)+length(k, k+1, lopt);
        lg1=length(i, k+1, lopt)+length(j, k, lopt)+length(i+1, j+1,lopt);
        lgtemp=lg1-lg0;
        if(lgtemp<-ZERO){           /* 改良解の発見 */
          lg=lg+lgtemp;             /* 改良解の構成 */
          for(h=i+1; h<=k; h++) tm[h-i-1]=lopt[h%n];
          for(h=k+1; h<=j; h++) lopt[(h-k+i)%n]=lopt[h%n];
          for(h=0; h<k-i; h++) lopt[(j-k+i+1+h)%n]=tm[k-i-1-h];
          printf("improved solution by Or-OPT\n");
          goto IMPROVED;
        }
        if(k==i+1) continue;
        lg1=length(i, k+1, lopt)+length(j, i+1, lopt)+length(k, j+1, lopt); /* 逆方向の挿入 */
        lgtemp=lg1-lg0;
        if(lgtemp<-ZERO){                /* 改良解の発見 */
          lg=lg+lgtemp;                  /* 改良解の構成 */
          for(h=i+1; h<=k; h++) tm[h-i-1]=lopt[h%n];
          for(h=k+1; h<=j; h++) lopt[(h-k+i)%n]=lopt[h%n];
          for(h=0; h<k-i; h++) lopt[(j-h)%n]=tm[k-i-1-h];
          printf("improved solution by Or-OPT\n");
          goto IMPROVED;
        }
      }
    }
  }
  return(lg);                   /* 局所探索終了 */
IMPROVED:                       /* 暫定解の更新：次の近傍探索へ */
  printtour(lopt, lg); i0=(i+1)%n; 
  goto RESTART;
}

float length(int i, int j, int *x)
/* p[x[i%n]]とp[x[j%n]]のユークリッド距離 */
{
  return(sqrt(pow(p[x[i%n]].x-p[x[j%n]].x, 2)+pow(p[x[i%n]].y-p[x[j%n]].y, 2)));
}

void printtour(int *x, float lg)
/* 巡回路xとその長さlgを出力 */
{
  int i;
  printf("tour = (");
  for(i=0; i<n; i++) printf("%d ", x[i]); printf(")\n");
  printf("length = %f\n", lg);
}

int rand_from(int min, int max)
/* 区間[min, max]からランダムに整数を選ぶ */
/* randは0からRAND_MAXまでの乱整数を生成する標準関数 */
{
  return((int)(((double)rand()/((unsigned)RAND_MAX+1))*(max-min+1))+min);
}