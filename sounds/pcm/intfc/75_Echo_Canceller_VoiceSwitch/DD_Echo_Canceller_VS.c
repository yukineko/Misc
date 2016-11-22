////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ �h���b�O���h���b�v�M�������v���O����  //
//  �K���t�B���^�ɂ��G�R�[�L�����Z��(�{�C�X�X�C�b�`)//
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.01.24.//
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define  MEM_SIZE 16000                                     // �����������̃T�C�Y
#define  N          128                                     // �t�B���^����

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

    int      i;                                             // for���[�v�p
    int      D;                                             // �X�s�[�J�|�}�C�N�Ԃ̏����x����
    double   d[MEM_SIZE+1]={0};                             // ���[�b�ҐM��
    double   x, e;                                          // �ϑ��M���C�덷�M��
    double   g[N+1]={0};                                    // ���m�n�̌W��
    double   h[N+1]={0};                                    // �K���t�B���^�W��
    double   dh;                                            // �K���t�B���^�o��
    double   norm, mu;                                      // �m�����ƃX�e�b�v�T�C�Y
    double   d_pow, e_pow, lambda;                          // �o�̓p���[�C�덷�p���[�C�Y�p�W��

    ///////////////////////////////
    //                           //
    //      �ϐ��̏����ݒ�       //
    //                           //
    ///////////////////////////////

    D=10;                                                   // �����x���̑傫��
    mu=0.01;                                                // �X�e�b�v�T�C�Y
    srand( (unsigned int)time( NULL ) );                    // �����̎��ݒ�
    for(i=0;i<N;i++){                                       // ���m�n�̌W���𗐐��Őݒ�
        g[i]=( (double)rand()/RAND_MAX*2-1.0 )*0.05;        // �e�W����-0.05����0.05�Őݒ�
    }
    d_pow=0.0;                                              // �o�̓p���[
    e_pow=0.0;                                              // �G�R�[�p���[
    lambda=0.995;                                           // �Y�p�W��

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

        d[t]=( (double)rand()/RAND_MAX*2-1.0 ) * 0.05;       // �m�C�Y����(���[�b�ҐM��)
        d_pow=lambda*d_pow+d[t]*d[t];                       // �o�̓p���[

        x=0,dh=0, norm=0;
        for(i=0;i<N;i++){
            x = x + g[i] * d[(t-D-i+MEM_SIZE)%MEM_SIZE];    // ���[�b�ҐM���̃G�R�[�M���쐬
            dh= dh+ h[i] * d[(t-D-i+MEM_SIZE)%MEM_SIZE];    // �K���t�B���^�o�� = ���[�b�ҐM���̋[���G�R�[
            norm=norm+d[(t-D-i+MEM_SIZE)%MEM_SIZE]*d[(t-D-i+MEM_SIZE)%MEM_SIZE];// �m�����̌v�Z
        }
        x = x + s[t];                                       // �ϑ��M��
        e = x - dh;                                         // ����덷
        if(norm>0.00001){                                   // �m���������l�ȏ�Ȃ�t�B���^�W�����X�V
            for(i=0;i<N;i++){                               // NLMS�A���S���Y���Ńt�B���^�W���X�V
                h[i] = h[i] + mu * d[(t-D-i+MEM_SIZE)%MEM_SIZE] * e/norm;
            }
        }
        e_pow=lambda*e_pow+e*e;                             // ����덷�p���[
        if(d_pow>e_pow){                                    // �덷�M���̕����������Ȃ牓�[�b�҂݂̂Ɣ��f
            e=0;                                            // �ߒ[�b�ҐM�����[���ɂ���
        }
        y[t] = e;                                           // ����덷���o�͂Ƃ���

//************************************************************************//

        //      y[t] = atan(y[t])/(M_PI/2.0);               // �N���b�v�h�~
        output = y[t]*32767;                                   // �o�͂𐮐���
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
