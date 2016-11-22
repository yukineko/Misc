////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ ドラッグ＆ドロップ信号処理プログラム  //
//  BMPファイルからのパターンプレイバック             //
//  cosの利用                                         //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.01.10.//
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define  MEM_SIZE 16000                                                         // 音声メモリのサイズ
#define  Fs       16000                                                         // サンプリング周波数
#define  CH       1                                                             // チャネル数

///////////////////////////////////////////////
//                                           //
//              メイン関数                   //
//                                           //
///////////////////////////////////////////////
int main(int argc, char **argv){
    //////////////////////////////////
    //                              //
    //  bmpファイル読み出し用変数   //
    //                              //
    //////////////////////////////////
    FILE *f1, *f2;
    
    unsigned short tmp1;                                                        // 2バイト変数
    unsigned long  tmp2;                                                        // 4バイト変数
    unsigned long  DataStartPoint;                                              // 4バイト変数
    unsigned long  DataGetPoint;                                                //4バイト変数
    unsigned char  xinput;                                                      // 入力画像データ
    int wd,hi,len, CL;                                                          // 横サイズ，縦サイズ，長さ，カラー
    double  R, G, B;                                                            // RGBの輝度
    double  Rout, Gout, Bout;                                                   // RGB要素の出力用

    int m, n=0;                                                                 // 座標の変数(m行n列)

    //////////////////////////////////
    //                              //
    //  wavファイル書き出し用変数   //
    //                              //
    //////////////////////////////////
    char outname[256]={0};                                                      // 出力ファイル名
    unsigned short channel;                                                     // 出力チャンネル数
    unsigned short BytePerSample;                                               // 1サンプル当たりのバイト数
    unsigned long  file_size;                                                   // 出力ファイルサイズ
    unsigned long  Fs_out;                                                      // 出力サンプリング周波数
    unsigned long  BytePerSec;                                                  // 1秒当たりのバイト数
    unsigned long  data_len;                                                    // 出力波形のサンプル数
    unsigned long  fmt_chnk    =16;                                             // fmtチャンクのバイト数.PCMの場合は16bit
    unsigned short BitPerSample=16;                                             // 1サンプルのビット数
    unsigned short fmt_ID      =1;                                              // fmt ID. PCMの場合は1
    double         Xamp[Fs+1]={0};                                              // 振幅スペクトル

//************************************************************************//

    ///////////////////////////////
    //                           //
    //      信号処理用変数       //
    //                           //
    ///////////////////////////////
    int      t      = 0;                                                        // 時刻の変数
    long int t_out  = 0;                                                        // 時刻の変数
    int      i;                                                                 // forループ用変数
    short    output;                                                            // 読込み変数と書出し変数
    double   y[MEM_SIZE+1]={0};                                                 // 出力データ格納用変数

//************************************************************************//


    //////////////////////////////////////
    //                                  //
    // 実行ファイルの使い方に関する警告 //
    //                                  //
    //////////////////////////////////////
    if( argc != 2 ){
        fprintf( stderr, "Usage: ./a input.bmp \n" );
        exit(-1);
    }
    if( (f1 = fopen(argv[1], "rb")) == NULL ){                                  // 出力ファイルが開けない場合の警告
        fprintf( stderr, "Cannot open %s\n", argv[1] );
        exit(-2);
    }

    ////////////////////////////////////////////
    //                                        //
    //      bmpファイルヘッダの読み込み       //
    //                                        //
    ////////////////////////////////////////////
    fseek(f1, 10, SEEK_SET);                                                    // ヘッダサイズ情報に移動
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                               // 4Byte情報取得
    DataStartPoint=tmp2;
    fseek(f1, 18, SEEK_SET);                                                    // 幅の情報位置に移動
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                               // 4Byte情報取得
    wd=tmp2;
    fseek(f1, 22, SEEK_SET);                                                    // 高さ情報位置に移動
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                               // 4Byte情報取得
    hi=tmp2;
    fseek(f1, 34, SEEK_SET);                                                    // データ数情報位置に移動
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                               // 4Byte情報取得
    if(DataStartPoint==54)CL=3;                                                 // カラー画像ならCL=3
    else CL=1;                                                                  // グレー画像ならCL=0

    printf("BMP data is\n");
    printf("Width         = %d \n", wd );
    printf("Height        = %d \n", hi);
    printf("Image Size    = %d\n",  hi*wd);

    //////////////////////////////////////
    //                                  //
    //     出力waveファイルの準備       //
    //                                  //
    //////////////////////////////////////
    Fs_out        = Fs;                                                         // 出力サンプリング周波数を設定
    channel       = 1;                                                          // 出力チャネル数を設定
    data_len      = channel*(hi*2*wd)*2;                                        // 全Byte数(1サンプルで2Byte)
    file_size     = 36+data_len;                                                // 全体ファイルサイズ
    BytePerSec    = Fs_out*channel*2;                                           // 1秒当たりのバイト数
    BytePerSample = channel*2;                                                  // 1サンプル当たりのバイト数
    sscanf(argv[1],"%[^-^.]s", outname);                                        // 入力ファイル名取得 (拡張子除外)
    strcat(outname,"_output.wav");                                              // outname="入力ファイル名_output.wav"
    f2=fopen(outname,"wb");                                                     // 出力ファイルオープン．存在しない場合は作成される

    //////////////////////////////////////
    //                                  //
    //     出力ヘッダ情報書き込み       //
    //                                  //
    //////////////////////////////////////
    fprintf(f2, "RIFF");                                                        // "RIFF"
    fwrite(&file_size,    sizeof(unsigned long ), 1, f2);                       // ファイルサイズ
    fprintf(f2, "WAVEfmt ");                                                    // "WAVEfmt"
    fwrite(&fmt_chnk,     sizeof(unsigned long ), 1, f2);                       // fmt_chnk=16 (ビット数)
    fwrite(&fmt_ID,       sizeof(unsigned short), 1, f2);                       // fmt ID=1 (PCM)
    fwrite(&channel,      sizeof(unsigned short), 1, f2);                       // 出力チャネル数
    fwrite(&Fs_out,       sizeof(unsigned long ), 1, f2);                       // 出力のサンプリング周波数
    fwrite(&BytePerSec,   sizeof(unsigned long ), 1, f2);                       // 1秒当たりのバイト数
    fwrite(&BytePerSample,sizeof(unsigned short ),1, f2);                       // 1サンプル当たりのバイト数
    fwrite(&BitPerSample, sizeof(unsigned short ),1, f2);                       // 1サンプルのビット数(16ビット)
    fprintf(f2, "data");                                                        // "data"
    fwrite(&data_len,     sizeof(unsigned long ), 1, f2);                       // 出力データの長さ

    printf("\nOutput WAVE data is\n");
    printf("Channel       = %d ch\n",  channel);                                // 出力チャネル数の表示
    printf("Sample rate   = %d Hz \n", Fs_out);                                 // 出力サンプリング周波数の表示
    printf("Sample number = %d\n",     data_len/channel/2);                     // 出力信号の長さの表示

    ///////////////////////////////////
    //                               //
    //        メインループ           //
    //                               //
    ///////////////////////////////////
    fseek(f1, DataStartPoint, SEEK_SET);                                        // データ先頭位置に移動
    while(1){                                                                   // メインループ
        if(t>=(hi*2)){
            for(m=0;m<hi;m++){
                DataGetPoint=DataStartPoint + ( m*wd + n )*CL;                  // データ取得位置
                fseek(f1, DataGetPoint, SEEK_SET);                              // データ取得位置に移動
                if(fread( &xinput, sizeof(unsigned char), 1,f1) < 1) break;     // 画像読み込み
                B=G=R=xinput/255.0;
                if(CL==3){                                                      // カラー画像は3色読み取り
                    if(fread( &xinput, sizeof(unsigned char), 1,f1) < 1) break; //Green読み込み
                    G=xinput/255.0;
                    if(fread( &xinput, sizeof(unsigned char), 1,f1) < 1) break; //Red読み込み
                    R=xinput/255.0;
                }
                Xamp[m] = sqrt( (B+G+R)/3.0 );                                  // スペクトログラムをパワースペクトルとみなす
//                Xamp[m] = exp( (B+G+R)/3.0/20.0 )-1;                            // スペクトログラムを自然対数とみなす
//                Xamp[m] = pow( 10, (B+G+R)/3.0/20.0 )-1;                        // スペクトログラムを常用対数とみなす
            }
            t=0;
            n++;
            if(n>=wd)break;
        }

        y[t]=0;
        for(i=0;i<hi;i++){                                                      // 出力音の作成
            y[t] = y[t] + Xamp[i]/hi * cos(2*M_PI*i/(2*hi)*t_out);
        }
//        y[t] = atan(y[t])/(M_PI/2.0);                                           // クリップ防止
        output = y[t]*32767;                                                    // 出力振幅調整
        fwrite(&output, sizeof(short), 1, f2);                                  // 結果の書き出し
        if(channel==2){                                                         // ステレオ入力の場合
            fwrite(&output, sizeof(short), 1, f2);                              // Rch書き込み(=Lch)
        }
        t++;
        t_out++;                                                                // 時刻の更新
    }

    fclose(f1);                                                                 // 入力ファイルを閉じる
    fclose(f2);                                                                 // 出力ファイルを閉じる
    return 0;                                                                   // メイン関数の終了
}
