#include <iostream>
#include <stdio.h>
// #define CATCH_CONFIG_MAIN
// #include <catch.h>
#include <math.h>
#include <convolution_3x3s1.h>
#include <convolution_3x3s2.h>
#include <opencv2/opencv.hpp>
#include <convolution_3x3s1_winograd.h>
using namespace std;
using namespace cv;

bool cmp(float x, float y){
    if(fabs(x - y) < 0.01){
        return true;
    }
    else{
        return false;
    }
}

float a[500]={1.5480e+00, 5.8775e+00, 9.6133e+00, 6.4162e+00, 9.1700e+00,
           5.5001e-01, 5.1094e+00, 8.4759e+00, 7.6829e+00, 9.3233e+00,
          6.9859e-01, 6.0790e+00, 7.8294e+00, 1.0041e+00, 1.6304e+00,
           2.1083e+00, 3.9958e+00, 7.6131e+00, 4.7507e+00, 9.3334e-01,
          7.6546e+00, 1.8259e+00, 2.6263e+00, 8.7056e-01, 2.8469e+00,
           5.1353e+00, 8.3510e-02, 8.6044e+00, 9.0078e+00, 2.2211e+00,
          7.7555e+00, 5.4000e+00, 5.9109e+00, 2.5644e+00, 2.9786e+00,
           4.2672e+00, 5.5485e-01, 2.7934e+00, 7.8426e+00, 2.0968e+00,
          2.3183e+00, 5.2440e+00, 7.8705e+00, 2.6232e+00, 8.8240e-01,
           9.5979e+00, 3.8301e+00, 8.0344e+00, 4.8134e+00, 4.1824e+00,
          8.7715e+00, 2.5819e+00, 7.2201e+00, 3.1098e+00, 5.6182e+00,
           1.3349e+00, 5.6184e+00, 4.5425e+00, 7.4400e+00, 3.3589e+00,
          6.2519e+00, 8.8930e+00, 7.0625e-01, 4.4240e-01, 8.8392e+00,
           1.2925e+00, 5.6234e+00, 8.8943e+00, 3.2807e+00, 4.0718e+00,
          2.7979e-01, 2.6724e+00, 2.2702e+00, 4.6280e+00, 4.2642e+00,
           5.2570e-01, 6.0902e+00, 4.3844e+00, 8.4485e+00, 9.7065e+00,
          1.5372e+00, 8.7131e+00, 2.2015e+00, 7.3346e+00, 8.7162e+00,
           2.2825e+00, 4.1035e-02, 7.0461e+00, 8.1087e+00, 8.4296e+00,
          9.7889e-03, 1.5212e+00, 8.2262e+00, 4.3486e+00, 6.9203e+00,
           4.9700e+00, 7.7963e-01, 3.6494e+00, 5.3299e+00, 1.4696e+00,

         5.5889e+00, 6.3574e+00, 1.3794e+00, 4.7155e+00, 4.2287e+00,
           1.8340e-01, 5.5382e+00, 6.3895e+00, 8.2433e+00, 4.1653e+00,
          5.5717e+00, 4.9475e+00, 1.6770e+00, 5.8382e+00, 2.3871e+00,
           8.9229e+00, 6.4787e+00, 7.6982e+00, 1.1365e+00, 7.1705e+00,
          9.8642e+00, 2.3417e+00, 6.3776e+00, 4.4630e+00, 5.3187e+00,
           3.3176e+00, 6.3829e+00, 1.9295e+00, 2.9324e+00, 6.5841e+00,
          7.8217e+00, 2.9277e+00, 1.2376e-01, 7.2448e+00, 2.2611e+00,
           1.7493e+00, 5.7942e+00, 2.9681e+00, 7.5918e+00, 6.6146e+00,
          6.9443e+00, 6.0428e-01, 7.9485e+00, 6.6637e+00, 3.2353e+00,
           4.1481e+00, 4.3988e+00, 6.3277e+00, 1.5636e+00, 5.2194e+00,
          4.3284e+00, 1.6745e+00, 8.8592e+00, 7.9151e+00, 2.6001e+00,
           5.4445e+00, 8.6557e+00, 7.0630e+00, 6.2278e+00, 4.8123e+00,
          5.3911e-02, 3.4562e+00, 2.2699e+00, 5.0026e+00, 4.4603e-02,
           4.6422e+00, 8.4199e+00, 3.8745e+00, 3.3628e+00, 2.3751e+00,
          9.7502e+00, 3.5378e+00, 1.6925e+00, 5.4719e+00, 2.2683e+00,
           4.1163e+00, 4.5583e+00, 3.5481e+00, 6.5044e+00, 6.2782e-01,
          5.8499e+00, 3.9503e-01, 8.1536e-01, 3.7303e-01, 2.4043e+00,
           5.1011e+00, 4.6399e+00, 1.2346e+00, 5.2527e+00, 6.1268e+00,
          2.3451e+00, 7.4442e+00, 5.7781e+00, 8.0052e+00, 7.5939e+00,
           9.6714e+00, 2.6139e+00, 6.5328e+00, 4.6924e+00, 2.9129e+00,

         6.9133e+00, 8.3163e-01, 1.8732e+00, 8.0232e+00, 5.0372e+00,
           7.3535e+00, 8.4078e+00, 6.5604e+00, 1.8240e+00, 5.4781e+00,
          5.1575e+00, 3.6449e+00, 9.5463e+00, 4.2575e+00, 2.5124e+00,
           5.2931e+00, 2.0006e+00, 2.7188e+00, 2.5953e+00, 2.8444e+00,
          1.5866e-01, 2.1032e+00, 4.9427e+00, 3.6383e+00, 9.4935e+00,
           3.2960e+00, 5.3648e+00, 7.8853e+00, 6.4272e+00, 9.5614e+00,
          6.2513e+00, 8.7905e+00, 9.0289e-01, 7.1936e+00, 3.9622e+00,
           2.1502e+00, 3.8065e+00, 4.6028e+00, 4.5721e+00, 2.1749e+00,
          7.7009e+00, 6.4854e+00, 3.1069e+00, 2.6464e+00, 3.0073e+00,
           4.9176e+00, 7.8864e+00, 8.5972e+00, 5.1561e+00, 4.3476e+00,
          9.6193e+00, 4.5959e+00, 1.6737e-01, 2.5863e-01, 3.8770e+00,
           2.5752e+00, 6.3135e+00, 1.0976e+00, 1.2940e-01, 2.2749e-01,
          6.6554e+00, 3.4406e+00, 9.2675e+00, 5.2269e+00, 3.2702e+00,
           1.0702e+00, 2.9240e+00, 7.5236e+00, 7.9678e+00, 3.8971e+00,
          9.9326e+00, 4.3822e+00, 3.3436e+00, 6.2567e+00, 5.6966e+00,
           1.2007e-01, 1.4366e+00, 8.6156e+00, 2.1096e+00, 8.1120e+00,
          7.1337e+00, 7.2302e+00, 4.8199e+00, 6.5662e+00, 6.1351e+00,
           2.4189e+00, 4.1872e+00, 9.7356e+00, 5.5227e+00, 3.6657e+00,
          2.2605e+00, 4.5600e+00, 8.1104e+00, 8.4619e+00, 2.0900e+00,
           4.7765e+00, 1.2588e+00, 3.6344e+00, 4.8285e+00, 8.5846e+00};

float b[500]={0.0702, -0.1526, -0.1847,
          -0.0466, -0.0504,  0.0816,
          -0.0611,  0.0748, -0.0969,

         -0.1493, -0.1657,  0.1117,
           0.1352, -0.1042,  0.1104,
          -0.0247, -0.1886,  0.0763,

         -0.0805,  0.0616, -0.0343,
           0.1010,  0.0529,  0.1107,
          -0.0941, -0.0366,  0.1568,


         0.0228, -0.0169,  0.1525,
           0.1742, -0.0797, -0.0777,
           0.0085, -0.1077,  0.0905,

          0.1112, -0.0396,  0.1472,
          -0.0912,  0.0970,  0.0555,
           0.1468,  0.0485,  0.0012,

          0.0795, -0.1610,  0.0404,
          -0.0045, -0.0445, -0.0742,
           0.1024,  0.0473, -0.0688,


        -0.0964,  0.0565, -0.0356,
          -0.0157,  0.0897,  0.1722,
           0.0990, -0.1606,  0.0444,

          0.0440, -0.0426, -0.0113,
          -0.0697,  0.0332,  0.1524,
          -0.1203,  0.1630, -0.0322,

          0.1090, -0.1319,  0.0852,
           0.1661,  0.0769, -0.1244,
          -0.0273,  0.1301, -0.1693,


         0.1651,  0.1510, -0.0525,
          -0.1049,  0.1426, -0.1724,
           0.1578,  0.0190, -0.0768,

          0.0792, -0.1145, -0.1019,
           0.0409, -0.1052, -0.0492,
          -0.1590, -0.0963,  0.0471,

          0.0985,  0.0161, -0.0511,
           0.0730,  0.1322,  0.0069,
           0.1071,  0.0662,  0.1317};

float c[500]={-2.4761, -1.9590, -1.3506, -1.6769,  0.0984, -1.7435, -3.3222,
           -2.4793,
          -4.2214,  0.2944, -0.0379, -0.6271,  0.7030, -2.8851, -1.5225,
            0.0988,
          -0.2823, -0.0583, -3.3062, -1.2015, -0.0465, -2.8265, -2.6943,
           -1.7582,
          -1.3513, -2.2955, -1.8455, -0.0558,  0.1854, -0.2373, -3.1952,
           -1.0001,
           0.4403, -2.4631, -3.4588, -1.1032, -2.0142, -1.8421, -1.6912,
           -3.0500,
          -1.8352, -0.8971, -2.2060, -1.1602, -2.0078, -0.6668, -2.9153,
           -2.7325,
           0.7684,  1.5323, -2.2025, -1.0858, -0.3976, -3.2698, -1.2460,
           -1.5728,
          -1.9735,  0.2686, -2.8976, -1.5354, -3.0331, -1.2985,  0.2067,
           -3.1905,

          2.1182,  2.6900,  3.8145,  1.8124,  3.4180,  2.5028,  3.4275,
            3.4651,
           4.4942,  1.1560,  0.8931,  3.5069,  0.8744,  3.8783,  0.3514,
            2.7860,
           4.0202,  0.6629,  4.3729,  1.5991,  1.8559,  3.5852,  2.0135,
            3.3397,
           1.9321,  4.1216,  2.2039,  1.5893,  1.0170,  2.3229,  4.2461,
            1.9253,
           3.0816,  3.0685,  3.9909,  2.6417,  2.5292,  2.8947,  1.7210,
            4.0203,
           5.4389,  3.5676,  1.9867,  1.5359,  5.6516,  0.2276,  3.5902,
            3.4787,
           0.2666,  0.8349,  1.4671,  1.6165,  3.7320,  2.1925,  2.1294,
            1.2317,
           1.1431,  3.1972,  1.4267,  3.1623,  5.2240,  2.6262,  0.2027,
            3.2327,

          1.4691,  3.0138, -0.4488,  3.1668,  0.3369,  3.7112, -0.2989,
            0.0064,
           1.8537, -1.4433,  2.3344,  2.3399,  0.5098,  2.7405,  2.2040,
            2.4210,
           1.1001,  2.1938,  2.2017,  0.9050,  0.7383,  0.5843,  3.0610,
            2.4639,
           3.6400,  4.5878, -0.6257,  1.2634,  2.3990,  2.7177,  3.0403,
            1.9930,
           3.0047,  4.0563,  1.1793, -1.0167,  3.9129,  1.5684,  1.6159,
            2.6260,
          -0.2051,  0.4214,  2.7610,  2.1631,  2.8556, -0.4850,  2.2344,
            1.2001,
           0.7722,  0.7984,  0.9748,  2.8093,  2.0007,  0.2722,  2.1225,
            2.4052,
           3.1255,  0.7409,  4.0335,  1.7021,  2.3204,  0.3206,  1.7863,
            4.1390,

          1.1584,  3.7797,  1.9622,  2.9551,  2.0574, -0.4729,  1.5615,
            5.3322,
           0.2988,  4.6822,  2.4902,  0.3645,  1.5755, -0.6293,  2.2440,
            3.3931,
           2.3647,  1.6246, -0.2896,  0.1565,  2.7754,  1.2756,  1.8539,
            4.5792,
           4.9879,  1.6202, -1.1194, -0.1381,  3.1125, -1.6979,  0.1655,
            0.2010,
           3.2421,  2.7775, -0.0585,  0.9053,  0.8418,  0.5252,  1.5060,
            4.0030,
           2.6449,  1.5230,  1.0899,  3.7448, -2.0981, -1.2096,  2.8817,
            2.3770,
           4.3689,  3.7118,  1.8544,  5.3197,  0.7465, -0.3593,  1.9903,
            3.6599,
           3.9646,  0.3316,  1.9363,  3.2947, -0.4532, -1.0674,  1.6930,
            2.1063};

int main(){
    
    const int inh = 10;
    const int inw = 10;
    const int inch = 3;
    const int outch = 4;
    const int outw = 8;
    const int outh = 8;

    float *kernel = new float[3 * 3 * outch * inch];
    
    for(int i = 0; i < 3 * 3 * outch * inch; i++){
      kernel[i] = b[i];
    }

    int nnOutchannel = outch >> 2;
    int remainOutChannel = nnOutchannel << 2;
    
    int packOutChannel = nnOutchannel + (outch % 4 + 3) / 4;
    int packOutH = 1;
    int packOutW = (8 * 8 * inch * 4);

    float *kernel_tm = new float[outch * inch * 64];
    float *kernel_tm2 = new float[packOutChannel * packOutH * packOutW];

    conv3x3s1WinogradTransformKenel(kernel, kernel_tm, kernel_tm2, inch, outch);
    
    printf("HERE!\n");

    float *src = new float[inch * inh * inw];
    for(int i = 0; i < inh * inw * inch; i++){
      src[i] = a[i];
    }

    float *dest = new float[outch * outw * outh];

    int64 st = cvGetTickCount();

    for(int i = 0; i < 10; i++){
      for(int j = 0; j < outw * outh * outch; j++) dest[j] = 0.f;
      conv3x3s1WinogradNeon(src, inw, inh, inch, kernel_tm2, packOutH, packOutW, dest, outw, outh, outch);
    }
    
    double duration = (cv::getTickCount() - st) / cv::getTickFrequency() * 100;

    // for(int i = 0; i < outw * outh * outch ; i++){
    //     bool flag = cmp(dest[i], c[i]);
    //     if(flag == false){
    //         printf("WA: %d\n", i);
    //         printf("Expected: %.4f, ConvOutput: %.4f\n", c[i], dest[i]);
    //     }
    // }

    printf("Time: %.5f\n", duration);

    // for(int i = 0; i < outw * outh * outch; i++){
    //     printf("%.5f ", dest[i]);
    // }

    // printf("\n");
    free(src);
    free(kernel);
    free(dest);
    free(kernel_tm);
    free(kernel_tm2);

    return 0;
}