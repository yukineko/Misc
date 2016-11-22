////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ ���A���^�C���M�������v���O����        //
//  �I�[���p�X�t�B���^�̏c���ڑ�FFT��IFFT�ɂ��HPF    //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.02.10.//
////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>

#define MEM_SIZE 16000                                               // �����������̃T�C�Y
#define Fs       16000                                               // ���A���^�C�������̃T���v�����O���g��
#define NUM_BUF  8                                                   // �����ێ��p�o�b�t�@�̐�
#define BUF_SIZE 160                                                 // �e�o�b�t�@�̃T�C�Y
#define FFT_SIZE 32                                                  // FFT�_��

///////////////////////////////////////////////
//                                           //
//                                           //
//            FFT�֐��̍\�z                  //
//                                           //
//                                           //
///////////////////////////////////////////////
static double Wr[FFT_SIZE+1],Wi[FFT_SIZE+1];                         // FFT �d��
static double Fr[FFT_SIZE+1],Fi[FFT_SIZE+1],Xr[FFT_SIZE+1],Xi[FFT_SIZE+1]; //�����Ƌ���
static double xin[FFT_SIZE+1],z[FFT_SIZE+1];                         // FFT���͂Əo��
static double Xamp[FFT_SIZE+1];                                      // �U���X�y�N�g��
static double omega;                                                 // FFT�p���g��
static int    St,br[FFT_SIZE+1];                                     // FFT�X�e�[�W�ԍ��C�r�b�g���]

void bitr( void );                                                   // �r�b�g���]�p�֐��̐錾
void fft ( void );                                                   // FFT�֐��̐錾
void Wnk ( void );                                                   // FFT�o�^�t���C(BF)���Z�̏d�݊֐��̐錾
void ifft( void );                                                   // IFFT�֐��̐錾
void init( void );                                                   // FFT�֘A�ϐ��̏����ݒ�

///////////////////////////////////////////////
//                                           //
//                �����ݒ�                   //
//                                           //
///////////////////////////////////////////////
void init(void){
    int i;
    St=log((double)FFT_SIZE)/log(2.0)+0.5;                           // FFT�A���S���Y���̒i���̎Z�o +0.5��int�ւ̑Ή�
    omega=2.0*M_PI/FFT_SIZE;
    bitr();Wnk();
}

void bitr( void ){                                                   // �r�b�g���]
    int loop,i,j;
    br[0]=0;                                                         // 0�Ԗڂ�0
    br[1]=FFT_SIZE/2;                                                // 1�Ԗڂ�N/2
    loop=1;
    for(j=0;j<St-1;j++){
        br[loop*2]=br[loop]/2;
        loop=loop*2;
        for(i=1;i<loop;i++){
            br[loop+i]=br[loop]+br[i];
        }
    }
}

void Wnk(void){                                                     // �d�݂̌v�Z
    int i;
    for(i=0;i<FFT_SIZE/2;i++){
        Wr[i]=cos(omega*i);                                         // �d�ݎ���
        Wi[i]=sin(omega*i);                                         // �d�݋���
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
    for(j=0;j<FFT_SIZE;j++){                                        // FFT���͂̐ݒ�
        Fr[br[j]]=xin[j];                                           // ����
        Fi[br[j]]=0.0;
    }
    _2_s=1;
    for(s=1;s<=St;s++){                                             // �X�e�[�W��J��Ԃ�
        _2_s_1=_2_s;
        _2_s=_2_s*2;
        roop=FFT_SIZE/_2_s;
        for(j=0;j<roop;j++){                                        // DFT�J��Ԃ�
            for(k=0;k<_2_s_1;k++){                                  // BF���Z�J��Ԃ�
                l=_2_s*j+k;                                         // BF�̏���͔ԍ�
                m=_2_s_1*(2*j+1)+k;                                 // BF�̉����͔ԍ�
                p=roop*k;                                           // �����͂ւ̏d�ݔԍ�
                Wxmr=Fr[m]*Wr[p]+Fi[m]*Wi[p];                       // �d�݁~�����͂̎���
                Wxmi=Fi[m]*Wr[p]-Fr[m]*Wi[p];                       // �d�݁~�����͂̋���
                Xr[m]=Fr[m]=Fr[l]-Wxmr;                             // BF�̉��o�͂̎���
                Xi[m]=Fi[m]=Fi[l]-Wxmi;                             // BF�̉��o�͂̋���
                Xr[l]=Fr[l]=Fr[l]+Wxmr;                             // BF�̏�o�͂̎���
                Xi[l]=Fi[l]=Fi[l]+Wxmi;                             // BF�̏�o�͂̋���
            }
        }
    }

    for(j=0;j<FFT_SIZE;j++){
        Xamp[j]=sqrt(Fr[j]*Fr[j]+Fi[j]*Fi[j]);                      // �U���X�y�N�g���̎Z�o
    }
}
///////////////////////////////////////////////
//                                           //
//                  IFFT                     //
//                                           //
///////////////////////////////////////////////
void ifft( void ){                                                  // �t�t�[���G�ϊ�
    int _2_s,_2_s_1,roop,l,m,p;
    int s,j,k;
    double Wxmr,Wxmi;

    for(j=0;j<FFT_SIZE;j++){                                        // �tFFT���͂̐ݒ�
        Fr[br[j]]=Xr[j];
        Fi[br[j]]=Xi[j];
    }
    _2_s=1;
    for(s=1;s<=St;s++){                                             // �X�e�[�W��J��Ԃ�
        _2_s_1=_2_s;
        _2_s=_2_s*2;
        roop=FFT_SIZE/_2_s;
        for(j=0;j<roop;j++){                                        // FFT�J��Ԃ�
            for(k=0;k<_2_s_1;k++){                                  // BF���Z�J��Ԃ�
                l=_2_s*j+k;                                         // BF�̏���͔ԍ�
                m=_2_s_1*(2*j+1)+k;                                 // BF�̉����͔ԍ�
                p=roop*k;                                           // �����͂ւ̏d�ݔԍ�
                Wxmr=Fr[m]*Wr[p]-Fi[m]*Wi[p];                       // �d�݁~�����͂̎���
                Wxmi=Fi[m]*Wr[p]+Fr[m]*Wi[p];                       // �d�݁~�����͂̋���
                z[m]=Fr[m]=Fr[l]-Wxmr;                              // BF�̉��o�͂̎���
                Fi[m]=Fi[l]-Wxmi;                                   // BF�̉��o�͂̋���
                z[l]=Fr[l]=Fr[l]+Wxmr;                              // BF�̏�o�͂̎���
                Fi[l]=Fi[l]+Wxmi;                                   // BF�̏�o�͂̋���
            }
        }
    }
}

///////////////////////////////////////////////
//                                           //
//              ���C���֐�                   //
//                                           //
///////////////////////////////////////////////
int main(void){
    /////////////////////////////////
    //                             //
    //  ���A���^�C�����o�͗p�ϐ�   //
    //                             //
    /////////////////////////////////
    char   key;                                                     // �X���[�Ə������̐ؑ֗p�ϐ�
    int in1;                                                        // ���̓o�b�t�@�ԍ�
    int out1;                                                       // �o�̓o�b�t�@�ԍ�
    int flag;                                                       // ���o�͏�Ԃ̃t���O
    int n;                                                          // �o�b�t�@�p�̎���
    short in_buf[NUM_BUF][BUF_SIZE]={0};                            // ���̓o�b�t�@
    WAVEHDR in_hdr[NUM_BUF];                                        // ���̓o�b�t�@�̃w�b�_
	HWAVEIN in_hdl = 0;                                             // ����(�^��)�f�o�C�X�̃n���h��
    short out_buf[NUM_BUF][BUF_SIZE]={0};                           // �o�̓o�b�t�@
    WAVEHDR out_hdr[NUM_BUF];                                       // �o�̓o�b�t�@�̃w�b�_
    HWAVEOUT out_hdl = 0;                                           // �o��(�Đ�)�f�o�C�X�̃n���h��

    WAVEFORMATEX wave_format_ex = {WAVE_FORMAT_PCM,                 // PCM
                                 1,                                 // ���m����
                                 Fs,                                // �T���v�����O���g��
                                 2*Fs,                              // 1�b������̉��f�[�^�T�C�Y�ibyte�j
                                 2,                                 // ���f�[�^�̍ŏ��P�ʁi2byte�j
                                 16,                                // �ʎq���r�b�g�i16bit�j
                                 0                                  // �I�v�V�������̃T�C�Y�i0byte)
                                };


//************************************************************************//

    ///////////////////////////////
    //                           //
    //      �M�������p�ϐ�       //
    //                           //
    ///////////////////////////////
    int t   = 0;                                                    // ���͂̎���
    int to  = 0;                                                    // �o�͂̎���
    int hflg= 0;                                                    // �f�ʂ��Ə������̏�ԃt���O
    int i;                                                          // for���[�v�p�ϐ�
    double s[MEM_SIZE+1]={0};                                       // ���̓f�[�^�i�[�p�ϐ�
    double y[MEM_SIZE+1]={0};                                       // �o�̓f�[�^�i�[�p�ϐ�
    
    double   x[FFT_SIZE+1] ={0};                                    // FFT�̓���
    double   w[FFT_SIZE+1] ={0};                                    // ���֐�

    double    g[FFT_SIZE+1]={0};                                    // �t�B���^�����O�W��
    double   u0[FFT_SIZE+1]={0}, u1[FFT_SIZE+1]={0};                // �I�[���p�X�t�B���^�̒x���M��
    double   a;                                                     // �I�[���p�X�t�B���^�̌W��

    ///////////////////////////////
    //                           //
    //       �ϐ��̏����ݒ�      //
    //                           //
    ///////////////////////////////

    init();                                                         //�r�b�g���]�C��݌W���̌v�Z
    for(i=0;i<FFT_SIZE;i++){                                        // ���֐��̐ݒ�
        w[i]=0.5*(1.0-cos(2.0*M_PI*i/(double)FFT_SIZE));
    }
    a = 0.5756;                                                     // �I�[���p�X�t�B���^�̌W��

//************************************************************************//


    ///////////////////////////////////////
    //                                   //
    //    ���o�̓f�o�C�X�̃I�[�v��       // 
    //                                   //
    ///////////////////////////////////////
    waveInOpen(&in_hdl, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);   // ����(�^��)�f�o�C�X�I�[�v��
    for (i = 0; i < NUM_BUF; i++){
        memset(&in_hdr[i], 0, sizeof(WAVEHDR));                     // �S���̓o�b�t�@��������
    }
    waveOutOpen(&out_hdl, 0, &wave_format_ex, 0, 0, CALLBACK_NULL); // �o��(�Đ�)�f�o�C�X�I�[�v��
    waveOutPause(out_hdl);                                          // �T�E���h�f�o�C�X�̈ꎞ��~
    for (i = 0; i < NUM_BUF; i++){                                  // �S�o�̓o�b�t�@��������
        memset(&out_hdr[i], 0, sizeof(WAVEHDR));
    }
    ///////////////////////////////////////
    //                                   //
    //    ���o�̓o�b�t�@�̏����ݒ�       // 
    //                                   //
    ///////////////////////////////////////
    for (i=0;i<NUM_BUF;i++){                                        // �o�b�t�@�̐������J��Ԃ�
        out_hdr[i].lpData = (char *)out_buf[i];                     // �o�̓o�b�t�@�f�[�^���X�V
        out_hdr[i].dwBufferLength = BUF_SIZE * 2;                   // �o�̓o�b�t�@�T�C�Y��ݒ�
        out_hdr[i].dwFlags = 0;                                     // �o�̓t���O�̃N���A
        waveOutPrepareHeader(out_hdl, &out_hdr[i], sizeof(WAVEHDR));// �o�̓o�b�t�@�����b�N
        waveOutWrite(out_hdl, &out_hdr[i], sizeof(WAVEHDR));        // �o�̓o�b�t�@���o�͑҂��L���[�ɑ��M
        in_hdr[i].lpData = (char *)in_buf[i];                       // ���̓o�b�t�@�f�[�^�̍X�V
        in_hdr[i].dwBufferLength = BUF_SIZE * 2;                    // ���̓o�b�t�@�T�C�Y��ݒ�
        in_hdr[i].dwFlags = 0;                                      // ���̓t���O�̃N���A
        waveInPrepareHeader(in_hdl, &in_hdr[i], sizeof(WAVEHDR));   // ���̓o�b�t�@�����b�N
        waveInAddBuffer(in_hdl, &in_hdr[i], sizeof(WAVEHDR));       // ���̓o�b�t�@����͑҂��L���[�ɑ��M
    }
    waveOutRestart(out_hdl);                                        // �����o�͊J�n
    waveInStart(in_hdl);                                            // �������͊J�n

    in1  = 0;                                                       // ���̓o�b�t�@�ԍ��̏�����
    out1 = 0;                                                       // �o�̓o�b�t�@�ԍ��̏�����
    flag = 0;                                                       // ���o�͔���t���O��0(����)�Ƃ���

    printf("[space]--> Through <=> Processing\n");                  // [space]�ŏ����؂�ւ�
    printf("[Enter]--> End\n");                                     // [Enter]�ŏI��
    printf("Through\n");                                            // �f�ʂ��ł��邱�Ƃ�\��


    ///////////////////////////////////
    //                               //
    //        ���C�����[�v           //
    //                               //
    ///////////////////////////////////
    while (1){
        //////////////////////////
        //                      //
        //    �o�͏��o������    //
        //                      //
        //////////////////////////
        if (flag == 1){                                             // flag=1�Ȃ�o�͏��
            if ((out_hdr[out1].dwFlags & WHDR_DONE) != 0){          // �o�̓o�b�t�@�̃t���O���0�łȂ��Ȃ珈�������s
                for (n = 0; n < BUF_SIZE; n++){                     // �o�͂�BUF_SIZE�T���v�������������o��
                    to=(to+1)%MEM_SIZE;                             // �o�͎����̍X�V
                    out_buf[out1][n] = y[to]*32768;                 // �o�̓f�[�^���L�^
                }
                waveOutUnprepareHeader(out_hdl, &out_hdr[out1], sizeof(WAVEHDR)); // �o�̓o�b�t�@���N���[���A�b�v
                out_hdr[out1].lpData = (char *)out_buf[out1];       // �o�̓f�[�^���X�V
//                out_hdr[out1].dwBufferLength = BUF_SIZE * 2;        // �o�̓o�b�t�@�T�C�Y��ݒ�
                out_hdr[out1].dwFlags = 0;                          // �o�̓o�b�t�@�̃t���O����0�ɂ���
                waveOutPrepareHeader(out_hdl, &out_hdr[out1], sizeof(WAVEHDR));   // �����X�V���o�̓o�b�t�@������
                waveOutWrite(out_hdl, &out_hdr[out1], sizeof(WAVEHDR));// �o�͑҂��L���[�ɏo�̓o�b�t�@�f�[�^�𑗐M
                out1=(out1+1)%NUM_BUF;                              // �o�̓o�b�t�@�ԍ����X�V
                flag = 0;                                           // ���o�̓t���O�� flag=0 (���͏��) �ɂ���
            }
        }
        //////////////////////////
        //                      //
        //    ���͓Ǎ��ݏ���    //
        //                      //
        //////////////////////////
        if ((in_hdr[in1].dwFlags & WHDR_DONE) != 0){                // ���̓o�b�t�@�̃t���O���0�łȂ��Ȃ珈�������s
            waveInUnprepareHeader(in_hdl, &in_hdr[in1], sizeof(WAVEHDR)); // �^���ς݂̓��̓o�b�t�@��I��
            for (n = 0; n < BUF_SIZE; n++){                         // BUF_SIZE�T���v�����̓��͂�Ǎ��݁C��������
                t=(t+1)%MEM_SIZE;                                   // ���͎����̍X�V
                s[t] = in_buf[in1][n]/32768.0;                      // ���̓o�b�t�@����f�[�^�ǂݍ��� (���K��)
                y[t] = s[t];                                        // �o�́����͂̑f�ʂ�������
                if(hflg!=0){                                        // ������ԃt���O��0�ȊO�Ȃ�M�����������s

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

                    x[0]=s[t];
                    for(i=1;i<FFT_SIZE;i++){
                        u0[i] =     x[i-1] + a * u1[i];                 // IIR���o��
                         x[i] = -a * u0[i] + u1[i];                     // APF�o�́����i�ւ̓���
                        u1[i] = u0[i];                                  // �M���x������
                    }
                    for(i=0;i<FFT_SIZE;i++){
                        xin[i]=x[i]*w[i];                               // ���֐���������
                    }

                    ///////////////////////////
                    //                       //
                    //        FFT����        //
                    //                       //
                    ///////////////////////////
                    for(i=0;i<=FFT_SIZE/2;i++){
                        Xr[i]=Xr[FFT_SIZE-i]=0.0;                       // �Q�C����0�Ƃ���
                        if( i>10 ){                                     // �S����16�ш�
                            Xr[i]=Xr[FFT_SIZE-i]=1.0;                   // �w����g���̃Q�C����1�ɂ���(BPF)
                        }
                        Xi[i]=Xi[FFT_SIZE-i]=0.0;                       // �����͏��0
                    }
                    ifft();
                    for(i=0;i<FFT_SIZE;i++){
                        g[i] = z[(FFT_SIZE/2+i)%FFT_SIZE]/FFT_SIZE;     // �Q�C����IFFT������V�t�g���ăt�B���^�W��������
                        g[i] = g[i]*w[i];                               // �t�B���^�W���ɑ����|����
                    }
                    y[t] = 0;                                           // �o�͂�������
                    for(i=0;i<FFT_SIZE;i++){
                        y[t] = y[t] + x[i]*g[i];                        // �eAPF�o�͂��t�B���^�����O
                    }

//************************************************************************//

//                    y[t]=atan(y[t])/(M_PI/2.0);                     // �N���b�v�h�~
                }
            }
            in_hdr[in1].dwFlags = 0;                                // ���̓o�b�t�@�̃t���O����0�ɂ���
            waveInPrepareHeader(in_hdl, &in_hdr[in1], sizeof(WAVEHDR));   // �����X�V�����̓o�b�t�@������
            waveInAddBuffer(in_hdl, &in_hdr[in1], sizeof(WAVEHDR)); // ���͑҂��L���[�ɓ��̓o�b�t�@�f�[�^�𑗐M
            in1=(in1+1)%NUM_BUF;                                    // ���̓o�b�t�@�̍X�V
            flag = 1;                                               // ���o�̓t���O�� flag=1 (�o�͏��)�ɂ���
        }

        ////////////////////////////////////////////////////
        //                                                //
        //        �L�[�{�[�h���͂ɑ΂��鏈���̐ݒ�        //
        //                                                //
        //                                                //
        //  [�f�t�H���g�ݒ�]                              //
        //  �E�X�y�[�X�L�[�őf�ʂ��ƐM��������؂�ւ�    //
        //  �EEnter�L�[�Ńv���O�����I��                   //
        //                                                //
        ////////////////////////////////////////////////////
        if (kbhit()){                                               // �L�[�{�[�h���͂���������
            key = getch();                                          // �L�[�̃`�F�b�N
            if (key == 32){                                         // �X�y�[�X�L�[��������Ƃ�
            	hflg=(hflg+1)%2;                                    // �����t���O�ύX
                if(hflg==1) printf("Processing\n");                 // �������ł��邱�Ƃ�\��
                else        printf("Through\n");                    // �f�ʂ��ł��邱�Ƃ�\��
            }
            if (key == 13){                                         // Enter�L�[�������ꂽ�Ƃ�
                waveInStop(in_hdl);                                 // ���͂��~
                for (i = 0; i < NUM_BUF; i++){                      // �S���̓o�b�t�@�̃A�����b�N
                    if ((in_hdr[i].dwFlags & WHDR_PREPARED) != 0){
                        waveInUnprepareHeader(in_hdl, &in_hdr[i], sizeof(WAVEHDR ));
                    }
                }
                waveInClose(in_hdl);                                // ����(�^��)�f�o�C�X�̃N���[�Y
                waveOutPause(out_hdl);                              // �o�͂��~
                for (i = 0; i < NUM_BUF; i++){
                    if ((out_hdr[i].dwFlags & WHDR_PREPARED) != 0){ // �S�o�̓o�b�t�@�̃A�����b�N
                        waveOutUnprepareHeader(out_hdl, &out_hdr[i], sizeof(WAVEHDR));
                    }
                }
                waveOutClose(out_hdl);                              // �o��(�Đ�)�f�o�C�X�̃N���[�Y
                return 0;
            }
        }
    }                                                               //���C�����[�v�I���
}
