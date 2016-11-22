////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ �h���b�O���h���b�v�M�������v���O����  //
//  BMP�t�@�C������̃p�^�[���v���C�o�b�N             //
//  cos�̗��p                                         //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.01.10.//
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define  MEM_SIZE 16000                                                         // �����������̃T�C�Y
#define  Fs       16000                                                         // �T���v�����O���g��
#define  CH       1                                                             // �`���l����

///////////////////////////////////////////////
//                                           //
//              ���C���֐�                   //
//                                           //
///////////////////////////////////////////////
int main(int argc, char **argv){
    //////////////////////////////////
    //                              //
    //  bmp�t�@�C���ǂݏo���p�ϐ�   //
    //                              //
    //////////////////////////////////
    FILE *f1, *f2;
    
    unsigned short tmp1;                                                        // 2�o�C�g�ϐ�
    unsigned long  tmp2;                                                        // 4�o�C�g�ϐ�
    unsigned long  DataStartPoint;                                              // 4�o�C�g�ϐ�
    unsigned long  DataGetPoint;                                                //4�o�C�g�ϐ�
    unsigned char  xinput;                                                      // ���͉摜�f�[�^
    int wd,hi,len, CL;                                                          // ���T�C�Y�C�c�T�C�Y�C�����C�J���[
    double  R, G, B;                                                            // RGB�̋P�x
    double  Rout, Gout, Bout;                                                   // RGB�v�f�̏o�͗p

    int m, n=0;                                                                 // ���W�̕ϐ�(m�sn��)

    //////////////////////////////////
    //                              //
    //  wav�t�@�C�������o���p�ϐ�   //
    //                              //
    //////////////////////////////////
    char outname[256]={0};                                                      // �o�̓t�@�C����
    unsigned short channel;                                                     // �o�̓`�����l����
    unsigned short BytePerSample;                                               // 1�T���v��������̃o�C�g��
    unsigned long  file_size;                                                   // �o�̓t�@�C���T�C�Y
    unsigned long  Fs_out;                                                      // �o�̓T���v�����O���g��
    unsigned long  BytePerSec;                                                  // 1�b������̃o�C�g��
    unsigned long  data_len;                                                    // �o�͔g�`�̃T���v����
    unsigned long  fmt_chnk    =16;                                             // fmt�`�����N�̃o�C�g��.PCM�̏ꍇ��16bit
    unsigned short BitPerSample=16;                                             // 1�T���v���̃r�b�g��
    unsigned short fmt_ID      =1;                                              // fmt ID. PCM�̏ꍇ��1
    double         Xamp[Fs+1]={0};                                              // �U���X�y�N�g��

//************************************************************************//

    ///////////////////////////////
    //                           //
    //      �M�������p�ϐ�       //
    //                           //
    ///////////////////////////////
    int      t      = 0;                                                        // �����̕ϐ�
    long int t_out  = 0;                                                        // �����̕ϐ�
    int      i;                                                                 // for���[�v�p�ϐ�
    short    output;                                                            // �Ǎ��ݕϐ��Ə��o���ϐ�
    double   y[MEM_SIZE+1]={0};                                                 // �o�̓f�[�^�i�[�p�ϐ�

//************************************************************************//


    //////////////////////////////////////
    //                                  //
    // ���s�t�@�C���̎g�����Ɋւ���x�� //
    //                                  //
    //////////////////////////////////////
    if( argc != 2 ){
        fprintf( stderr, "Usage: ./a input.bmp \n" );
        exit(-1);
    }
    if( (f1 = fopen(argv[1], "rb")) == NULL ){                                  // �o�̓t�@�C�����J���Ȃ��ꍇ�̌x��
        fprintf( stderr, "Cannot open %s\n", argv[1] );
        exit(-2);
    }

    ////////////////////////////////////////////
    //                                        //
    //      bmp�t�@�C���w�b�_�̓ǂݍ���       //
    //                                        //
    ////////////////////////////////////////////
    fseek(f1, 10, SEEK_SET);                                                    // �w�b�_�T�C�Y���Ɉړ�
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                               // 4Byte���擾
    DataStartPoint=tmp2;
    fseek(f1, 18, SEEK_SET);                                                    // ���̏��ʒu�Ɉړ�
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                               // 4Byte���擾
    wd=tmp2;
    fseek(f1, 22, SEEK_SET);                                                    // �������ʒu�Ɉړ�
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                               // 4Byte���擾
    hi=tmp2;
    fseek(f1, 34, SEEK_SET);                                                    // �f�[�^�����ʒu�Ɉړ�
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                               // 4Byte���擾
    if(DataStartPoint==54)CL=3;                                                 // �J���[�摜�Ȃ�CL=3
    else CL=1;                                                                  // �O���[�摜�Ȃ�CL=0

    printf("BMP data is\n");
    printf("Width         = %d \n", wd );
    printf("Height        = %d \n", hi);
    printf("Image Size    = %d\n",  hi*wd);

    //////////////////////////////////////
    //                                  //
    //     �o��wave�t�@�C���̏���       //
    //                                  //
    //////////////////////////////////////
    Fs_out        = Fs;                                                         // �o�̓T���v�����O���g����ݒ�
    channel       = 1;                                                          // �o�̓`���l������ݒ�
    data_len      = channel*(hi*2*wd)*2;                                        // �SByte��(1�T���v����2Byte)
    file_size     = 36+data_len;                                                // �S�̃t�@�C���T�C�Y
    BytePerSec    = Fs_out*channel*2;                                           // 1�b������̃o�C�g��
    BytePerSample = channel*2;                                                  // 1�T���v��������̃o�C�g��
    sscanf(argv[1],"%[^-^.]s", outname);                                        // ���̓t�@�C�����擾 (�g���q���O)
    strcat(outname,"_output.wav");                                              // outname="���̓t�@�C����_output.wav"
    f2=fopen(outname,"wb");                                                     // �o�̓t�@�C���I�[�v���D���݂��Ȃ��ꍇ�͍쐬�����

    //////////////////////////////////////
    //                                  //
    //     �o�̓w�b�_��񏑂�����       //
    //                                  //
    //////////////////////////////////////
    fprintf(f2, "RIFF");                                                        // "RIFF"
    fwrite(&file_size,    sizeof(unsigned long ), 1, f2);                       // �t�@�C���T�C�Y
    fprintf(f2, "WAVEfmt ");                                                    // "WAVEfmt"
    fwrite(&fmt_chnk,     sizeof(unsigned long ), 1, f2);                       // fmt_chnk=16 (�r�b�g��)
    fwrite(&fmt_ID,       sizeof(unsigned short), 1, f2);                       // fmt ID=1 (PCM)
    fwrite(&channel,      sizeof(unsigned short), 1, f2);                       // �o�̓`���l����
    fwrite(&Fs_out,       sizeof(unsigned long ), 1, f2);                       // �o�͂̃T���v�����O���g��
    fwrite(&BytePerSec,   sizeof(unsigned long ), 1, f2);                       // 1�b������̃o�C�g��
    fwrite(&BytePerSample,sizeof(unsigned short ),1, f2);                       // 1�T���v��������̃o�C�g��
    fwrite(&BitPerSample, sizeof(unsigned short ),1, f2);                       // 1�T���v���̃r�b�g��(16�r�b�g)
    fprintf(f2, "data");                                                        // "data"
    fwrite(&data_len,     sizeof(unsigned long ), 1, f2);                       // �o�̓f�[�^�̒���

    printf("\nOutput WAVE data is\n");
    printf("Channel       = %d ch\n",  channel);                                // �o�̓`���l�����̕\��
    printf("Sample rate   = %d Hz \n", Fs_out);                                 // �o�̓T���v�����O���g���̕\��
    printf("Sample number = %d\n",     data_len/channel/2);                     // �o�͐M���̒����̕\��

    ///////////////////////////////////
    //                               //
    //        ���C�����[�v           //
    //                               //
    ///////////////////////////////////
    fseek(f1, DataStartPoint, SEEK_SET);                                        // �f�[�^�擪�ʒu�Ɉړ�
    while(1){                                                                   // ���C�����[�v
        if(t>=(hi*2)){
            for(m=0;m<hi;m++){
                DataGetPoint=DataStartPoint + ( m*wd + n )*CL;                  // �f�[�^�擾�ʒu
                fseek(f1, DataGetPoint, SEEK_SET);                              // �f�[�^�擾�ʒu�Ɉړ�
                if(fread( &xinput, sizeof(unsigned char), 1,f1) < 1) break;     // �摜�ǂݍ���
                B=G=R=xinput/255.0;
                if(CL==3){                                                      // �J���[�摜��3�F�ǂݎ��
                    if(fread( &xinput, sizeof(unsigned char), 1,f1) < 1) break; //Green�ǂݍ���
                    G=xinput/255.0;
                    if(fread( &xinput, sizeof(unsigned char), 1,f1) < 1) break; //Red�ǂݍ���
                    R=xinput/255.0;
                }
                Xamp[m] = sqrt( (B+G+R)/3.0 );                                  // �X�y�N�g���O�������p���[�X�y�N�g���Ƃ݂Ȃ�
//                Xamp[m] = exp( (B+G+R)/3.0/20.0 )-1;                            // �X�y�N�g���O���������R�ΐ��Ƃ݂Ȃ�
//                Xamp[m] = pow( 10, (B+G+R)/3.0/20.0 )-1;                        // �X�y�N�g���O��������p�ΐ��Ƃ݂Ȃ�
            }
            t=0;
            n++;
            if(n>=wd)break;
        }

        y[t]=0;
        for(i=0;i<hi;i++){                                                      // �o�͉��̍쐬
            y[t] = y[t] + Xamp[i]/hi * cos(2*M_PI*i/(2*hi)*t_out);
        }
//        y[t] = atan(y[t])/(M_PI/2.0);                                           // �N���b�v�h�~
        output = y[t]*32767;                                                    // �o�͐U������
        fwrite(&output, sizeof(short), 1, f2);                                  // ���ʂ̏����o��
        if(channel==2){                                                         // �X�e���I���͂̏ꍇ
            fwrite(&output, sizeof(short), 1, f2);                              // Rch��������(=Lch)
        }
        t++;
        t_out++;                                                                // �����̍X�V
    }

    fclose(f1);                                                                 // ���̓t�@�C�������
    fclose(f2);                                                                 // �o�̓t�@�C�������
    return 0;                                                                   // ���C���֐��̏I��
}
