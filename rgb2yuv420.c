#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <arm_neon.h>

void rgb2yuv420sp(int rgb_type, int yuv_type, unsigned char * yuv420sp, unsigned char * rgb, int width, int height)   //NV12 or NV21
{
    int frameSize = width * height;

    int yIndex = 0;
    int uvIndex = frameSize;

    int index = 0;
    int i, j;
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i += 2) 
        {
            int R = rgb[(j * width + i) * rgb_type + 2];
            int G = rgb[(j * width + i) * rgb_type + 1];
            int B = rgb[(j * width + i) * rgb_type + 0];

            int R1 = rgb[(j * width + i + 1) * rgb_type + 2];
            int G1 = rgb[(j * width + i + 1) * rgb_type + 1];
            int B1 = rgb[(j * width + i + 1) * rgb_type + 0];            

            // well known RGB to YUV algorithm
            int Y = (( 66 * R + 129 * G +  25 * B + 128) >> 8) +  16;
            int U = ((-38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
            int V = ((112 * R -  94 * G -  18 * B + 128) >> 8) + 128;

            int Y1 = (( 66 * R1 + 129 * G1 +  25 * B1 + 128) >> 8) +  16;
            int U1 = ((-38 * R1 -  74 * G1 + 112 * B1 + 128) >> 8) + 128;
            int V1 = ((112 * R1 -  94 * G1 -  18 * B1 + 128) >> 8) + 128;            

            // YUV420 has a plane of Y and interleaved planes of VU each sampled by a factor of 2
            //    meaning for every 4 Y pixels there are 1 V and 1 U.  Note the sampling is every other
            //    pixel AND every other scanline.
            yuv420sp[yIndex++] = (unsigned char) ((Y < 0)? 0: ((Y > 255) ? 255 : Y));
            yuv420sp[yIndex++] = (unsigned char) ((Y1< 0)? 0: ((Y1 > 255) ? 255 : Y1));
            if(j % 2 == 0)
            {
                if(yuv_type == 0)       //NV12:UVUVUVUV
                {
                    yuv420sp[uvIndex + i] = (unsigned char) ((V + V1) / 2);
                    yuv420sp[uvIndex + i  + 1] = (unsigned char) ((U + U1) / 2);
                }
                else                //NV21：VUVUVUVU
                {
                    yuv420sp[uvIndex + i] = (unsigned char) ((U + U1) / 2);
                    yuv420sp[uvIndex + i + 1] = (unsigned char) ((V + V1) / 2);                            
                }            
            }
            else    //The second line calculates the average UV value with the first line
            {
                V = (V + V1) / 2;
                U = (U + U1) / 2;
                if(yuv_type == 0)
                {
                    yuv420sp[uvIndex + i] = (unsigned char) ((yuv420sp[uvIndex + i] + V) / 2);
                    yuv420sp[uvIndex + i + 1] = (unsigned char) ((yuv420sp[uvIndex + i + 1] + U) / 2);
                }
                else
                {
                    yuv420sp[uvIndex + i] = (unsigned char) ((yuv420sp[uvIndex + i] + U) / 2);
                    yuv420sp[uvIndex + i + 1] = (unsigned char) ((yuv420sp[uvIndex + i + 1] + V) / 2);
                }
            }
        }

        if(j % 2 != 0)
        {
            uvIndex += width;
        }
    }
}

void rgb2yuv420p(int rgb_type, int yuv_type, unsigned char * yuv420p, unsigned char * rgb, int width, int height) //YV12 or YU12/i420
{
    int frameSize = width * height;

    int yIndex = 0, vIndex, uIndex;

    if(yuv_type == 2)
    {
        vIndex = frameSize;
        uIndex = frameSize + (frameSize / 2) / 2;
    }
    else
    {
        uIndex = frameSize;
        vIndex = frameSize + (frameSize / 2) / 2;
    }
    

    int index = 0;
    int i, j;
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i += 2) 
        {
            int R = rgb[(j * width + i) * rgb_type + 2];
            int G = rgb[(j * width + i) * rgb_type + 1];
            int B = rgb[(j * width + i) * rgb_type + 0];

            int R1 = rgb[(j * width + i + 1) * rgb_type + 2];
            int G1 = rgb[(j * width + i + 1) * rgb_type + 1];
            int B1 = rgb[(j * width + i + 1) * rgb_type + 0];            

            // well known RGB to YUV algorithm
            int Y = (( 66 * R + 129 * G +  25 * B + 128) >> 8) +  16;
            int U = ((-38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
            int V = ((112 * R -  94 * G -  18 * B + 128) >> 8) + 128;

            int Y1 = (( 66 * R1 + 129 * G1 +  25 * B1 + 128) >> 8) +  16;
            int U1 = ((-38 * R1 -  74 * G1 + 112 * B1 + 128) >> 8) + 128;
            int V1 = ((112 * R1 -  94 * G1 -  18 * B1 + 128) >> 8) + 128;            

            // YUV420 has a plane of Y and interleaved planes of VU each sampled by a factor of 2
            //    meaning for every 4 Y pixels there are 1 V and 1 U.  Note the sampling is every other
            //    pixel AND every other scanline.
            yuv420p[yIndex++] = (unsigned char) ((Y < 0)? 0: ((Y > 255) ? 255 : Y));
            yuv420p[yIndex++] = (unsigned char) ((Y1< 0)? 0: ((Y1 > 255) ? 255 : Y1));
            if(j % 2 == 0)
            {
                yuv420p[vIndex + index] = (unsigned char) ((V + V1) / 2);
                yuv420p[uIndex + index] = (unsigned char) ((U + U1) / 2);        
            }
            else    //The second line calculates the average UV value with the first line
            {
                V = (V + V1) / 2;
                U = (U + U1) / 2;
                yuv420p[vIndex + index] = (unsigned char) ((yuv420p[vIndex + index] + V) / 2);
                yuv420p[uIndex + index] = (unsigned char) ((yuv420p[uIndex + index] + U) / 2);
            }
            index++;
        }
        index = 0;

        if(j % 2 != 0)
        {
            uIndex += width / 2;
            vIndex += width / 2;
        }
    }
}

void rgb2yuv420sp_NEON(int rgb_type, int yuv_type, unsigned char * __restrict__ yuv420sp, unsigned char * __restrict__ argb, int width, int height)
{
    const uint16x8_t u16_rounding = vdupq_n_u16(128);
    const int16x8_t s16_rounding = vdupq_n_s16(128);
    const int8x8_t s8_rounding = vdup_n_s8(128);
    const uint8x16_t offset = vdupq_n_u8(16);

    int frameSize = width * height;

    int yIndex = 0;
    int uvIndex = frameSize, secUVIndex = frameSize;

    int i;
    int j;

    uint8x16x3_t pixel_rgb;
    for (j = 0; j < height; j++) {
        for (i = 0; i < width >> 4; i++) {
            // Load rgb
            if(rgb_type == 3)
            {
                pixel_rgb = vld3q_u8(argb);
            }
            else
            {
                uint8x16x4_t pixel_argb = vld4q_u8(argb);
                pixel_rgb.val[0] = pixel_argb.val[0];
                pixel_rgb.val[1] = pixel_argb.val[1];
                pixel_rgb.val[2] = pixel_argb.val[2];
            }
            argb += rgb_type * 16;

            uint8x8x2_t uint8_r;
            uint8x8x2_t uint8_g;
            uint8x8x2_t uint8_b;
            uint8_r.val[0] = vget_low_u8(pixel_rgb.val[2]);
            uint8_r.val[1] = vget_high_u8(pixel_rgb.val[2]);
            uint8_g.val[0] = vget_low_u8(pixel_rgb.val[1]);
            uint8_g.val[1] = vget_high_u8(pixel_rgb.val[1]);
            uint8_b.val[0] = vget_low_u8(pixel_rgb.val[0]);
            uint8_b.val[1] = vget_high_u8(pixel_rgb.val[0]);

            // NOTE:
            // declaration may not appear after executable statement in block
            uint16x8x2_t uint16_y;

            uint8x8_t scalar = vdup_n_u8(66);
            uint8x16_t y;

            uint16_y.val[0] = vmull_u8(uint8_r.val[0], scalar);
            uint16_y.val[1] = vmull_u8(uint8_r.val[1], scalar);
            scalar = vdup_n_u8(129);
            uint16_y.val[0] = vmlal_u8(uint16_y.val[0], uint8_g.val[0], scalar);
            uint16_y.val[1] = vmlal_u8(uint16_y.val[1], uint8_g.val[1], scalar);
            scalar = vdup_n_u8(25);
            uint16_y.val[0] = vmlal_u8(uint16_y.val[0], uint8_b.val[0], scalar);
            uint16_y.val[1] = vmlal_u8(uint16_y.val[1], uint8_b.val[1], scalar);

            uint16_y.val[0] = vaddq_u16(uint16_y.val[0], u16_rounding);
            uint16_y.val[1] = vaddq_u16(uint16_y.val[1], u16_rounding);

            y = vcombine_u8(vqshrn_n_u16(uint16_y.val[0], 8), vqshrn_n_u16(uint16_y.val[1], 8));
            y = vaddq_u8(y, offset);

            vst1q_u8(yuv420sp + yIndex, y);
            yIndex += 16;

            int16x8_t u_scalar = vdupq_n_s16(-38);
            int16x8_t v_scalar = vdupq_n_s16(112);
   
            uint16x8_t tmp_val;
            
            //相邻两个u8合并为一个u16
            tmp_val = vreinterpretq_u16_u8(pixel_rgb.val[2]);
            /***
             * 1.分别取u16 高8位和低8位
             * 2.高八位右移8位与原低8位值求平均值
            ***/
            int16x8_t r = vreinterpretq_s16_u16(vrshrq_n_u16(vaddq_u16(vmovl_u8(vqshrn_n_u16(tmp_val, 8)), vmovl_u8(vmovn_u16(tmp_val))), 1));

            tmp_val = vreinterpretq_u16_u8(pixel_rgb.val[1]);
            int16x8_t g = vreinterpretq_s16_u16(vrshrq_n_u16(vaddq_u16(vmovl_u8(vqshrn_n_u16(tmp_val, 8)), vmovl_u8(vmovn_u16(tmp_val))), 1));  

            tmp_val = vreinterpretq_u16_u8(pixel_rgb.val[0]);
            int16x8_t b = vreinterpretq_s16_u16(vrshrq_n_u16(vaddq_u16(vmovl_u8(vqshrn_n_u16(tmp_val, 8)), vmovl_u8(vmovn_u16(tmp_val))), 1));                                              

            int16x8_t u;
            int16x8_t v;
            uint8x8x2_t uv;

            u = vmulq_n_s16(r, -38);
            v = vmulq_n_s16(r, 112);

            u_scalar = vdupq_n_s16(-74);
            v_scalar = vdupq_n_s16(-94);
            u = vmlaq_s16(u, g, u_scalar);
            v = vmlaq_s16(v, g, v_scalar);

            u_scalar = vdupq_n_s16(112);
            v_scalar = vdupq_n_s16(-18);
            u = vmlaq_s16(u, b, u_scalar);
            v = vmlaq_s16(v, b, v_scalar);

            u = vaddq_s16(u, s16_rounding);
            v = vaddq_s16(v, s16_rounding);

            if (j % 2 == 0) {
                if(yuv_type == 0)
                {
                    uv.val[1] = vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(u, 8), s8_rounding));
                    uv.val[0] = vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(v, 8), s8_rounding));
                }
                else
                {
                    uv.val[0] = vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(u, 8), s8_rounding));
                    uv.val[1] = vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(v, 8), s8_rounding));
                }

                vst2_u8(yuv420sp + uvIndex, uv);

                uvIndex += 2 * 8;
            }
            else
            {
                uint8x8x2_t before_uv = vld2_u8(yuv420sp + secUVIndex);
                //uint16x8_t pre_u = vmovl_u8(before_uv.val[1]);
                //uint16x8_t pre_v = vmovl_u8(before_uv.val[0]);
                //uint16x8_t cur_u = vmovl_u8(vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(u, 8), s8_rounding)));
                //uint16x8_t cur_v = vmovl_u8(vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(v, 8), s8_rounding)));                
                if(yuv_type == 0)
                {
                    uv.val[1] = vmovn_u16(vrshrq_n_u16(vaddq_u16(vmovl_u8(vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(u, 8), s8_rounding))), vmovl_u8(before_uv.val[1])), 1));
                    uv.val[0] = vmovn_u16(vrshrq_n_u16(vaddq_u16(vmovl_u8(vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(v, 8), s8_rounding))), vmovl_u8(before_uv.val[0])), 1));                    
                }
                else
                {
                    uv.val[0] = vmovn_u16(vrshrq_n_u16(vaddq_u16(vmovl_u8(vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(u, 8), s8_rounding))), vmovl_u8(before_uv.val[0])), 1));
                    uv.val[1] = vmovn_u16(vrshrq_n_u16(vaddq_u16(vmovl_u8(vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(v, 8), s8_rounding))), vmovl_u8(before_uv.val[1])), 1));  
                }

                vst2_u8(yuv420sp + secUVIndex, uv);

                secUVIndex += 2 * 8;
            }       

        }

        // Handle leftovers
        for (i = ((width >> 4) << 4); i < width; i++) {
            uint8_t R = argb[2];
            uint8_t G = argb[1];
            uint8_t B = argb[0];
            argb += 4;

            // well known RGB to YUV algorithm
            uint8_t Y = (( 66 * R + 129 * G +  25 * B + 128) >> 8) +  16;
            uint8_t U = ((-38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
            uint8_t V = ((112 * R -  94 * G -  18 * B + 128) >> 8) + 128;

            // NV21 has a plane of Y and interleaved planes of VU each sampled by a factor of 2
            //    meaning for every 4 Y pixels there are 1 V and 1 U.  Note the sampling is every other
            //    pixel AND every other scanline.
            yuv420sp[yIndex++] = Y;
            if (j % 2 == 0 && i % 2 == 0) {
                yuv420sp[uvIndex++] = V;
                yuv420sp[uvIndex++] = U;
            }
        }        

    }
}

void rgb2yuv420p_NEON(int rgb_type, int yuv_type, unsigned char * __restrict__ yuv420p, unsigned char * __restrict__ argb, int width, int height)
{
    const uint16x8_t u16_rounding = vdupq_n_u16(128);
    const int16x8_t s16_rounding = vdupq_n_s16(128);
    const int8x8_t s8_rounding = vdup_n_s8(128);
    const uint8x16_t offset = vdupq_n_u8(16);

    int frameSize = width * height;

    int yIndex = 0, uIndex, vIndex, tmpIndex = 0;

    if(yuv_type == 2)
    {
        vIndex = frameSize;
        uIndex = frameSize + (frameSize / 2) / 2;
    }
    else
    {
        uIndex = frameSize;
        vIndex = frameSize + (frameSize / 2) / 2;
    }

    int i;
    int j;

    uint8x16x3_t pixel_rgb;
    for (j = 0; j < height; j++) {
        for (i = 0; i < width >> 4; i++) 
        {
            // Load rgb
            if(rgb_type == 3)
            {
                pixel_rgb = vld3q_u8(argb);
            }
            else
            {
                uint8x16x4_t pixel_argb = vld4q_u8(argb);
                pixel_rgb.val[0] = pixel_argb.val[0];
                pixel_rgb.val[1] = pixel_argb.val[1];
                pixel_rgb.val[2] = pixel_argb.val[2];
            }
            argb += rgb_type * 16;

            uint8x8x2_t uint8_r;
            uint8x8x2_t uint8_g;
            uint8x8x2_t uint8_b;
            uint8_r.val[0] = vget_low_u8(pixel_rgb.val[2]);
            uint8_r.val[1] = vget_high_u8(pixel_rgb.val[2]);
            uint8_g.val[0] = vget_low_u8(pixel_rgb.val[1]);
            uint8_g.val[1] = vget_high_u8(pixel_rgb.val[1]);
            uint8_b.val[0] = vget_low_u8(pixel_rgb.val[0]);
            uint8_b.val[1] = vget_high_u8(pixel_rgb.val[0]);

            // NOTE:
            // declaration may not appear after executable statement in block
            uint16x8x2_t uint16_y;

            uint8x8_t scalar = vdup_n_u8(66);
            uint8x16_t y;

            uint16_y.val[0] = vmull_u8(uint8_r.val[0], scalar);
            uint16_y.val[1] = vmull_u8(uint8_r.val[1], scalar);
            scalar = vdup_n_u8(129);
            uint16_y.val[0] = vmlal_u8(uint16_y.val[0], uint8_g.val[0], scalar);
            uint16_y.val[1] = vmlal_u8(uint16_y.val[1], uint8_g.val[1], scalar);
            scalar = vdup_n_u8(25);
            uint16_y.val[0] = vmlal_u8(uint16_y.val[0], uint8_b.val[0], scalar);
            uint16_y.val[1] = vmlal_u8(uint16_y.val[1], uint8_b.val[1], scalar);

            uint16_y.val[0] = vaddq_u16(uint16_y.val[0], u16_rounding);
            uint16_y.val[1] = vaddq_u16(uint16_y.val[1], u16_rounding);

            y = vcombine_u8(vqshrn_n_u16(uint16_y.val[0], 8), vqshrn_n_u16(uint16_y.val[1], 8));
            y = vaddq_u8(y, offset);

            vst1q_u8(yuv420p + yIndex, y);
            yIndex += 16;

            int16x8_t u_scalar = vdupq_n_s16(-38);
            int16x8_t v_scalar = vdupq_n_s16(112);
   
            uint16x8_t tmp_val;
            
            //相邻两个u8合并为一个u16
            tmp_val = vreinterpretq_u16_u8(pixel_rgb.val[2]);
            /***
             * 1.分别取u16 高8位和低8位
             * 2.高八位右移8位与原低8位值求平均值
            ***/
            int16x8_t r = vreinterpretq_s16_u16(vrshrq_n_u16(vaddq_u16(vmovl_u8(vqshrn_n_u16(tmp_val, 8)), vmovl_u8(vmovn_u16(tmp_val))), 1));

            tmp_val = vreinterpretq_u16_u8(pixel_rgb.val[1]);
            int16x8_t g = vreinterpretq_s16_u16(vrshrq_n_u16(vaddq_u16(vmovl_u8(vqshrn_n_u16(tmp_val, 8)), vmovl_u8(vmovn_u16(tmp_val))), 1));  

            tmp_val = vreinterpretq_u16_u8(pixel_rgb.val[0]);
            int16x8_t b = vreinterpretq_s16_u16(vrshrq_n_u16(vaddq_u16(vmovl_u8(vqshrn_n_u16(tmp_val, 8)), vmovl_u8(vmovn_u16(tmp_val))), 1));                                              

            int16x8_t u;
            int16x8_t v;

            u = vmulq_n_s16(r, -38);
            v = vmulq_n_s16(r, 112);

            u_scalar = vdupq_n_s16(-74);
            v_scalar = vdupq_n_s16(-94);
            u = vmlaq_s16(u, g, u_scalar);
            v = vmlaq_s16(v, g, v_scalar);

            u_scalar = vdupq_n_s16(112);
            v_scalar = vdupq_n_s16(-18);
            u = vmlaq_s16(u, b, u_scalar);
            v = vmlaq_s16(v, b, v_scalar);

            u = vaddq_s16(u, s16_rounding);
            v = vaddq_s16(v, s16_rounding);

            if (j % 2 == 0) {
                vst1_u8(yuv420p + vIndex + tmpIndex, vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(v, 8), s8_rounding)));
                vst1_u8(yuv420p + uIndex + tmpIndex, vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(u, 8), s8_rounding)));
            }
            else
            {
                //uint8x8_t before_u = vld1_u8(yuv420sp + uIndex + tmpIndex);
                //uint8x8_t before_v = vld1_u8(yuv420sp + vIndex + tmpIndex);
                //uint16x8_t pre_u = vmovl_u8(before_uv.val[1]);
                //uint16x8_t pre_v = vmovl_u8(before_uv.val[0]);
                //uint16x8_t cur_u = vmovl_u8(vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(u, 8), s8_rounding)));
                //uint16x8_t cur_v = vmovl_u8(vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(v, 8), s8_rounding)));
                vst1_u8(yuv420p + vIndex + tmpIndex, vmovn_u16(vrshrq_n_u16(vaddq_u16(vmovl_u8(vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(v, 8), s8_rounding))), vmovl_u8(vld1_u8(yuv420p + vIndex + tmpIndex))), 1)));
                vst1_u8(yuv420p + uIndex + tmpIndex, vmovn_u16(vrshrq_n_u16(vaddq_u16(vmovl_u8(vreinterpret_u8_s8(vadd_s8(vqshrn_n_s16(u, 8), s8_rounding))), vmovl_u8(vld1_u8(yuv420p + uIndex + tmpIndex))), 1)));
            }
            tmpIndex += 8;
        }
        tmpIndex = 0;

        if(j % 2 != 0)
        {
            uIndex += width / 2;
            vIndex += width / 2;
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 5) {
        printf("<prog> <argb_type> <yuv_type> <rgb input file> <yuv output file>\n");
        printf("<rgb_type> 3:rgb 4:argb\n");
        printf("<yuv_type> 0:NV12 1:NV21 2:YV12 3:YU12/i420\n");
        return 0;
    }

    FILE* fp_in;
    if ((fp_in = fopen(argv[3], "r")) == NULL) {
        printf("Error open input file!\n");
        exit(1);
    }

    int width = 1024, height = 768;

    int rgb_type = atoi(argv[1]);
    int yuv_type = atoi(argv[2]);
    int channel_num, time = 0;
    if(rgb_type == 3)
    {
        channel_num = width * height * 3;
    }
    else
    {
        channel_num = width * height * 4;
    }    

    int channel_datasize = channel_num * sizeof(unsigned char);
    int channel_num_yuv = width * height * 6 / 4;
    int channel_datasize_yuv = channel_num_yuv * sizeof(unsigned char);
    unsigned char *rgb = (unsigned char *) malloc(channel_datasize);
    unsigned char *yuv = (unsigned char *) malloc(channel_datasize_yuv);

    if (fread(rgb, sizeof(unsigned char), channel_num, fp_in) == EOF) {
        printf("fread error!\n");
        exit(0);
    }

    fclose(fp_in);


    // Compute
    struct timespec start, end;
    double total_time;
	printf("start to convert\n");
    clock_gettime(CLOCK_REALTIME, &start);
    if(yuv_type <= 1)
    {
        rgb2yuv420sp_NEON(rgb_type, yuv_type, yuv, rgb, width, height);  //NV12 or NV21
    }
    else
    {
        rgb2yuv420p_NEON(rgb_type, yuv_type, yuv, rgb, width, height);   //YV12 or YU12/i420
    } 
    clock_gettime(CLOCK_REALTIME,&end);
    total_time = (double)(end.tv_sec - start.tv_sec) * 1000 + (double)(end.tv_nsec - start.tv_nsec) / (double)1000000L;
    printf("RGB2YUV_NEON: %f ms\n", total_time);


    FILE* fp_out;
    if ((fp_out = fopen(argv[4], "w")) == NULL) {
        printf("Error open output file!\n");
        exit(1);
    }
    if (fwrite(yuv, sizeof(unsigned char), channel_num_yuv, fp_out) != channel_datasize_yuv) {
        printf("fwrite error!\n");
        exit(0);
    }
    fclose(fp_out);

    free(rgb);
    free(yuv);

    return 0;
}