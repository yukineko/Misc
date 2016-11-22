////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ ドラッグ＆ドロップ信号処理プログラム  //
//  スペクトログラム作成                              //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.01.08.//
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define  MEM_SIZE 16000                                     // 音声メモリのサイズ
#define  FFT_SIZE 512                                       // FFT点数
#define  OL       2                                         // フレームシフト ＝ FFT_SIZE / OL
///////////////////////////////////////////////
//                                           //
//                                           //
//            FFT関数の構築                  //
//                                           //
//                                           //
///////////////////////////////////////////////
static double Wr[FFT_SIZE+1],Wi[FFT_SIZE+1];                // FFT 重み
static double Fr[FFT_SIZE+1],Fi[FFT_SIZE+1],Xr[FFT_SIZE+1],Xi[FFT_SIZE+1]; //実部と虚部
static double xin[FFT_SIZE+1],z[FFT_SIZE+1];                // FFT入力と出力
static double Xamp[FFT_SIZE+1];                             // 振幅スペクトル
static double omega;                                        // FFT角周波数
static int    St,br[FFT_SIZE+1];                            // FFTステージ番号，ビット反転

void bitr( void );                                          // ビット反転用関数の宣言
void fft ( void );                                          // FFT関数の宣言
void Wnk ( void );                                          // FFTバタフライ(BF)演算の重み関数の宣言
void ifft( void );                                          // IFFT関数の宣言
void init( void );                                          // FFT関連変数の初期設定

///////////////////////////////////////////////
//                                           //
//                初期設定                   //
//                                           //
///////////////////////////////////////////////
void init(void){
    int i;
    St=log((double)FFT_SIZE)/log(2.0)+0.5;                  // FFTアルゴリズムの段数の算出 +0.5はintへの対応
    omega=2.0*M_PI/FFT_SIZE;
    bitr();Wnk();
}

void bitr( void ){                                          // ビット反転
    int loop,i,j;
    br[0]=0;                                                // 0番目は0
    br[1]=FFT_SIZE/2;                                       // 1番目はN/2
    loop=1;
    for(j=0;j<St-1;j++){
        br[loop*2]=br[loop]/2;
        loop=loop*2;
        for(i=1;i<loop;i++){
            br[loop+i]=br[loop]+br[i];
        }
    }
}

void Wnk(void){                                             // 重みの計算
    int i;
    for(i=0;i<FFT_SIZE/2;i++){
        Wr[i]=cos(omega*i);                                 // 重み実部
        Wi[i]=sin(omega*i);                                 // 重み虚部
    }
}

///////////////////////////////////////////////
//                                           //
//                   FFT                     //
//                                           //
///////////////////////////////////////////////
void fft( void ){
    int _2_s,_2_s_1,roop,l,m,p;
    int s,j,k;
    double Wxmr,Wxmi;
    for(j=0;j<FFT_SIZE;j++){                                // FFT入力の設定
        Fr[br[j]]=xin[j];                                   // 入力
        Fi[br[j]]=0.0;
    }
    _2_s=1;
    for(s=1;s<=St;s++){                                     // ステージ回繰り返し
        _2_s_1=_2_s;
        _2_s=_2_s*2;
        roop=FFT_SIZE/_2_s;
        for(j=0;j<roop;j++){                                // DFT繰り返し
            for(k=0;k<_2_s_1;k++){                          // BF演算繰り返し
                l=_2_s*j+k;                                 // BFの上入力番号
                m=_2_s_1*(2*j+1)+k;                         // BFの下入力番号
                p=roop*k;                                   // 下入力への重み番号
                Wxmr=Fr[m]*Wr[p]+Fi[m]*Wi[p];               // 重み×下入力の実部
                Wxmi=Fi[m]*Wr[p]-Fr[m]*Wi[p];               // 重み×下入力の虚部
                Xr[m]=Fr[m]=Fr[l]-Wxmr;                     // BFの下出力の実部
                Xi[m]=Fi[m]=Fi[l]-Wxmi;                     // BFの下出力の虚部
                Xr[l]=Fr[l]=Fr[l]+Wxmr;                     // BFの上出力の実部
                Xi[l]=Fi[l]=Fi[l]+Wxmi;                     // BFの上出力の虚部
            }
        }
    }

    for(j=0;j<FFT_SIZE;j++){
        Xamp[j]=sqrt(Fr[j]*Fr[j]+Fi[j]*Fi[j]);              // 振幅スペクトルの算出
    }
}

///////////////////////////////////////////////
//                                           //
//                  IFFT                     //
//                                           //
///////////////////////////////////////////////
void ifft( void ){                                          // 逆フーリエ変換
    int _2_s,_2_s_1,roop,l,m,p;
    int s,j,k;
    double Wxmr,Wxmi;

    for(j=0;j<FFT_SIZE;j++){                                // 逆FFT入力の設定
        Fr[br[j]]=Xr[j];
        Fi[br[j]]=Xi[j];
    }
    _2_s=1;
    for(s=1;s<=St;s++){                                     // ステージ回繰り返し
        _2_s_1=_2_s;
        _2_s=_2_s*2;
        roop=FFT_SIZE/_2_s;
        for(j=0;j<roop;j++){                                // FFT繰り返し
            for(k=0;k<_2_s_1;k++){                          // BF演算繰り返し
                l=_2_s*j+k;                                 // BFの上入力番号
                m=_2_s_1*(2*j+1)+k;                         // BFの下入力番号
                p=roop*k;                                   // 下入力への重み番号
                Wxmr=Fr[m]*Wr[p]-Fi[m]*Wi[p];               // 重み×下入力の実部
                Wxmi=Fi[m]*Wr[p]+Fr[m]*Wi[p];               // 重み×下入力の虚部
                z[m]=Fr[m]=Fr[l]-Wxmr;                      // BFの下出力の実部
                Fi[m]=Fi[l]-Wxmi;                           // BFの下出力の虚部
                z[l]=Fr[l]=Fr[l]+Wxmr;                      // BFの上出力の実部
                Fi[l]=Fi[l]+Wxmi;                           // BFの上出力の虚部
            }
        }
    }
}

///////////////////////////////////////////////
//                                           //
//              メイン関数                   //
//                                           //
///////////////////////////////////////////////
int main(int argc, char **argv){
    ///////////////////////////////
    //                           //
    //  ファイル読み込み用変数   //
    //                           //
    ///////////////////////////////
    FILE *f1;
    unsigned short tmp1;                                    // 2バイト変数
    unsigned long  tmp2;                                    // 4バイト変数
    int Fs;                                                 // 入力サンプリング周波数
    int ch;                                                 // 入力チャネル数
    int len;                                                // 入力信号の長さ
    
    ///////////////////////////////
    //                           //
    //  ファイル書き出し用変数   //
    //                           //
    ///////////////////////////////
    FILE *f2;
    
    char outname[256]={0};                                  // 出力ファイル名
    unsigned long  file_size;                               // ファイルサイズ 4Byte
    unsigned long  width;                                   // 画像の幅
    unsigned long  height;                                  // 画像の高さ
    unsigned long  zero  = 0;                               // '0'書き込み用
    unsigned long  one   = 1;                               // '1'書き込み用
	unsigned long  FileHeaderSize=54;                       // 全ヘッダーサイズ(カラー画像用)
    unsigned long  InfoHeaderSize=40;                       // 情報ヘッダーサイズ
    unsigned long  data_len;                                // 波形データのサンプル数
    unsigned long  color = 24;                              // 1画素あたりの色数
    unsigned long  image_size;                              // 出力画像サイズ
    unsigned char  Rout, Gout, Bout;                        // RGB要素の出力用

//************************************************************************//

    ///////////////////////////////
    //                           //
    //      信号処理用変数       //
    //                           //
    ///////////////////////////////
    int      t      = 0;                                    // 時刻の変数
    long int t_out  = 0;                                    // 終了時刻計測用の変数
    int      add_len= 0;                                    // 出力信号を延長するサンプル数
    short    input, output;                                 // 読込み変数と書出し変数
    double   s[MEM_SIZE+1]={0};                             // 入力データ格納用変数
    double   y[MEM_SIZE+1]={0};                             // 出力データ格納用変数
    double   z1[OL][FFT_SIZE+1]={0};                        // オーバーラップの出力保存用

    long int l,i;                                           // FFT用変数

    int      u=0, nm[OL+1]={0};                             // 各フレームの時間管理
    double   x[OL][FFT_SIZE+1] ={0};                        // FFTの入力
    double   spec;                                          // スペクトログラムの輝度値

//************************************************************************//


    //////////////////////////////////////
    //                                  //
    // 実行ファイルの使い方に関する警告 //
    //                                  //
    //////////////////////////////////////
    if( argc != 2 ){                                        // 使用方法が間違った場合の警告
        fprintf( stderr, "Usage: ./a input.wav \n" );
        exit(-1);
    }
    if( (f1 = fopen(argv[1], "rb")) == NULL ){              // 出力ファイルが開けない場合の警告
        fprintf( stderr, "Cannot open %s\n", argv[1] );
        exit(-2);
    }

    ////////////////////////////////////////////
    //                                        //
    //  入力waveファイルのヘッダ情報読み込み  //
    //                                        //
    ////////////////////////////////////////////
    printf("Wave data is\n");
    fseek(f1, 22L, SEEK_SET);                               // チャネル情報位置に移動
    fread ( &tmp1, sizeof(unsigned short), 1, f1);          // チャネル情報読込 2Byte
    ch=tmp1;                                                // 入力チャネル数の記録
    fread ( &tmp2, sizeof(unsigned long), 1, f1);           // サンプリング周波数の読込 4Byte
    Fs = tmp2;                                              // サンプリング周波数の記録
    fseek(f1, 40L, SEEK_SET);                               // サンプル数情報位置に移動
    fread ( &tmp2, sizeof(unsigned long), 1, f1);           // データのサンプル数取得 4Byte
    len=tmp2/2/ch;                                          // 音声の長さの記録 (2Byteで1サンプル)

    printf("Channel       = %d ch\n",  ch);                 // 入力チャネル数の表示
    printf("Sample rate   = %d Hz \n", Fs);                 // 入力サンプリング周波数の表示
    printf("Sample number = %d\n",     len);                // 入力信号の長さの表示

    //////////////////////////////////////////
    //                                      //
    //      書き出しbmpファイルの設定      //
    //                                      //
    //////////////////////////////////////////

    width          = FFT_SIZE;                              // 画像の幅（FFTサイズ）
    height         = len*OL/FFT_SIZE;                       // 画像の高さ(フレーム長)
    if(width%4  != 0)width = width  - width%4;              // 幅を4の倍数に調整
    image_size     = width * height * 3;                    // 画像サイズ(3色)
    file_size      = image_size + FileHeaderSize;           // 全体ファイルサイズ(byte)

    //////////////////////////////////////
    //                                  //
    //      ヘッダー書き込み            //
    //                                  //
    //////////////////////////////////////
    sscanf(argv[1],"%[^-^.]s", outname);                                // 入力ファイル名取得 (拡張子除外)
    strcat(outname,"_output.bmp");                                      // outname="入力ファイル名_output.wav"
    f2=fopen(outname,"wb");                                             // 出力ファイルオープン．存在しない場合は作成される

    fprintf(f2, "BM");                                                  // 'BM'
    fwrite(&file_size,      sizeof(unsigned long ), 1, f2);             // ファイルサイズ
    fwrite(&zero,           sizeof(unsigned short), 1, f2);             // 0(予約領域)
    fwrite(&zero,           sizeof(unsigned short), 1, f2);             // 0(予約領域)
    fwrite(&FileHeaderSize, sizeof(unsigned long ), 1, f2);             // 画像データまでのオフセット
    fwrite(&InfoHeaderSize, sizeof(unsigned long ), 1, f2);             // 情報ヘッダサイズ
    fwrite(&width,          sizeof(unsigned long ), 1, f2);             // 画像の幅
    fwrite(&height,         sizeof(unsigned long ), 1, f2);             // 画像の長さ
    fwrite(&one,            sizeof(unsigned short), 1, f2);             // プレーン数(常に1)
    fwrite(&color,          sizeof(unsigned short), 1, f2);             // 1画素あたりの色数
    fwrite(&zero,           sizeof(unsigned long ), 1, f2);             // 0(無圧縮)
    fwrite(&image_size,     sizeof(unsigned long ), 1, f2);             // 画像サイズ
    fwrite(&zero,           sizeof(unsigned long ), 1, f2);             // 水平解像度
    fwrite(&zero,           sizeof(unsigned long ), 1, f2);             // 垂直解像度
    fwrite(&zero,           sizeof(unsigned long ), 1, f2);             // パレットの色の数
    fwrite(&zero,           sizeof(unsigned long ), 1, f2);             // 重要パレットのインデックス

    printf("\nOutput BMP data is\n");
    printf("Width         = %d \n", width );
    printf("Height        = %d \n", height);
    printf("Image Size    = %d\n",  image_size/3);

//************************************************************************//

    ///////////////////////////////
    //                           //
    //      変数の初期設定       //
    //                           //
    ///////////////////////////////

    init();                                                 // ビット反転，乗み係数の計算
    l = 0;                                                  // FFT開始時刻管理

    for (i=0;i<OL;i++){
        nm[i]=FFT_SIZE/OL*i;                                // フレーム開始時間の調整
    }

//************************************************************************//

    ///////////////////////////////////
    //                               //
    //        メインループ           //
    //                               //
    ///////////////////////////////////
    fseek(f1, 44L, SEEK_SET);                               // 音声データ開始位置に移動
    while(1){                                               // メインループ
        if(fread( &input, sizeof(short), 1,f1) < 1){        // 音声を input に読込み
            if( t_out > len+add_len ) break;                // ループ終了判定
            else input=0;                                   // ループ継続かつ音声読込み終了なら input=0
        }
        s[t] = input/32768.0;                               // 音声の最大値を1とする(正規化)


//************************************************************************//

        ////////////////////////////////////////////////////
        //                                                //
        //              Signal Processing                 //
        //                                                //
        //  現在時刻tの入力 s[t] から出力 y[t] をつくる   //
        //                                                //
        //  ※ tは0からMEM_SIZE-1までをループ             //
        //                                                //
        ////////////////////////////////////////////////////

        y[t]=0;
        for (i=0;i<OL;i++){
            x[i][nm[i]]=s[t];                               // 次の入力をそれぞれのフレームに格納
            y[t]=y[t]+z1[i][nm[i]]/FFT_SIZE/(OL/2.0);       // 出力計算
        }
        
        for(i=0;i<OL;i++){
            nm[i]=(nm[i]+1)%FFT_SIZE;                       // 各フレームのカウントアップ
        }
        if(l>=FFT_SIZE/OL){
            for(i=0;i<FFT_SIZE;i++){
                xin[i]=x[u][i]*0.5*(1.0-cos(2.0*M_PI*i/(double)FFT_SIZE));//FFT入力
            }
            fft();                                          // FFT
            ///////////////////////////
            //                       //
            //        FFT処理        //
            //                       //
            ///////////////////////////
            for(i=0;i<FFT_SIZE;i++){
                spec = Xamp[i]*Xamp[i]*256;                 // パワースペクトル
//                spec = 10*log10(spec);                    // 対数パワースペクトル
                if(spec>255)spec=255;                       // 輝度の最大値を255とする
                if(spec<0)  spec=0;                         // 輝度の最小値を0とする
                Bout=Gout=Rout=spec;
                fwrite(&Bout, sizeof(unsigned char), 1, f2);//乗算結果の書き込み
                fwrite(&Gout, sizeof(unsigned char), 1, f2);//乗算結果の書き込み
                fwrite(&Rout, sizeof(unsigned char), 1, f2);//乗算結果の書き込み
            }
            l=0;
            u=(u+1)%OL;                                     // 出力保存用配列 z1[u][i] を変更
        }
        l++;                                                // FFT用の時刻管理

//************************************************************************//

        t=(t+1)%MEM_SIZE;                                   // 時刻 t の更新
        t_out++;                                            // ループ終了時刻の計測
    }
    fclose(f1);                                             // 入力ファイルを閉じる
    fclose(f2);                                             // 出力ファイルを閉じる
    return 0;                                               // メイン関数の終了
}
