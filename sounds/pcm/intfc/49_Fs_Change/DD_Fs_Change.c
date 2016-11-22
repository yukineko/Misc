////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ ドラッグ＆ドロップ信号処理プログラム  //
//  sinc補間によるFs変換                              //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                        2015.12.27. //
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define  MEM_SIZE 16000                                     // 音声メモリのサイズ
#define  To_Fs    32000                                      // 出力のサンプリング周波数
#define  N        64                                        // フィルタ次数

int main(int argc, char **argv){
    ///////////////////////////////
    //                           //
    //  ファイル読み書き用変数   //
    //                           //
    ///////////////////////////////
    FILE *f1, *f2;
    unsigned short tmp1;                                    // 2バイト変数
    unsigned long  tmp2;                                    // 4バイト変数
    int Fs;                                                 // 入力サンプリング周波数
    int ch;                                                 // 入力チャネル数
    int len;                                                // 入力信号の長さ
    
    char outname[256]={0};                                  // 出力ファイル名
    unsigned short channel;                                 // 出力チャンネル数
    unsigned short BytePerSample;                           // 1サンプル当たりのバイト数
    unsigned long  file_size;                               // 出力ファイルサイズ
    unsigned long  Fs_out;                                  // 出力サンプリング周波数
    unsigned long  BytePerSec;                              // 1秒当たりのバイト数
    unsigned long  data_len;                                // 出力波形のサンプル数
    unsigned long  fmt_chnk    =16;                         // fmtチャンクのバイト数.PCMの場合は16bit
    unsigned short BitPerSample=16;                         // 1サンプルのビット数
    unsigned short fmt_ID      =1;                          // fmt ID. PCMの場合は1


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

    int      i, L, to=0;                                    // 時刻管理用変数
    double   gt, Frt, sinc;                                 // サンプリング周波数の比
    double   x[MEM_SIZE+1]={0};                             // 観測信号
    double   h[N+1]={0};                                    // フィルタ係数
    double   fe;                                            // 遮断周波数
    
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

    //////////////////////////////////////
    //                                  //
    //     出力waveファイルの準備       //
    //                                  //
    //////////////////////////////////////
    Fs_out        = To_Fs;                                  // 出力サンプリング周波数を設定
    channel       = ch;                                     // 出力チャネル数を設定
    data_len      = channel*(len+add_len)*2*(To_Fs/(double)Fs);// 出力データの長さ = 全Byte数 (1サンプルで2Byte)
    file_size     = 36+data_len;                            // 全体ファイルサイズ
    BytePerSec    = Fs_out*channel*2;                       // 1秒当たりのバイト数
    BytePerSample = channel*2;                              // 1サンプル当たりのバイト数
    sscanf(argv[1],"%[^-^.]s", outname);                    // 入力ファイル名取得 (拡張子除外)
    strcat(outname,"_output.wav");                          // outname="入力ファイル名_output.wav"
    f2=fopen(outname,"wb");                                 // 出力ファイルオープン．存在しない場合は作成される

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
    ///////////////////////////////
    //                           //
    //      変数の初期設定       //
    //                           //
    ///////////////////////////////

    L =25;                                                  // 補間に使う信号の数
    gt=0;                                                   // 出力時間（実数）
    Frt=(double)Fs/To_Fs;                                   // 入出力サンプリング周波数の比(in/out)
    
    ////////////////////////////////////////////
    //                                        //
    //   アンチエイリアスフィルタの係数設定   //
    //                                        //
    ////////////////////////////////////////////
    if(Frt>1) fe = To_Fs/2.0/Fs;                            // 新しいサンプリング周波数に対する最大周波数
    else      fe = Fs/2.0/To_Fs;
    for(i=-N/2;i<=N/2;i++){                                 // LPFの設計
        if(i==0) h[N/2+i]=2.0*fe;
        else{
            h[N/2+i] = 2.0*fe*sin(2.0*M_PI*fe*i)/(2.0*M_PI*fe*i);
        }
        h[N/2+i]=h[N/2+i]*0.5*(1.0-cos(2.0*M_PI*(N/2+i)/N));
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

        x[t]=s[t];                                          // x[t]を入力とする
        /////////////////////////////////////////////////
        //  Fs>Fs_out のときのアンチエイリアスフィルタ //
        /////////////////////////////////////////////////
        if(Frt>1){                                          // 出力サンプリング周波数の方が小さい場合
            s[t]=0;
            for(i=0;i<=N;i++){
                s[t]=s[t]+x[(t-i+MEM_SIZE)%MEM_SIZE]*h[i];  // LPF出力をs[t]とする
            }
        }

        while( (int)gt==(t-L+MEM_SIZE)%MEM_SIZE ){          // Lサンプル過去の信号を中心に出力を作成
            y[to]=0;
            for(i=(int)gt-L; i<(int)gt+L; i++){             // 新しいサンプリング点における値を作成
                //////////////////////
                //  sinc関数の設定  //
                //////////////////////
                if( gt-i==0 )sinc=1.0;
                else sinc = sin( M_PI*(gt-i) )/( M_PI*(gt-i) );
                //////////////////////
                //   補間出力作成   //
                //////////////////////
                y[to] = y[to] + s[(i+MEM_SIZE)%MEM_SIZE] * sinc;// 補間出力をy[to]とする
            }
            output=y[to]*32767;                              // 出力を補間出力y[to]とする
            /////////////////////////////////////////////////
            //  Fs<Fs_out のときのアンチエイリアスフィルタ //
            /////////////////////////////////////////////////
            if(Frt<1){                                      // 出力サンプリング周波数の方が大きい場合
                x[to]=0;
                for(i=0;i<=N;i++){                          // y[to]に対するLPF出力をx[to]とする
                    x[to]=x[to]+y[(to-i+MEM_SIZE)%MEM_SIZE]*h[i];
                }
                output=x[to]*32767;                         // 出力をx[t]とする
            }
            for(i=1;i<=channel;i++){
                fwrite(&output, sizeof(short), 1, f2);      // 結果の書き込み
            }
            to=(to+1)%MEM_SIZE;                             // 出力時刻を1つ進める
            gt = gt + Frt;                                  // 出力「時間」を進める
            if(gt>=MEM_SIZE)gt=gt-MEM_SIZE;
        }

//************************************************************************//

        if(ch==2){                                          // ステレオ入力の場合
            if(fread(&input, sizeof(short), 1, f1)<1) break;// Rchのカラ読み込み
        }
        t=(t+1)%MEM_SIZE;                                   // 時刻 t の更新
        t_out++;                                            // ループ終了時刻の計測
    }
    fclose(f1);                                             // 入力ファイルを閉じる
    fclose(f2);                                             // 出力ファイルを閉じる
    return 0;                                               // メイン関数の終了
}
