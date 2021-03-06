////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ リアルタイム信号処理プログラム        //
//  ZPS置き換え処理(ノイズ強制付加)                   //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2015.12.05.//
////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>

#define MEM_SIZE 16000                                               // 音声メモリのサイズ
#define Fs       16000                                               // リアルタイム処理のサンプリング周波数
#define NUM_BUF  8                                                   // 音声保持用バッファの数
#define BUF_SIZE 160                                                 // 各バッファのサイズ
#define FFT_SIZE 512                                                 // FFT点数
#define pw       1                                                   // ZPS変換の振幅のべき

///////////////////////////////////////////////
//                                           //
//                                           //
//            FFT関数の構築                  //
//                                           //
//                                           //
///////////////////////////////////////////////
static double Wr[FFT_SIZE+1],Wi[FFT_SIZE+1];                         // FFT 重み
static double Fr[FFT_SIZE+1],Fi[FFT_SIZE+1],Xr[FFT_SIZE+1],Xi[FFT_SIZE+1]; //実部と虚部
static double xin[FFT_SIZE+1],z[FFT_SIZE+1];                         // FFT入力と出力
static double Xamp[FFT_SIZE+1];                                      // 振幅スペクトル
static double Xphs[FFT_SIZE+1];                                      // 位相スペクトル
static double omega;                                                 // FFT角周波数
static int    St,br[FFT_SIZE+1];                                     // FFTステージ番号，ビット反転
static double s0[FFT_SIZE+1]={0};                           // ゼロ位相信号

void bitr( void );                                                   // ビット反転用関数の宣言
void fft ( void );                                                   // FFT関数の宣言
void Wnk ( void );                                                   // FFTバタフライ(BF)演算の重み関数の宣言
void ifft( void );                                                   // IFFT関数の宣言
void init( void );                                                   // FFT関連変数の初期設定

///////////////////////////////////////////////
//                                           //
//                初期設定                   //
//                                           //
///////////////////////////////////////////////
void init(void){
    int i;
    St=log((double)FFT_SIZE)/log(2.0)+0.5;                           // FFTアルゴリズムの段数の算出 +0.5はintへの対応
    omega=2.0*M_PI/FFT_SIZE;
    bitr();Wnk();
}

void bitr( void ){                                                   // ビット反転
    int loop,i,j;
    br[0]=0;                                                         // 0番目は0
    br[1]=FFT_SIZE/2;                                                // 1番目はN/2
    loop=1;
    for(j=0;j<St-1;j++){
        br[loop*2]=br[loop]/2;
        loop=loop*2;
        for(i=1;i<loop;i++){
            br[loop+i]=br[loop]+br[i];
        }
    }
}

void Wnk(void){                                                     // 重みの計算
    int i;
    for(i=0;i<FFT_SIZE/2;i++){
        Wr[i]=cos(omega*i);                                         // 重み実部
        Wi[i]=sin(omega*i);                                         // 重み虚部
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
    for(j=0;j<FFT_SIZE;j++){                                        // FFT入力の設定
        Fr[br[j]]=xin[j];                                           // 入力
        Fi[br[j]]=0.0;
    }
    _2_s=1;
    for(s=1;s<=St;s++){                                             // ステージ回繰り返し
        _2_s_1=_2_s;
        _2_s=_2_s*2;
        roop=FFT_SIZE/_2_s;
        for(j=0;j<roop;j++){                                        // DFT繰り返し
            for(k=0;k<_2_s_1;k++){                                  // BF演算繰り返し
                l=_2_s*j+k;                                         // BFの上入力番号
                m=_2_s_1*(2*j+1)+k;                                 // BFの下入力番号
                p=roop*k;                                           // 下入力への重み番号
                Wxmr=Fr[m]*Wr[p]+Fi[m]*Wi[p];                       // 重み×下入力の実部
                Wxmi=Fi[m]*Wr[p]-Fr[m]*Wi[p];                       // 重み×下入力の虚部
                Xr[m]=Fr[m]=Fr[l]-Wxmr;                             // BFの下出力の実部
                Xi[m]=Fi[m]=Fi[l]-Wxmi;                             // BFの下出力の虚部
                Xr[l]=Fr[l]=Fr[l]+Wxmr;                             // BFの上出力の実部
                Xi[l]=Fi[l]=Fi[l]+Wxmi;                             // BFの上出力の虚部
            }
        }
    }

    for(j=0;j<FFT_SIZE;j++){
        Xamp[j]=sqrt(Fr[j]*Fr[j]+Fi[j]*Fi[j]);                      // 振幅スペクトルの算出
    }
}


///////////////////////////////////////////////
//                                           //
//                  IFFT                     //
//                                           //
///////////////////////////////////////////////
void ifft( void ){                                                  // 逆フーリエ変換
    int _2_s,_2_s_1,roop,l,m,p;
    int s,j,k;
    double Wxmr,Wxmi;

    for(j=0;j<FFT_SIZE;j++){                                        // 逆FFT入力の設定
        Fr[br[j]]=Xr[j];
        Fi[br[j]]=Xi[j];
    }
    _2_s=1;
    for(s=1;s<=St;s++){                                             // ステージ回繰り返し
        _2_s_1=_2_s;
        _2_s=_2_s*2;
        roop=FFT_SIZE/_2_s;
        for(j=0;j<roop;j++){                                        // FFT繰り返し
            for(k=0;k<_2_s_1;k++){                                  // BF演算繰り返し
                l=_2_s*j+k;                                         // BFの上入力番号
                m=_2_s_1*(2*j+1)+k;                                 // BFの下入力番号
                p=roop*k;                                           // 下入力への重み番号
                Wxmr=Fr[m]*Wr[p]-Fi[m]*Wi[p];                       // 重み×下入力の実部
                Wxmi=Fi[m]*Wr[p]+Fr[m]*Wi[p];                       // 重み×下入力の虚部
                z[m]=Fr[m]=Fr[l]-Wxmr;                              // BFの下出力の実部
                Fi[m]=Fi[l]-Wxmi;                                   // BFの下出力の虚部
                z[l]=Fr[l]=Fr[l]+Wxmr;                              // BFの上出力の実部
                Fi[l]=Fi[l]+Wxmi;                                   // BFの上出力の虚部
            }
        }
    }
}

///////////////////////////////////////////////
//                                           //
//                  ZPT                      //
//                                           //
///////////////////////////////////////////////
void zpt(void){
    int k;
    fft();
    for(k=0;k<FFT_SIZE;k++){
        if(Xr[k]!=0){
            Xphs[k]=atan2(Xi[k],Xr[k]);                     // 位相記録．-πからπのラジアン値．
        }
        Xr[k]=pow(Xamp[k], pw);                             // 振幅のpw乗をゼロ位相信号とする
        Xi[k]=0;                                            // 虚部は0
    }
    ifft();                                                 // IFFTでZPSを作成
    for(k=0;k<FFT_SIZE;k++){
        s0[k]=z[k]/FFT_SIZE;                                // x0として記憶
    }
}
///////////////////////////////////////////////
//                                           //
//                  IZPT                     //
//                                           //
///////////////////////////////////////////////
void izpt(void){
    int k;
    for(k=0;k<FFT_SIZE;k++){
        xin[k]=s0[k];                                       // FFT入力をZPSとする
    }
    fft();                                                  // ZPSから振幅スペクトルを得る
   	for(k=0;k<FFT_SIZE;k++){
        if(Fr[k]<=0)Fr[k]=0;
        Xamp[k]=Fr[k]; 			//振幅の算出
    }
    for(k=0;k<FFT_SIZE;k++){
        Xr[k]=Xamp[k]*cos(Xphs[k]);                         // 位相と融合して実部をつくる
        Xi[k]=Xamp[k]*sin(Xphs[k]);                         // 位相と融合して虚部をつくる
    }
    ifft();                                                 // IFFTにより時間領域信号を得る(z[n])
}


///////////////////////////////////////////////
//                                           //
//              メイン関数                   //
//                                           //
///////////////////////////////////////////////
int main(void){
    /////////////////////////////////
    //                             //
    //  リアルタイム入出力用変数   //
    //                             //
    /////////////////////////////////
    char   key;                                                     // スルーと処理音の切替用変数
    int in1;                                                        // 入力バッファ番号
    int out1;                                                       // 出力バッファ番号
    int flag;                                                       // 入出力状態のフラグ
    int n;                                                          // バッファ用の時刻
    short in_buf[NUM_BUF][BUF_SIZE]={0};                            // 入力バッファ
    WAVEHDR in_hdr[NUM_BUF];                                        // 入力バッファのヘッダ
	HWAVEIN in_hdl = 0;                                             // 入力(録音)デバイスのハンドル
    short out_buf[NUM_BUF][BUF_SIZE]={0};                           // 出力バッファ
    WAVEHDR out_hdr[NUM_BUF];                                       // 出力バッファのヘッダ
    HWAVEOUT out_hdl = 0;                                           // 出力(再生)デバイスのハンドル

    WAVEFORMATEX wave_format_ex = {WAVE_FORMAT_PCM,                 // PCM
                                 1,                                 // モノラル
                                 Fs,                                // サンプリング周波数
                                 2*Fs,                              // 1秒あたりの音データサイズ（byte）
                                 2,                                 // 音データの最小単位（2byte）
                                 16,                                // 量子化ビット（16bit）
                                 0                                  // オプション情報のサイズ（0byte)
                                };


//************************************************************************//

    ///////////////////////////////
    //                           //
    //      信号処理用変数       //
    //                           //
    ///////////////////////////////
    int t   = 0;                                                    // 入力の時刻
    int to  = 0;                                                    // 出力の時刻
    int hflg= 0;                                                    // 素通しと処理音の状態フラグ
    int i;                                                          // forループ用変数
    double s[MEM_SIZE+1]={0};                                       // 入力データ格納用変数
    double y[MEM_SIZE+1]={0};                                       // 出力データ格納用変数
    
    long int l;                                                     // FFT用変数
    double   x[FFT_SIZE+1] ={0};                                    // FFTの入力
    double   x1[FFT_SIZE+1]={0};                                    // 半フレームシフト時の入力保持用
    double   z1[FFT_SIZE+1]={0};                                    // ハーフオーバーラップの出力保持用

    int      L, pos;                                                // 置き換え点数, 第2ピークの位置
    double   max,sc[FFT_SIZE+1]={0};                                // ピーク探索，スケーリング関数

    ///////////////////////////////
    //                           //
    //       変数の初期設定      //
    //                           //
    ///////////////////////////////

    init();                                                         // ビット反転，乗み係数の計算
    l = 0;                                                          // FFT開始時刻管理

    L = 20;                                                        // 置き換えるサンプル数

//************************************************************************//


    ///////////////////////////////////////
    //                                   //
    //    入出力デバイスのオープン       // 
    //                                   //
    ///////////////////////////////////////
    waveInOpen(&in_hdl, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);   // 入力(録音)デバイスオープン
    for (i = 0; i < NUM_BUF; i++){
        memset(&in_hdr[i], 0, sizeof(WAVEHDR));                     // 全入力バッファを初期化
    }
    waveOutOpen(&out_hdl, 0, &wave_format_ex, 0, 0, CALLBACK_NULL); // 出力(再生)デバイスオープン
    waveOutPause(out_hdl);                                          // サウンドデバイスの一時停止
    for (i = 0; i < NUM_BUF; i++){                                  // 全出力バッファを初期化
        memset(&out_hdr[i], 0, sizeof(WAVEHDR));
    }
    ///////////////////////////////////////
    //                                   //
    //    入出力バッファの初期設定       // 
    //                                   //
    ///////////////////////////////////////
    for (i=0;i<NUM_BUF;i++){                                        // バッファの数だけ繰り返し
        out_hdr[i].lpData = (char *)out_buf[i];                     // 出力バッファデータを更新
        out_hdr[i].dwBufferLength = BUF_SIZE * 2;                   // 出力バッファサイズを設定
        out_hdr[i].dwFlags = 0;                                     // 出力フラグのクリア
        waveOutPrepareHeader(out_hdl, &out_hdr[i], sizeof(WAVEHDR));// 出力バッファをロック
        waveOutWrite(out_hdl, &out_hdr[i], sizeof(WAVEHDR));        // 出力バッファを出力待ちキューに送信
        in_hdr[i].lpData = (char *)in_buf[i];                       // 入力バッファデータの更新
        in_hdr[i].dwBufferLength = BUF_SIZE * 2;                    // 入力バッファサイズを設定
        in_hdr[i].dwFlags = 0;                                      // 入力フラグのクリア
        waveInPrepareHeader(in_hdl, &in_hdr[i], sizeof(WAVEHDR));   // 入力バッファをロック
        waveInAddBuffer(in_hdl, &in_hdr[i], sizeof(WAVEHDR));       // 入力バッファを入力待ちキューに送信
    }
    waveOutRestart(out_hdl);                                        // 音声出力開始
    waveInStart(in_hdl);                                            // 音声入力開始

    in1  = 0;                                                       // 入力バッファ番号の初期化
    out1 = 0;                                                       // 出力バッファ番号の初期化
    flag = 0;                                                       // 入出力判定フラグを0(入力)とする

    printf("[space]--> Through <=> Processing\n");                  // [space]で処理切り替え
    printf("[Enter]--> End\n");                                     // [Enter]で終了
    printf("Through\n");                                            // 素通しであることを表示


    ///////////////////////////////////
    //                               //
    //        メインループ           //
    //                               //
    ///////////////////////////////////
    while (1){
        //////////////////////////
        //                      //
        //    出力書出し処理    //
        //                      //
        //////////////////////////
        if (flag == 1){                                             // flag=1なら出力状態
            if ((out_hdr[out1].dwFlags & WHDR_DONE) != 0){          // 出力バッファのフラグ情報が0でないなら処理を実行
                for (n = 0; n < BUF_SIZE; n++){                     // 出力をBUF_SIZEサンプル分だけ書き出す
                    to=(to+1)%MEM_SIZE;                             // 出力時刻の更新
                    out_buf[out1][n] = y[to]*32768;                 // 出力データを記録
                }
                waveOutUnprepareHeader(out_hdl, &out_hdr[out1], sizeof(WAVEHDR)); // 出力バッファをクリーンアップ
                out_hdr[out1].lpData = (char *)out_buf[out1];       // 出力データを更新
//                out_hdr[out1].dwBufferLength = BUF_SIZE * 2;        // 出力バッファサイズを設定
                out_hdr[out1].dwFlags = 0;                          // 出力バッファのフラグ情報を0にする
                waveOutPrepareHeader(out_hdl, &out_hdr[out1], sizeof(WAVEHDR));   // 情報を更新し出力バッファを準備
                waveOutWrite(out_hdl, &out_hdr[out1], sizeof(WAVEHDR));// 出力待ちキューに出力バッファデータを送信
                out1=(out1+1)%NUM_BUF;                              // 出力バッファ番号を更新
                flag = 0;                                           // 入出力フラグを flag=0 (入力状態) にする
            }
        }
        //////////////////////////
        //                      //
        //    入力読込み処理    //
        //                      //
        //////////////////////////
        if ((in_hdr[in1].dwFlags & WHDR_DONE) != 0){                // 入力バッファのフラグ情報が0でないなら処理を実行
            waveInUnprepareHeader(in_hdl, &in_hdr[in1], sizeof(WAVEHDR)); // 録音済みの入力バッファを選択
            for (n = 0; n < BUF_SIZE; n++){                         // BUF_SIZEサンプル分の入力を読込み，処理する
                t=(t+1)%MEM_SIZE;                                   // 入力時刻の更新
                s[t] = in_buf[in1][n]/32768.0;                      // 入力バッファからデータ読み込み (正規化)
                if(t%8000==0)s[t]=s[t]+0.9;                         // 入力にノイズを追加
                y[t] = s[t];                                        // 出力＝入力の素通しをつくる
                if(hflg!=0){                                        // 処理状態フラグが0以外なら信号処理を実行


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

                    x[l] = x1[l];                                   // FFT_SIZE/2までの入力をx[l]に格納
                    x1[l]= x[FFT_SIZE/2+l]=s[t];                    // FFT_SIZE/2以降の入力をx[l]に格納
                    y[t] = (z[l]+z1[l])/FFT_SIZE;                   // IFFTで得た出力
                    z1[l]= z[FFT_SIZE/2+l];                         // ハーフオーバーラップ用に出力記録

                    if( l>=FFT_SIZE/2){                             // 半フレームごとにFFTを実行
                        for(i=0;i<FFT_SIZE;i++){
                            xin[i]=x[i]*0.5*(1.0-cos(2.0*M_PI*i/(double)FFT_SIZE));// 窓関数をかける
                        }
                        zpt();                                      // ゼロ位相変換

                        ////////////////////////////////////////////////////
                        //                                                //
                        //               ゼロ位相信号処理                 //
                        //                                                //
                        ////////////////////////////////////////////////////
                        //////////////////
                        //  ピーク探索  //
                        //////////////////
                        max=0;
                        pos=0;
                        for(i=Fs/300;i<Fs/100;i++){                     // 60Hz〜300Hzでピーク探索
                            if(s0[i]>max){
                                max=s0[i];                              // ピーク値更新
                                pos=i;                                  // ピーク位置更新
                            }
                        }
                        ////////////////////////
                        //  スケーリング関数  //
                        ////////////////////////
                        for (i=0;i<L;i++){
                            sc[i]= 1.0+cos(2.0*M_PI*(i+pos)/FFT_SIZE);  // 窓関数の逆関数を作る
                            if(sc[i]!=0){
                                sc[i]=(1.0+cos(2.0*M_PI*i/FFT_SIZE))/sc[i];
                            }
                        }
                        ////////////////////////
                        //  Lサンプル置換え   //
                        ////////////////////////
                        s0[0] = sc[0] * s0[pos];
                        for (i=1;i<L;i++){            
                            s0[i] = sc[i] * s0[pos+i];                  // 振幅を調整して置き換え
                            s0[FFT_SIZE-i] = s0[i];                     // 対称性の確保
                        }
                        izpt();                                     // 逆ゼロ位相変換
                        l=0;
                    }
                    l++;                                            // FFT用の時刻管理

//************************************************************************//

//                    y[t]=atan(y[t])/(M_PI/2.0);                     // クリップ防止
                }
            }
            in_hdr[in1].dwFlags = 0;                                // 入力バッファのフラグ情報を0にする
            waveInPrepareHeader(in_hdl, &in_hdr[in1], sizeof(WAVEHDR));   // 情報を更新し入力バッファを準備
            waveInAddBuffer(in_hdl, &in_hdr[in1], sizeof(WAVEHDR)); // 入力待ちキューに入力バッファデータを送信
            in1=(in1+1)%NUM_BUF;                                    // 入力バッファの更新
            flag = 1;                                               // 入出力フラグを flag=1 (出力状態)にする
        }

        ////////////////////////////////////////////////////
        //                                                //
        //        キーボード入力に対する処理の設定        //
        //                                                //
        //                                                //
        //  [デフォルト設定]                              //
        //  ・スペースキーで素通しと信号処理を切り替え    //
        //  ・Enterキーでプログラム終了                   //
        //                                                //
        ////////////////////////////////////////////////////
        if (kbhit()){                                               // キーボード入力があったか
            key = getch();                                          // キーのチェック
            if (key == 32){                                         // スペースキーが押されとき
            	hflg=(hflg+1)%2;                                    // 処理フラグ変更
                if(hflg==1) printf("Processing\n");                 // 処理中であることを表示
                else        printf("Through\n");                    // 素通しであることを表示
            }
            if (key == 13){                                         // Enterキーが押されたとき
                waveInStop(in_hdl);                                 // 入力を停止
                for (i = 0; i < NUM_BUF; i++){                      // 全入力バッファのアンロック
                    if ((in_hdr[i].dwFlags & WHDR_PREPARED) != 0){
                        waveInUnprepareHeader(in_hdl, &in_hdr[i], sizeof(WAVEHDR ));
                    }
                }
                waveInClose(in_hdl);                                // 入力(録音)デバイスのクローズ
                waveOutPause(out_hdl);                              // 出力を停止
                for (i = 0; i < NUM_BUF; i++){
                    if ((out_hdr[i].dwFlags & WHDR_PREPARED) != 0){ // 全出力バッファのアンロック
                        waveOutUnprepareHeader(out_hdl, &out_hdr[i], sizeof(WAVEHDR));
                    }
                }
                waveOutClose(out_hdl);                              // 出力(再生)デバイスのクローズ
                return 0;
            }
        }
    }                                                               //メインループ終わり
}
