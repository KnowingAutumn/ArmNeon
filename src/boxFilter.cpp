#include "boxFilter.h"
#include <chrono>
#include <random>
#include <iostream>

#if __ARM_NEON
#include <arm_neon.h>
#endif // __ARM_NEON

void BoxFilterOrigin(float *Src, float *Dest, int Width, int Height, int Radius){
    for(int Y = 0; Y < Height; Y++){
        for(int X = 0; X < Width; X++){
            int ST_Y = Y - Radius;
            if(ST_Y < 0) ST_Y = 0;
            int EN_Y = Y + Radius;
            if(EN_Y > Height-1) EN_Y = Height-1;
            int ST_X = X - Radius;
            if(ST_X < 0) ST_X = 0;
            int EN_X = X + Radius;
            if(EN_X > Width-1) EN_X = Width-1;
            float sum = 0;
            for(int ty = ST_Y; ty <= EN_Y; ty++){
                for(int tx = ST_X; tx <= EN_X; tx++){
                    sum += Src[ty * Width + tx];
                }
            }
            Dest[Y * Width + X] = sum;
        }
    }
}


void BoxFilterOpenCV(float *Src, float *Dest, int Width, int Height, int Radius, vector <float>&cache){
    float *cachePtr = &(cache[0]);
    // chuizhi
    for(int Y = 0; Y < Height; Y++){
        for(int X = 0; X < Width; X++){
            int ST_X = X - Radius;
            if(ST_X < 0) ST_X = 0;
            int EN_X = X + Radius;
            if(EN_X > Width-1) EN_X = Width-1;
            float sum = 0;
            for(int tx = ST_X; tx <= EN_X; tx++){
                sum += Src[Y * Width + tx];
            }
            cachePtr[Y * Width + X] = sum;
        }
    }

    //shuiping
    for(int Y = 0; Y < Height; Y++){
        int ST_Y = Y - Radius;
        if(ST_Y < 0) ST_Y = 0;
        int EN_Y = Y + Radius;
        if(EN_Y > Height-1) EN_Y = Height-1;
        for(int X = 0; X < Width; X++){
            float sum = 0;
            for(int ty = ST_Y; ty <= EN_Y; ty++){
                sum += cachePtr[ty * Width + X];
            }
            Dest[Y * Width + X] = sum;
        }
    }
}

 void BoxFilterOpenCV2(float *Src, float *Dest, int Width, int Height, int Radius, vector<float>&cache){
     float *cachePtr = &(cache[0]);
     //chuizhi
     for(int Y = 0; Y < Height; Y++){
         int Stride = Y * Width;
         //head
         float sum = 0;
         for(int X = 0; X < Radius; X++){
             sum += Src[Stride + X];
         }
         for(int X = 0; X <= Radius; X++){
             sum += Src[Stride + X + Radius];
             cachePtr[Stride + X] = sum;
         }
        //middle
        for(int X = Radius+1; X <= Width-1-Radius; X++){
            sum += Src[Stride + X + Radius];
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
        //tail
        for(int X = Width - Radius; X < Width; X++){
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
     }
    //shuipin

    for(int X = 0; X < Width; X++){
        //head
        float sum = 0;
        for(int Y = 0; Y < Radius; Y++){
            sum += cachePtr[Y * Width + X];
        }
        for(int Y = 0; Y <= Radius; Y++){
            sum += cachePtr[Y * Width + Radius * Width + X];
            Dest[Y * Width + X] = sum;
        }
        //middle
        for(int Y = Radius+1; Y <= Height-1-Radius; Y++){
            sum += cachePtr[Y * Width + Radius * Width + X];
            sum -= cachePtr[Y * Width - (Radius + 1) * Width + X];
            Dest[Y * Width + X] = sum;
        }
        //tail
        for(int Y = Height-Radius; Y < Height; Y++){
            sum -= cachePtr[Y * Width - (Radius + 1) * Width + X];
            Dest[Y * Width + X] = sum;
        }
    }    
 }

void BoxFilterCache(float *Src, float *Dest, int Width, int Height, int Radius, vector<float>&cache){
    float *cachePtr = &(cache[0]);
     //chuizhi
    for(int Y = 0; Y < Height; Y++){
        int Stride = Y * Width;
        //head
        float sum = 0;
        for(int X = 0; X < Radius; X++){
            sum += Src[Stride + X];
        }
        for(int X = 0; X <= Radius; X++){
            sum += Src[Stride + X + Radius];
            cachePtr[Stride + X] = sum;
        }
        //middle
        for(int X = Radius+1; X <= Width-1-Radius; X++){
            sum += Src[Stride + X + Radius];
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
        //tail
        for(int X = Width - Radius; X < Width; X++){
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
    }

    vector <float> colsum;
    colsum.resize(Width); 
    float *colsumPtr = &(colsum[0]);
    for(int X = 0;  X < Width; X++){
        colsumPtr[X] = 0;
    }
    //shuipin
    for(int Y = 0; Y < Radius; Y++){
        int Stride = Y * Width;
        for(int X = 0; X < Width; X++){
            colsumPtr[X] += colsumPtr[Stride + X];
        }
    }
    //head
    for(int Y = 0; Y <= Radius; Y++){
        int Stride = Y * Width;
        for(int X = 0; X < Width; X++){
            colsumPtr[X] += cachePtr[(Y + Radius) * Width + X];
            Dest[Stride + X] = colsumPtr[X];
        }
    }
    //middle
    for(int Y = Radius+1; Y <= Height-1-Radius; Y++){
        int Stride = Y * Width;
        for(int X = 0; X < Width; X++){
            colsumPtr[X] += cachePtr[(Y + Radius) * Width + X];
            colsumPtr[X] -= cachePtr[(Y - Radius - 1) * Width + X];
            Dest[Stride + X] = colsumPtr[X];
        }
    }
    //tail
    for(int Y = Height-Radius; Y < Height; Y++){
        int Stride = Y * Width;
        for(int X = 0; X < Width; X++){
            colsumPtr[X] -= cachePtr[(Y - Radius - 1) * Width + X];
            Dest[Stride + X] = colsumPtr[X];
        }
    }
}

void BoxFilterNeonIntrinsics(float *Src, float *Dest, int Width, int Height, int Radius, vector<float>&cache){
    float *cachePtr = &(cache[0]);
     //chuizhi
    for(int Y = 0; Y < Height; Y++){
        int Stride = Y * Width;
        //head
        float sum = 0;
        for(int X = 0; X < Radius; X++){
            sum += Src[Stride + X];
        }
        for(int X = 0; X <= Radius; X++){
            sum += Src[Stride + X + Radius];
            cachePtr[Stride + X] = sum;
        }
        //middle
        for(int X = Radius+1; X <= Width-1-Radius; X++){
            sum += Src[Stride + X + Radius];
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
        //tail
        for(int X = Width - Radius; X < Width; X++){
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
    }

    // shuipin
    vector <float> colsum;
    colsum.resize(Width); 
    float *colsumPtr = &(colsum[0]);
    for(int X = 0;  X < Width; X++){
        colsumPtr[X] = 0;
    }

    int Block = Width >> 2;
    int Remain = Width - (Block << 2);

    //Origin
    // for(int Y = 0; Y < Radius; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] += colsumPtr[Stride + X];
    //     }
    // }
    for(int Y = 0; Y < Radius; Y++){
        int Stride = Y * Width;
        float* tmpColSumPtr = colsumPtr;
        float* tmpCachePtr = cachePtr;

        int n = Block;
        int re = Remain;

        for(; n > 0; n--){
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t cache = vld1q_f32(tmpCachePtr);
            float32x4_t sum = vaddq_f32(colsum, cache);

            vst1q_f32(tmpColSumPtr, sum); 

            tmpColSumPtr += 4;
            tmpCachePtr += 4;
        }

        for (; re > 0; re--) {
            *tmpColSumPtr += *tmpCachePtr;
            tmpColSumPtr ++;
            tmpCachePtr ++;
        }
    }

    //head
    //Origin
    // for(int Y = 0; Y <= Radius; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] += cachePtr[(Y + Radius) * Width + X];
    //         Dest[Stride + X] = colsumPtr[X];
    //     }
    // }
    for(int Y = 0; Y <= Radius; Y++){
        int Stride = Y * Width;
        float* addPtr = cachePtr + (Y + Radius) * Width;
        float* destPtr = Dest + Stride;

        float *tmpDestPtr = destPtr;
        float *tmpColSumPtr = colsumPtr;
        float *tmpaddPtr = addPtr;

        int n = Block;
        int re = Remain;
        for(; n > 0; n--){
            float32x4_t add = vld1q_f32(tmpaddPtr);
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t sum = vaddq_f32(colsum, add);
            
            vst1q_f32(tmpColSumPtr, sum);
            vst1q_f32(tmpDestPtr, sum);

            tmpaddPtr += 4;
            tmpColSumPtr += 4;
            tmpDestPtr += 4;
        }

        for (; re > 0; re--) {
            *tmpColSumPtr += *tmpaddPtr;
            *tmpDestPtr = *tmpColSumPtr;
            tmpaddPtr++;
            tmpColSumPtr++;
            tmpDestPtr++;
        }
    }

    //middle
    //Origin
    // for(int Y = Radius+1; Y <= Height-1-Radius; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] += cachePtr[(Y + Radius) * Width + X];
    //         colsumPtr[X] -= cachePtr[(Y - Radius - 1) * Width + X];
    //         Dest[Stride + X] = colsumPtr[X];
    //     }
    // }
    for(int Y = Radius+1; Y <= Height-1-Radius; Y++){
        int Stride = Y * Width;
        float *addPtr = cachePtr + (Y + Radius) * Width;
        float *subPtr = cachePtr + (Y - Radius - 1) * Width;
        float *destPtr = Dest + Stride;

        float *tmpDestPtr = destPtr;
        float *tmpaddPtr = addPtr;
        float *tmpsubPtr = subPtr;
        float *tmpColSumPtr = colsumPtr;

        int n = Block;
        int re = Remain;
        
        for(; n > 0; n--){
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t add = vld1q_f32(tmpaddPtr);
            float32x4_t sub = vld1q_f32(tmpsubPtr);
            float32x4_t sum = vaddq_f32(colsum, add);
            sum = vsubq_f32(sum, sub);
            
            vst1q_f32(tmpColSumPtr, sum);
            vst1q_f32(tmpDestPtr, sum);

            tmpaddPtr += 4;
            tmpsubPtr += 4;
            tmpColSumPtr += 4;
            tmpDestPtr += 4;
        }
        for(;re > 0; re--){
            *tmpColSumPtr += *tmpaddPtr;
            *tmpColSumPtr -= *tmpsubPtr;
            *tmpDestPtr = *tmpColSumPtr;
            tmpaddPtr++;
            tmpColSumPtr++;
            tmpDestPtr++;
            tmpsubPtr++;
        }
    }
    //tail
    //Origin
    //tail
    // for(int Y = Height-Radius; Y < Height; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] -= cachePtr[(Y - Radius - 1) * Width + X];
    //         Dest[Stride + X] = colsumPtr[X];
    //     }
    // }
    for(int Y = Height - Radius; Y < Height; Y++){
        int Stride = Y * Width;
        float *subPtr = cachePtr + (Y - Radius - 1) * Width;
        float *destPtr = Dest + Stride;

        float *tmpDestPtr = destPtr;
        float *tmpsubPtr = subPtr;
        float *tmpColSumPtr = colsumPtr;

        int n = Block;
        int re = Remain;

        for(; n > 0; n--){
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t sub = vld1q_f32(tmpsubPtr);
            float32x4_t sum = vsubq_f32(colsum, sub);
            
            vst1q_f32(tmpColSumPtr, sum);
            vst1q_f32(tmpDestPtr, sum);

            tmpsubPtr += 4;
            tmpColSumPtr += 4;
            tmpDestPtr += 4;
        } 

        for(;re > 0; re--){
            *tmpColSumPtr -= *tmpsubPtr;
            *tmpDestPtr = *tmpColSumPtr;
            tmpColSumPtr++;
            tmpDestPtr++;
            tmpsubPtr++;
        }       
    }
}


void BoxFilterNeonAssembly(float *Src, float *Dest, int Width, int Height, int Radius, vector<float>&cache){
    float *cachePtr = &(cache[0]);
     //chuizhi
    for(int Y = 0; Y < Height; Y++){
        int Stride = Y * Width;
        //head
        float sum = 0;
        for(int X = 0; X < Radius; X++){
            sum += Src[Stride + X];
        }
        for(int X = 0; X <= Radius; X++){
            sum += Src[Stride + X + Radius];
            cachePtr[Stride + X] = sum;
        }
        //middle
        for(int X = Radius+1; X <= Width-1-Radius; X++){
            sum += Src[Stride + X + Radius];
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
        //tail
        for(int X = Width - Radius; X < Width; X++){
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
    }

    // shuipin
    vector <float> colsum;
    colsum.resize(Width); 
    float *colsumPtr = &(colsum[0]);
    for(int X = 0;  X < Width; X++){
        colsumPtr[X] = 0;
    }

    int Block = Width >> 2;
    int Remain = Width - (Block << 2);

    //Origin
    // for(int Y = 0; Y < Radius; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] += colsumPtr[Stride + X];
    //     }
    // }
    for(int Y = 0; Y < Radius; Y++){
        int Stride = Y * Width;
        float* tmpColSumPtr = colsumPtr;
        float* tmpCachePtr = cachePtr;

        int n = Block;
        int re = Remain;

        for(; n > 0; n--){
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t cache = vld1q_f32(tmpCachePtr);
            float32x4_t sum = vaddq_f32(colsum, cache);

            vst1q_f32(tmpColSumPtr, sum); 

            tmpColSumPtr += 4;
            tmpCachePtr += 4;
        }

        for (; re > 0; re--) {
            *tmpColSumPtr += *tmpCachePtr;
            tmpColSumPtr ++;
            tmpCachePtr ++;
        }
    }

    //head
    //Origin
    // for(int Y = 0; Y <= Radius; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] += cachePtr[(Y + Radius) * Width + X];
    //         Dest[Stride + X] = colsumPtr[X];
    //     }
    // }
    for(int Y = 0; Y <= Radius; Y++){
        int Stride = Y * Width;
        float* addPtr = cachePtr + (Y + Radius) * Width;
        float* destPtr = Dest + Stride;

        float *tmpDestPtr = destPtr;
        float *tmpColSumPtr = colsumPtr;
        float *tmpaddPtr = addPtr;

        int n = Block;
        int re = Remain;
        for(; n > 0; n--){
            float32x4_t add = vld1q_f32(tmpaddPtr);
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t sum = vaddq_f32(colsum, add);
            
            vst1q_f32(tmpColSumPtr, sum);
            vst1q_f32(tmpDestPtr, sum);

            tmpaddPtr += 4;
            tmpColSumPtr += 4;
            tmpDestPtr += 4;
        }

        for (; re > 0; re--) {
            *tmpColSumPtr += *tmpaddPtr;
            *tmpDestPtr = *tmpColSumPtr;
            tmpaddPtr++;
            tmpColSumPtr++;
            tmpDestPtr++;
        }
    }

    //middle
    //Origin
    // for(int Y = Radius+1; Y <= Height-1-Radius; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] += cachePtr[(Y + Radius) * Width + X];
    //         colsumPtr[X] -= cachePtr[(Y - Radius - 1) * Width + X];
    //         Dest[Stride + X] = colsumPtr[X];
    //     }
    // }
    for(int Y = Radius+1; Y <= Height-1-Radius; Y++){
        int Stride = Y * Width;
        float *addPtr = cachePtr + (Y + Radius) * Width;
        float *subPtr = cachePtr + (Y - Radius - 1) * Width;
        float *destPtr = Dest + Stride;

        float *tmpDestPtr = destPtr;
        float *tmpaddPtr = addPtr;
        float *tmpsubPtr = subPtr;
        float *tmpColSumPtr = colsumPtr;

        int n = Block;
        int re = Remain;
        
        // for(; n > 0; n--){
        //     float32x4_t colsum = vld1q_f32(tmpColSumPtr);
        //     float32x4_t add = vld1q_f32(tmpaddPtr);
        //     float32x4_t sub = vld1q_f32(tmpsubPtr);
        //     float32x4_t sum = vaddq_f32(colsum, add);
        //     sum = vsubq_f32(sum, sub);
            
        //     vst1q_f32(tmpColSumPtr, sum);
        //     vst1q_f32(tmpDestPtr, sum);

        //     tmpaddPtr += 4;
        //     tmpsubPtr += 4;
        //     tmpColSumPtr += 4;
        //     tmpDestPtr += 4;
        // }
        // ?????????????????????OutputOperands->InputOperands->????????????->Clobbers

        asm volatile(
            "0:                       \n" //??????0???????????????do while??????while(n>0)??????0
            "vld1.s32 {d0-d1}, [%0]!  \n" //?????????tmpaddPtr????????????????????????4???????????????{d0-d1}?????????q0?????????
                                         //???????????????32?????????????????????128??????????????????????????????????????????????????????
                                         //tmpaddPtr?????????4?????????????????????????????????????????????????????????
            
            "vld1.s32 {d2-d3}, [%1]!  \n" //???????????????tmpsubPtr?????????q1?????????
            "vld1.s32 {d4-d5}, [%2]   \n" //???????????????tmpColSumPtr?????????q2??????????????????tmpColSumPtr????????????
                                          //????????????????????????????????????????????????????????????

            "vadd.f32 q4, q0, q2      \n" //??????float32x4_t sum = vaddq_f32(colsum, add);
            "vsub.f32 q3, q4, q1      \n" //??????sum = vsubq_f32(sum, sub);

            "vst1.s32 {d6-d7}, [%3]!  \n" //???????????????????????????tmpDestPtr?????????????????????
            "vst1.s32 {d6-d7}, [%2]!  \n" //???????????????????????????tmpColSumPtr?????????????????????
            "subs %4, #1              \n" //n-=1
            "bne  0b                  \n" //bne??????n?????????0??? ??????0???????????????????????????0?????????????????????
            // OutputOperands 
            : "=r"(tmpaddPtr), 
            "=r"(tmpsubPtr),
            "=r"(tmpColSumPtr),
            "=r"(tmpDestPtr),
            "=r"(n)
            // InputOperands
            : "0"(tmpaddPtr),
            "1"(tmpsubPtr),
            "2"(tmpColSumPtr),
            "3"(tmpDestPtr),
            "4"(n)
            //Clobbers ???????????????q0,q1,q2,q3,q4????????????????????????
            : "cc", "memory", "q0", "q1", "q2", "q3", "q4"
        );

        for(;re > 0; re--){
            *tmpColSumPtr += *tmpaddPtr;
            *tmpColSumPtr -= *tmpsubPtr;
            *tmpDestPtr = *tmpColSumPtr;
            tmpaddPtr++;
            tmpColSumPtr++;
            tmpDestPtr++;
            tmpsubPtr++;
        }
    }
    //tail
    //Origin
    //tail
    // for(int Y = Height-Radius; Y < Height; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] -= cachePtr[(Y - Radius - 1) * Width + X];
    //         Dest[Stride + X] = colsumPtr[X];
    //     }
    // }
    for(int Y = Height - Radius; Y < Height; Y++){
        int Stride = Y * Width;
        float *subPtr = cachePtr + (Y - Radius - 1) * Width;
        float *destPtr = Dest + Stride;

        float *tmpDestPtr = destPtr;
        float *tmpsubPtr = subPtr;
        float *tmpColSumPtr = colsumPtr;

        int n = Block;
        int re = Remain;

        for(; n > 0; n--){
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t sub = vld1q_f32(tmpsubPtr);
            float32x4_t sum = vsubq_f32(colsum, sub);
            
            vst1q_f32(tmpColSumPtr, sum);
            vst1q_f32(tmpDestPtr, sum);

            tmpsubPtr += 4;
            tmpColSumPtr += 4;
            tmpDestPtr += 4;
        } 

        for(;re > 0; re--){
            *tmpColSumPtr -= *tmpsubPtr;
            *tmpDestPtr = *tmpColSumPtr;
            tmpColSumPtr++;
            tmpDestPtr++;
            tmpsubPtr++;
        }       
    }
}


void BoxFilterNeonAssemblyV1(float *Src, float *Dest, int Width, int Height, int Radius, vector<float>&cache){
    float *cachePtr = &(cache[0]);
     //chuizhi
    for(int Y = 0; Y < Height; Y++){
        int Stride = Y * Width;
        //head
        float sum = 0;
        for(int X = 0; X < Radius; X++){
            sum += Src[Stride + X];
        }
        for(int X = 0; X <= Radius; X++){
            sum += Src[Stride + X + Radius];
            cachePtr[Stride + X] = sum;
        }
        //middle
        for(int X = Radius+1; X <= Width-1-Radius; X++){
            sum += Src[Stride + X + Radius];
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
        //tail
        for(int X = Width - Radius; X < Width; X++){
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
    }

    // shuipin
    vector <float> colsum;
    colsum.resize(Width); 
    float *colsumPtr = &(colsum[0]);
    for(int X = 0;  X < Width; X++){
        colsumPtr[X] = 0;
    }

    int Block = Width >> 2;
    int Remain = Width - (Block << 2);

    //Origin
    // for(int Y = 0; Y < Radius; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] += colsumPtr[Stride + X];
    //     }
    // }
    for(int Y = 0; Y < Radius; Y++){
        int Stride = Y * Width;
        float* tmpColSumPtr = colsumPtr;
        float* tmpCachePtr = cachePtr;

        int n = Block;
        int re = Remain;

        for(; n > 0; n--){
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t cache = vld1q_f32(tmpCachePtr);
            float32x4_t sum = vaddq_f32(colsum, cache);

            vst1q_f32(tmpColSumPtr, sum); 

            tmpColSumPtr += 4;
            tmpCachePtr += 4;
        }

        for (; re > 0; re--) {
            *tmpColSumPtr += *tmpCachePtr;
            tmpColSumPtr ++;
            tmpCachePtr ++;
        }
    }

    //head
    //Origin
    // for(int Y = 0; Y <= Radius; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] += cachePtr[(Y + Radius) * Width + X];
    //         Dest[Stride + X] = colsumPtr[X];
    //     }
    // }
    for(int Y = 0; Y <= Radius; Y++){
        int Stride = Y * Width;
        float* addPtr = cachePtr + (Y + Radius) * Width;
        float* destPtr = Dest + Stride;

        float *tmpDestPtr = destPtr;
        float *tmpColSumPtr = colsumPtr;
        float *tmpaddPtr = addPtr;

        int n = Block;
        int re = Remain;
        for(; n > 0; n--){
            float32x4_t add = vld1q_f32(tmpaddPtr);
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t sum = vaddq_f32(colsum, add);
            
            vst1q_f32(tmpColSumPtr, sum);
            vst1q_f32(tmpDestPtr, sum);

            tmpaddPtr += 4;
            tmpColSumPtr += 4;
            tmpDestPtr += 4;
        }

        for (; re > 0; re--) {
            *tmpColSumPtr += *tmpaddPtr;
            *tmpDestPtr = *tmpColSumPtr;
            tmpaddPtr++;
            tmpColSumPtr++;
            tmpDestPtr++;
        }
    }

    //middle
    //Origin
    // for(int Y = Radius+1; Y <= Height-1-Radius; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] += cachePtr[(Y + Radius) * Width + X];
    //         colsumPtr[X] -= cachePtr[(Y - Radius - 1) * Width + X];
    //         Dest[Stride + X] = colsumPtr[X];
    //     }
    // }
    for(int Y = Radius+1; Y <= Height-1-Radius; Y++){
        int Stride = Y * Width;
        float *addPtr = cachePtr + (Y + Radius) * Width;
        float *subPtr = cachePtr + (Y - Radius - 1) * Width;
        float *destPtr = Dest + Stride;

        float *tmpDestPtr = destPtr;
        float *tmpaddPtr = addPtr;
        float *tmpsubPtr = subPtr;
        float *tmpColSumPtr = colsumPtr;

        int n = Block;
        int re = Remain;
        
        // for(; n > 0; n--){
        //     float32x4_t colsum = vld1q_f32(tmpColSumPtr);
        //     float32x4_t add = vld1q_f32(tmpaddPtr);
        //     float32x4_t sub = vld1q_f32(tmpsubPtr);
        //     float32x4_t sum = vaddq_f32(colsum, add);
        //     sum = vsubq_f32(sum, sub);
            
        //     vst1q_f32(tmpColSumPtr, sum);
        //     vst1q_f32(tmpDestPtr, sum);

        //     tmpaddPtr += 4;
        //     tmpsubPtr += 4;
        //     tmpColSumPtr += 4;
        //     tmpDestPtr += 4;
        // }
        // ?????????????????????OutputOperands->InputOperands->????????????->Clobbers

        asm volatile(
            "0:                       \n" //??????0???????????????do while??????while(n>0)??????0
            "pld      [%0, #128]      \n" 
            "vld1.s32 {d0-d1}, [%0]!  \n" //?????????tmpaddPtr????????????????????????4???????????????{d0-d1}?????????q0?????????
                                         //???????????????32?????????????????????128??????????????????????????????????????????????????????
                                         //tmpaddPtr?????????4?????????????????????????????????????????????????????????
            "pld      [%1, #128]      \n" 
            "vld1.s32 {d2-d3}, [%1]!  \n" //???????????????tmpsubPtr?????????q1?????????
            "pld      [%2, #128]      \n" 
            "vld1.s32 {d4-d5}, [%2]   \n" //???????????????tmpColSumPtr?????????q2??????????????????tmpColSumPtr????????????
                                          //????????????????????????????????????????????????????????????

            "vadd.f32 q4, q0, q2      \n" //??????float32x4_t sum = vaddq_f32(colsum, add);
            "vsub.f32 q3, q4, q1      \n" //??????sum = vsubq_f32(sum, sub);

            "vst1.s32 {d6-d7}, [%3]!  \n" //???????????????????????????tmpDestPtr?????????????????????
            "vst1.s32 {d6-d7}, [%2]!  \n" //???????????????????????????tmpColSumPtr?????????????????????
            "subs %4, #1              \n" //n-=1
            "bne  0b                  \n" //bne??????n?????????0??? ??????0???????????????????????????0?????????????????????
            // OutputOperands 
            : "=r"(tmpaddPtr), 
            "=r"(tmpsubPtr),
            "=r"(tmpColSumPtr),
            "=r"(tmpDestPtr),
            "=r"(n)
            // InputOperands
            : "0"(tmpaddPtr),
            "1"(tmpsubPtr),
            "2"(tmpColSumPtr),
            "3"(tmpDestPtr),
            "4"(n)
            //Clobbers ???????????????q0,q1,q2,q3,q4????????????????????????
            : "cc", "memory", "q0", "q1", "q2", "q3", "q4"
        );

        for(;re > 0; re--){
            *tmpColSumPtr += *tmpaddPtr;
            *tmpColSumPtr -= *tmpsubPtr;
            *tmpDestPtr = *tmpColSumPtr;
            tmpaddPtr++;
            tmpColSumPtr++;
            tmpDestPtr++;
            tmpsubPtr++;
        }
    }
    //tail
    //Origin
    //tail
    // for(int Y = Height-Radius; Y < Height; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] -= cachePtr[(Y - Radius - 1) * Width + X];
    //         Dest[Stride + X] = colsumPtr[X];
    //     }
    // }
    for(int Y = Height - Radius; Y < Height; Y++){
        int Stride = Y * Width;
        float *subPtr = cachePtr + (Y - Radius - 1) * Width;
        float *destPtr = Dest + Stride;

        float *tmpDestPtr = destPtr;
        float *tmpsubPtr = subPtr;
        float *tmpColSumPtr = colsumPtr;

        int n = Block;
        int re = Remain;

        for(; n > 0; n--){
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t sub = vld1q_f32(tmpsubPtr);
            float32x4_t sum = vsubq_f32(colsum, sub);
            
            vst1q_f32(tmpColSumPtr, sum);
            vst1q_f32(tmpDestPtr, sum);

            tmpsubPtr += 4;
            tmpColSumPtr += 4;
            tmpDestPtr += 4;
        } 

        for(;re > 0; re--){
            *tmpColSumPtr -= *tmpsubPtr;
            *tmpDestPtr = *tmpColSumPtr;
            tmpColSumPtr++;
            tmpDestPtr++;
            tmpsubPtr++;
        }       
    }
}


void BoxFilterNeonAssemblyV2(float *Src, float *Dest, int Width, int Height, int Radius, vector<float>&cache){
    float *cachePtr = &(cache[0]);
     //chuizhi
    for(int Y = 0; Y < Height; Y++){
        int Stride = Y * Width;
        //head
        float sum = 0;
        for(int X = 0; X < Radius; X++){
            sum += Src[Stride + X];
        }
        for(int X = 0; X <= Radius; X++){
            sum += Src[Stride + X + Radius];
            cachePtr[Stride + X] = sum;
        }
        //middle
        for(int X = Radius+1; X <= Width-1-Radius; X++){
            sum += Src[Stride + X + Radius];
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
        //tail
        for(int X = Width - Radius; X < Width; X++){
            sum -= Src[Stride + X - Radius - 1];
            cachePtr[Stride + X] = sum;
        }
    }

    // shuipin
    vector <float> colsum;
    colsum.resize(Width); 
    float *colsumPtr = &(colsum[0]);
    for(int X = 0;  X < Width; X++){
        colsumPtr[X] = 0;
    }

    int Block = Width >> 2;
    int Remain = Width - (Block << 2);

    //Origin
    // for(int Y = 0; Y < Radius; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] += colsumPtr[Stride + X];
    //     }
    // }
    for(int Y = 0; Y < Radius; Y++){
        int Stride = Y * Width;
        float* tmpColSumPtr = colsumPtr;
        float* tmpCachePtr = cachePtr;

        int n = Block;
        int re = Remain;

        for(; n > 0; n--){
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t cache = vld1q_f32(tmpCachePtr);
            float32x4_t sum = vaddq_f32(colsum, cache);

            vst1q_f32(tmpColSumPtr, sum); 

            tmpColSumPtr += 4;
            tmpCachePtr += 4;
        }

        for (; re > 0; re--) {
            *tmpColSumPtr += *tmpCachePtr;
            tmpColSumPtr ++;
            tmpCachePtr ++;
        }
    }

    //head
    //Origin
    // for(int Y = 0; Y <= Radius; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] += cachePtr[(Y + Radius) * Width + X];
    //         Dest[Stride + X] = colsumPtr[X];
    //     }
    // }
    for(int Y = 0; Y <= Radius; Y++){
        int Stride = Y * Width;
        float* addPtr = cachePtr + (Y + Radius) * Width;
        float* destPtr = Dest + Stride;

        float *tmpDestPtr = destPtr;
        float *tmpColSumPtr = colsumPtr;
        float *tmpaddPtr = addPtr;

        int n = Block;
        int re = Remain;
        for(; n > 0; n--){
            float32x4_t add = vld1q_f32(tmpaddPtr);
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t sum = vaddq_f32(colsum, add);
            
            vst1q_f32(tmpColSumPtr, sum);
            vst1q_f32(tmpDestPtr, sum);

            tmpaddPtr += 4;
            tmpColSumPtr += 4;
            tmpDestPtr += 4;
        }

        for (; re > 0; re--) {
            *tmpColSumPtr += *tmpaddPtr;
            *tmpDestPtr = *tmpColSumPtr;
            tmpaddPtr++;
            tmpColSumPtr++;
            tmpDestPtr++;
        }
    }

    //middle
    //Origin
    // for(int Y = Radius+1; Y <= Height-1-Radius; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] += cachePtr[(Y + Radius) * Width + X];
    //         colsumPtr[X] -= cachePtr[(Y - Radius - 1) * Width + X];
    //         Dest[Stride + X] = colsumPtr[X];
    //     }
    // }
    for(int Y = Radius+1; Y <= Height-1-Radius; Y++){
        int Stride = Y * Width;
        float *addPtr = cachePtr + (Y + Radius) * Width;
        float *subPtr = cachePtr + (Y - Radius - 1) * Width;
        float *destPtr = Dest + Stride;

        float *tmpDestPtr = destPtr;
        float *tmpaddPtr = addPtr;
        float *tmpsubPtr = subPtr;
        float *tmpColSumPtr = colsumPtr;

        int n = Width >> 3;
        int re = Width - (n << 3);
        
        // for(; n > 0; n--){
        //     float32x4_t colsum = vld1q_f32(tmpColSumPtr);
        //     float32x4_t add = vld1q_f32(tmpaddPtr);
        //     float32x4_t sub = vld1q_f32(tmpsubPtr);
        //     float32x4_t sum = vaddq_f32(colsum, add);
        //     sum = vsubq_f32(sum, sub);
            
        //     vst1q_f32(tmpColSumPtr, sum);
        //     vst1q_f32(tmpDestPtr, sum);

        //     tmpaddPtr += 4;
        //     tmpsubPtr += 4;
        //     tmpColSumPtr += 4;
        //     tmpDestPtr += 4;
        // }
        // ?????????????????????OutputOperands->InputOperands->????????????->Clobbers

        asm volatile(
            "0:                       \n"
            "pld      [%0, #256]      \n"
            "vld1.s32 {d0-d3}, [%0]!  \n" //q0,q1
            "pld      [%2, #256]      \n" 
            "vld1.s32 {d8-d11}, [%2]  \n" //q4,q5

            "vadd.f32 q6, q0, q4      \n"
            
            "vadd.f32 q7, q1, q5      \n"

            "pld      [%1, #256]      \n"
            "vld1.s32 {d4-d7}, [%1]!  \n" //q2,q3
            
            "vsub.f32 q6, q6, q2      \n"
            
            "vsub.f32 q7, q7, q3      \n"
            
            "vst1.s32 {d12-d15}, [%3]!  \n"//q8, q9
            
            "vst1.s32 {d12-d15}, [%2]!  \n"

            "subs %4, #1              \n"
            "bne  0b                  \n"

            // OutputOperands 
            : "=r"(tmpaddPtr), 
            "=r"(tmpsubPtr),
            "=r"(tmpColSumPtr),
            "=r"(tmpDestPtr),
            "=r"(n)
            // InputOperands
            : "0"(tmpaddPtr),
            "1"(tmpsubPtr),
            "2"(tmpColSumPtr),
            "3"(tmpDestPtr),
            "4"(n)
            //Clobbers ???????????????q0,q1,q2,q3,q4????????????????????????
            : "cc", "memory", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9"
        );

        for(;re > 0; re--){
            *tmpColSumPtr += *tmpaddPtr;
            *tmpColSumPtr -= *tmpsubPtr;
            *tmpDestPtr = *tmpColSumPtr;
            tmpaddPtr++;
            tmpColSumPtr++;
            tmpDestPtr++;
            tmpsubPtr++;
        }
    }
    //tail
    //Origin
    //tail
    // for(int Y = Height-Radius; Y < Height; Y++){
    //     int Stride = Y * Width;
    //     for(int X = 0; X < Width; X++){
    //         colsumPtr[X] -= cachePtr[(Y - Radius - 1) * Width + X];
    //         Dest[Stride + X] = colsumPtr[X];
    //     }
    // }
    for(int Y = Height - Radius; Y < Height; Y++){
        int Stride = Y * Width;
        float *subPtr = cachePtr + (Y - Radius - 1) * Width;
        float *destPtr = Dest + Stride;

        float *tmpDestPtr = destPtr;
        float *tmpsubPtr = subPtr;
        float *tmpColSumPtr = colsumPtr;

        int n = Block;
        int re = Remain;

        for(; n > 0; n--){
            float32x4_t colsum = vld1q_f32(tmpColSumPtr);
            float32x4_t sub = vld1q_f32(tmpsubPtr);
            float32x4_t sum = vsubq_f32(colsum, sub);
            
            vst1q_f32(tmpColSumPtr, sum);
            vst1q_f32(tmpDestPtr, sum);

            tmpsubPtr += 4;
            tmpColSumPtr += 4;
            tmpDestPtr += 4;
        } 

        for(;re > 0; re--){
            *tmpColSumPtr -= *tmpsubPtr;
            *tmpDestPtr = *tmpColSumPtr;
            tmpColSumPtr++;
            tmpDestPtr++;
            tmpsubPtr++;
        }       
    }
}

