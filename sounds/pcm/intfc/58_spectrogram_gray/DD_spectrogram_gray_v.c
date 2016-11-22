////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ �h���b�O���h���b�v�M�������v���O����  //
//  �X�y�N�g���O�����쐬                              //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.01.08.//
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define  MEM_SIZE 16000                                     // �����������̃T�C�Y
#define  FFT_SIZE 512                                       // FFT�_��
#define  OL       2                                         // �t���[���V�t�g �� FFT_SIZE / OL
///////////////////////////////////////////////
//                                           //
//                                           //
//            FFT�֐��̍\�z                  //
//                                           //
//                                           //
///////////////////////////////////////////////
static double Wr[FFT_SIZE+1],Wi[FFT_SIZE+1];                // FFT �d��
static double Fr[FFT_SIZE+1],Fi[FFT_SIZE+1],Xr[FFT_SIZE+1],Xi[FFT_SIZE+1]; //�����Ƌ���
static double xin[FFT_SIZE+1],z[FFT_SIZE+1];                // FFT���͂Əo��
static double Xamp[FFT_SIZE+1];                             // �U���X�y�N�g��
static double omega;                                        // FFT�p���g��
static int    St,br[FFT_SIZE+1];                            // FFT�X�e�[�W�ԍ��C�r�b�g���]

void bitr( void );                                          // �r�b�g���]�p�֐��̐錾
void fft ( void );                                          // FFT�֐��̐錾
void Wnk ( void );                                          // FFT�o�^�t���C(BF)���Z�̏d�݊֐��̐錾
void ifft( void );                                          // IFFT�֐��̐錾
void init( void );                                          // FFT�֘A�ϐ��̏����ݒ�

///////////////////////////////////////////////
//                                           //
//                �����ݒ�                   //
//                                           //
///////////////////////////////////////////////
void init(void){
    int i;
    St=log((double)FFT_SIZE)/log(2.0)+0.5;                  // FFT�A���S���Y���̒i���̎Z�o +0.5��int�ւ̑Ή�
    omega=2.0*M_PI/FFT_SIZE;
    bitr();Wnk();
}

void bitr( void ){                                          // �r�b�g���]
    int loop,i,j;
    br[0]=0;                                                // 0�Ԗڂ�0
    br[1]=FFT_SIZE/2;                                       // 1�Ԗڂ�N/2
    loop=1;
    for(j=0;j<St-1;j++){
        br[loop*2]=br[loop]/2;
        loop=loop*2;
        for(i=1;i<loop;i++){
            br[loop+i]=br[loop]+br[i];
        }
    }
}

void Wnk(void){                                             // �d�݂̌v�Z
    int i;
    for(i=0;i<FFT_SIZE/2;i++){
        Wr[i]=cos(omega*i);                                 // �d�ݎ���
        Wi[i]=sin(omega*i);                                 // �d�݋���
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
    for(j=0;j<FFT_SIZE;j++){                                // FFT���͂̐ݒ�
        Fr[br[j]]=xin[j];                                   // ����
        Fi[br[j]]=0.0;
    }
    _2_s=1;
    for(s=1;s<=St;s++){                                     // �X�e�[�W��J��Ԃ�
        _2_s_1=_2_s;
        _2_s=_2_s*2;
        roop=FFT_SIZE/_2_s;
        for(j=0;j<roop;j++){                                // DFT�J��Ԃ�
            for(k=0;k<_2_s_1;k++){                          // BF���Z�J��Ԃ�
                l=_2_s*j+k;                                 // BF�̏���͔ԍ�
                m=_2_s_1*(2*j+1)+k;                         // BF�̉����͔ԍ�
                p=roop*k;                                   // �����͂ւ̏d�ݔԍ�
                Wxmr=Fr[m]*Wr[p]+Fi[m]*Wi[p];               // �d�݁~�����͂̎���
                Wxmi=Fi[m]*Wr[p]-Fr[m]*Wi[p];               // �d�݁~�����͂̋���
                Xr[m]=Fr[m]=Fr[l]-Wxmr;                     // BF�̉��o�͂̎���
                Xi[m]=Fi[m]=Fi[l]-Wxmi;                     // BF�̉��o�͂̋���
                Xr[l]=Fr[l]=Fr[l]+Wxmr;                     // BF�̏�o�͂̎���
                Xi[l]=Fi[l]=Fi[l]+Wxmi;                     // BF�̏�o�͂̋���
            }
        }
    }

    for(j=0;j<FFT_SIZE;j++){
        Xamp[j]=sqrt(Fr[j]*Fr[j]+Fi[j]*Fi[j]);              // �U���X�y�N�g���̎Z�o
    }
}

///////////////////////////////////////////////
//                                           //
//                  IFFT                     //
//                                           //
///////////////////////////////////////////////
void ifft( void ){                                          // �t�t�[���G�ϊ�
    int _2_s,_2_s_1,roop,l,m,p;
    int s,j,k;
    double Wxmr,Wxmi;

    for(j=0;j<FFT_SIZE;j++){                                // �tFFT���͂̐ݒ�
        Fr[br[j]]=Xr[j];
        Fi[br[j]]=Xi[j];
    }
    _2_s=1;
    for(s=1;s<=St;s++){                                     // �X�e�[�W��J��Ԃ�
        _2_s_1=_2_s;
        _2_s=_2_s*2;
        roop=FFT_SIZE/_2_s;
        for(j=0;j<roop;j++){                                // FFT�J��Ԃ�
            for(k=0;k<_2_s_1;k++){                          // BF���Z�J��Ԃ�
                l=_2_s*j+k;                                 // BF�̏���͔ԍ�
                m=_2_s_1*(2*j+1)+k;                         // BF�̉����͔ԍ�
                p=roop*k;                                   // �����͂ւ̏d�ݔԍ�
                Wxmr=Fr[m]*Wr[p]-Fi[m]*Wi[p];               // �d�݁~�����͂̎���
                Wxmi=Fi[m]*Wr[p]+Fr[m]*Wi[p];               // �d�݁~�����͂̋���
                z[m]=Fr[m]=Fr[l]-Wxmr;                      // BF�̉��o�͂̎���
                Fi[m]=Fi[l]-Wxmi;                           // BF�̉��o�͂̋���
                z[l]=Fr[l]=Fr[l]+Wxmr;                      // BF�̏�o�͂̎���
                Fi[l]=Fi[l]+Wxmi;                           // BF�̏�o�͂̋���
            }
        }
    }
}

///////////////////////////////////////////////
//                                           //
//              ���C���֐�                   //
//                                           //
///////////////////////////////////////////////
int main(int argc, char **argv){
    ///////////////////////////////
    //                           //
    //  �t�@�C���ǂݍ��ݗp�ϐ�   //
    //                           //
    ///////////////////////////////
    FILE *f1;
    unsigned short tmp1;                                    // 2�o�C�g�ϐ�
    unsigned long  tmp2;                                    // 4�o�C�g�ϐ�
    int Fs;                                                 // ���̓T���v�����O���g��
    int ch;                                                 // ���̓`���l����
    int len;                                                // ���͐M���̒���
    
    ///////////////////////////////
    //                           //
    //  �t�@�C�������o���p�ϐ�   //
    //                           //
    ///////////////////////////////
    FILE *f2;
    
    char outname[256]={0};                                  // �o�̓t�@�C����
    unsigned long  file_size;                               // �t�@�C���T�C�Y 4Byte
    unsigned long  width;                                   // �摜�̕�
    unsigned long  height;                                  // �摜�̍���
    unsigned long  zero  = 0;                               // '0'�������ݗp
    unsigned long  one   = 1;                               // '1'�������ݗp
	unsigned long  FileHeaderSize=54;                       // �S�w�b�_�[�T�C�Y(�J���[�摜�p)
    unsigned long  InfoHeaderSize=40;                       // ���w�b�_�[�T�C�Y
    unsigned long  data_len;                                // �g�`�f�[�^�̃T���v����
    unsigned long  color = 24;                              // 1��f������̐F��
    unsigned long  image_size;                              // �o�͉摜�T�C�Y
    unsigned char  Rout, Gout, Bout;                        // RGB�v�f�̏o�͗p

//************************************************************************//

    ///////////////////////////////
    //                           //
    //      �M�������p�ϐ�       //
    //                           //
    ///////////////////////////////
    int      t      = 0;                                    // �����̕ϐ�
    long int t_out  = 0;                                    // �I�������v���p�̕ϐ�
    int      add_len= 0;                                    // �o�͐M������������T���v����
    short    input, output;                                 // �Ǎ��ݕϐ��Ə��o���ϐ�
    double   s[MEM_SIZE+1]={0};                             // ���̓f�[�^�i�[�p�ϐ�
    double   y[MEM_SIZE+1]={0};                             // �o�̓f�[�^�i�[�p�ϐ�
    double   z1[OL][FFT_SIZE+1]={0};                        // �I�[�o�[���b�v�̏o�͕ۑ��p

    long int l,i;                                           // FFT�p�ϐ�

    int      u=0, nm[OL+1]={0};                             // �e�t���[���̎��ԊǗ�
    double   x[OL][FFT_SIZE+1] ={0};                        // FFT�̓���
    double   spec;                                          // �X�y�N�g���O�����̋P�x�l

//************************************************************************//


    //////////////////////////////////////
    //                                  //
    // ���s�t�@�C���̎g�����Ɋւ���x�� //
    //                                  //
    //////////////////////////////////////
    if( argc != 2 ){                                        // �g�p���@���Ԉ�����ꍇ�̌x��
        fprintf( stderr, "Usage: ./a input.wav \n" );
        exit(-1);
    }
    if( (f1 = fopen(argv[1], "rb")) == NULL ){              // �o�̓t�@�C�����J���Ȃ��ꍇ�̌x��
        fprintf( stderr, "Cannot open %s\n", argv[1] );
        exit(-2);
    }

    ////////////////////////////////////////////
    //                                        //
    //  ����wave�t�@�C���̃w�b�_���ǂݍ���  //
    //                                        //
    ////////////////////////////////////////////
    printf("Wave data is\n");
    fseek(f1, 22L, SEEK_SET);                               // �`���l�����ʒu�Ɉړ�
    fread ( &tmp1, sizeof(unsigned short), 1, f1);          // �`���l�����Ǎ� 2Byte
    ch=tmp1;                                                // ���̓`���l�����̋L�^
    fread ( &tmp2, sizeof(unsigned long), 1, f1);           // �T���v�����O���g���̓Ǎ� 4Byte
    Fs = tmp2;                                              // �T���v�����O���g���̋L�^
    fseek(f1, 40L, SEEK_SET);                               // �T���v�������ʒu�Ɉړ�
    fread ( &tmp2, sizeof(unsigned long), 1, f1);           // �f�[�^�̃T���v�����擾 4Byte
    len=tmp2/2/ch;                                          // �����̒����̋L�^ (2Byte��1�T���v��)

    printf("Channel       = %d ch\n",  ch);                 // ���̓`���l�����̕\��
    printf("Sample rate   = %d Hz \n", Fs);                 // ���̓T���v�����O���g���̕\��
    printf("Sample number = %d\n",     len);                // ���͐M���̒����̕\��

    //////////////////////////////////////////
    //                                      //
    //      �����o��bmp�t�@�C���̐ݒ�      //
    //                                      //
    //////////////////////////////////////////

    width          = FFT_SIZE;                              // �摜�̕��iFFT�T�C�Y�j
    height         = len*OL/FFT_SIZE;                       // �摜�̍���(�t���[����)
    if(width%4  != 0)width = width  - width%4;              // ����4�̔{���ɒ���
    image_size     = width * height * 3;                    // �摜�T�C�Y(3�F)
    file_size      = image_size + FileHeaderSize;           // �S�̃t�@�C���T�C�Y(byte)

    //////////////////////////////////////
    //                                  //
    //      �w�b�_�[��������            //
    //                                  //
    //////////////////////////////////////
    sscanf(argv[1],"%[^-^.]s", outname);                                // ���̓t�@�C�����擾 (�g���q���O)
    strcat(outname,"_output.bmp");                                      // outname="���̓t�@�C����_output.wav"
    f2=fopen(outname,"wb");                                             // �o�̓t�@�C���I�[�v���D���݂��Ȃ��ꍇ�͍쐬�����

    fprintf(f2, "BM");                                                  // 'BM'
    fwrite(&file_size,      sizeof(unsigned long ), 1, f2);             // �t�@�C���T�C�Y
    fwrite(&zero,           sizeof(unsigned short), 1, f2);             // 0(�\��̈�)
    fwrite(&zero,           sizeof(unsigned short), 1, f2);             // 0(�\��̈�)
    fwrite(&FileHeaderSize, sizeof(unsigned long ), 1, f2);             // �摜�f�[�^�܂ł̃I�t�Z�b�g
    fwrite(&InfoHeaderSize, sizeof(unsigned long ), 1, f2);             // ���w�b�_�T�C�Y
    fwrite(&width,          sizeof(unsigned long ), 1, f2);             // �摜�̕�
    fwrite(&height,         sizeof(unsigned long ), 1, f2);             // �摜�̒���
    fwrite(&one,            sizeof(unsigned short), 1, f2);             // �v���[����(���1)
    fwrite(&color,          sizeof(unsigned short), 1, f2);             // 1��f������̐F��
    fwrite(&zero,           sizeof(unsigned long ), 1, f2);             // 0(�����k)
    fwrite(&image_size,     sizeof(unsigned long ), 1, f2);             // �摜�T�C�Y
    fwrite(&zero,           sizeof(unsigned long ), 1, f2);             // �����𑜓x
    fwrite(&zero,           sizeof(unsigned long ), 1, f2);             // �����𑜓x
    fwrite(&zero,           sizeof(unsigned long ), 1, f2);             // �p���b�g�̐F�̐�
    fwrite(&zero,           sizeof(unsigned long ), 1, f2);             // �d�v�p���b�g�̃C���f�b�N�X

    printf("\nOutput BMP data is\n");
    printf("Width         = %d \n", width );
    printf("Height        = %d \n", height);
    printf("Image Size    = %d\n",  image_size/3);

//************************************************************************//

    ///////////////////////////////
    //                           //
    //      �ϐ��̏����ݒ�       //
    //                           //
    ///////////////////////////////

    init();                                                 // �r�b�g���]�C��݌W���̌v�Z
    l = 0;                                                  // FFT�J�n�����Ǘ�

    for (i=0;i<OL;i++){
        nm[i]=FFT_SIZE/OL*i;                                // �t���[���J�n���Ԃ̒���
    }

//************************************************************************//

    ///////////////////////////////////
    //                               //
    //        ���C�����[�v           //
    //                               //
    ///////////////////////////////////
    fseek(f1, 44L, SEEK_SET);                               // �����f�[�^�J�n�ʒu�Ɉړ�
    while(1){                                               // ���C�����[�v
        if(fread( &input, sizeof(short), 1,f1) < 1){        // ������ input �ɓǍ���
            if( t_out > len+add_len ) break;                // ���[�v�I������
            else input=0;                                   // ���[�v�p���������Ǎ��ݏI���Ȃ� input=0
        }
        s[t] = input/32768.0;                               // �����̍ő�l��1�Ƃ���(���K��)


//************************************************************************//

        ////////////////////////////////////////////////////
        //                                                //
        //              Signal Processing                 //
        //                                                //
        //  ���ݎ���t�̓��� s[t] ����o�� y[t] ������   //
        //                                                //
        //  �� t��0����MEM_SIZE-1�܂ł����[�v             //
        //                                                //
        ////////////////////////////////////////////////////

        y[t]=0;
        for (i=0;i<OL;i++){
            x[i][nm[i]]=s[t];                               // ���̓��͂����ꂼ��̃t���[���Ɋi�[
            y[t]=y[t]+z1[i][nm[i]]/FFT_SIZE/(OL/2.0);       // �o�͌v�Z
        }
        
        for(i=0;i<OL;i++){
            nm[i]=(nm[i]+1)%FFT_SIZE;                       // �e�t���[���̃J�E���g�A�b�v
        }
        if(l>=FFT_SIZE/OL){
            for(i=0;i<FFT_SIZE;i++){
                xin[i]=x[u][i]*0.5*(1.0-cos(2.0*M_PI*i/(double)FFT_SIZE));//FFT����
            }
            fft();                                          // FFT
            ///////////////////////////
            //                       //
            //        FFT����        //
            //                       //
            ///////////////////////////
            for(i=0;i<FFT_SIZE;i++){
                spec = Xamp[i]*Xamp[i]*256;                 // �p���[�X�y�N�g��
//                spec = 10*log10(spec);                    // �ΐ��p���[�X�y�N�g��
                if(spec>255)spec=255;                       // �P�x�̍ő�l��255�Ƃ���
                if(spec<0)  spec=0;                         // �P�x�̍ŏ��l��0�Ƃ���
                Bout=Gout=Rout=spec;
                fwrite(&Bout, sizeof(unsigned char), 1, f2);//��Z���ʂ̏�������
                fwrite(&Gout, sizeof(unsigned char), 1, f2);//��Z���ʂ̏�������
                fwrite(&Rout, sizeof(unsigned char), 1, f2);//��Z���ʂ̏�������
            }
            l=0;
            u=(u+1)%OL;                                     // �o�͕ۑ��p�z�� z1[u][i] ��ύX
        }
        l++;                                                // FFT�p�̎����Ǘ�

//************************************************************************//

        t=(t+1)%MEM_SIZE;                                   // ���� t �̍X�V
        t_out++;                                            // ���[�v�I�������̌v��
    }
    fclose(f1);                                             // ���̓t�@�C�������
    fclose(f2);                                             // �o�̓t�@�C�������
    return 0;                                               // ���C���֐��̏I��
}
