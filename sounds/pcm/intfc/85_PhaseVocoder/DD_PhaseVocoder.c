////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ �h���b�O���h���b�v�M�������v���O����  //
//  Phase Vocoder                                     //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.02.06.//
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define  MEM_SIZE 1600                                      // �����������̃T�C�Y
#define  FFT_SIZE 512                                       // FFT�_��
#define  N        200                                       // �L������t���[����
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
        Xamp[j] = sqrt(Fr[j]*Fr[j]+Fi[j]*Fi[j]);            // �U���X�y�N�g���̎Z�o
        Xphs[j] = atan2(Fi[j], Fr[j]);                      // �ʑ��X�y�N�g���̎Z�o
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
    double   w[FFT_SIZE+1] ={0};                            // ���֐�

    double   Amp[N+1][FFT_SIZE/2+1]={0};                    // �U���X�y�N�g���ۑ��p
    double   Phs[N+1][FFT_SIZE/2+1]={0};                    // ���������p�̈ʑ��X�y�N�g���Ǘ�
    double   Xphase[FFT_SIZE/2+1]={0};                      // �ʑ��X�y�N�g��
    int      outlen, fm;                                    // �o�̓f�[�^���C���̓t���[���ԍ�
    double   fmout;                                         // �o�̓t���[���ԍ�
    double   alpha;                                         // �����̍����̔{��

    ///////////////////////////////
    //                           //
    //      �ϐ��̏����ݒ�       //
    //                           //
    ///////////////////////////////

    init();                                                 // �r�b�g���]�C��݌W���̌v�Z
    l    = 0;                                               // FFT�J�n�����Ǘ�
    for(i=0;i<FFT_SIZE;i++){                                // ���֐��̐ݒ�
        w[i]=0.5*(1.0-cos(2.0*M_PI*i/(double)FFT_SIZE));
    }
    fm   = 0;                                               // �����t���[���ԍ�
    fmout= 0.0;                                             // �o�͂̃t���[���ԍ�
    alpha= 0.5;                                             // �����̍����̔{��

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
    outlen = (int)(len/alpha);                              // �o�̓f�[�^�̒�������
    Fs_out        = Fs;                                     // �o�̓T���v�����O���g����ݒ�
    channel       = ch;                                     // �o�̓`���l������ݒ�
    data_len      = channel*(outlen)*2;                     // �o�̓f�[�^�̒��� = �SByte�� (1�T���v����2Byte)
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
    
        ////////////////////////////////////////////////
        //                                            //
        //       �t�F�[�Y�{�R�[�_(�������͕�)         //
        //                                            //
        ////////////////////////////////////////////////

        x[l] = x1[l];                                       // FFT_SIZE/2�܂ł̓��͂�x[l]�Ɋi�[
        x1[l]= x[FFT_SIZE/2+l]=s[t];                        // FFT_SIZE/2�ȍ~�̓��͂�x[l]�Ɋi�[

        if( l>=FFT_SIZE/2){                                 // ���t���[�����Ƃ�FFT�����s
            for(i=0;i<FFT_SIZE;i++){
                xin[i]=x[i]*w[i];                           // ���֐���������
            }
            fft();                                          // FFT
            ///////////////////////////
            //                       //
            //     FFT���ʂ̋L�^     //
            //                       //
            ///////////////////////////
            for(i=0;i<=FFT_SIZE/2;i++){
                Amp[fm][i]=Xamp[i];                         // �S�U���X�y�N�g���̋L�^
                Phs[fm][i]=Xphs[i];                         // �S�ʑ��X�y�N�g���̋L�^
            }
            l=0;
            fm=(fm+1);                                      // ���̓t���[���ԍ�
        }
        l++;                                                // FFT�p�̎����Ǘ�
        t=(t+1)%MEM_SIZE;                                   // ���� t �̍X�V
        t_out++;                                            // ���[�v�I�������̌v��
    }

    ////////////////////////////////////////////////
    //                                            //
	//       �t�F�[�Y�{�R�[�_(����������)         //
    //                                            //
    ////////////////////////////////////////////////
    l=0;
    t=0;
    for(t_out=0;t_out<outlen;t_out++){                      // ���C�����[�v
        y[t] = (z[l]+z1[l])/FFT_SIZE;                       // IFFT�œ����o��
        z1[l]= z[FFT_SIZE/2+l];                             // �n�[�t�I�[�o�[���b�v�p�ɏo�͋L�^

        if( l>=FFT_SIZE/2){                                 // ���t���[�����Ƃ�FFT�����s
            fmout=fmout+alpha;                              // �o�̓t���[���ԍ��̍X�V
            for(i=0;i<=FFT_SIZE/2;i++){
                Xphase[i]=Phs[(int)fmout][i];               // �ʑ��͂��̃t���[���̂��̂��̗p
                Xr[i]=Amp[(int)fmout][i]*cos(Xphase[i]);    // �����M���̎���
                Xi[i]=Amp[(int)fmout][i]*sin(Xphase[i]);    // �����M���̋���
                Xr[FFT_SIZE-i]= Xr[i];                      // �����͋��Ώ�
                Xi[FFT_SIZE-i]=-Xi[i];                      // �����͊�Ώ�
            }
            ifft();                                         // IFFT
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
    }
    fclose(f1);                                             // ���̓t�@�C�������
    fclose(f2);                                             // �o�̓t�@�C�������
    return 0;                                               // ���C���֐��̏I��
}