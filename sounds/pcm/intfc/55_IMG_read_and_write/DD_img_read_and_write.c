////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ �h���b�O���h���b�v�M�������v���O����  //
//  BMP�t�@�C���ǂݍ���(�O���[��)�Ə����o��(�J���[)   //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.01.03.//
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char **argv){
    ///////////////////////////////
    //                           //
    //  �t�@�C���ǂݏ����p�ϐ�   //
    //                           //
    ///////////////////////////////
    FILE *f1, *f2;
    unsigned short tmp1;                                                // 2�o�C�g�ϐ�
    unsigned long  tmp2;                                                // 4�o�C�g�ϐ�
    unsigned long  DataStartPoint;                                      // 4�o�C�g�ϐ�
    unsigned char  xinput;                                              // ���͉摜�f�[�^
    int wd,hi,len, CL;                                                  // ���T�C�Y�C�c�T�C�Y�C�����C�J���[
    
    char outname[256]={0};                                              // �o�̓t�@�C����
    unsigned long  file_size;                                           // �t�@�C���T�C�Y 4Byte
    unsigned long  width;                                               // �摜�̕�
    unsigned long  height;                                              // �摜�̍���
    unsigned long  zero  = 0;                                           // '0'�������ݗp
    unsigned long  one   = 1;                                           // '1'�������ݗp
	unsigned long  FileHeaderSize=54;                                   // �S�w�b�_�[�T�C�Y(�J���[�摜�p)
    unsigned long  InfoHeaderSize=40;                                   // ���w�b�_�[�T�C�Y
    unsigned long  data_len;                                            // �g�`�f�[�^�̃T���v����
    unsigned long  color = 24;                                          // 1��f������̐F��
    unsigned long  image_size;                                          // �o�͉摜�T�C�Y

    unsigned char  R, G, B;                                             // RGB�̋P�x
    unsigned char  Rout, Gout, Bout;                                    // RGB�v�f�̏o�͗p

    int m, n;                                                           // ���W�̕ϐ�(m�sn��)


    //////////////////////////////////////
    //                                  //
    // ���s�t�@�C���̎g�����Ɋւ���x�� //
    //                                  //
    //////////////////////////////////////
    if( argc != 2 ){                                                    // �g�p���@���Ԉ�����ꍇ�̌x��
        fprintf( stderr, "Usage: ./a input.wav \n" );
        exit(-1);
    }
    if( (f1 = fopen(argv[1], "rb")) == NULL ){                          // �o�̓t�@�C�����J���Ȃ��ꍇ�̌x��
        fprintf( stderr, "Cannot open %s\n", argv[1] );
        exit(-2);
    }

    ////////////////////////////////////////////
    //                                        //
    //          bmp�t�@�C���̓ǂݍ���         //
    //                                        //
    ////////////////////////////////////////////
    fseek(f1, 10, SEEK_SET);                                            // �w�b�_�T�C�Y���Ɉړ�
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                       // 4Byte���擾
    DataStartPoint=tmp2;
    fseek(f1, 18, SEEK_SET);                                            // ���̏��ʒu�Ɉړ�
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                       // 4Byte���擾
    wd=tmp2;
    fseek(f1, 22, SEEK_SET);                                            // �������ʒu�Ɉړ�
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                       // 4Byte���擾
    hi=tmp2;
    fseek(f1, 34, SEEK_SET);                                            // �f�[�^�����ʒu�Ɉړ�
    fread ( &tmp2, sizeof(unsigned long), 1, f1);                       // 4Byte���擾
    len=tmp2;
    if(DataStartPoint==54)CL=3;                                         // �J���[�摜�Ȃ�CL=3
    else CL=1;                                                          // �O���[�摜�Ȃ�CL=0

    printf("BMP data is\n");
    printf("Width         = %d \n", wd );
    printf("Height        = %d \n", hi);
    printf("Image Size    = %d\n",  len/CL);

    //////////////////////////////////////////
    //                                      //
    //      �����o��bmp�t�@�C���̐ݒ�      //
    //                                      //
    //////////////////////////////////////////

    width          = wd;                                                // �摜�̕�
    height         = hi;                                                // �摜�̍���
    image_size     = width * height * 3;                                // �摜�T�C�Y(3�F)
    file_size      = image_size + FileHeaderSize;                       // �S�̃t�@�C���T�C�Y(byte)

    //////////////////////////////////////
    //                                  //
    //      �w�b�_�[��������            //
    //                                  //
    //////////////////////////////////////
    sscanf(argv[1],"%[^-^.]s", outname);                                // ���̓t�@�C�����擾(�g���q���O)
    strcat(outname,"_output.bmp");                                      // ������A��
    f2=fopen(outname,"wb");                                             // �o�̓t�@�C���I�[�v��

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

    m=0;                                                                // �s�ԍ�
    n=0;                                                                // ��ԍ�

    ///////////////////////////////////
    //                               //
    //        ���C�����[�v           //
    //                               //
    ///////////////////////////////////
    fseek(f1, DataStartPoint, SEEK_SET);                                // �f�[�^�擪�ʒu�Ɉړ�
    while(1){                                                           // ���C�����[�v
        if(fread( &xinput, sizeof(unsigned char), 1,f1) < 1) break;     // �摜�ǂݍ���
        B=G=R=xinput;
        if(CL==3){                                                      // �J���[�摜��3�F�ǂݎ��
            if(fread( &xinput, sizeof(unsigned char), 1,f1) < 1) break; //Green�ǂݍ���
            G=xinput;
            if(fread( &xinput, sizeof(unsigned char), 1,f1) < 1) break; //Red�ǂݍ���
            R=xinput;
        }


//************************************************************************//

        ////////////////////////////////////////////////////
        //                                                //
        //              Signal Processing                 //
        //                                                //
        //           RGB���͂���RGB�o�͂�����           //
        //                                                //
        ////////////////////////////////////////////////////

        Bout = B;                                                       // �� �ǂݎ��
        Gout = G;                                                       // �� �ǂݎ��
        Rout = R;                                                       // �� �ǂݎ��

//************************************************************************//

        fwrite(&Bout, sizeof(unsigned char), 1, f2);                    //��Z���ʂ̏�������
        fwrite(&Gout, sizeof(unsigned char), 1, f2);                    //��Z���ʂ̏�������
        fwrite(&Rout, sizeof(unsigned char), 1, f2);                    //��Z���ʂ̏�������

        n=(n+1)%width;                                                  //�摜�f�[�^�ʒu�X�V
        if(n==0){	
            m=(m+1)%height;                                             //�摜�f�[�^�ʒu�X�V
        }
    }
    fclose(f1);                                                         // ���̓t�@�C�������
    fclose(f2);                                                         // �o�̓t�@�C�������
    return 0;                                                           // ���C���֐��̏I��
}
