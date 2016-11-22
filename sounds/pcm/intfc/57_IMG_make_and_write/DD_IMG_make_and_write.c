////////////////////////////////////////////////////////
//                                                    //
//  Borland C++ �h���b�O���h���b�v�M�������v���O����  //
//  BMP�t�@�C���̍쐬�Ə����o��                       //
//                                                    //
//                        Presented by Arata KAWAMURA //
//                                         2016.01.06.//
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(){
    ///////////////////////////////
    //                           //
    //  �t�@�C�������o���p�ϐ�   //
    //                           //
    ///////////////////////////////
    FILE *f2;
    
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
    unsigned char  Rout, Gout, Bout;                                    // RGB�v�f�̏o�͗p
    int m, n;                                                           // ���W�̕ϐ�(m�sn��)

    //////////////////////////////////////////
    //                                      //
    //      �����o��bmp�t�@�C���̐ݒ�      //
    //                                      //
    //////////////////////////////////////////

    width          = 256;                                               // �摜�̕�
    height         = 256;                                               // �摜�̍���
    if(width%4  != 0)width = width  - width%4;                          // ����4�̔{���ɒ���
    image_size     = width * height * 3;                                // �摜�T�C�Y(3�F)
    file_size      = image_size + FileHeaderSize;                       // �S�̃t�@�C���T�C�Y(byte)

    //////////////////////////////////////
    //                                  //
    //      �w�b�_�[��������            //
    //                                  //
    //////////////////////////////////////
    f2=fopen("output.bmp","wb");                                        // �o�̓t�@�C���I�[�v��

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

    ///////////////////////////////////
    //                               //
    //        ���C�����[�v           //
    //                               //
    ///////////////////////////////////
//**********************************************************************//
    for(m=0;m<(int)height;m++){                                         // �s�̍X�V
        for(n=0;n<(int)width;n++){                                      // ��̍X�V
            Bout = m;                                                   // �F�ŉ��i���P�x  0�C�ŏ�i���P�xMAX
            Gout = height - m;                                          // �΁F�ŉ��i���P�xMAX�C�ŏ�i���P�x0
            Rout = n;                                                   // �ԁF�ō��񂪋P�x  0, �ŉE�񂪋P�xMAX

            fwrite(&Bout, sizeof(unsigned char), 1, f2);                //���ʂ̏�������
            fwrite(&Gout, sizeof(unsigned char), 1, f2);                //���ʂ̏�������
            fwrite(&Rout, sizeof(unsigned char), 1, f2);                //���ʂ̏�������
        }
    }
//**********************************************************************//
    fclose(f2);                                                         // �o�̓t�@�C�������
    return 0;                                                           // ���C���֐��̏I��
}
