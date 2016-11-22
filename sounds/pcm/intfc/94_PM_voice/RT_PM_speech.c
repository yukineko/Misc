////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ リアルタイム信号処理プログラム        //
//  PM(位相変調) Voice                                //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.02.08.//
////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>

#define MEM_SIZE 48000                                              // 音声メモリのサイズ
#define Fs       96000                                              // リアルタイム処理のサンプリング周波数
#define NUM_BUF  8                                                  // 音声保持用バッファの数
#define BUF_SIZE 1600                                               // 各バッファのサイズ

int main(void){
    /////////////////////////////////
    //                             //
    //  リアルタイム入出力用変数   //
    //                             //
    /////////////////////////////////
    char key;                                                       // スルーと処理音の切替用変数
    int  in1;                                                       // 入力バッファ番号
    int  out1;                                                      // 出力バッファ番号
    int  flag;                                                      // 入出力状態のフラグ
    int  n;                                                         // バッファ用の時刻
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

    int    N=64;                                                    // フィルタ次数
    double h[64+1]={0};                                             // フィルタ係数
    double Fe;                                                      // 遮断周波数
    double Fc;
    double x_PM;                                                    // PM変調信号
    double xI[64+1]={0};                                            // 実部信号
    double xQ[64+1]={0};                                            // 虚部信号
    double yI, yQ;                                                  // LPF出力
    int    k;
    ///////////////////////////////
    //                           //
    //      変数の初期設定       //
    //                           //
    ///////////////////////////////

    Fc = 20000.0;                                                   // 変調周波数[Hz]
    Fe = 8000.0;                                                    // 遮断周波数[Hz]
    k    = 0;                                                       // 管理用時刻の初期値

    ///////////////////////////////
    //                           //
    //       LPF係数の計算       //
    //                           //
    ///////////////////////////////
    Fe = Fe/Fs;                                                    // 遮断周波数をサンプリング周波数で正規化
    for(i=-N/2;i<=N/2;i++){                                        // 係数の設定
        if(i==0) h[N/2+i]=2.0*Fe;
        else{
            h[N/2+i]=2.0*Fe*sin(2.0*M_PI*Fe*i)/(2.0*M_PI*Fe*i);
        }
        h[N/2+i]=h[N/2+i]*0.5*(1.0-cos(2.0*M_PI*(N/2+i)/N));
    }

//************************************************************************//


    printf("[space]--> Through <=> Processing\n");                  // [space]で処理切り替え
    printf("[Enter]--> End\n");                                     // [Enter]で終了
    printf("Through\n");                                            // 素通しであることを表示

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
                y[t] = s[t];                                        // 出力＝入力の素通しをつくる
                if(hflg!=0){                                        // 処理状態フラグが1なら信号処理を実行


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

                    x_PM = 0.5*cos(2*M_PI*Fc/Fs*t + s[t]);              // PM変調信号
        
                    xI[k]= x_PM *   cos(2*M_PI*Fc/Fs*t) ;               // 実部の信号
                    xQ[k]= x_PM * (-sin(2*M_PI*Fc/Fs*t));               // 虚部の信号

                    yI = 0;
                    yQ = 0;
                    for(i=0;i<=N;i++){
                        yI = yI + xI[(k-i+N)%N]*h[i];                   // 実部のLPF出力
                        yQ = yQ + xQ[(k-i+N)%N]*h[i];                   // 虚部のLPF出力
                    }
                    if( yQ+yI!=0 && yI*yQ!=0){
                        y[t] = atan2(yQ, yI);                           // 現在の位相
                    }

                    if(hflg==1)y[t]=x_PM;                               // FM変調信号
                    if(hflg==2)y[t]=xI[k];                              // 復調第1段階 = 周波数シフト
                    if(hflg==3)y[t]=yI;                                 // 復調第2段階 = LPF
                    k=(k+1)%N;
//************************************************************************//

//                    y[t]=atan(y[t])/(M_PI/2.0);                         // クリップ防止

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
                hflg = (hflg+1)%5;                                  // 処理フラグ変更
                if(hflg==0)printf("Through\n");                     // 素通しであることを表示
                if(hflg==1)printf("FM_signal\n");                   // 出力状態の表示
                if(hflg==2)printf("DeModulation 1/3 step\n");       // 出力状態の表示
                if(hflg==3)printf("DeModulation 2/3 step\n");       // 出力状態の表示
                if(hflg==4)printf("DeModulation 3/3 step\n");       // 出力状態の表示
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
