////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ リアルタイム信号処理プログラム        //
//  適応フィルタによるエコーキャンセラ(ボイススイッチ)//
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.01.24.//
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
#define ch_out   1                                                  // チャネル数
#define D        1600                                               // スピーカ－マイク間の初期遅延(使用環境により調整必要)
#define N        1024                                               // 適応フィルタ次数

int main(int argc, char **argv){
    /////////////////////////////////
    //                             //
    //  リアルタイム入出力用変数   //
    //                             //
    /////////////////////////////////
    char key;                                                       // スルーと処理音の切替用変数
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
    int hflg= 1;                                                    // 素通しと処理音の状態フラグ
    int i;                                                          // forループ用変数
    double s[MEM_SIZE+1]={0};                                       // 入力データ格納用変数
    double y[MEM_SIZE+1]={0};                                       // 出力データ格納用変数
    
    long int l = 0;                                                 // 出力時間のカウンタ
    short  input, output;                                           // 出力書き込み用変数
    double   d[MEM_SIZE+1]={0};                                     // 遠端話者信号
    double   x, e;                                                  // 観測信号，誤差信号
    double   h[N+1]={0};                                            // 適応フィルタ係数
    double   dh;                                                    // 適応フィルタ出力
    double   norm, mu;                                              // ノルムとステップサイズ
    double   d_pow, e_pow, lambda;                                  // 出力パワー，誤差パワー，忘却係数
    double   alpha;                                                 // 減衰定数

    ///////////////////////////////
    //                           //
    //  ファイル読み書き用変数   //
    //                           //
    ///////////////////////////////
    FILE *f1, *f2;
    unsigned short tmp1;                                            // 2バイト変数
    unsigned long  tmp2;                                            // 4バイト変数
    int ch;                                                         // チャネル数
    int len;                                                        // 入力信号の長さ
    
    char outname[256]={0};                                          // 出力ファイル名
    unsigned short channel;                                         // 出力チャンネル数
    unsigned short BytePerSample;                                   // 1サンプル当たりのバイト数
    unsigned long  file_size;                                       // 出力ファイルサイズ
    unsigned long  Fs_out;                                          // 出力サンプリング周波数
    unsigned long  BytePerSec;                                      // 1秒当たりのバイト数
    unsigned long  data_len;                                        // 出力波形のサンプル数
    unsigned long  fmt_chnk    =16;                                 // fmtチャンクのバイト数.PCMの場合は16bit
    unsigned short BitPerSample=16;                                 // 1サンプルのビット数
    unsigned short fmt_ID      =1;                                  // fmt ID. PCMの場合は1

    ///////////////////////////////
    //                           //
    //       変数の初期設定      //
    //                           //
    ///////////////////////////////

    d_pow = 0.0;                                                    // 出力パワー
    e_pow = 0.0;                                                    // エコーパワー
    lambda= 0.995;                                                  // 忘却係数
    alpha = 0.4;                                                    // 減衰定数

//************************************************************************//

    //////////////////////////////////////
    //                                  //
    // 実行ファイルの使い方に関する警告 //
    //                                  //
    //////////////////////////////////////
    if( argc != 2 ){                                                // 使用方法が間違った場合の警告
        fprintf( stderr, "Usage: ./a input.wav \n" );
        exit(-1);
    }
    if( (f1 = fopen(argv[1], "rb")) == NULL ){                      // 出力ファイルが開けない場合の警告
        fprintf( stderr, "Cannot open %s\n", argv[1] );
        exit(-2);
    }

    ////////////////////////////////////////////
    //                                        //
    //  入力waveファイルのヘッダ情報読み込み  //
    //                                        //
    ////////////////////////////////////////////
    printf("Wave data is\n");
    fseek(f1, 22L, SEEK_SET);                                       // チャネル情報位置に移動
    fread ( &tmp1, sizeof(unsigned short), 1, f1);                  // チャネル情報読込 2Byte
    ch=tmp1;                                                        // 入力チャネル数の記録
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                   // サンプリング周波数の読込 4Byte
    fseek(f1, 40L, SEEK_SET);                                       // サンプル数情報位置に移動
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                   // データのサンプル数取得 4Byte
    len=tmp2/2/ch;                                                  // 音声の長さの記録 (2Byteで1サンプル)

    printf("Channel       = %d ch\n",  ch);                         // 入力チャネル数の表示
    printf("Sample rate   = %d Hz \n", Fs);                         // 入力サンプリング周波数の表示
    printf("Sample number = %d\n",     len);                        // 入力信号の長さの表示

    //////////////////////////////////////
    //                                  //
    //     出力waveファイルの準備       //
    //                                  //
    //////////////////////////////////////
    Fs_out        = Fs;                                             // 出力サンプリング周波数を設定
    channel       = ch_out;                                         // 出力チャネル数を設定
    data_len      = channel*Fs*Length*2;                            // 全Byte数(1サンプルで2Byte)
    file_size     = 36+data_len;                                    // 全体ファイルサイズ
    BytePerSec    = Fs_out*channel*2;                               // 1秒当たりのバイト数
    BytePerSample = channel*2;                                      // 1サンプル当たりのバイト数
    sscanf(argv[1],"%[^-^.]s", outname);                            // 入力ファイル名取得 (拡張子除外)
    strcat(outname,"_output.wav");                                  // outname="入力ファイル名_output.wav"
    f2=fopen(outname,"wb");                                         // 出力ファイルオープン．存在しない場合は作成される

    //////////////////////////////////////
    //                                  //
    //     出力ヘッダ情報書き込み       //
    //                                  //
    //////////////////////////////////////
    fprintf(f2, "RIFF");                                    // "RIFF"
    fwrite(&file_size,    sizeof(unsigned long ), 1, f2);   // ファイルサイズ
    fprintf(f2, "WAVEfmt ");                                // "WAVEfmt"
    fwrite(&fmt_chnk,     sizeof(unsigned long ), 1, f2);   // fmt_chnk=16 (ビット数)
    fwrite(&fmt_ID,       sizeof(unsigned short), 1, f2);   // fmt ID=1 (PCM)
    fwrite(&channel,      sizeof(unsigned short), 1, f2);   // 出力チャネル数
    fwrite(&Fs_out,       sizeof(unsigned long ), 1, f2);   // 出力のサンプリング周波数
    fwrite(&BytePerSec,   sizeof(unsigned long ), 1, f2);   // 1秒当たりのバイト数
    fwrite(&BytePerSample,sizeof(unsigned short ),1, f2);   // 1サンプル当たりのバイト数
    fwrite(&BitPerSample, sizeof(unsigned short ),1, f2);   // 1サンプルのビット数(16ビット)
    fprintf(f2, "data");                                    // "data"
    fwrite(&data_len,     sizeof(unsigned long ), 1, f2);   // 出力データの長さ

    printf("\nOutput WAVE data is\n");
    printf("Channel       = %d ch\n",  channel);            // 出力チャネル数の表示
    printf("Sample rate   = %d Hz \n", Fs_out);             // 出力サンプリング周波数の表示
    printf("Sample number = %d\n",     data_len/ch/2);      // 出力信号の長さの表示
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

    mu   = 0.2;                                                     // 適応フィルタのステップサイズ

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

                    if(fread( &input, sizeof(short), 1,f1) < 1){
                        fseek(f1, 44L, SEEK_SET);                   // ファイルポインタをデータ先頭にセット
                    }
                    d[t] = input/32768.0;                           // 遠端話者信号を読み込んで出力
                    y[t] = d[t];                                    // マイク入力s[t]を含めないで出力
                    d_pow= lambda*d_pow+y[t]*y[t];                  // 出力パワー

                    dh=0, norm=0;
                    for(i=0;i<N;i++){
                        dh= dh+ h[i] * d[(t-D-i+MEM_SIZE)%MEM_SIZE];// 適応フィルタ出力 = 遠端話者信号の擬似エコー
                        norm=norm+d[(t-D-i+MEM_SIZE)%MEM_SIZE]*d[(t-D-i+MEM_SIZE)%MEM_SIZE];// ノルムの計算
                    }
                    e=s[t]-dh;                                      // 推定誤差 = 観測信号－適応フィルタ出力
                    if(norm>0.00001){                               // ノルムが一定値以上で係数更新
                        for(i=0;i<N;i++){                           // NLMSアルゴリズム
                            h[i] = h[i] + mu * d[(t-D-i+MEM_SIZE)%MEM_SIZE] * e/norm;
                        }
                    }
                    e_pow=lambda*e_pow+e*e;                         // 推定誤差パワー
                    if(d_pow>e_pow){                                // 誤差信号の方が小さいなら遠端話者のみと判断
                        e=e*alpha;                                  // 近端話者信号を減衰する
                    }
                    output=e*32767;                                 // 後半はエコーキャンセラ出力を記録
                    if(l>2*Fs/2) mu=0.0001;                         // 2秒以降はステップサイズ小
                    fwrite(&output, sizeof(short), 1, f2);          // ファイルへの書き込み
                    l++;                                            // 時間の更新
                    if(l>Length*Fs) goto Fin;                       // 指定時間が経過すると終了

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
    }                                                               // メインループ終わり
Fin:
    printf("\nEnd!\n");                                             // 終了の表示
    fclose(f1);
    fclose(f2);
    return 0;
}
