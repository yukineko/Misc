////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ リアルタイム信号処理プログラム        //
//  ステレオ録音してWAVファイルを作成                 //
//  バッファサイズ増でハイレゾ録音(Fs>44100)も可能    //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.01.27.//
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
#define Length   10                                                 // 作成するデータの長さ[sec]
#define ch       2                                                  // チャネル数

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
                                 2,                                 // ステレオ
                                 Fs,                                // サンプリング周波数
                                 4*Fs,                              // 1秒あたりの音データサイズ（byte）
                                 4,                                 // 音データの最小単位（2byte）
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
    int hflg= 1;                                                    // 素通しと処理音の状態フラグ
    int i;                                                          // forループ用変数
    double s[MEM_SIZE+1]={0};                                       // 入力データ格納用変数
    double y[MEM_SIZE+1]={0};                                       // 出力データ格納用変数
    
    long int l = 0;                                                 // 出力時間のカウンタ
    short    output;                                                // 出力書き込み用変数
    ///////////////////////////////
    //                           //
    //  ファイル書き出し用変数   //
    //                           //
    ///////////////////////////////
    FILE *f2;
    unsigned short tmp1;                                            // 2バイト変数
    unsigned long  tmp2;                                            // 4バイト変数
    
    unsigned short channel;                                         // 出力チャンネル数
    unsigned short BytePerSample;                                   // 1サンプル当たりのバイト数
    unsigned long  file_size;                                       // 出力ファイルサイズ
    unsigned long  Fs_out;                                          // 出力サンプリング周波数
    unsigned long  BytePerSec;                                      // 1秒当たりのバイト数
    unsigned long  data_len;                                        // 出力波形のサンプル数
    unsigned long  fmt_chnk    =16;                                 // fmtチャンクのバイト数.PCMの場合は16bit
    unsigned short BitPerSample=16;                                 // 1サンプルのビット数
    unsigned short fmt_ID      =1;                                  // fmt ID. PCMの場合は1

    //////////////////////////////////////
    //                                  //
    //     出力waveファイルの準備       //
    //                                  //
    //////////////////////////////////////
    f2=fopen("rec.wav","wb");                                       // 書き出し用ファイル
    Fs_out        = Fs;                                             // 出力サンプリング周波数を設定
    channel       = ch;                                             // 出力チャネル数を設定
    data_len      = channel*Fs*Length*2;                            // 出力データの長さ = 全Byte数 (1サンプルで2Byte)
    file_size     = 36+data_len;                                    // 全体ファイルサイズ
    BytePerSec    = Fs_out*channel*2;                               // 1秒当たりのバイト数
    BytePerSample = channel*2;                                      // 1サンプル当たりのバイト数
    
    //////////////////////////////////////
    //                                  //
    //     出力ヘッダ情報書き込み       //
    //                                  //
    //////////////////////////////////////
    fprintf(f2, "RIFF");                                            // "RIFF"
    fwrite(&file_size,    sizeof(unsigned long ), 1, f2);           // ファイルサイズ
    fprintf(f2, "WAVEfmt ");                                        // "WAVEfmt"
    fwrite(&fmt_chnk,     sizeof(unsigned long ), 1, f2);           // fmt_chnk=16 (ビット数)
    fwrite(&fmt_ID,       sizeof(unsigned short), 1, f2);           // fmt ID=1 (PCM)
    fwrite(&channel,      sizeof(unsigned short), 1, f2);           // 出力チャネル数
    fwrite(&Fs_out,       sizeof(unsigned long ), 1, f2);           // 出力のサンプリング周波数
    fwrite(&BytePerSec,   sizeof(unsigned long ), 1, f2);           // 1秒当たりのバイト数
    fwrite(&BytePerSample,sizeof(unsigned short ),1, f2);           // 1サンプル当たりのバイト数
    fwrite(&BitPerSample, sizeof(unsigned short ),1, f2);           // 1サンプルのビット数(16ビット)
    fprintf(f2, "data");                                            // "data"
    fwrite(&data_len,     sizeof(unsigned long ), 1, f2);           // 出力データの長さ

    printf("\nOutput WAVE data is\n");
    printf("Channel       = %d ch\n",  channel);                    // 出力チャネル数の表示
    printf("Sample rate   = %d Hz \n", Fs_out);                     // 出力サンプリング周波数の表示
    printf("Sample number = %d\n",     data_len/ch/2);              // 出力信号の長さの表示
    
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

    printf("[Enter] --> Rec %d[sec]\n", Length);
    getchar();
    printf("Start!\n");


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
                out_hdr[out1].dwBufferLength = BUF_SIZE * 2;        // 出力バッファサイズを設定
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

                    y[t]=s[t];                                        // 出力=入力
                    output = y[t]*32767;                              // 出力を整数化
                    fwrite(&output, sizeof(short), 1, f2);            // 結果の書き出し
                    if(l%Fs==0)printf("%d ",Length-l/Fs);             // 残り時間の表示
                    l++;                                              // 出力カウンタ更新
                    if(l>Length*Fs)goto Fin;                          // 時間が来たら終了
                    y[t]=0;                                           // スピーカ出力をオフにする

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
    }                                                               // メインループ終わり
Fin:
    printf("\nEnd!\n");                                             // 終了の表示
    return 0;
}
