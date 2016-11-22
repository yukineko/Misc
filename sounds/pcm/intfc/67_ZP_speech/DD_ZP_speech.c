////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ �h���b�O���h���b�v�M�������v���O����  //
//  �[���ʑ�����                                      //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.01.21.//
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define  MEM_SIZE 16000                                     // �����������̃T�C�Y
#define  FFT_SIZE 512                                       // FFT�_��
#define  pw       1                                         // ZPS�ϊ��̐U���ׂ̂�

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
static double Xphs[FFT_SIZE+1];                             // �ʑ��X�y�N�g��
static double omega;                                        // FFT�p���g��
static int    St,br[FFT_SIZE+1];                            // FFT�X�e�[�W�ԍ��C�r�b�g���]
static double s0[FFT_SIZE+1]={0};                           // �[���ʑ��M��

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

void Wnk(void){                                            // �d�݂̌v�Z
    int i;
    for(i=0;i<FFT_SIZE/2;i++){
        Wr[i]=cos(omega*i);                                // �d�ݎ���
        Wi[i]=sin(omega*i);                                // �d�݋���
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
//                  ZPT                      //
//                                           //
///////////////////////////////////////////////
void zpt(void){
    int k;
    fft();
    for(k=0;k<FFT_SIZE;k++){
        if(Xr[k]!=0){
            Xphs[k]=atan2(Xi[k],Xr[k]);                     // �ʑ��L�^�D-�΂���΂̃��W�A���l�D
        }
        Xr[k]=pow(Xamp[k], pw);                             // �U����pw����[���ʑ��M���Ƃ���
        Xi[k]=0;                                            // ������0
    }
    ifft();                                                 // IFFT��ZPS���쐬
    for(k=0;k<FFT_SIZE;k++){
        s0[k]=z[k]/FFT_SIZE;                                // s0�Ƃ��ċL��
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
        xin[k]=s0[k];                                       // FFT���͂�ZPS�Ƃ���
    }
    fft();                                                  // ZPS����U���X�y�N�g���𓾂�
   	for(k=0;k<FFT_SIZE;k++){
        if(Fr[k]<=0)Fr[k]=0;
        Xamp[k]=Fr[k]; 			//�U���̎Z�o
    }
    for(k=0;k<FFT_SIZE;k++){
        Xr[k]=Xamp[k]*cos(Xphs[k]);                         // �ʑ��ƗZ�����Ď���������
        Xi[k]=Xamp[k]*sin(Xphs[k]);                         // �ʑ��ƗZ�����ċ���������
    }
    ifft();                                                 // IFFT�ɂ�莞�ԗ̈�M���𓾂�(z[n])
}


///////////////////////////////////////////////
//                                           //
//              ���C���֐�                   //
//                                           //
///////////////////////////////////////////////
int main(int argc, char **argv){
    ///////////////////////////////
    //                           //
    //  �t�@�C���ǂݏ����p�ϐ�   //
    //                           //
    ///////////////////////////////
    FILE *f1, *f2;
    unsigned short tmp1;                                    // 2�o�C�g�ϐ�
    unsigned long  tmp2;                                    // 4�o�C�g�ϐ�
    int Fs;                                                 // ���̓T���v�����O���g��
    int ch;                                                 // ���̓`���l����
    int len;                                                // ���͐M���̒���
    
    char outname[256]={0};                                  // �o�̓t�@�C����
    unsigned short channel;                                 // �o�̓`�����l����
    unsigned short BytePerSample;                           // 1�T���v��������̃o�C�g��
    unsigned long  file_size;                               // �o�̓t�@�C���T�C�Y
    unsigned long  Fs_out;                                  // �o�̓T���v�����O���g��
    unsigned long  BytePerSec;                              // 1�b������̃o�C�g��
    unsigned long  data_len;                                // �o�͔g�`�̃T���v����
    unsigned long  fmt_chnk    =16;                         // fmt�`�����N�̃o�C�g��.PCM�̏ꍇ��16bit
    unsigned short BitPerSample=16;                         // 1�T���v���̃r�b�g��
    unsigned short fmt_ID      =1;                          // fmt ID. PCM�̏ꍇ��1


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

    long int l,i;                                           // FFT�p�ϐ�
    double   x[FFT_SIZE+1] ={0};                            // FFT�̓���
    double   x1[FFT_SIZE+1]={0};                            // ���t���[���V�t�g���̓��͕ێ��p
    double   z1[FFT_SIZE+1]={0};                            // �n�[�t�I�[�o�[���b�v�̏o�͕ێ��p

    ///////////////////////////////
    //                           //
    //      �ϐ��̏����ݒ�       //
    //                           //
    ///////////////////////////////

    init();                                                 // �r�b�g���]�C��݌W���̌v�Z
    l    =  0;                                              // FFT�J�n�����Ǘ�

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

    //////////////////////////////////////
    //                                  //
    //     �o��wave�t�@�C���̏���       //
    //                                  //
    //////////////////////////////////////
    Fs_out        = Fs;                                     // �o�̓T���v�����O���g����ݒ�
    channel       = ch;                                     // �o�̓`���l������ݒ�
    data_len      = channel*(len+add_len)*2;                // �o�̓f�[�^�̒��� = �SByte�� (1�T���v����2Byte)
    file_size     = 36+data_len;                            // �S�̃t�@�C���T�C�Y
    BytePerSec    = Fs_out*channel*2;                       // 1�b������̃o�C�g��
    BytePerSample = channel*2;                              // 1�T���v��������̃o�C�g��
    sscanf(argv[1],"%[^-^.]s", outname);                    // ���̓t�@�C�����擾 (�g���q���O)
    strcat(outname,"_output.wav");                          // outname="���̓t�@�C����_output.wav"
    f2=fopen(outname,"wb");                                 // �o�̓t�@�C���I�[�v���D���݂��Ȃ��ꍇ�͍쐬�����

    //////////////////////////////////////
    //                                  //
    //     �o�̓w�b�_��񏑂�����       //
    //                                  //
    //////////////////////////////////////
    fprintf(f2, "RIFF");                                    // "RIFF"
    fwrite(&file_size,    sizeof(unsigned long ), 1, f2);   // �t�@�C���T�C�Y
    fprintf(f2, "WAVEfmt ");                                // "WAVEfmt"
    fwrite(&fmt_chnk,     sizeof(unsigned long ), 1, f2);   // fmt_chnk=16 (�r�b�g��)
    fwrite(&fmt_ID,       sizeof(unsigned short), 1, f2);   // fmt ID=1 (PCM)
    fwrite(&channel,      sizeof(unsigned short), 1, f2);   // �o�̓`���l����
    fwrite(&Fs_out,       sizeof(unsigned long ), 1, f2);   // �o�͂̃T���v�����O���g��
    fwrite(&BytePerSec,   sizeof(unsigned long ), 1, f2);   // 1�b������̃o�C�g��
    fwrite(&BytePerSample,sizeof(unsigned short ),1, f2);   // 1�T���v��������̃o�C�g��
    fwrite(&BitPerSample, sizeof(unsigned short ),1, f2);   // 1�T���v���̃r�b�g��(16�r�b�g)
    fprintf(f2, "data");                                    // "data"
    fwrite(&data_len,     sizeof(unsigned long ), 1, f2);   // �o�̓f�[�^�̒���

    printf("\nOutput WAVE data is\n");
    printf("Channel       = %d ch\n",  channel);            // �o�̓`���l�����̕\��
    printf("Sample rate   = %d Hz \n", Fs_out);             // �o�̓T���v�����O���g���̕\��
    printf("Sample number = %d\n",     data_len/ch/2);      // �o�͐M���̒����̕\��

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

        x[l] = x1[l];                                       // FFT_SIZE/2�܂ł̓��͂�x[l]�Ɋi�[
        x1[l]= x[FFT_SIZE/2+l]=s[t];                        // FFT_SIZE/2�ȍ~�̓��͂�x[l]�Ɋi�[
        y[t] = (z[l]+z1[l]);                                // ZPS�œ����o��
        z1[l]= z[FFT_SIZE/2+l];                             // �n�[�t�I�[�o�[���b�v�p�ɏo�͋L�^

        if( l>=FFT_SIZE/2){                                 // ���t���[�����Ƃ�FFT�����s
            for(i=0;i<FFT_SIZE;i++){
                xin[i]=x[i]*0.5*(1.0-cos(2.0*M_PI*i/(double)FFT_SIZE));// ���֐���������
            }
            zpt();                                          // �[���ʑ��ϊ�

            ////////////////////////////////////////////////////
            //                                                //
            //               �[���ʑ��M������                 //
            //                                                //
            ////////////////////////////////////////////////////
        
            for(i=0;i<FFT_SIZE;i++){
                z[i]=s0[i];                                 // �[���ʑ�����
            }
            l=0;
        }
        l++;                                                // FFT�p�̎����Ǘ�

//************************************************************************//

//        y[t] = atan(y[t])/(M_PI/2.0);                       // �N���b�v�h�~
        output = y[t]*32767;                                // �o�͂𐮐���
        fwrite(&output, sizeof(short), 1, f2);              // ���ʂ̏����o��
        if(ch==2){                                          // �X�e���I���͂̏ꍇ
            if(fread(&input, sizeof(short), 1, f1)<1) break;// Rch�̃J���ǂݍ���
            fwrite(&output, sizeof(short), 1, f2);          // Rch��������(=Lch)
        }
        t=(t+1)%MEM_SIZE;                                   // ���� t �̍X�V
        t_out++;                                            // ���[�v�I�������̌v��
    }
    fclose(f1);                                             // ���̓t�@�C�������
    fclose(f2);                                             // �o�̓t�@�C�������
    return 0;                                               // ���C���֐��̏I��
}
