//11.3.24 dl2dbg  in git geladen
//
//--------------------------------------------------
/*

### Goal
The construction and programming of the SDR receiver was a learning exercise for me. 


MIT License

Copyright (c) [2024 [DL2DBG]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// library for the Adafruit  ILI9341
//https://github.com/PaulStoffregen/ILI9341_t3/blob/master/ILI9341_t3.h

#include <ILI9341_t3.h>
#include <font_Arial.h>  // from ILI9341_t3

// Encoder Library, for measuring quadrature encoded signals
//https://github.com/PaulStoffregen/Encoder/blob/master/Encoder.h
#include <Encoder.h>
//PIN encoder
Encoder myEnc(25, 26);


/* 
//PIN max98357 speaker amplifier
max98357: The speaker amplifier must be connected to the following pin.
The I2S signals are used in "master" mode, where Teensy creates all 3 clock signals and controls all data timing.
Teensy 4.x
Pin	Signal	Direction
4	  BCLK	Output
(33	MCLK	Output) //is not connectet
2	  TX  	Output
3	  LRCLK	Output
*/


// Arduino LIB Etherkit Si5351 Jason Milldrum  v2.1.4
#include <si5351.h>

// Start frequency and min max limits
signed long long freq_si = 7030000;      // start frequenz
signed long long freq_si_min = 7000000;  // start frequenz
signed long long freq_si_max = 7300000;  // start frequenz

// Parameters for the rotary encoder menu
const char* encoder_parameters[] = { "Freq", "Step", "Head", "Line Gain", "I/Q Gain", "Speaker", "" };
int encoder_values[] = { 0, 2, 20, 14, 47, 2, 00 };
int encoder_colors[] = { ILI9341_BLUE, ILI9341_GREEN, ILI9341_GREEN, ILI9341_GREEN, ILI9341_GREEN, ILI9341_GREEN };

//create a class Si5351

Si5351 si5351;

void setup_SI5351() {
  //http://zl2ctm.blogspot.com/2019/06/homebrew-804020m-ssb-sdr-phasing-rig.html
  // copy and  some changes
  // from
  //https://github.com/etherkit/si5351-avr-tiny-minimal/blob/master/README.md
  // Start serial and initialize the Si5351

  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);

  // We will output 7.03 MHz on CLK0 and CLK1.
  // A PLLA frequency of 705 MHz was chosen to give an even
  // divisor by 14.1 MHz.
  unsigned long long freq = 70300000000ULL;  //cw Freq :-)
  unsigned long long pll_freq = 70500000000ULL;
  // si5351.set_correction(-6190, SI5351_PLL_INPUT_XO);
  si5351.set_correction(+26000, SI5351_PLL_INPUT_XO);  // set_correction is required during initial setup

  // Set CLK0 and CLK1 to use PLLA as the MS source.
  // This is not explicitly necessary in v2 of this library,
  // as these are already the default assignments.
  // si5351.set_ms_source(SI5351_CLK0, SI5351_PLLA);
  // si5351.set_ms_source(SI5351_CLK1, SI5351_PLLA);

  // Set CLK0 and CLK1 to output 14.1 MHz with a fixed PLL frequency
  si5351.set_freq_manual(freq, pll_freq, SI5351_CLK0);
  si5351.set_freq_manual(freq, pll_freq, SI5351_CLK1);

  // Now we can set CLK1 to have a 90 deg phase shift by entering
  // 50 in the CLK1 phase register, since the ratio of the PLL to
  // the clock frequency is 50.
  si5351.set_phase(SI5351_CLK0, 0);
  si5351.set_phase(SI5351_CLK1, 50);

  // We need to reset the PLL before they will be in phase alignment
  si5351.pll_reset(SI5351_PLLA);

  // Query a status update and wait a bit to let the Si5351 populate the
  // status flags correctly.
}

void set_vfo_si5351(signed long long freq) {
  signed long long pll_freq = 70500000000ULL;

  si5351.set_freq_manual(freq * 100, pll_freq, SI5351_CLK0);
  si5351.set_freq_manual(freq * 100, pll_freq, SI5351_CLK1);
  si5351.pll_reset(SI5351_PLLA);
  si5351.pll_reset(SI5351_PLLA);
}


// Create your "SDR design" using the tool 'https://www.pjrc.com/teensy/' and copy the configuration into the C code.
// Design Tool code www.pjrc.com/teensy
// https://www.pjrc.com/teensy/gui/
//
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=119.75,250
AudioAmplifier           amp2;           //xy=293.75,633
AudioAmplifier           amp1;           //xy=302.75,533
AudioFilterFIR           RX_Hilbert_Plus_45; //xy=475.75,533
AudioFilterFIR           RX_Hilbert_Minus_45; //xy=480.75,634
AudioMixer4              mixer3;         //xy=716.75,548
AudioMixer4              mixer4;         //xy=718.75,631
AudioMixer4              mixer1;         //xy=728.75,179
AudioFilterStateVariable filter1;        //xy=849.75,395
AudioFilterFIR           BPF;            //xy=888.75,181
AudioFilterBiquad        biquad1;        //xy=888.75,327.75
AudioAnalyzeFFT1024      fft1024_3;      //xy=927.75,546
AudioAnalyzeFFT1024      fft1024_2;      //xy=932.75,630
AudioMixer4              mixer2;         //xy=1050.75,346
AudioAmplifier           amp3;           //xy=1161.75,169
AudioOutputI2S           i2s1;           //xy=1209.75,338
AudioAnalyzeRMS          rms1;           //xy=1214.75,406
AudioAnalyzeFFT1024      fft1024_1;      //xy=1217.75,519
AudioOutputI2S2          i2s2_1;         //xy=1304.75,257
AudioConnection          patchCord1(i2s2, 0, mixer1, 0);
AudioConnection          patchCord2(i2s2, 0, amp1, 0);
AudioConnection          patchCord3(i2s2, 1, mixer1, 1);
AudioConnection          patchCord4(i2s2, 1, amp2, 0);
AudioConnection          patchCord5(amp2, RX_Hilbert_Minus_45);
AudioConnection          patchCord6(amp1, RX_Hilbert_Plus_45);
AudioConnection          patchCord7(RX_Hilbert_Plus_45, 0, mixer3, 0);
AudioConnection          patchCord8(RX_Hilbert_Plus_45, 0, mixer1, 2);
AudioConnection          patchCord9(RX_Hilbert_Plus_45, 0, mixer4, 0);
AudioConnection          patchCord10(RX_Hilbert_Minus_45, 0, mixer3, 1);
AudioConnection          patchCord11(RX_Hilbert_Minus_45, 0, mixer4, 1);
AudioConnection          patchCord12(mixer3, fft1024_3);
AudioConnection          patchCord13(mixer4, fft1024_2);
AudioConnection          patchCord14(mixer4, 0, mixer1, 3);
AudioConnection          patchCord15(mixer1, BPF);
AudioConnection          patchCord16(mixer1, 0, filter1, 0);
AudioConnection          patchCord17(mixer1, biquad1);
AudioConnection          patchCord18(filter1, 0, mixer2, 2);
AudioConnection          patchCord19(filter1, 1, mixer2, 3);
AudioConnection          patchCord20(BPF, 0, mixer2, 0);
AudioConnection          patchCord21(biquad1, 0, mixer2, 1);
AudioConnection          patchCord22(mixer2, fft1024_1);
AudioConnection          patchCord23(mixer2, 0, i2s1, 0);
AudioConnection          patchCord24(mixer2, 0, i2s1, 1);
AudioConnection          patchCord25(mixer2, rms1);
AudioConnection          patchCord26(mixer2, amp3);
AudioConnection          patchCord27(amp3, 0, i2s2_1, 0);
AudioConnection          patchCord28(amp3, 0, i2s2_1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=129.75,188
// GUItool: end automatically generated code


// end automatically generated code
/*

FIR filter designed with
 http://t-filter.appspot.com
*/

// Filer ---------------------------------------------------------------------------------

// This is a FIR filter, built at http://t-filter.engineerjs.com/, with the
// inputs:
//
//  Sampling Freq: 44117
//  From:    0Hz To:   150Hz Gain: 0 Att: -30dB
//  From:  350Hz To:   850Hz Gain: 1 Att:   5dB
//  From: 1050Hz To: 22050Hz Gain: 0 Att: -30dB
//

#define FILTER_TAP_NUM 200

static short filter_taps[FILTER_TAP_NUM] = { 676, -10, -10, -10, -10,
                                             -10, -11, -11, -10, -10, -9, -8, -7, -5, -2, 1, 5, 9, 14, 20, 26,
                                             33, 41, 48, 56, 64, 72, 80, 88, 95, 101, 106, 110, 112, 113, 112,
                                             109, 104, 97, 86, 74, 58, 40, 20, -4, -30, -59, -89, -122, -157,
                                             -194, -231, -270, -309, -348, -386, -424, -460, -495, -527, -556,
                                             -582, -604, -622, -635, -643, -646, -644, -635, -620, -600, -573,
                                             -540, -501, -456, -406, -351, -291, -225, -158, -87, -12, 65, 143,
                                             221, 299, 377, 452, 526, 596, 662, 724, 781, 832, 877, 915, 946,
                                             969, 985, 993, 993, 985, 969, 946, 915, 877, 832, 781, 724, 662,
                                             596, 526, 452, 377, 299, 221, 143, 65, -12, -87, -158, -225, -291,
                                             -351, -406, -456, -501, -540, -573, -600, -620, -635, -644, -646,
                                             -643, -635, -622, -604, -582, -556, -527, -495, -460, -424, -386,
                                             -348, -309, -270, -231, -194, -157, -122, -89, -59, -30, -4, 20, 40,
                                             58, 74, 86, 97, 104, 109, 112, 113, 112, 110, 106, 101, 95, 88, 80,
                                             72, 64, 56, 48, 41, 33, 26, 20, 14, 9, 5, 1, -2, -5, -7, -8, -9,
                                             -10, -10, -11, -11, -10, -10, -10, -10, -10, 676 };

// For the SDR, the IQ phase of the signal is delayed by 45 degrees using two Hilbert filters.
// Hilbert_Plus_45_Coeffs
// Hilbert_Minus_45_Coeffs

#define NO_HILBERT_COEFFS 100
// Used to define the Hilbert transform filter arrays. More typical than 'const int'.
// Iowa Hills Hilbert transform filter coefficients
// copy from
//http://zl2ctm.blogspot.com/2019/06/homebrew-804020m-ssb-sdr-phasing-rig.html

const short Hilbert_Plus_45_Coeffs[NO_HILBERT_COEFFS] = {
  (short)(32768 * 483.9743406915770E-9),
  (short)(32768 * 1.866685817417670E-6),
  (short)(32768 * 4.392570072495770E-6),
  (short)(32768 * 8.820636712774380E-6),
  (short)(32768 * 0.000016184764415848),
  (short)(32768 * 0.000027249288101724),
  (short)(32768 * 0.000041920439805110),
  (short)(32768 * 0.000059016345361008),
  (short)(32768 * 0.000076756191268614),
  (short)(32768 * 0.000094055071776657),
  (short)(32768 * 0.000112261583662573),
  (short)(32768 * 0.000136518354793722),
  (short)(32768 * 0.000175696544399550),
  (short)(32768 * 0.000240067250940666),
  (short)(32768 * 0.000336609833081347),
  (short)(32768 * 0.000462990951098187),
  (short)(32768 * 0.000602421399161912),
  (short)(32768 * 0.000722301614841713),
  (short)(32768 * 0.000779305755125654),
  (short)(32768 * 0.000732056920489209),
  (short)(32768 * 0.000559948250896998),
  (short)(32768 * 0.000283585816755988),
  (short)(32768 * -0.000020215761694049),
  (short)(32768 * -0.000216819118897174),
  (short)(32768 * -0.000131758715732279),
  (short)(32768 * 0.000409948902196685),
  (short)(32768 * 0.001528130963104050),
  (short)(32768 * 0.003227564604759798),
  (short)(32768 * 0.005351504118228274),
  (short)(32768 * 0.007567804706902511),
  (short)(32768 * 0.009402820162611196),
  (short)(32768 * 0.010328512135999630),
  (short)(32768 * 0.009894246835039863),
  (short)(32768 * 0.007879714166593881),
  (short)(32768 * 0.004433421126740721),
  (short)(32768 * 0.000156366633965185),
  (short)(32768 * -0.003904389468521576),
  (short)(32768 * -0.006371988171406650),
  (short)(32768 * -0.005761887634323113),
  (short)(32768 * -0.000778938771957753),
  (short)(32768 * 0.009365085367419172),
  (short)(32768 * 0.024681757404317366),
  (short)(32768 * 0.044251382160327521),
  (short)(32768 * 0.066233642104189930),
  (short)(32768 * 0.088062621169129954),
  (short)(32768 * 0.106806616459214951),
  (short)(32768 * 0.119635362035632908),
  (short)(32768 * 0.124309482163432433),
  (short)(32768 * 0.119596382589365807),
  (short)(32768 * 0.105526834497225247),
  (short)(32768 * 0.083435851600156402),
  (short)(32768 * 0.055774084237545388),
  (short)(32768 * 0.025722508803868269),
  (short)(32768 * -0.003316774367731974),
  (short)(32768 * -0.028256325810852603),
  (short)(32768 * -0.046784860848984686),
  (short)(32768 * -0.057671422122216751),
  (short)(32768 * -0.060863961385426720),
  (short)(32768 * -0.057377469068775784),
  (short)(32768 * -0.049008885222883866),
  (short)(32768 * -0.037947605693328487),
  (short)(32768 * -0.026365178611104038),
  (short)(32768 * -0.016063741251826878),
  (short)(32768 * -0.008242320709669780),
  (short)(32768 * -0.003409329314875374),
  (short)(32768 * -0.001436295024424050),
  (short)(32768 * -0.001719754923178513),
  (short)(32768 * -0.003400970055132929),
  (short)(32768 * -0.005589187214751837),
  (short)(32768 * -0.007542651980327935),
  (short)(32768 * -0.008778747041127889),
  (short)(32768 * -0.009105231860961261),
  (short)(32768 * -0.008583286966676333),
  (short)(32768 * -0.007445876442758468),
  (short)(32768 * -0.005999873442098177),
  (short)(32768 * -0.004537732597630097),
  (short)(32768 * -0.003276341625911221),
  (short)(32768 * -0.002330120643710241),
  (short)(32768 * -0.001715559019593159),
  (short)(32768 * -0.001377498498508186),
  (short)(32768 * -0.001224659509681699),
  (short)(32768 * -0.001162953813794283),
  (short)(32768 * -0.001118850197066992),
  (short)(32768 * -0.001049829425495061),
  (short)(32768 * -0.000943240226180188),
  (short)(32768 * -0.000807608474652057),
  (short)(32768 * -0.000661296218869203),
  (short)(32768 * -0.000522622836450865),
  (short)(32768 * -0.000403818527830429),
  (short)(32768 * -0.000309261220275339),
  (short)(32768 * -0.000236981246010256),
  (short)(32768 * -0.000181726899678497),
  (short)(32768 * -0.000137960767232952),
  (short)(32768 * -0.000101749677323657),
  (short)(32768 * -0.000071268112804006),
  (short)(32768 * -0.000046246069278189),
  (short)(32768 * -0.000026984224473470),
  (short)(32768 * -0.000013519855860183),
  (short)(32768 * -5.268419079329310E-6),
  (short)(32768 * -1.152120275972750E-6)
};

// Iowa Hills Hilbert transform filter coefficients
const short Hilbert_Minus_45_Coeffs[NO_HILBERT_COEFFS] = {
  (short)(32768 * -1.152120275972720E-6),
  (short)(32768 * -5.268419079329100E-6),
  (short)(32768 * -0.000013519855860182),
  (short)(32768 * -0.000026984224473469),
  (short)(32768 * -0.000046246069278187),
  (short)(32768 * -0.000071268112804004),
  (short)(32768 * -0.000101749677323656),
  (short)(32768 * -0.000137960767232950),
  (short)(32768 * -0.000181726899678496),
  (short)(32768 * -0.000236981246010254),
  (short)(32768 * -0.000309261220275334),
  (short)(32768 * -0.000403818527830420),
  (short)(32768 * -0.000522622836450852),
  (short)(32768 * -0.000661296218869189),
  (short)(32768 * -0.000807608474652046),
  (short)(32768 * -0.000943240226180185),
  (short)(32768 * -0.001049829425495072),
  (short)(32768 * -0.001118850197067017),
  (short)(32768 * -0.001162953813794315),
  (short)(32768 * -0.001224659509681719),
  (short)(32768 * -0.001377498498508169),
  (short)(32768 * -0.001715559019593075),
  (short)(32768 * -0.002330120643710066),
  (short)(32768 * -0.003276341625910951),
  (short)(32768 * -0.004537732597629757),
  (short)(32768 * -0.005999873442097817),
  (short)(32768 * -0.007445876442758181),
  (short)(32768 * -0.008583286966676212),
  (short)(32768 * -0.009105231860961396),
  (short)(32768 * -0.008778747041128323),
  (short)(32768 * -0.007542651980328638),
  (short)(32768 * -0.005589187214752684),
  (short)(32768 * -0.003400970055133710),
  (short)(32768 * -0.001719754923178943),
  (short)(32768 * -0.001436295024423829),
  (short)(32768 * -0.003409329314874256),
  (short)(32768 * -0.008242320709667652),
  (short)(32768 * -0.016063741251823811),
  (short)(32768 * -0.026365178611100354),
  (short)(32768 * -0.037947605693324706),
  (short)(32768 * -0.049008885222880681),
  (short)(32768 * -0.057377469068773987),
  (short)(32768 * -0.060863961385426962),
  (short)(32768 * -0.057671422122219533),
  (short)(32768 * -0.046784860848990188),
  (short)(32768 * -0.028256325810860565),
  (short)(32768 * -0.003316774367741772),
  (short)(32768 * 0.025722508803857579),
  (short)(32768 * 0.055774084237534945),
  (short)(32768 * 0.083435851600147395),
  (short)(32768 * 0.105526834497218600),
  (short)(32768 * 0.119596382589362227),
  (short)(32768 * 0.124309482163432142),
  (short)(32768 * 0.119635362035635753),
  (short)(32768 * 0.106806616459220294),
  (short)(32768 * 0.088062621169136893),
  (short)(32768 * 0.066233642104197507),
  (short)(32768 * 0.044251382160334737),
  (short)(32768 * 0.024681757404323448),
  (short)(32768 * 0.009365085367423625),
  (short)(32768 * -0.000778938771955104),
  (short)(32768 * -0.005761887634322139),
  (short)(32768 * -0.006371988171407000),
  (short)(32768 * -0.003904389468522771),
  (short)(32768 * 0.000156366633963640),
  (short)(32768 * 0.004433421126739255),
  (short)(32768 * 0.007879714166592786),
  (short)(32768 * 0.009894246835039278),
  (short)(32768 * 0.010328512135999562),
  (short)(32768 * 0.009402820162611529),
  (short)(32768 * 0.007567804706903086),
  (short)(32768 * 0.005351504118228925),
  (short)(32768 * 0.003227564604760380),
  (short)(32768 * 0.001528130963104480),
  (short)(32768 * 0.000409948902196933),
  (short)(32768 * -0.000131758715732195),
  (short)(32768 * -0.000216819118897210),
  (short)(32768 * -0.000020215761694147),
  (short)(32768 * 0.000283585816755878),
  (short)(32768 * 0.000559948250896909),
  (short)(32768 * 0.000732056920489157),
  (short)(32768 * 0.000779305755125639),
  (short)(32768 * 0.000722301614841724),
  (short)(32768 * 0.000602421399161936),
  (short)(32768 * 0.000462990951098213),
  (short)(32768 * 0.000336609833081367),
  (short)(32768 * 0.000240067250940678),
  (short)(32768 * 0.000175696544399555),
  (short)(32768 * 0.000136518354793723),
  (short)(32768 * 0.000112261583662573),
  (short)(32768 * 0.000094055071776658),
  (short)(32768 * 0.000076756191268616),
  (short)(32768 * 0.000059016345361010),
  (short)(32768 * 0.000041920439805112),
  (short)(32768 * 0.000027249288101726),
  (short)(32768 * 0.000016184764415849),
  (short)(32768 * 8.820636712774440E-6),
  (short)(32768 * 4.392570072495580E-6),
  (short)(32768 * 1.866685817417500E-6),
  (short)(32768 * 483.9743406915230E-9)
};


//https://www.pjrc.com/teensy/td_libs_Bounce.html
#include <Bounce.h>
//Definition of Teensy hardware pin.

Bounce button1 = Bounce(41, 15);  // 15 = 15 ms debounce time
Bounce button2 = Bounce(40, 15);

//Definition of Teensy hardware pin.
Bounce button_encoder = Bounce(24, 15);

// Use these with the Teensy 4.x and Audio Shield Rev D or D2
// Color 320x240 TFT Touchscreen, ILI9341 Controller Chip
//PIN TFT Touchscreen Definition of Teensy hardware pin.
#define TFT_DC 9
#define TFT_CS 22
#define TFT_RST 255  // 255 = unused, connect to 3.3V
#define TFT_MOSI 11
#define TFT_SCLK 13
#define TFT_MISO 12

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);

// Use these with the Teensy Audio Shield
//PIN Audio Shield Definition of Teensy hardware pin.
#define SDCARD_CS_PIN 10
#define SDCARD_MOSI_PIN 7  // Teensy 4 ignores this, uses pin 11
#define SDCARD_SCK_PIN 14  // Teensy 4 ignores this, uses pin 13


// The display size and color to use
const unsigned int matrix_width = 29;
const unsigned int matrix_height = 12;

const unsigned int matrix_width_pixel = 320;
const unsigned int matrix_height_pixel = 240;


void show_scale(void) {
  //int y_base = matrix_height_pixel;
  int y_base = matrix_height_pixel - 8;

  for (int16_t i = 0; i < 256; i += 23)  ///12 bei 4 or 23 bei 1

    if (i % 50 == 0) {                                  // Check if i is divisible by 50
      tft.drawFastVLine(i, y_base, 8, ILI9341_YELLOW);  // Draw a longer line
    } else {
      tft.drawFastVLine(i, y_base, 3, ILI9341_YELLOW);  // Draw a shorter line
    }
}


// draw the spectrum display
void show_spectrum(void) {

  int y_base = matrix_height_pixel;

  int scale = 1;
  for (int16_t i = 0; i < 100; i += 1) {
    float bar = fft1024_1.read(i * 1, i * 1 + 1) * 1024 / 4 / scale;
    //Serial.println(bar);
    if (bar > 150) bar = 150;  // begrenzen

    tft.drawFastVLine(i, y_base - bar - 12, bar, ILI9341_GREEN);       //zeichne Linie
    tft.drawFastVLine(i, y_base - 12 - 150, 150 - bar, ILI9341_BLUE);  //zeichne den rest der Line, damit wird gelöscht
  }
}

// FFT show_spectrum2 USB (Upper Sideband) und LSB (Lower Sideband)
// draw the spectrum display
// For the LSB, the FFT data will be read backwards, those are the mirrored frequencies.
// read(70-i*1,70-i) -> 70-i

void show_spectrum2(void) {

  int y_base = matrix_height_pixel;

  int scale = 1;
  for (int16_t i = 0; i < 70; i += 1) {
    float bar = fft1024_2.read(70 - i * 1, 70 - i) * 1024 / 4 / scale;
    //Serial.println(bar);
    if (bar > 150) bar = 150;  // begrenzen

    tft.drawFastVLine(110 + i, y_base - bar - 12, bar, ILI9341_GREEN);       //zeichne Linie
    tft.drawFastVLine(110 + i, y_base - 12 - 150, 150 - bar, ILI9341_BLUE);  //zeichne den rest der Line, damit wird gelöscht
  }
}
// draw the spectrum display
void show_spectrum3(void) {

  int y_base = matrix_height_pixel;

  int scale = 1;
  for (int16_t i = 0; i < 70; i += 1) {
    float bar = fft1024_3.read(i * 1, i) * 1024 / 4 / scale;
    //Serial.println(bar);
    if (bar > 150) bar = 150;  // begrenzen

    tft.drawFastVLine(110 + 70 + 5 + i, y_base - bar - 12, bar, ILI9341_GREEN);       //zeichne Linie
    tft.drawFastVLine(110 + 70 + 5 + i, y_base - 12 - 150, 150 - bar, ILI9341_BLUE);  //zeichne den rest der Line, damit wird gelöscht
  }
}

// just as an overview which character sets are available
/* zeichensatz grössen
    Arial_9
    Arial_12
    Arial_18
    Arial_24
    Arial_36
    Arial_48
*/
void TFT_Print_headline(String txt) {

  tft.setTextColor(ILI9341_YELLOW);
  tft.setFont(Arial_9);
  tft.setRotation(1);  // Make sure it's in the same rotation
  tft.fillRect(2, 5, 200, 11, ILI9341_BLACK);
  tft.setCursor(2, 5);
  tft.println(txt);
}

void TFT_Print_menue_a(String txt, int i) {
  tft.setCursor(280, 80);
  tft.fillRect(280, 80, 50, 11, ILI9341_BLACK);
  tft.print(txt);
  tft.print(i);
}
void TFT_Print_menue_b(String txt, int i) {
  tft.setCursor(280, 92);
  tft.fillRect(280, 92, 50, 11, ILI9341_BLACK);
  tft.print(txt);
  tft.print(i);
}

void TFT_Print_headline_spectrum(String txt) {

  tft.setTextColor(ILI9341_YELLOW);
  tft.setFont(Arial_9);
  tft.setRotation(1);  // Make sure it's in the same rotation
  tft.fillRect(2, 65, 200, 11, ILI9341_BLACK);
  tft.setCursor(2, 65);
  tft.println(txt);
}


void TFT_Print_txt(int v, const char* txt, int clour) {
  tft.setTextColor(clour);  //ILI9341_YELLOW
  if (clour == ILI9341_BLUE) {
    tft.fillRect(200, 12 * v + 1, 280, 11, ILI9341_YELLOW);
  } else {
    tft.fillRect(200, 12 * v + 1, 280, 11, ILI9341_BLACK);
  };
  tft.setCursor(200, 12 * v + 1);
  tft.println(txt);
  tft.setTextColor(ILI9341_GREEN);
}



void TFT_Print_value(int v, float wert, int clour) {
  tft.setTextColor(clour);  //ILI9341_YELLOW
  if (clour == ILI9341_BLUE) {
    tft.fillRect(280, 12 * v + 1, 300, 11, ILI9341_YELLOW);
  } else {
    tft.fillRect(280, 12 * v + 1, 300, 11, ILI9341_BLACK);
  };
  tft.setCursor(280, 12 * v + 1);
  tft.println(wert);
  tft.setTextColor(ILI9341_GREEN);
}


void Print_value() {
  // Print values with corresponding colors
  for (int i = 0; i < 6; ++i) {
    TFT_Print_txt(i, encoder_parameters[i], encoder_colors[i]);
    TFT_Print_value(i, encoder_values[i], encoder_colors[i]);
  }
  TFT_Print_value_freq(freq_si, encoder_values[1]);
}



void TFT_Print_value_freq(float wert, int step) {
#define xx 60
#define yy 40
  char txt[20];
  sprintf(txt, "%.0f\n", wert);

  tft.setFont(Arial_18);

  tft.fillRect(xx, yy + 3 + 18, 120, 2, ILI9341_BLACK);
  tft.fillRect((xx + (7 - step) * 14), yy + 3 + 18, 13, 2, ILI9341_YELLOW);
  tft.fillRect((xx + 56), yy + 1 + 18, 2, 2, ILI9341_YELLOW);
  tft.fillRect((xx + 13), yy + 1 + 18, 2, 2, ILI9341_YELLOW);

  tft.fillRect(xx, yy, 120, 18, ILI9341_BLACK);
  tft.setCursor(xx, yy);

  tft.println(txt);
  tft.setTextColor(ILI9341_GREEN);
  tft.setFont(Arial_9);
}

void setMixer1Gain(int input) {  //Depending on the input, the mixer.gain will be set to 1.0 and the others to 0.0.
  for (int i = 0; i < 4; i++) {
    mixer1.gain(i, (i == input) ? 1.0 : 0.0);
  }
  TFT_Print_menue_a("mix1:", input);
}

void setMixer2Gain(int input) {
  for (int i = 0; i < 4; i++) {
    mixer2.gain(i, (i == input) ? 1.0 : 0.0);
  }
  TFT_Print_menue_b("mix2:", input);
}



//https://www.pjrc.com/teensy/td_timing_elaspedMillis.html
//These special variable types automatically increase as time elapses.
//It's easy to check if a certain time has elapsed, while your program performs other work or checks for user input.
elapsedMillis msecs;
elapsedMillis msecs_encoder;

//Definition and setting of the state variables for the button und encoder menu.
int mixer1_setting = 3;   // mixer/ button // 3 is LSB
int mixer2_setting = 0;   // mixer/ button // 0 is Filer BPF
int encoder_setting = 0;  //encoder Value


// --------- setup setup setup --------------------------
void setup() {

  delay(250);
  AudioMemory(28);
  delay(250);

  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(0.01);

  //gain must be less than 1.0
  //at 1.0, the runtime in the amp is less than at <1.0,
  //so the addition of the IQ signals is not zero. the sideband suppression is then poor.
  //  amp1.gain (0.50);
  amp2.gain(0.90);
  amp1.gain(0.01f * encoder_values[4]);  // Set the value from the table encoder_values.

  amp3.gain(0.01f * encoder_values[5]);       // Set the value speaker, from the table encoder_values.
  sgtl5000_1.lineInLevel(encoder_values[3]);  // Set the value from the table encoder_values.



  // copy from
  //https://github.com/DD4WH/Teensy-SDR-Rx/blob/master/Teensy_SDR.ino
  // The comment was adopted.
  // codec hardware AGC works but it looks at the entire input bandwidth
  // ie codec HW AGC works on the strongest signal, not necessarily what you are listening to
  // it should work well for ALC (mic input) though

  // At this point, keep it simple and use what is available :-)

#define HW_AGC  // define for codec AGC
#ifdef HW_AGC
  /* COMMENTS FROM Teensy Audio library:
    Valid values for dap_avc parameters
	maxGain; Maximum gain that can be applied
	0 - 0 dB
	1 - 6.0 dB
	2 - 12 dB
	lbiResponse; Integrator Response
	0 - 0 mS
	1 - 25 mS
	2 - 50 mS
	3 - 100 mS
	hardLimit
	0 - Hard limit disabled. AVC Compressor/Expander enabled.
	1 - Hard limit enabled. The signal is limited to the programmed threshold (signal saturates at the threshold)
	threshold
	floating point in range 0 to -96 dB
	attack
	floating point figure is dB/s rate at which gain is increased
	decay
	floating point figure is dB/s rate at which gain is reduced
*/
  sgtl5000_1.autoVolumeControl(2, 1, 0, -30.0, 3.0, 70.0);  // see comments above
  sgtl5000_1.autoVolumeEnable();
#endif

  Serial.begin(19200);
  delay(50);

  tft.setClock(16000000);
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);

  TFT_Print_headline(__DATE__ " | " __TIME__);  // show compile date/time to check

  //PIN encoder
  //PIN button
  pinMode(0, INPUT_PULLUP);   //encoder
  pinMode(40, INPUT_PULLUP);  //button
  pinMode(41, INPUT_PULLUP);  //button


  BPF.begin(filter_taps, FILTER_TAP_NUM);  // tft.setRotation(1); // Make sure it's in the same rotation
                                           // int x_base = matrix_width_pixel;

  RX_Hilbert_Plus_45.begin(Hilbert_Plus_45_Coeffs, NO_HILBERT_COEFFS);
  RX_Hilbert_Minus_45.begin(Hilbert_Minus_45_Coeffs, NO_HILBERT_COEFFS);




  // Mixers1-2 are set via the function."
  setMixer1Gain(mixer1_setting);
  setMixer2Gain(mixer2_setting);

  mixer3.gain(0, 1.0);  //  The mixer was placed after the Hilbert filter.
  mixer3.gain(1, -1.0);

  mixer4.gain(0, 1.0);
  mixer4.gain(1, 1.0);

  show_scale();

  // Windowing in FFT lingo refers to algorithms that limit the data so that it stays within a range (or window) of bins.
  //https://protosupplies.com/learn/audio-tutorials-and-workshop/audio-workshop-14/

  fft1024_1.windowFunction(AudioWindowHanning1024);
  fft1024_2.windowFunction(AudioWindowHanning1024);
  fft1024_3.windowFunction(AudioWindowHanning1024);

  //PIN SI5351  <Wire.h>  Hardware PIN  18 , 19 default
  setup_SI5351();
  set_vfo_si5351(freq_si);

  Print_value();

  delay(1000);                            //Wait to read the startup message.
  TFT_Print_headline("SDR Teensy kiss");  //SDR: Keep it Simple, Kiss

  TFT_Print_headline_spectrum("FFT SDR Audio     LSB / USB ");
}

// --------- loop loop loop --------------------------

void loop() {

  static long oldPosition = 0;  // Static variable, to maintain the old value between function calls.

  long newPosition_long = myEnc.read();  // Read the rotary encoder as well and notice or compare it with the old value.
  int newPosition = newPosition_long;

  if (newPosition != oldPosition) {  //and notice or compare it with the old value.
    oldPosition = newPosition;

    switch (encoder_setting) {
      case 0:  // freq

        if (abs(newPosition) >= 4) {  // nur agieren bei jeden 4tem


          int direction = 1;
          if (newPosition < 0) { direction = -1; }

          int step_speed = 1;  // The step size of the rotary encoder is adjusted based on the speed.

          if (msecs_encoder > 300) {
            msecs_encoder = 0;
            step_speed = 1;
          }  //Three different times are being evaluated.
          else if (msecs_encoder > 100) {
            msecs_encoder = 0;
            step_speed = 10;
          } else if (msecs_encoder > 50) {
            msecs_encoder = 0;
            step_speed = 100;
          } else {
            msecs_encoder = 0;
            step_speed = 1000;
          }


          // TFT_Print_value(14, step_speed , ILI9341_GREEN);   //Control display indicating which step size was used."


          signed long long step = pow(10, encoder_values[1] - 1);  //The step size can also be adjusted via the menu

          freq_si = freq_si + step_speed * step * direction;


          // TFT_Print_value(12, encoder_values[1] , ILI9341_GREEN);//Control display indicating which step size was used."
          // TFT_Print_value(13, step , ILI9341_GREEN); //Control display indicating which step size was used."

          //Checking the frequency band limits
          if (freq_si <= freq_si_min) { freq_si = freq_si_min; }
          if (freq_si >= freq_si_max) { freq_si = freq_si_max; }

          TFT_Print_value_freq(freq_si, encoder_values[1]);

          set_vfo_si5351(freq_si);

          //Clear the encoder. The encoder is reset after each query."
          newPosition = 0;
          myEnc.write(0);
          encoder_values[encoder_setting] = 0;
        };
        break;

      case 1:  // step
        if (newPosition % 4 == 0) {
          newPosition = newPosition / 4;  // max step is 7 -> Mhz
          if (encoder_values[encoder_setting] + newPosition >= 7) {
            encoder_values[encoder_setting] = 7;
            newPosition = 0;
            myEnc.write(0);
          }
          if (encoder_values[encoder_setting] + newPosition < 1) {
            encoder_values[encoder_setting] = 1;
            newPosition = 0;
            myEnc.write(0);
          }
          TFT_Print_value(encoder_setting, encoder_values[encoder_setting] + newPosition, encoder_colors[encoder_setting]);
          TFT_Print_value_freq(freq_si, encoder_values[1] + newPosition);
        };
        break;
      case 3:  // line gain
        if (newPosition % 4 == 0) {
          newPosition = newPosition / 4;
          if (encoder_values[encoder_setting] + newPosition >= 15) {
            encoder_values[encoder_setting] = 15;
            newPosition = 0;
            myEnc.write(0);
          }
          if (encoder_values[encoder_setting] + newPosition < 1) {
            encoder_values[encoder_setting] = 1;
            newPosition = 0;
            myEnc.write(0);
          }
          TFT_Print_value(encoder_setting, encoder_values[encoder_setting] + newPosition, encoder_colors[encoder_setting]);
          //level can be 0 to 15.  5 is the Teensy Audio Library's default
          sgtl5000_1.lineInLevel(encoder_values[3] + newPosition, encoder_values[3] + newPosition);
        };
        break;

      case 2:  // Volume Headphone output   // https://protosupplies.com/learn/audio-tutorials-and-workshop/audio-workshop-8/
        if (newPosition % 4 == 0) {
          newPosition = newPosition / 4;
          if (encoder_values[encoder_setting] + newPosition >= 100) {
            encoder_values[encoder_setting] = 100;
            newPosition = 0;
            myEnc.write(0);
          }
          if (encoder_values[encoder_setting] + newPosition < 0) {
            encoder_values[encoder_setting] = 0;
            newPosition = 0;
            myEnc.write(0);
          }
          TFT_Print_value(encoder_setting, encoder_values[encoder_setting] + newPosition, encoder_colors[encoder_setting]);
          sgtl5000_1.volume(0.01f * (encoder_values[2] + newPosition));
          //amp3.gain(0.01f* (encoder_values[encoder_setting] + newPosition));
        };
        break;
      case 5:  // Volume Speacker output
               //https://protosupplies.com/learn/audio-tutorials-and-workshop/audio-workshop-8/
        if (newPosition % 4 == 0) {
          newPosition = newPosition / 4;
          if (encoder_values[encoder_setting] + newPosition >= 100) {
            encoder_values[encoder_setting] = 100;
            newPosition = 0;
            myEnc.write(0);
          }
          if (encoder_values[encoder_setting] + newPosition < 0) {
            encoder_values[encoder_setting] = 0;
            newPosition = 0;
            myEnc.write(0);
          }
          TFT_Print_value(encoder_setting, encoder_values[encoder_setting] + newPosition, encoder_colors[encoder_setting]);
          amp3.gain(0.01f * (encoder_values[encoder_setting] + newPosition));
        };
        break;
      case 4:  // IQ gain
               //https://forum.pjrc.com/index.php?threads/audio-amplifier-object-library.52026/
        if (newPosition % 4 == 0) {
          newPosition = newPosition / 4;
          if (encoder_values[encoder_setting] + newPosition >= 100) {
            encoder_values[encoder_setting] = 100;
            newPosition = 0;
            myEnc.write(0);
          }
          if (encoder_values[encoder_setting] + newPosition < 0) {
            encoder_values[encoder_setting] = 0;
            newPosition = 0;
            myEnc.write(0);
          }
          TFT_Print_value(encoder_setting, encoder_values[encoder_setting] + newPosition, encoder_colors[encoder_setting]);
          // Adjustment for equalizing the I and Q signals to have identical strength, so that we can adjust the sideband suppression.
          amp1.gain(0.01f * (encoder_values[encoder_setting] + newPosition));
        };
        break;
      default:  // not in use
        if (newPosition % 4 == 0) {
          newPosition = newPosition / 4;
          if (encoder_values[encoder_setting] + newPosition >= 100) {
            encoder_values[encoder_setting] = 100;
            newPosition = 0;
            myEnc.write(0);
          }
          if (encoder_values[encoder_setting] + newPosition < 0) {
            encoder_values[encoder_setting] = 0;
            newPosition = 0;
            myEnc.write(0);
          }
          TFT_Print_value(encoder_setting, encoder_values[encoder_setting] + newPosition, encoder_colors[encoder_setting]);
        };
        //clear encoder
        myEnc.write(0);
        oldPosition = 0;
        newPosition = 0;
    }
  }

// read the knob and adjust the filter frequency
// PIN A2 (16)
  int knob2 = analogRead(A2);
  // quick and dirty equation for exp scale frequency adjust
  float freqf =  expf((float)knob2 / 150.0) * 10.0 + 80.0;
  filter1.frequency(freqf);
  TFT_Print_value(14,freqf,ILI9341_GREEN);

 // biquad1.setLowpass(0,  freqf, 0.54);
  biquad1.setBandpass(0,  freqf, 0.54);
  

  // Checking Button
  button1.update();
  button2.update();
  button_encoder.update();


  //  button switch from mixer1
  if (button1.fallingEdge()) {
    setMixer1Gain(mixer1_setting);
    mixer1_setting = (mixer1_setting + 1) % 4;
  }
  // button switch from mixer2
  if (button2.fallingEdge()) {
    setMixer2Gain(mixer2_setting);
    mixer2_setting = (mixer2_setting + 1) % 4;
  }

  if (button_encoder.fallingEdge()) {
    // If 'button_encoder.falling', then the inputs from 'encoder_values' should be stored and the status color should be changed.

    if (encoder_setting == 0) {
      encoder_values[0] = 0;
    } else {

      encoder_values[encoder_setting] = encoder_values[encoder_setting] + newPosition / 4;
    }
    if (newPosition != 0) {
      encoder_colors[encoder_setting] = ILI9341_GREEN;  //status color should be changed
      encoder_colors[0] = ILI9341_BLUE;
      encoder_setting = 0;
    } else {  //The function increases the value of encoder_setting by one and then limits it to the range of 0 to 5 using the modulo operation.
      encoder_setting = (encoder_setting + 1) % 6;
      encoder_colors[encoder_setting] = ILI9341_BLUE;
      encoder_colors[(encoder_setting + 5) % 6] = ILI9341_GREEN;  // Reset previous color
    }
    Print_value();
  }

  if (msecs > 15) {  //Only perform a refresh every x ms.
    msecs = 0;

    if (fft1024_1.available()) { show_spectrum(); }
    if (fft1024_2.available()) { show_spectrum2(); }
    if (fft1024_3.available()) { show_spectrum3(); }

    //Track the signal RMS amplitude. Useful for audio level response projects, and general troubleshooting.
    if (rms1.available()) { TFT_Print_value(10, rms1.read(), ILI9341_GREEN); }
  }
}


// source ->
//https://forum.pjrc.com/index.php?threads/sig-gen-si5351-4-khz-to-100-mhz.61378/
//Notes: Teensy 3.6 to Si5351. (Wiring: 3v3 - Vin, Gnd - Gnd, Pin18 - SDA, Pin19 - SCL)
//       Using "Etherkit si5351" Library v2.1.4 (installed using "Manage Libraries...")
//       Copyright (C) 2015 - 2016 Jason Milldrum <milldrum@gmail.com>
//       CLK0 Output = 4KHz to 120 MHz

//https://www.qsl.net/py2ohh/trx/si5351quadrature/si5351quadrature.html

//http://zl2ctm.blogspot.com/2019/06/homebrew-804020m-ssb-sdr-phasing-rig.html

// Advanced Microcontroller-based Audio Workshop
//
//http://www.pjrc.com/store/audio_tutorial_kit.html
//https://hackaday.io/project/8292-microcontroller-audio-workshop-had-supercon-2015
//