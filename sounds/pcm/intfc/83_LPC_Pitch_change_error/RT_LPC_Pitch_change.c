////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ リアルタイム信号処理プログラム        //
//  線形予測分析合成(ノイズ音源)                      //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                        2016.02.06. //
////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>

#define MEM_SIZE 16000                                              // 音声メモリのサイズ
#define Fs       16000                                              // リアルタイム処理のサンプリング周波数
#define NUM_BUF  8                                                  // 音声保持用バッファの数
#define BUF_SIZE 160                                                // 各バッファのサイズ
#define N  512                                                      // 自己相関計算用サンプル数
#define P  28                                                       // AR次数

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

    double   rho[P+1]={0}, delta[P+1]={0}, sigma[P+1]={0};          // AR係数推定
    double   h[P+1]={0}, r[N+1]={0}, a[P+1][P+1]={0};               // 自己相関関数
    int      l, m, tau;                                             // 繰り返し計算管理用
    double   rs[MEM_SIZE+1]={0};                                    // 合成信号
    double   rmax;                                                  // ピッチ抽出用
    int      pitch;                                                 // ピッチ
    double   e[N+1]={0};                                            // 予測誤差
    double   k, alpha;                                              // ピッチ変更用の変数

    ///////////////////////////////
    //                           //
    //      変数の初期設定       //
    //                           //
    ///////////////////////////////

    h[0]  = 1.0;                                                    // 線形予測係数h[0]=1
    alpha = 2.0;                                                    // 高さを何倍にするか
    k     = 0;                                                      // 出力時刻管理

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

                    if(l>=N){                                       // Nサンプルごとに線形予測分析を実行
                    /////////////////////////////
                    //                         //
                    //      線形予測分析       //
                    //                         //
                    /////////////////////////////

                        pitch=1, rmax=0;                            // ピッチの初期化
                        for(tau=0;tau<N;tau++){                     // 時間差tauの設定
                            r[tau]=0;                               // 自己相関の初期化
                            i=0;
                            while(tau+i<=N){                        // 自己相関の計算
                                r[tau]=r[tau]+s[(t-i+MEM_SIZE)%MEM_SIZE]*s[(t-tau-i+MEM_SIZE)%MEM_SIZE];
                                i++;
                            }
                            r[tau]=r[tau]/N;                        // 自己相関の初期化

                            //ピッチ抽出
                            if(tau>=20 && rmax<r[tau]){             // 音声ピッチの範囲内で最大相関検出
                                rmax=r[tau];
                                pitch=tau;                          // ピッチ推定値
                            }
                        }

                        // レビンソン・ダービンアルゴリズム
                        sigma[0]=r[0];                              // sigma[0] = 時間差0の自己相関関数
                        for(m=0;m<P;m++){
                            delta[m+1]=r[m+1];                      // deltaの初期値 = 自己相関関数
                            for(i=1;i<=m;i++){
                                delta[m+1]=delta[m+1]+a[m][i]*r[m+1-i]; // AR係数aを使ってdeltaを更新
                            }
                            if(fabs(sigma[m])==0.0 || fabs(delta[m+1])>2*fabs(sigma[m])){
                                rho[m+1]   =0.0;                    // deltaが0かsigmaの2倍より大きい場合はrho=0
                                a[m+1][m+1]=rho[m+1];               // AR係数をrhoにする
                            }
                            else{                             
                                rho[m+1]=-delta[m+1]/sigma[m];      // 反射係数rhoの更新
                                a[m+1][m+1]=rho[m+1];               // AR係数a[m+1]の更新
                            }
                            sigma[m+1]=sigma[m]*(1.0-(rho[m+1]*rho[m+1]));// sigma[m+1]の更新
                            for(i=1;i<=m;i++){
                                a[m+1][i]=a[m][i]+rho[m+1]*a[m][m+1-i];// AR係数a[1]からa[m]の更新
                            }
                        }

                        h[0]=1.0;                                   // 線形予測係数h[0]
                        for(i=1;i<=P;i++){
                            h[i]=a[P][i];                           // 線形予測係数h[1]からh[P]の計算
                        }
                        l=0;
                    }

                    e[l]=0;
                    for(i=0;i<=P;i++){
                        e[l] = e[l] + h[i]*s[(t-i+MEM_SIZE)%MEM_SIZE];// 予測誤差（音源）
                    }
                
                    l++;

                    /////////////////////////////
                    //                         //
                    //        音声合成         //
                    //                         //
                    /////////////////////////////

                    rs[t]=e[(int)k];                                // 音源の設定
                    for(i=1;i<=P;i++){
                        rs[t] = rs[t] - h[i]*rs[(t-i+MEM_SIZE)%MEM_SIZE];// 音声合成
                    }

                    y[t]=rs[t];                                     // 合成音声を出力
                
                    k = k+alpha;                                    // 出力の時刻インデックス
                    if(k>=N)k=k-N;                                  // kを0からNの範囲内にする

            ////////////////////////////////////////////////////
            //                                                //
            //              Signal Processing                 //
            //                                                //
            //                  ここまで                      //
            //                                                //
            ////////////////////////////////////////////////////

//************************************************************************//

                    y[t]=atan(y[t])/(M_PI/2.0);                     // クリップ防止
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
