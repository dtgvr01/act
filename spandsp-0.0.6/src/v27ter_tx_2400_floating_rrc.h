#define TX_PULSESHAPER_2400_GAIN        1.000000f
#define TX_PULSESHAPER_2400_COEFF_SETS  20
static const float tx_pulseshaper_2400[TX_PULSESHAPER_2400_COEFF_SETS][9] =
{
    {
           0.0050262000f,     /* Filter 0 */
           0.0107704139f,
          -0.0150784957f,
          -0.0753922186f,
           0.5814534468f,
           0.5814534467f,
          -0.0753922186f,
          -0.0150784958f,
           0.0107704138f
    },
    {
           0.0036769615f,     /* Filter 1 */
           0.0132151788f,
          -0.0108416505f,
          -0.0962477546f,
           0.6703977440f,
           0.4915574819f,
          -0.0543875540f,
          -0.0179957590f,
           0.0079493141f
    },
    {
           0.0020271558f,     /* Filter 2 */
           0.0151310510f,
          -0.0054150757f,
          -0.1159725361f,
           0.7564987991f,
           0.4025543098f,
          -0.0341116997f,
          -0.0195425378f,
           0.0049156947f
    },
    {
           0.0001575810f,     /* Filter 3 */
           0.0163856892f,
           0.0009922305f,
          -0.1335090670f,
           0.8378713095f,
           0.3161906111f,
          -0.0153166439f,
          -0.0197430347f,
           0.0018355829f
    },
    {
          -0.0018345654f,     /* Filter 4 */
           0.0168753676f,
           0.0080958440f,
          -0.1477565768f,
           0.9126905920f,
           0.2340689766f,
           0.0013877594f,
          -0.0186894802f,
          -0.0011314547f
    },
    {
          -0.0038402663f,     /* Filter 5 */
           0.0165323368f,
           0.0155436576f,
          -0.1576073958f,
           0.9792460719f,
           0.1576074027f,
           0.0155436234f,
          -0.0165323579f,
          -0.0038401980f
    },
    {
          -0.0057441249f,     /* Filter 6 */
           0.0153307048f,
           0.0229275670f,
          -0.1619859170f,
           1.0359921022f,
           0.0880058111f,
           0.0268485018f,
          -0.0134685577f,
          -0.0061665144f
    },
    {
          -0.0074304100f,     /* Filter 7 */
           0.0132904398f,
           0.0297988399f,
          -0.1598887983f,
           1.0815943709f,
           0.0262205341f,
           0.0351527390f,
          -0.0097281388f,
          -0.0080126759f
    },
    {
          -0.0087894106f,     /* Filter 8 */
           0.0104791762f,
           0.0356867213f,
          -0.1504249558f,
           1.1149702967f,
          -0.0270525930f,
           0.0404511628f,
          -0.0055604096f,
          -0.0093110523f
    },
    {
          -0.0097237709f,     /* Filter 9 */
           0.0070115966f,
           0.0401196552f,
          -0.1328538467f,
           1.1353220123f,
          -0.0713862188f,
           0.0428697867f,
          -0.0012200205f,
          -0.0100260766f
    },
    {
          -0.0101544658f,     /* Filter 10 */
           0.0030462740f,
           0.0426483506f,
          -0.1066205506f,
           1.1421607836f,
          -0.1066205506f,
           0.0426483506f,
           0.0030462740f,
          -0.0101544658f
    },
    {
          -0.0100260766f,     /* Filter 11 */
          -0.0012200205f,
           0.0428697867f,
          -0.0713862187f,
           1.1353220123f,
          -0.1328538468f,
           0.0401196552f,
           0.0070115966f,
          -0.0097237709f
    },
    {
          -0.0093110523f,     /* Filter 12 */
          -0.0055604096f,
           0.0404511629f,
          -0.0270525929f,
           1.1149702967f,
          -0.1504249558f,
           0.0356867212f,
           0.0104791761f,
          -0.0087894106f
    },
    {
          -0.0080126759f,     /* Filter 13 */
          -0.0097281388f,
           0.0351527391f,
           0.0262205342f,
           1.0815943708f,
          -0.1598887984f,
           0.0297988399f,
           0.0132904397f,
          -0.0074304100f
    },
    {
          -0.0061665144f,     /* Filter 14 */
          -0.0134685577f,
           0.0268485019f,
           0.0880058111f,
           1.0359921022f,
          -0.1619859171f,
           0.0229275670f,
           0.0153307048f,
          -0.0057441249f
    },
    {
          -0.0038401980f,     /* Filter 15 */
          -0.0165323579f,
           0.0155436234f,
           0.1576074029f,
           0.9792460718f,
          -0.1576073958f,
           0.0155436575f,
           0.0165323368f,
          -0.0038402663f
    },
    {
          -0.0011314547f,     /* Filter 16 */
          -0.0186894801f,
           0.0013877595f,
           0.2340689767f,
           0.9126905919f,
          -0.1477565768f,
           0.0080958439f,
           0.0168753675f,
          -0.0018345654f
    },
    {
           0.0018355830f,     /* Filter 17 */
          -0.0197430346f,
          -0.0153166438f,
           0.3161906112f,
           0.8378713094f,
          -0.1335090671f,
           0.0009922304f,
           0.0163856892f,
           0.0001575810f
    },
    {
           0.0049156947f,     /* Filter 18 */
          -0.0195425377f,
          -0.0341116997f,
           0.4025543099f,
           0.7564987990f,
          -0.1159725361f,
          -0.0054150757f,
           0.0151310509f,
           0.0020271558f
    },
    {
           0.0079493141f,     /* Filter 19 */
          -0.0179957590f,
          -0.0543875540f,
           0.4915574821f,
           0.6703977439f,
          -0.0962477546f,
          -0.0108416506f,
           0.0132151788f,
           0.0036769615f
    }
};
