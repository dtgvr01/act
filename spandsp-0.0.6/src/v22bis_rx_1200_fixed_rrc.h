#define RX_PULSESHAPER_1200_GAIN        32767.000000f
#define RX_PULSESHAPER_1200_COEFF_SETS  12
static const int16_t rx_pulseshaper_1200_re[RX_PULSESHAPER_1200_COEFF_SETS][37] =
{
    {
              19,     /* Filter 0 */
             125,
             162,
               0,
            -234,
            -269,
             -70,
              66,
             -65,
            -180,
             187,
             903,
            1071,
               0,
           -1676,
           -2284,
            -825,
            1681,
            2960,
            1739,
            -884,
           -2540,
           -1943,
               0,
            1379,
            1259,
             293,
            -356,
            -307,
             -38,
             -34,
            -216,
            -229,
               0,
             208,
             191,
              40
    },
    {
              21,     /* Filter 1 */
             131,
             166,
               0,
            -235,
            -266,
             -67,
              58,
             -83,
            -194,
             195,
             932,
            1096,
               0,
           -1700,
           -2308,
            -831,
            1688,
            2964,
            1737,
            -880,
           -2521,
           -1922,
               0,
            1354,
            1228,
             284,
            -340,
            -284,
             -28,
             -38,
            -222,
            -231,
               0,
             205,
             185,
              39
    },
    {
              22,     /* Filter 2 */
             136,
             171,
               0,
            -235,
            -264,
             -65,
              51,
            -101,
            -207,
             204,
             961,
            1122,
               0,
           -1723,
           -2331,
            -836,
            1695,
            2967,
            1734,
            -876,
           -2502,
           -1901,
               0,
            1328,
            1198,
             275,
            -324,
            -262,
             -18,
             -41,
            -228,
            -232,
               0,
             201,
             180,
              37
    },
    {
              24,     /* Filter 3 */
             142,
             175,
               0,
            -236,
            -260,
             -63,
              43,
            -120,
            -221,
             212,
             990,
            1148,
               0,
           -1747,
           -2354,
            -842,
            1701,
            2969,
            1730,
            -872,
           -2482,
           -1880,
               0,
            1303,
            1168,
             266,
            -309,
            -241,
              -8,
             -45,
            -234,
            -233,
               0,
             198,
             175,
              35
    },
    {
              26,     /* Filter 4 */
             148,
             179,
               0,
            -236,
            -257,
             -60,
              35,
            -139,
            -235,
             221,
            1020,
            1174,
               0,
           -1769,
           -2376,
            -847,
            1707,
            2971,
            1726,
            -867,
           -2462,
           -1858,
               0,
            1277,
            1138,
             256,
            -294,
            -220,
               0,
             -48,
            -239,
            -234,
               0,
             194,
             169,
              33
    },
    {
              28,     /* Filter 5 */
             153,
             183,
               0,
            -236,
            -253,
             -57,
              26,
            -159,
            -250,
             230,
            1049,
            1199,
               0,
           -1792,
           -2398,
            -853,
            1712,
            2972,
            1722,
            -862,
           -2441,
           -1837,
               0,
            1251,
            1108,
             247,
            -279,
            -199,
               9,
             -51,
            -244,
            -235,
               0,
             191,
             164,
              31
    },
    {
              29,     /* Filter 6 */
             159,
             187,
               0,
            -236,
            -249,
             -54,
              18,
            -178,
            -264,
             239,
            1079,
            1225,
               0,
           -1814,
           -2420,
            -858,
            1718,
            2972,
            1718,
            -858,
           -2420,
           -1814,
               0,
            1225,
            1079,
             239,
            -264,
            -178,
              18,
             -54,
            -249,
            -236,
               0,
             187,
             159,
              29
    },
    {
              31,     /* Filter 7 */
             164,
             191,
               0,
            -235,
            -244,
             -51,
               9,
            -199,
            -279,
             247,
            1108,
            1251,
               0,
           -1837,
           -2441,
            -862,
            1722,
            2972,
            1712,
            -853,
           -2398,
           -1792,
               0,
            1199,
            1049,
             230,
            -250,
            -159,
              26,
             -57,
            -253,
            -236,
               0,
             183,
             153,
              28
    },
    {
              33,     /* Filter 8 */
             169,
             194,
               0,
            -234,
            -239,
             -48,
               0,
            -220,
            -294,
             256,
            1138,
            1277,
               0,
           -1858,
           -2462,
            -867,
            1726,
            2971,
            1707,
            -847,
           -2376,
           -1769,
               0,
            1174,
            1020,
             221,
            -235,
            -139,
              35,
             -60,
            -257,
            -236,
               0,
             179,
             148,
              26
    },
    {
              35,     /* Filter 9 */
             175,
             198,
               0,
            -233,
            -234,
             -45,
              -8,
            -241,
            -309,
             266,
            1168,
            1303,
               0,
           -1880,
           -2482,
            -872,
            1730,
            2969,
            1701,
            -842,
           -2354,
           -1747,
               0,
            1148,
             990,
             212,
            -221,
            -120,
              43,
             -63,
            -260,
            -236,
               0,
             175,
             142,
              24
    },
    {
              37,     /* Filter 10 */
             180,
             201,
               0,
            -232,
            -228,
             -41,
             -18,
            -262,
            -324,
             275,
            1198,
            1328,
               0,
           -1901,
           -2502,
            -876,
            1734,
            2967,
            1695,
            -836,
           -2331,
           -1723,
               0,
            1122,
             961,
             204,
            -207,
            -101,
              51,
             -65,
            -264,
            -235,
               0,
             171,
             136,
              22
    },
    {
              39,     /* Filter 11 */
             185,
             205,
               0,
            -231,
            -222,
             -38,
             -28,
            -284,
            -340,
             284,
            1228,
            1354,
               0,
           -1922,
           -2521,
            -880,
            1737,
            2964,
            1688,
            -831,
           -2308,
           -1700,
               0,
            1096,
             932,
             195,
            -194,
             -83,
              58,
             -67,
            -266,
            -235,
               0,
             166,
             131,
              21
    }
};
static const int16_t rx_pulseshaper_1200_im[RX_PULSESHAPER_1200_COEFF_SETS][37] =
{
    {
             -59,     /* Filter 0 */
             -40,
             118,
             257,
             170,
             -87,
            -216,
             -90,
               0,
            -248,
            -576,
            -293,
             778,
            1705,
            1218,
            -742,
           -2540,
           -2314,
               0,
            2394,
            2721,
             825,
           -1411,
           -2072,
           -1002,
             409,
             903,
             490,
               0,
             -53,
             106,
              70,
            -166,
            -289,
            -151,
              62,
             125
    },
    {
             -65,     /* Filter 1 */
             -42,
             121,
             261,
             170,
             -86,
            -209,
             -80,
               0,
            -267,
            -602,
            -303,
             796,
            1737,
            1235,
            -749,
           -2558,
           -2324,
               0,
            2391,
            2709,
             819,
           -1396,
           -2043,
            -983,
             399,
             875,
             468,
               0,
             -39,
             118,
              72,
            -167,
            -288,
            -149,
              60,
             120
    },
    {
             -70,     /* Filter 2 */
             -44,
             124,
             265,
             171,
             -85,
            -201,
             -70,
               0,
            -285,
            -628,
            -312,
             815,
            1768,
            1252,
            -757,
           -2575,
           -2333,
               0,
            2387,
            2696,
             813,
           -1381,
           -2013,
            -965,
             389,
             847,
             447,
               0,
             -25,
             129,
              74,
            -168,
            -286,
            -146,
              58,
             114
    },
    {
             -75,     /* Filter 3 */
             -46,
             127,
             268,
             171,
             -84,
            -194,
             -59,
               0,
            -305,
            -654,
            -321,
             834,
            1799,
            1269,
            -764,
           -2593,
           -2342,
               0,
            2382,
            2683,
             806,
           -1366,
           -1983,
            -946,
             379,
             818,
             426,
               0,
             -12,
             139,
              76,
            -169,
            -284,
            -144,
              56,
             108
    },
    {
             -81,     /* Filter 4 */
             -48,
             130,
             271,
             171,
             -83,
            -186,
             -48,
               0,
            -324,
            -681,
            -331,
             853,
            1830,
            1285,
            -772,
           -2609,
           -2350,
               0,
            2376,
            2670,
             800,
           -1350,
           -1953,
            -928,
             370,
             790,
             405,
               0,
               0,
             149,
              77,
            -170,
            -282,
            -141,
              55,
             103
    },
    {
             -86,     /* Filter 5 */
             -49,
             133,
             274,
             171,
             -82,
            -177,
             -37,
               0,
            -344,
            -708,
            -341,
             871,
            1861,
            1302,
            -779,
           -2625,
           -2357,
               0,
            2371,
            2655,
             793,
           -1334,
           -1923,
            -909,
             360,
             763,
             384,
               0,
              13,
             159,
              79,
            -171,
            -280,
            -138,
              53,
              97
    },
    {
             -92,     /* Filter 6 */
             -51,
             136,
             277,
             171,
             -80,
            -168,
             -25,
               0,
            -364,
            -735,
            -350,
             890,
            1892,
            1318,
            -786,
           -2641,
           -2364,
               0,
            2364,
            2641,
             786,
           -1318,
           -1892,
            -890,
             350,
             735,
             364,
               0,
              25,
             168,
              80,
            -171,
            -277,
            -136,
              51,
              92
    },
    {
             -97,     /* Filter 7 */
             -53,
             138,
             280,
             171,
             -79,
            -159,
             -13,
               0,
            -384,
            -763,
            -360,
             909,
            1923,
            1334,
            -793,
           -2655,
           -2371,
               0,
            2357,
            2625,
             779,
           -1302,
           -1861,
            -871,
             341,
             708,
             344,
               0,
              37,
             177,
              82,
            -171,
            -274,
            -133,
              49,
              86
    },
    {
            -103,     /* Filter 8 */
             -55,
             141,
             282,
             170,
             -77,
            -149,
               0,
               0,
            -405,
            -790,
            -370,
             928,
            1953,
            1350,
            -800,
           -2670,
           -2376,
               0,
            2350,
            2609,
             772,
           -1285,
           -1830,
            -853,
             331,
             681,
             324,
               0,
              48,
             186,
              83,
            -171,
            -271,
            -130,
              48,
              81
    },
    {
            -108,     /* Filter 9 */
             -56,
             144,
             284,
             169,
             -76,
            -139,
              12,
               0,
            -426,
            -818,
            -379,
             946,
            1983,
            1366,
            -806,
           -2683,
           -2382,
               0,
            2342,
            2593,
             764,
           -1269,
           -1799,
            -834,
             321,
             654,
             305,
               0,
              59,
             194,
              84,
            -171,
            -268,
            -127,
              46,
              75
    },
    {
            -114,     /* Filter 10 */
             -58,
             146,
             286,
             168,
             -74,
            -129,
              25,
               0,
            -447,
            -847,
            -389,
             965,
            2013,
            1381,
            -813,
           -2696,
           -2387,
               0,
            2333,
            2575,
             757,
           -1252,
           -1768,
            -815,
             312,
             628,
             285,
               0,
              70,
             201,
              85,
            -171,
            -265,
            -124,
              44,
              70
    },
    {
            -120,     /* Filter 11 */
             -60,
             149,
             288,
             167,
             -72,
            -118,
              39,
               0,
            -468,
            -875,
            -399,
             983,
            2043,
            1396,
            -819,
           -2709,
           -2391,
               0,
            2324,
            2558,
             749,
           -1235,
           -1737,
            -796,
             303,
             602,
             267,
               0,
              80,
             209,
              86,
            -170,
            -261,
            -121,
              42,
              65
    }
};
