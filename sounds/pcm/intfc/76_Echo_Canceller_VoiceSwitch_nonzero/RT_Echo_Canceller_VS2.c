////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ ���A���^�C���M�������v���O����        //
//  �K���t�B���^�ɂ��G�R�[�L�����Z��(�{�C�X�X�C�b�`)//
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

#define MEM_SIZE 16000                                              // �����������̃T�C�Y
#define Fs       16000                                              // ���A���^�C�������̃T���v�����O���g��
#define NUM_BUF  8                                                  // �����ێ��p�o�b�t�@�̐�
#define BUF_SIZE 160                                                // �e�o�b�t�@�̃T�C�Y

#define Length   10                                                 // �쐬����f�[�^�̒���[sec]
#define ch_out   1                                                  // �`���l����
#define D        1600                                               // �X�s�[�J�|�}�C�N�Ԃ̏����x��(�g�p���ɂ�蒲���K�v)
#define N        1024                                               // �K���t�B���^����

int main(int argc, char **argv){
    /////////////////////////////////
    //                             //
    //  ���A���^�C�����o�͗p�ϐ�   //
    //                             //
    /////////////////////////////////
    char key;                                                       // �X���[�Ə������̐ؑ֗p�ϐ�
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
    int hflg= 1;                                                    // �f�ʂ��Ə������̏�ԃt���O
    int i;                                                          // for���[�v�p�ϐ�
    double s[MEM_SIZE+1]={0};                                       // ���̓f�[�^�i�[�p�ϐ�
    double y[MEM_SIZE+1]={0};                                       // �o�̓f�[�^�i�[�p�ϐ�
    
    long int l = 0;                                                 // �o�͎��Ԃ̃J�E���^
    short  input, output;                                           // �o�͏������ݗp�ϐ�
    double   d[MEM_SIZE+1]={0};                                     // ���[�b�ҐM��
    double   x, e;                                                  // �ϑ��M���C�덷�M��
    double   h[N+1]={0};                                            // �K���t�B���^�W��
    double   dh;                                                    // �K���t�B���^�o��
    double   norm, mu;                                              // �m�����ƃX�e�b�v�T�C�Y
    double   d_pow, e_pow, lambda;                                  // �o�̓p���[�C�덷�p���[�C�Y�p�W��
    double   alpha;                                                 // �����萔

    ///////////////////////////////
    //                           //
    //  �t�@�C���ǂݏ����p�ϐ�   //
    //                           //
    ///////////////////////////////
    FILE *f1, *f2;
    unsigned short tmp1;                                            // 2�o�C�g�ϐ�
    unsigned long  tmp2;                                            // 4�o�C�g�ϐ�
    int ch;                                                         // �`���l����
    int len;                                                        // ���͐M���̒���
    
    char outname[256]={0};                                          // �o�̓t�@�C����
    unsigned short channel;                                         // �o�̓`�����l����
    unsigned short BytePerSample;                                   // 1�T���v��������̃o�C�g��
    unsigned long  file_size;                                       // �o�̓t�@�C���T�C�Y
    unsigned long  Fs_out;                                          // �o�̓T���v�����O���g��
    unsigned long  BytePerSec;                                      // 1�b������̃o�C�g��
    unsigned long  data_len;                                        // �o�͔g�`�̃T���v����
    unsigned long  fmt_chnk    =16;                                 // fmt�`�����N�̃o�C�g��.PCM�̏ꍇ��16bit
    unsigned short BitPerSample=16;                                 // 1�T���v���̃r�b�g��
    unsigned short fmt_ID      =1;                                  // fmt ID. PCM�̏ꍇ��1

    ///////////////////////////////
    //                           //
    //       �ϐ��̏����ݒ�      //
    //                           //
    ///////////////////////////////

    d_pow = 0.0;                                                    // �o�̓p���[
    e_pow = 0.0;                                                    // �G�R�[�p���[
    lambda= 0.995;                                                  // �Y�p�W��
    alpha = 0.4;                                                    // �����萔

//************************************************************************//

    //////////////////////////////////////
    //                                  //
    // ���s�t�@�C���̎g�����Ɋւ���x�� //
    //                                  //
    //////////////////////////////////////
    if( argc != 2 ){                                                // �g�p���@���Ԉ�����ꍇ�̌x��
        fprintf( stderr, "Usage: ./a input.wav \n" );
        exit(-1);
    }
    if( (f1 = fopen(argv[1], "rb")) == NULL ){                      // �o�̓t�@�C�����J���Ȃ��ꍇ�̌x��
        fprintf( stderr, "Cannot open %s\n", argv[1] );
        exit(-2);
    }

    ////////////////////////////////////////////
    //                                        //
    //  ����wave�t�@�C���̃w�b�_���ǂݍ���  //
    //                                        //
    ////////////////////////////////////////////
    printf("Wave data is\n");
    fseek(f1, 22L, SEEK_SET);                                       // �`���l�����ʒu�Ɉړ�
    fread ( &tmp1, sizeof(unsigned short), 1, f1);                  // �`���l�����Ǎ� 2Byte
    ch=tmp1;                                                        // ���̓`���l�����̋L�^
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                   // �T���v�����O���g���̓Ǎ� 4Byte
    fseek(f1, 40L, SEEK_SET);                                       // �T���v�������ʒu�Ɉړ�
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                   // �f�[�^�̃T���v�����擾 4Byte
    len=tmp2/2/ch;                                                  // �����̒����̋L�^ (2Byte��1�T���v��)

    printf("Channel       = %d ch\n",  ch);                         // ���̓`���l�����̕\��
    printf("Sample rate   = %d Hz \n", Fs);                         // ���̓T���v�����O���g���̕\��
    printf("Sample number = %d\n",     len);                        // ���͐M���̒����̕\��

    //////////////////////////////////////
    //                                  //
    //     �o��wave�t�@�C���̏���       //
    //                                  //
    //////////////////////////////////////
    Fs_out        = Fs;                                             // �o�̓T���v�����O���g����ݒ�
    channel       = ch_out;                                         // �o�̓`���l������ݒ�
    data_len      = channel*Fs*Length*2;                            // �SByte��(1�T���v����2Byte)
    file_size     = 36+data_len;                                    // �S�̃t�@�C���T�C�Y
    BytePerSec    = Fs_out*channel*2;                               // 1�b������̃o�C�g��
    BytePerSample = channel*2;                                      // 1�T���v��������̃o�C�g��
    sscanf(argv[1],"%[^-^.]s", outname);                            // ���̓t�@�C�����擾 (�g���q���O)
    strcat(outname,"_output.wav");                                  // outname="���̓t�@�C����_output.wav"
    f2=fopen(outname,"wb");                                         // �o�̓t�@�C���I�[�v���D���݂��Ȃ��ꍇ�͍쐬�����

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

    mu   = 0.2;                                                     // �K���t�B���^�̃X�e�b�v�T�C�Y

    printf("[Enter] --> Rec %d[sec]\n", Length);
    getchar();
    printf("Start!\n");


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

                    if(fread( &input, sizeof(short), 1,f1) < 1){
                        fseek(f1, 44L, SEEK_SET);                   // �t�@�C���|�C���^���f�[�^�擪�ɃZ�b�g
                    }
                    d[t] = input/32768.0;                           // ���[�b�ҐM����ǂݍ���ŏo��
                    y[t] = d[t];                                    // �}�C�N����s[t]���܂߂Ȃ��ŏo��
                    d_pow= lambda*d_pow+y[t]*y[t];                  // �o�̓p���[

                    dh=0, norm=0;
                    for(i=0;i<N;i++){
                        dh= dh+ h[i] * d[(t-D-i+MEM_SIZE)%MEM_SIZE];// �K���t�B���^�o�� = ���[�b�ҐM���̋[���G�R�[
                        norm=norm+d[(t-D-i+MEM_SIZE)%MEM_SIZE]*d[(t-D-i+MEM_SIZE)%MEM_SIZE];// �m�����̌v�Z
                    }
                    e=s[t]-dh;                                      // ����덷 = �ϑ��M���|�K���t�B���^�o��
                    if(norm>0.00001){                               // �m���������l�ȏ�ŌW���X�V
                        for(i=0;i<N;i++){                           // NLMS�A���S���Y��
                            h[i] = h[i] + mu * d[(t-D-i+MEM_SIZE)%MEM_SIZE] * e/norm;
                        }
                    }
                    e_pow=lambda*e_pow+e*e;                         // ����덷�p���[
                    if(d_pow>e_pow){                                // �덷�M���̕����������Ȃ牓�[�b�҂݂̂Ɣ��f
                        e=e*alpha;                                  // �ߒ[�b�ҐM������������
                    }
                    output=e*32767;                                 // �㔼�̓G�R�[�L�����Z���o�͂��L�^
                    if(l>2*Fs/2) mu=0.0001;                         // 2�b�ȍ~�̓X�e�b�v�T�C�Y��
                    fwrite(&output, sizeof(short), 1, f2);          // �t�@�C���ւ̏�������
                    l++;                                            // ���Ԃ̍X�V
                    if(l>Length*Fs) goto Fin;                       // �w�莞�Ԃ��o�߂���ƏI��

//************************************************************************//

                    y[t]=atan(y[t])/(M_PI/2.0);                     // �N���b�v�h�~
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
    }                                                               // ���C�����[�v�I���
Fin:
    printf("\nEnd!\n");                                             // �I���̕\��
    fclose(f1);
    fclose(f2);
    return 0;
}
