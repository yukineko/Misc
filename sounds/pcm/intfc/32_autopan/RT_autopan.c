////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ ���A���^�C���M�������v���O����        //
//  �I�[�g�p��                                        //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2015.12.07.//
////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>

#define MEM_SIZE 16000                                              // �����������̃T�C�Y
#define Fs       16000                                              // ���A���^�C�������̃T���v�����O���g��
#define NUM_BUF  8                                                  // �����ێ��p�o�b�t�@�̐�
#define BUF_SIZE 160                                                // �e�o�b�t�@�̃T�C�Y

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
    WAVEFORMATEX wave_format_ex_out = {WAVE_FORMAT_PCM,             // PCM
                                 2,                                 // �X�e���I
                                 Fs,                                // �T���v�����O���g��
                                 4*Fs,                              // 1�b������̉��f�[�^�T�C�Y�ibyte�j
                                 4,                                 // ���f�[�^�̍ŏ��P�ʁi2byte�j
                                 16,                                // �ʎq�����x�i16bit�j
                                 0                                  // �I�v�V�������̃T�C�Y�i0byte
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
    double y_L[MEM_SIZE+1]={0};                                     // Lch�o�̓f�[�^�i�[�p�ϐ�
    double y_R[MEM_SIZE+1]={0};                                     // Rch�o�̓f�[�^�i�[�p�ϐ�
    
    long int l = 0;                                                 // �����g�p�̎����Ǘ�
    double   a, r;                                                  // �����g�p�����[�^
    
    ///////////////////////////////
    //                           //
    //       �ϐ��̏����ݒ�      //
    //                           //
    ///////////////////////////////
    
    r=0.2;                                                  // �����g�̎��g��

    ///////////////////////////////////////
    //                                   //
    //    ���o�̓f�o�C�X�̃I�[�v��       // 
    //                                   //
    ///////////////////////////////////////
    waveInOpen(&in_hdl, 0, &wave_format_ex, 0, 0, CALLBACK_NULL);   // ����(�^��)�f�o�C�X�I�[�v��
    for (i = 0; i < NUM_BUF; i++){
        memset(&in_hdr[i], 0, sizeof(WAVEHDR));                     // �S���̓o�b�t�@��������
    }
    waveOutOpen(&out_hdl, 0, &wave_format_ex_out, 0, 0, CALLBACK_NULL); // �o��(�Đ�)�f�o�C�X�I�[�v��
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
                for (n = 0; n < BUF_SIZE/2; n++){                   // �o�͂�BUF_SIZE�T���v�������������o��
                    to=(to+1)%MEM_SIZE;                             // �o�͎����̍X�V
                    out_buf[out1][2*n]   = y_L[to]*32767;           // �o�b�t�@��Lch�o�̓f�[�^��������
                    out_buf[out1][2*n+1] = y_R[to]*32767;           // �o�b�t�@��Rch�o�̓f�[�^��������
                }
                waveOutWrite(out_hdl, &out_hdr[out1], sizeof(WAVEHDR));// �o�͑҂��L���[�ɏo�̓o�b�t�@�f�[�^�𑗐M
                out1=(out1+1)%NUM_BUF;                              // �o�̓o�b�t�@�ԍ����X�V
                for (n = 0; n < BUF_SIZE/2; n++){                   // ���͂�2�{�o�͂��邽�ߍēxBUF_SIZE�T���v�������������o��
                    to=(to+1)%MEM_SIZE;                             // �o�͎����̍X�V
                    out_buf[out1][2*n]   = y_L[to]*32767;           // �o�b�t�@��Lch�o�̓f�[�^��������
                    out_buf[out1][2*n+1] = y_R[to]*32767;           // �o�b�t�@��Rch�o�̓f�[�^��������
                }
                waveOutUnprepareHeader(out_hdl, &out_hdr[out1], sizeof(WAVEHDR)); // �o�̓o�b�t�@���N���[���A�b�v
                out_hdr[out1].lpData = (char *)out_buf[out1];       // �o�̓f�[�^���X�V
                out_hdr[out1].dwBufferLength = BUF_SIZE * 2;        // �o�̓o�b�t�@�T�C�Y��ݒ�
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
                y_L[t] = s[t];                                      // �o�́����͂̑f�ʂ�������
                y_R[t] = s[t];                                      // �o�́����͂̑f�ʂ�������
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

                    a = 1.0 + sin(2.0*M_PI*r*l/(double)Fs);         // �����g���X�V
                    y_L[t] = a*s[t];                                // �����g����͂ɏ悶�����̂�Lch�o��

                    a = 1.0 - sin(2.0*M_PI*r*l/(double)Fs);         // ���̐����g���X�V
                    y_R[t] = a*s[t];                                // �����g����͂ɏ悶�����̂�Rch�o��

                    l=(l+1)%(10*Fs);                                // �����g�����p�̎���

//************************************************************************//

                  //  y[t]=atan(y[t])/(M_PI/2.0);                     // �N���b�v�h�~
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