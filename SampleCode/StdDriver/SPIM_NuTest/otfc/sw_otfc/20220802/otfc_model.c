#include <stdio.h>                            /* Include standard types */
#include <stdint.h>                           /* Include standard types */
#include "NuMicro.h"
#include "otfc_model.h"

//------------------------------------------------------------------------------
//uint32_t otfc_ans0, otfc_ans1, otfc_ans2, otfc_ans3; //The output of otfc_model()
static uint32_t otfc_ans[4] = {0};

static uint32_t plaintext0, plaintext1, plaintext2, plaintext3;

//------------------------------------------------------------------------------
uint32_t GetOTFCAnsKey(uint8_t u8Index)
{
    return otfc_ans[u8Index];
}

/*-----------------------------------------------------------
test AES DMA mode
Field : 256 bit key
Operation : ECB mode
-----------------------------------------------------------*/
void ral1(uint32_t in0, uint32_t in1, uint32_t in2, uint32_t in3)
{
    int i, j;
    int in[128];
    int out[128];
    uint32_t TT, PP;

    for (i = 0; i < 4; i++)
    {
        if (i == 0)      TT = in0;
        else if (i == 1) TT = in1;
        else if (i == 2) TT = in2;
        else          TT = in3;

        for (j = 0; j < 32; j++)
        {
            in[i * 32 + j] = TT % 2;
            TT = TT >> 1;
        }
    }

    out[127] = in[ 12];
    out[126] = in[  1];
    out[125] = in[107];
    out[124] = in[114];
    out[123] = in[104];
    out[122] = in[103];
    out[121] = in[ 74];
    out[120] = in[ 63];
    out[119] = in[  4];
    out[118] = in[ 52];
    out[117] = in[110];
    out[116] = in[ 66];
    out[115] = in[ 89];
    out[114] = in[ 14];
    out[113] = in[ 35];
    out[112] = in[106];
    out[111] = in[ 16];
    out[110] = in[ 25];
    out[109] = in[ 95];
    out[108] = in[ 19];
    out[107] = in[ 86];
    out[106] = in[126];
    out[105] = in[ 81];
    out[104] = in[  8];
    out[103] = in[ 92];
    out[102] = in[125];
    out[101] = in[ 34];
    out[100] = in[ 85];
    out[ 99] = in[ 29];
    out[ 98] = in[118];
    out[ 97] = in[109];
    out[ 96] = in[ 56];
    out[ 95] = in[119];
    out[ 94] = in[ 46];
    out[ 93] = in[ 13];
    out[ 92] = in[100];
    out[ 91] = in[ 42];
    out[ 90] = in[ 91];
    out[ 89] = in[ 98];
    out[ 88] = in[ 50];
    out[ 87] = in[ 97];
    out[ 86] = in[ 30];
    out[ 85] = in[ 23];
    out[ 84] = in[  3];
    out[ 83] = in[  2];
    out[ 82] = in[112];
    out[ 81] = in[ 44];
    out[ 80] = in[ 84];
    out[ 79] = in[ 90];
    out[ 78] = in[ 22];
    out[ 77] = in[ 40];
    out[ 76] = in[121];
    out[ 75] = in[123];
    out[ 74] = in[ 62];
    out[ 73] = in[  7];
    out[ 72] = in[ 59];
    out[ 71] = in[  6];
    out[ 70] = in[ 67];
    out[ 69] = in[ 72];
    out[ 68] = in[ 69];
    out[ 67] = in[ 41];
    out[ 66] = in[ 21];
    out[ 65] = in[ 96];
    out[ 64] = in[116];
    out[ 63] = in[ 45];
    out[ 62] = in[ 65];
    out[ 61] = in[124];
    out[ 60] = in[108];
    out[ 59] = in[113];
    out[ 58] = in[ 82];
    out[ 57] = in[ 80];
    out[ 56] = in[ 31];
    out[ 55] = in[117];
    out[ 54] = in[ 61];
    out[ 53] = in[ 51];
    out[ 52] = in[ 26];
    out[ 51] = in[ 28];
    out[ 50] = in[ 37];
    out[ 49] = in[ 77];
    out[ 48] = in[ 24];
    out[ 47] = in[ 79];
    out[ 46] = in[ 88];
    out[ 45] = in[ 64];
    out[ 44] = in[ 83];
    out[ 43] = in[ 75];
    out[ 42] = in[ 78];
    out[ 41] = in[ 76];
    out[ 40] = in[ 73];
    out[ 39] = in[ 93];
    out[ 38] = in[ 60];
    out[ 37] = in[ 32];
    out[ 36] = in[ 53];
    out[ 35] = in[127];
    out[ 34] = in[ 10];
    out[ 33] = in[101];
    out[ 32] = in[ 39];
    out[ 31] = in[111];
    out[ 30] = in[ 68];
    out[ 29] = in[ 17];
    out[ 28] = in[ 94];
    out[ 27] = in[115];
    out[ 26] = in[ 70];
    out[ 25] = in[ 18];
    out[ 24] = in[ 38];
    out[ 23] = in[ 49];
    out[ 22] = in[ 55];
    out[ 21] = in[ 33];
    out[ 20] = in[ 54];
    out[ 19] = in[ 47];
    out[ 18] = in[122];
    out[ 17] = in[ 48];
    out[ 16] = in[ 15];
    out[ 15] = in[ 71];
    out[ 14] = in[  5];
    out[ 13] = in[ 43];
    out[ 12] = in[120];
    out[ 11] = in[ 27];
    out[ 10] = in[ 36];
    out[  9] = in[ 99];
    out[  8] = in[ 20];
    out[  7] = in[ 58];
    out[  6] = in[ 57];
    out[  5] = in[  9];
    out[  4] = in[ 11];
    out[  3] = in[  0];
    out[  2] = in[ 87];
    out[  1] = in[102];
    out[  0] = in[105];

    for (i = 0; i < 4; i++)
    {
        TT = 0;
        PP = 1;

        for (j = 0; j < 32; j++)
        {
            if (out[i * 32 + j] == 1) TT = TT + PP;

            PP = PP << 1;
        }

        if (i == 0)      plaintext0 = TT;
        else if (i == 1) plaintext1 = TT;
        else if (i == 2) plaintext2 = TT;
        else          plaintext3 = TT;
    }
}

void ral2(uint32_t in0, uint32_t in1, uint32_t in2, uint32_t in3)
{
    int i, j;
    int in[128];
    int out[128];
    uint32_t TT, PP;

    for (i = 0; i < 4; i++)
    {
        if (i == 0)      TT = in0;
        else if (i == 1) TT = in1;
        else if (i == 2) TT = in2;
        else          TT = in3;

        for (j = 0; j < 32; j++)
        {
            in[i * 32 + j] = TT % 2;
            TT = TT >> 1;
        }
    }

    out[127] = in[ 84];
    out[126] = in[ 47];
    out[125] = in[ 29];
    out[124] = in[126];
    out[123] = in[ 10];
    out[122] = in[ 15];
    out[121] = in[ 43];
    out[120] = in[ 90];
    out[119] = in[ 42];
    out[118] = in[ 24];
    out[117] = in[ 70];
    out[116] = in[ 54];
    out[115] = in[ 89];
    out[114] = in[ 64];
    out[113] = in[ 76];
    out[112] = in[ 82];
    out[111] = in[  3];
    out[110] = in[109];
    out[109] = in[  7];
    out[108] = in[127];
    out[107] = in[ 18];
    out[106] = in[ 78];
    out[105] = in[ 21];
    out[104] = in[104];
    out[103] = in[ 38];
    out[102] = in[ 72];
    out[101] = in[  2];
    out[100] = in[  5];
    out[ 99] = in[ 63];
    out[ 98] = in[ 13];
    out[ 97] = in[ 39];
    out[ 96] = in[ 30];
    out[ 95] = in[ 58];
    out[ 94] = in[ 77];
    out[ 93] = in[ 33];
    out[ 92] = in[ 48];
    out[ 91] = in[ 35];
    out[ 90] = in[ 26];
    out[ 89] = in[ 14];
    out[ 88] = in[ 44];
    out[ 87] = in[ 25];
    out[ 86] = in[ 57];
    out[ 85] = in[ 55];
    out[ 84] = in[105];
    out[ 83] = in[ 79];
    out[ 82] = in[ 50];
    out[ 81] = in[ 46];
    out[ 80] = in[ 86];
    out[ 79] = in[112];
    out[ 78] = in[ 49];
    out[ 77] = in[ 67];
    out[ 76] = in[ 81];
    out[ 75] = in[120];
    out[ 74] = in[114];
    out[ 73] = in[101];
    out[ 72] = in[110];
    out[ 71] = in[107];
    out[ 70] = in[  9];
    out[ 69] = in[117];
    out[ 68] = in[ 73];
    out[ 67] = in[ 45];
    out[ 66] = in[102];
    out[ 65] = in[122];
    out[ 64] = in[ 96];
    out[ 63] = in[ 83];
    out[ 62] = in[ 32];
    out[ 61] = in[113];
    out[ 60] = in[ 65];
    out[ 59] = in[ 75];
    out[ 58] = in[ 85];
    out[ 57] = in[ 69];
    out[ 56] = in[ 98];
    out[ 55] = in[106];
    out[ 54] = in[123];
    out[ 53] = in[ 68];
    out[ 52] = in[ 41];
    out[ 51] = in[124];
    out[ 50] = in[ 12];
    out[ 49] = in[100];
    out[ 48] = in[ 16];
    out[ 47] = in[ 34];
    out[ 46] = in[  4];
    out[ 45] = in[  8];
    out[ 44] = in[115];
    out[ 43] = in[ 17];
    out[ 42] = in[ 95];
    out[ 41] = in[103];
    out[ 40] = in[ 40];
    out[ 39] = in[ 56];
    out[ 38] = in[ 28];
    out[ 37] = in[ 37];
    out[ 36] = in[ 74];
    out[ 35] = in[ 71];
    out[ 34] = in[ 31];
    out[ 33] = in[ 94];
    out[ 32] = in[ 97];
    out[ 31] = in[ 80];
    out[ 30] = in[  1];
    out[ 29] = in[ 22];
    out[ 28] = in[ 91];
    out[ 27] = in[ 27];
    out[ 26] = in[ 62];
    out[ 25] = in[ 23];
    out[ 24] = in[111];
    out[ 23] = in[ 93];
    out[ 22] = in[ 52];
    out[ 21] = in[ 11];
    out[ 20] = in[ 19];
    out[ 19] = in[  0];
    out[ 18] = in[ 87];
    out[ 17] = in[ 92];
    out[ 16] = in[125];
    out[ 15] = in[119];
    out[ 14] = in[ 88];
    out[ 13] = in[116];
    out[ 12] = in[108];
    out[ 11] = in[118];
    out[ 10] = in[ 59];
    out[  9] = in[ 36];
    out[  8] = in[121];
    out[  7] = in[ 51];
    out[  6] = in[ 66];
    out[  5] = in[  6];
    out[  4] = in[ 53];
    out[  3] = in[ 20];
    out[  2] = in[ 61];
    out[  1] = in[ 99];
    out[  0] = in[ 60];

    for (i = 0; i < 4; i++)
    {
        TT = 0;
        PP = 1;

        for (j = 0; j < 32; j++)
        {
            if (out[i * 32 + j] == 1) TT = TT + PP;

            PP = PP << 1;
        }

        if (i == 0)      plaintext0 = TT;
        else if (i == 1) plaintext1 = TT;
        else if (i == 2) plaintext2 = TT;
        else          plaintext3 = TT;
    }
}

void ral3(uint32_t in0, uint32_t in1, uint32_t in2, uint32_t in3)
{
    int i, j;
    int in[128];
    int out[128];
    uint32_t TT, PP;

    for (i = 0; i < 4; i++)
    {
        if (i == 0)      TT = in0;
        else if (i == 1) TT = in1;
        else if (i == 2) TT = in2;
        else          TT = in3;

        for (j = 0; j < 32; j++)
        {
            in[i * 32 + j] = TT % 2;
            TT = TT >> 1;
        }
    }

    out[127] = in[ 83];
    out[126] = in[ 38];
    out[125] = in[ 88];
    out[124] = in[ 56];
    out[123] = in[112];
    out[122] = in[111];
    out[121] = in[121];
    out[120] = in[ 54];
    out[119] = in[ 62];
    out[118] = in[ 13];
    out[117] = in[109];
    out[116] = in[ 11];
    out[115] = in[ 72];
    out[114] = in[ 25];
    out[113] = in[ 99];
    out[112] = in[ 70];
    out[111] = in[ 36];
    out[110] = in[ 17];
    out[109] = in[ 20];
    out[108] = in[ 58];
    out[107] = in[ 79];
    out[106] = in[ 15];
    out[105] = in[ 19];
    out[104] = in[ 94];
    out[103] = in[ 16];
    out[102] = in[ 57];
    out[101] = in[ 35];
    out[100] = in[ 33];
    out[ 99] = in[ 42];
    out[ 98] = in[ 63];
    out[ 97] = in[101];
    out[ 96] = in[ 34];
    out[ 95] = in[  2];
    out[ 94] = in[ 66];
    out[ 93] = in[ 80];
    out[ 92] = in[ 95];
    out[ 91] = in[ 50];
    out[ 90] = in[127];
    out[ 89] = in[ 18];
    out[ 88] = in[ 27];
    out[ 87] = in[113];
    out[ 86] = in[ 52];
    out[ 85] = in[ 97];
    out[ 84] = in[105];
    out[ 83] = in[ 10];
    out[ 82] = in[ 91];
    out[ 81] = in[ 75];
    out[ 80] = in[ 60];
    out[ 79] = in[ 82];
    out[ 78] = in[ 84];
    out[ 77] = in[ 49];
    out[ 76] = in[106];
    out[ 75] = in[ 45];
    out[ 74] = in[ 23];
    out[ 73] = in[125];
    out[ 72] = in[  4];
    out[ 71] = in[ 64];
    out[ 70] = in[  9];
    out[ 69] = in[ 40];
    out[ 68] = in[ 43];
    out[ 67] = in[ 41];
    out[ 66] = in[ 92];
    out[ 65] = in[100];
    out[ 64] = in[122];
    out[ 63] = in[ 61];
    out[ 62] = in[ 31];
    out[ 61] = in[110];
    out[ 60] = in[ 69];
    out[ 59] = in[ 74];
    out[ 58] = in[  7];
    out[ 57] = in[102];
    out[ 56] = in[ 51];
    out[ 55] = in[ 68];
    out[ 54] = in[ 65];
    out[ 53] = in[104];
    out[ 52] = in[ 46];
    out[ 51] = in[ 81];
    out[ 50] = in[ 71];
    out[ 49] = in[ 37];
    out[ 48] = in[ 76];
    out[ 47] = in[ 78];
    out[ 46] = in[ 12];
    out[ 45] = in[ 47];
    out[ 44] = in[ 28];
    out[ 43] = in[108];
    out[ 42] = in[ 67];
    out[ 41] = in[  1];
    out[ 40] = in[118];
    out[ 39] = in[ 90];
    out[ 38] = in[ 89];
    out[ 37] = in[120];
    out[ 36] = in[ 93];
    out[ 35] = in[ 32];
    out[ 34] = in[ 29];
    out[ 33] = in[123];
    out[ 32] = in[ 55];
    out[ 31] = in[119];
    out[ 30] = in[ 21];
    out[ 29] = in[ 98];
    out[ 28] = in[ 73];
    out[ 27] = in[  0];
    out[ 26] = in[ 44];
    out[ 25] = in[ 59];
    out[ 24] = in[ 87];
    out[ 23] = in[ 96];
    out[ 22] = in[ 24];
    out[ 21] = in[  8];
    out[ 20] = in[107];
    out[ 19] = in[116];
    out[ 18] = in[  5];
    out[ 17] = in[ 85];
    out[ 16] = in[114];
    out[ 15] = in[ 14];
    out[ 14] = in[ 48];
    out[ 13] = in[ 39];
    out[ 12] = in[ 53];
    out[ 11] = in[126];
    out[ 10] = in[ 26];
    out[  9] = in[115];
    out[  8] = in[103];
    out[  7] = in[ 86];
    out[  6] = in[117];
    out[  5] = in[ 77];
    out[  4] = in[ 30];
    out[  3] = in[  6];
    out[  2] = in[ 22];
    out[  1] = in[  3];
    out[  0] = in[124];

    for (i = 0; i < 4; i++)
    {
        TT = 0;
        PP = 1;

        for (j = 0; j < 32; j++)
        {
            if (out[i * 32 + j] == 1) TT = TT + PP;

            PP = PP << 1;
        }

        if (i == 0)      plaintext0 = TT;
        else if (i == 1) plaintext1 = TT;
        else if (i == 2) plaintext2 = TT;
        else          plaintext3 = TT;
    }
}

void ral4(uint32_t in0, uint32_t in1, uint32_t in2, uint32_t in3)
{
    int i, j;
    int in[128];
    int out[128];
    uint32_t TT, PP;

    for (i = 0; i < 4; i++)
    {
        if (i == 0)      TT = in0;
        else if (i == 1) TT = in1;
        else if (i == 2) TT = in2;
        else          TT = in3;

        for (j = 0; j < 32; j++)
        {
            in[i * 32 + j] = TT % 2;
            TT = TT >> 1;
        }
    }

    out[127] = in[ 37];
    out[126] = in[ 99];
    out[125] = in[ 52];
    out[124] = in[ 42];
    out[123] = in[ 16];
    out[122] = in[ 63];
    out[121] = in[ 81];
    out[120] = in[ 21];
    out[119] = in[ 76];
    out[118] = in[ 25];
    out[117] = in[ 31];
    out[116] = in[ 33];
    out[115] = in[  1];
    out[114] = in[  4];
    out[113] = in[ 84];
    out[112] = in[  2];
    out[111] = in[ 10];
    out[110] = in[ 95];
    out[109] = in[ 41];
    out[108] = in[ 75];
    out[107] = in[ 72];
    out[106] = in[ 12];
    out[105] = in[ 29];
    out[104] = in[109];
    out[103] = in[ 39];
    out[102] = in[ 30];
    out[101] = in[ 90];
    out[100] = in[ 93];
    out[ 99] = in[ 40];
    out[ 98] = in[ 50];
    out[ 97] = in[ 22];
    out[ 96] = in[ 57];
    out[ 95] = in[  5];
    out[ 94] = in[ 88];
    out[ 93] = in[ 20];
    out[ 92] = in[ 18];
    out[ 91] = in[ 26];
    out[ 90] = in[119];
    out[ 89] = in[ 73];
    out[ 88] = in[ 54];
    out[ 87] = in[123];
    out[ 86] = in[ 27];
    out[ 85] = in[ 17];
    out[ 84] = in[  9];
    out[ 83] = in[ 11];
    out[ 82] = in[ 13];
    out[ 81] = in[ 43];
    out[ 80] = in[ 32];
    out[ 79] = in[ 74];
    out[ 78] = in[ 46];
    out[ 77] = in[ 68];
    out[ 76] = in[ 53];
    out[ 75] = in[ 35];
    out[ 74] = in[118];
    out[ 73] = in[ 77];
    out[ 72] = in[ 96];
    out[ 71] = in[ 28];
    out[ 70] = in[ 70];
    out[ 69] = in[124];
    out[ 68] = in[126];
    out[ 67] = in[108];
    out[ 66] = in[ 65];
    out[ 65] = in[ 19];
    out[ 64] = in[ 78];
    out[ 63] = in[ 66];
    out[ 62] = in[ 55];
    out[ 61] = in[122];
    out[ 60] = in[ 98];
    out[ 59] = in[117];
    out[ 58] = in[ 69];
    out[ 57] = in[ 85];
    out[ 56] = in[ 86];
    out[ 55] = in[ 56];
    out[ 54] = in[ 48];
    out[ 53] = in[106];
    out[ 52] = in[ 47];
    out[ 51] = in[102];
    out[ 50] = in[120];
    out[ 49] = in[ 92];
    out[ 48] = in[ 60];
    out[ 47] = in[115];
    out[ 46] = in[ 45];
    out[ 45] = in[127];
    out[ 44] = in[ 71];
    out[ 43] = in[ 87];
    out[ 42] = in[114];
    out[ 41] = in[ 51];
    out[ 40] = in[116];
    out[ 39] = in[  8];
    out[ 38] = in[ 38];
    out[ 37] = in[  7];
    out[ 36] = in[ 91];
    out[ 35] = in[112];
    out[ 34] = in[ 97];
    out[ 33] = in[ 64];
    out[ 32] = in[ 23];
    out[ 31] = in[ 83];
    out[ 30] = in[ 67];
    out[ 29] = in[125];
    out[ 28] = in[113];
    out[ 27] = in[ 24];
    out[ 26] = in[ 15];
    out[ 25] = in[  6];
    out[ 24] = in[ 49];
    out[ 23] = in[110];
    out[ 22] = in[ 82];
    out[ 21] = in[  0];
    out[ 20] = in[ 80];
    out[ 19] = in[100];
    out[ 18] = in[ 14];
    out[ 17] = in[111];
    out[ 16] = in[104];
    out[ 15] = in[ 61];
    out[ 14] = in[ 62];
    out[ 13] = in[ 79];
    out[ 12] = in[ 34];
    out[ 11] = in[  3];
    out[ 10] = in[103];
    out[  9] = in[ 59];
    out[  8] = in[105];
    out[  7] = in[ 89];
    out[  6] = in[ 94];
    out[  5] = in[121];
    out[  4] = in[ 36];
    out[  3] = in[ 58];
    out[  2] = in[ 44];
    out[  1] = in[101];
    out[  0] = in[107];

    for (i = 0; i < 4; i++)
    {
        TT = 0;
        PP = 1;

        for (j = 0; j < 32; j++)
        {
            if (out[i * 32 + j] == 1) TT = TT + PP;

            PP = PP << 1;
        }

        if (i == 0)      plaintext0 = TT;
        else if (i == 1) plaintext1 = TT;
        else if (i == 2) plaintext2 = TT;
        else          plaintext3 = TT;
    }
}

int xor_bits(uint32_t in0, uint32_t in1)
{
    int i;
    uint32_t input  = in0 ^ in1;
    uint32_t output = 0;

    for (i = 0; i < 32; i++)  output = output ^ ((input >> i) & 0x1);

    return output;
}

int otfc_sw_check_result(uint32_t addr, uint32_t data, int err)
{
    uint32_t prdata;

    prdata = inp32(addr);

    if (prdata != data)
    {
        printf("Compare data error!!!!");
        printf("SRAM addr = 0x%X", addr);
        //printd(addr);
        printf("SRAM data = 0x%X", prdata);
        //printd(prdata);
        printf("Solution  = 0x%X", data);
        //printd(data);
        err = err + 1;
    }

    return err;
}

void crypto_clock_power_setting()
{
    // unlock register protection
    outp32(PROT_BA, 0x59);
    outp32(PROT_BA, 0x16);
    outp32(PROT_BA, 0x88);

    // enable crypto power enable
    outp32(SYS_BA + SYS_PSWCTL, (inp32(SYS_BA + SYS_PSWCTL) | 0x00001000));

    // enable crypto clock enable
    outp32(CLK_BA + AHBCLK, (inp32(CLK_BA + AHBCLK) | 0x00001000));

    // enable keystore and SRAM power enable
    outp32(SYS_BA + SYS_PSWCTL, (inp32(SYS_BA + SYS_PSWCTL) | 0x00302000));

    // enable keystore clock enable
    outp32(CLK_BA + AHBCLK, (inp32(CLK_BA + AHBCLK) | 0x00004000));
}

//OTF_PR1_KEY0, OTF_PR1_KEY1, OTF_PR1_KEY2, OTF_PR1_KEY3, OTF_PR1_NON0(scramble key), OTF_PR1_NON1, OTF_PR1_NON2, OTF_PR1_NON3, ADDR
void otfc_model(uint32_t key0,  uint32_t key1,  uint32_t key2,  uint32_t key3,
                uint32_t data0, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t addr)
{
    uint32_t prdata, sel, sel_k0, sel_k1;
    uint32_t taddr, taddr0, taddr1, taddr2, taddr3;

    int S_Box[256] =
    {
        //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, //0
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, //1
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, //2
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, //3
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, //4
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, //5
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, //6
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, //7
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, //8
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, //9
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, //A
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, //B
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, //C
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, //D
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, //E
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16  //F
    };

    // enable crypto clock enable
    crypto_clock_power_setting();

    //Nuvoton address cipher function
    if ((addr & 0x00000003) == 0x0) taddr = ((addr & 0xFFFFFFF0) + 0x1) ^ data0;

    if ((addr & 0x00000003) == 0x1) taddr = ((addr & 0xFFFFFFF0) + 0x2) ^ data0;

    if ((addr & 0x00000003) == 0x2) taddr = ((addr & 0xFFFFFFF0) + 0x4) ^ data0;

    if ((addr & 0x00000003) == 0x3) taddr = ((addr & 0xFFFFFFF0) + 0x8) ^ data0;

    taddr0 = S_Box[(taddr >> 0  & 0xFF)];
    taddr1 = S_Box[(taddr >> 8  & 0xFF)];
    taddr2 = S_Box[(taddr >> 16 & 0xFF)];
    taddr3 = S_Box[(taddr >> 24 & 0xFF)];
    taddr = taddr0 + (taddr1 << 8) + (taddr2 << 16) + (taddr3 << 24);

    sel_k1 = xor_bits(key3, key2);
    sel_k0 = xor_bits(key1, key0);
    sel = (((data0 ^ addr) & 0x30) >> 4) ^ (sel_k0 | sel_k1 << 1);

    printf("sel = 0x%X", sel);
    //printd(sel);

    if (sel == 0)      ral1(taddr, data1, data2, data3);
    else if (sel == 1) ral2(taddr, data1, data2, data3);
    else if (sel == 2) ral3(taddr, data1, data2, data3);
    else            ral4(taddr, data1, data2, data3);

    outp32(CRPT_SRAM_BASE + 0x0000, plaintext0);
    outp32(CRPT_SRAM_BASE + 0x0004, plaintext1);
    outp32(CRPT_SRAM_BASE + 0x0008, plaintext2);
    outp32(CRPT_SRAM_BASE + 0x000C, plaintext3);

    //key
    outp32(CRPT_BA + CRPT_AES_KEY0, key0);
    outp32(CRPT_BA + CRPT_AES_KEY1, key1);
    outp32(CRPT_BA + CRPT_AES_KEY2, key2);
    outp32(CRPT_BA + CRPT_AES_KEY3, key3);

    //Source ADDR      CRPT_SRAM_BASE+0x0000
    //Destination ADDR CRPT_SRAM_BASE+0x1000
    outp32(CRPT_BA + CRPT_AES_SADDR, CRPT_SRAM_BASE + 0x0000);
    outp32(CRPT_BA + CRPT_AES_DADDR, CRPT_SRAM_BASE + 0x1000);
    //DMACNT
    outp32(CRPT_BA + CRPT_AES_CNT, 0x00000010);
    // set AES control register
    outp32(CRPT_BA + CRPT_AES_CTL, 0x00010081); //  encryption,  DMA mode key = 128

    prdata = inp32(CRPT_BA + CRPT_AES_STS) & 0x00000001;

    while (prdata != 0x00000000) prdata = inp32(CRPT_BA + CRPT_AES_STS) & 0x00000001;

    printf("AES done!!!!!!");
    //otfc_ans0 = inp32(CRPT_SRAM_BASE + 0x1000);
    //otfc_ans1 = inp32(CRPT_SRAM_BASE + 0x1004);
    //otfc_ans2 = inp32(CRPT_SRAM_BASE + 0x1008);
    //otfc_ans3 = inp32(CRPT_SRAM_BASE + 0x100c);
    otfc_ans[0] = inp32(CRPT_SRAM_BASE + 0x1000);
    otfc_ans[1] = inp32(CRPT_SRAM_BASE + 0x1004);
    otfc_ans[2] = inp32(CRPT_SRAM_BASE + 0x1008);
    otfc_ans[3] = inp32(CRPT_SRAM_BASE + 0x100c);

    printf("otfc_ans0 = %x\r\n", otfc_ans[0]);
    printf("otfc_ans1 = %x\r\n", otfc_ans[1]);
    printf("otfc_ans2 = %x\r\n", otfc_ans[2]);
    printf("otfc_ans3 = %x\r\n", otfc_ans[3]);
}
