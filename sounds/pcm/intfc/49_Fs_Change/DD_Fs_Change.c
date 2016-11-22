////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ �h���b�O���h���b�v�M�������v���O����  //
//  sinc��Ԃɂ��Fs�ϊ�                              //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                        2015.12.27. //
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define  MEM_SIZE 16000                                     // �����������̃T�C�Y
#define  To_Fs    32000                                      // �o�͂̃T���v�����O���g��
#define  N        64                                        // �t�B���^����

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

    int      i, L, to=0;                                    // �����Ǘ��p�ϐ�
    double   gt, Frt, sinc;                                 // �T���v�����O���g���̔�
    double   x[MEM_SIZE+1]={0};                             // �ϑ��M��
    double   h[N+1]={0};                                    // �t�B���^�W��
    double   fe;                                            // �Ւf���g��
    
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
    Fs_out        = To_Fs;                                  // �o�̓T���v�����O���g����ݒ�
    channel       = ch;                                     // �o�̓`���l������ݒ�
    data_len      = channel*(len+add_len)*2*(To_Fs/(double)Fs);// �o�̓f�[�^�̒��� = �SByte�� (1�T���v����2Byte)
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

//************************************************************************//
    ///////////////////////////////
    //                           //
    //      �ϐ��̏����ݒ�       //
    //                           //
    ///////////////////////////////

    L =25;                                                  // ��ԂɎg���M���̐�
    gt=0;                                                   // �o�͎��ԁi�����j
    Frt=(double)Fs/To_Fs;                                   // ���o�̓T���v�����O���g���̔�(in/out)
    
    ////////////////////////////////////////////
    //                                        //
    //   �A���`�G�C���A�X�t�B���^�̌W���ݒ�   //
    //                                        //
    ////////////////////////////////////////////
    if(Frt>1) fe = To_Fs/2.0/Fs;                            // �V�����T���v�����O���g���ɑ΂���ő���g��
    else      fe = Fs/2.0/To_Fs;
    for(i=-N/2;i<=N/2;i++){                                 // LPF�̐݌v
        if(i==0) h[N/2+i]=2.0*fe;
        else{
            h[N/2+i] = 2.0*fe*sin(2.0*M_PI*fe*i)/(2.0*M_PI*fe*i);
        }
        h[N/2+i]=h[N/2+i]*0.5*(1.0-cos(2.0*M_PI*(N/2+i)/N));
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

        x[t]=s[t];                                          // x[t]����͂Ƃ���
        /////////////////////////////////////////////////
        //  Fs>Fs_out �̂Ƃ��̃A���`�G�C���A�X�t�B���^ //
        /////////////////////////////////////////////////
        if(Frt>1){                                          // �o�̓T���v�����O���g���̕����������ꍇ
            s[t]=0;
            for(i=0;i<=N;i++){
                s[t]=s[t]+x[(t-i+MEM_SIZE)%MEM_SIZE]*h[i];  // LPF�o�͂�s[t]�Ƃ���
            }
        }

        while( (int)gt==(t-L+MEM_SIZE)%MEM_SIZE ){          // L�T���v���ߋ��̐M���𒆐S�ɏo�͂��쐬
            y[to]=0;
            for(i=(int)gt-L; i<(int)gt+L; i++){             // �V�����T���v�����O�_�ɂ�����l���쐬
                //////////////////////
                //  sinc�֐��̐ݒ�  //
                //////////////////////
                if( gt-i==0 )sinc=1.0;
                else sinc = sin( M_PI*(gt-i) )/( M_PI*(gt-i) );
                //////////////////////
                //   ��ԏo�͍쐬   //
                //////////////////////
                y[to] = y[to] + s[(i+MEM_SIZE)%MEM_SIZE] * sinc;// ��ԏo�͂�y[to]�Ƃ���
            }
            output=y[to]*32767;                              // �o�͂��ԏo��y[to]�Ƃ���
            /////////////////////////////////////////////////
            //  Fs<Fs_out �̂Ƃ��̃A���`�G�C���A�X�t�B���^ //
            /////////////////////////////////////////////////
            if(Frt<1){                                      // �o�̓T���v�����O���g���̕����傫���ꍇ
                x[to]=0;
                for(i=0;i<=N;i++){                          // y[to]�ɑ΂���LPF�o�͂�x[to]�Ƃ���
                    x[to]=x[to]+y[(to-i+MEM_SIZE)%MEM_SIZE]*h[i];
                }
                output=x[to]*32767;                         // �o�͂�x[t]�Ƃ���
            }
            for(i=1;i<=channel;i++){
                fwrite(&output, sizeof(short), 1, f2);      // ���ʂ̏�������
            }
            to=(to+1)%MEM_SIZE;                             // �o�͎�����1�i�߂�
            gt = gt + Frt;                                  // �o�́u���ԁv��i�߂�
            if(gt>=MEM_SIZE)gt=gt-MEM_SIZE;
        }

//************************************************************************//

        if(ch==2){                                          // �X�e���I���͂̏ꍇ
            if(fread(&input, sizeof(short), 1, f1)<1) break;// Rch�̃J���ǂݍ���
        }
        t=(t+1)%MEM_SIZE;                                   // ���� t �̍X�V
        t_out++;                                            // ���[�v�I�������̌v��
    }
    fclose(f1);                                             // ���̓t�@�C�������
    fclose(f2);                                             // �o�̓t�@�C�������
    return 0;                                               // ���C���֐��̏I��
}
