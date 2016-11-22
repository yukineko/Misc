////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ ドラッグ＆ドロップ信号処理プログラム  //
//  BMPファイルの作成と書き出し                       //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.01.06.//
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(){
    ///////////////////////////////
    //                           //
    //  ファイル書き出し用変数   //
    //                           //
    ///////////////////////////////
    FILE *f2;
    
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
    unsigned char  Rout, Gout, Bout;                                    // RGB要素の出力用
    int m, n;                                                           // 座標の変数(m行n列)

    //////////////////////////////////////////
    //                                      //
    //      書き出しbmpファイルの設定      //
    //                                      //
    //////////////////////////////////////////

    width          = 256;                                               // 画像の幅
    height         = 256;                                               // 画像の高さ
    if(width%4  != 0)width = width  - width%4;                          // 幅を4の倍数に調整
    image_size     = width * height * 3;                                // 画像サイズ(3色)
    file_size      = image_size + FileHeaderSize;                       // 全体ファイルサイズ(byte)

    //////////////////////////////////////
    //                                  //
    //      ヘッダー書き込み            //
    //                                  //
    //////////////////////////////////////
    f2=fopen("output.bmp","wb");                                        // 出力ファイルオープン

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

    ///////////////////////////////////
    //                               //
    //        メインループ           //
    //                               //
    ///////////////////////////////////
//**********************************************************************//
    for(m=0;m<(int)height;m++){                                         // 行の更新
        for(n=0;n<(int)width;n++){                                      // 列の更新
            Bout = m;                                                   // 青：最下段が輝度  0，最上段が輝度MAX
            Gout = height - m;                                          // 緑：最下段が輝度MAX，最上段が輝度0
            Rout = n;                                                   // 赤：最左列が輝度  0, 最右列が輝度MAX

            fwrite(&Bout, sizeof(unsigned char), 1, f2);                //結果の書き込み
            fwrite(&Gout, sizeof(unsigned char), 1, f2);                //結果の書き込み
            fwrite(&Rout, sizeof(unsigned char), 1, f2);                //結果の書き込み
        }
    }
//**********************************************************************//
    fclose(f2);                                                         // 出力ファイルを閉じる
    return 0;                                                           // メイン関数の終了
}
