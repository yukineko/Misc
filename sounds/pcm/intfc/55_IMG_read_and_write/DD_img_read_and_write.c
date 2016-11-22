////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ ドラッグ＆ドロップ信号処理プログラム  //
//  BMPファイル読み込み(グレー可)と書き出し(カラー)   //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.01.03.//
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char **argv){
    ///////////////////////////////
    //                           //
    //  ファイル読み書き用変数   //
    //                           //
    ///////////////////////////////
    FILE *f1, *f2;
    unsigned short tmp1;                                                // 2バイト変数
    unsigned long  tmp2;                                                // 4バイト変数
    unsigned long  DataStartPoint;                                      // 4バイト変数
    unsigned char  xinput;                                              // 入力画像データ
    int wd,hi,len, CL;                                                  // 横サイズ，縦サイズ，長さ，カラー
    
    char outname[256]={0};                                              // 出力ファイル名
    unsigned long  file_size;                                           // ファイルサイズ 4Byte
    unsigned long  width;                                               // 画像の幅
    unsigned long  height;                                              // 画像の高さ
    unsigned long  zero  = 0;                                           // '0'書き込み用
    unsigned long  one   = 1;                                           // '1'書き込み用
	unsigned long  FileHeaderSize=54;                                   // 全ヘッダーサイズ(カラー画像用)
    unsigned long  InfoHeaderSize=40;                                   // 情報ヘッダーサイズ
    unsigned long  data_len;                                            // 波形データのサンプル数
    unsigned long  color = 24;                                          // 1画素あたりの色数
    unsigned long  image_size;                                          // 出力画像サイズ

    unsigned char  R, G, B;                                             // RGBの輝度
    unsigned char  Rout, Gout, Bout;                                    // RGB要素の出力用

    int m, n;                                                           // 座標の変数(m行n列)


    //////////////////////////////////////
    //                                  //
    // 実行ファイルの使い方に関する警告 //
    //                                  //
    //////////////////////////////////////
    if( argc != 2 ){                                                    // 使用方法が間違った場合の警告
        fprintf( stderr, "Usage: ./a input.wav \n" );
        exit(-1);
    }
    if( (f1 = fopen(argv[1], "rb")) == NULL ){                          // 出力ファイルが開けない場合の警告
        fprintf( stderr, "Cannot open %s\n", argv[1] );
        exit(-2);
    }

    ////////////////////////////////////////////
    //                                        //
    //          bmpファイルの読み込み         //
    //                                        //
    ////////////////////////////////////////////
    fseek(f1, 10, SEEK_SET);                                            // ヘッダサイズ情報に移動
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                       // 4Byte情報取得
    DataStartPoint=tmp2;
    fseek(f1, 18, SEEK_SET);                                            // 幅の情報位置に移動
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                       // 4Byte情報取得
    wd=tmp2;
    fseek(f1, 22, SEEK_SET);                                            // 高さ情報位置に移動
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                       // 4Byte情報取得
    hi=tmp2;
    fseek(f1, 34, SEEK_SET);                                            // データ数情報位置に移動
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                       // 4Byte情報取得
    len=tmp2;
    if(DataStartPoint==54)CL=3;                                         // カラー画像ならCL=3
    else CL=1;                                                          // グレー画像ならCL=0

    printf("BMP data is\n");
    printf("Width         = %d \n", wd );
    printf("Height        = %d \n", hi);
    printf("Image Size    = %d\n",  len/CL);

    //////////////////////////////////////////
    //                                      //
    //      書き出しbmpファイルの設定      //
    //                                      //
    //////////////////////////////////////////

    width          = wd;                                                // 画像の幅
    height         = hi;                                                // 画像の高さ
    image_size     = width * height * 3;                                // 画像サイズ(3色)
    file_size      = image_size + FileHeaderSize;                       // 全体ファイルサイズ(byte)

    //////////////////////////////////////
    //                                  //
    //      ヘッダー書き込み            //
    //                                  //
    //////////////////////////////////////
    sscanf(argv[1],"%[^-^.]s", outname);                                // 入力ファイル名取得(拡張子除外)
    strcat(outname,"_output.bmp");                                      // 文字列連結
    f2=fopen(outname,"wb");                                             // 出力ファイルオープン

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

    m=0;                                                                // 行番号
    n=0;                                                                // 列番号

    ///////////////////////////////////
    //                               //
    //        メインループ           //
    //                               //
    ///////////////////////////////////
    fseek(f1, DataStartPoint, SEEK_SET);                                // データ先頭位置に移動
    while(1){                                                           // メインループ
        if(fread( &xinput, sizeof(unsigned char), 1,f1) < 1) break;     // 画像読み込み
        B=G=R=xinput;
        if(CL==3){                                                      // カラー画像は3色読み取り
            if(fread( &xinput, sizeof(unsigned char), 1,f1) < 1) break; //Green読み込み
            G=xinput;
            if(fread( &xinput, sizeof(unsigned char), 1,f1) < 1) break; //Red読み込み
            R=xinput;
        }


//************************************************************************//

        ////////////////////////////////////////////////////
        //                                                //
        //              Signal Processing                 //
        //                                                //
        //           RGB入力からRGB出力をつくる           //
        //                                                //
        ////////////////////////////////////////////////////

        Bout = B;                                                       // 青 読み取り
        Gout = G;                                                       // 緑 読み取り
        Rout = R;                                                       // 赤 読み取り

//************************************************************************//

        fwrite(&Bout, sizeof(unsigned char), 1, f2);                    //乗算結果の書き込み
        fwrite(&Gout, sizeof(unsigned char), 1, f2);                    //乗算結果の書き込み
        fwrite(&Rout, sizeof(unsigned char), 1, f2);                    //乗算結果の書き込み

        n=(n+1)%width;                                                  //画像データ位置更新
        if(n==0){	
            m=(m+1)%height;                                             //画像データ位置更新
        }
    }
    fclose(f1);                                                         // 入力ファイルを閉じる
    fclose(f2);                                                         // 出力ファイルを閉じる
    return 0;                                                           // メイン関数の終了
}
