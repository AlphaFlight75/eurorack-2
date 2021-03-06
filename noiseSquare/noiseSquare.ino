#ifdef EXTERNAL_IDE

#include <Arduino.h>
#define PIN B,4


int main(void) {

  init();
  setup();

  while(true) {
    loop();
  }
}

#endif

//////////////////////////////////////////////////

#include <portManipulations.h>
#include <mapping.h>
#include "multiChannelOscillator.h"
#include "osc_noiseHW.h"
#include "digitalNoise.h"


//#define DEBUG

extern MultiChannelOscillator oscil;
extern osc_noiseHW hardware;
digitalNoise digiNoise;

const uint8_t numbMetallicChannels = 6;
const uint8_t numbCowbellChannels = 2;

uint16_t minFrequencies[numbMetallicChannels + numbCowbellChannels] = {153,185,267,327,465,1023,377,293};//587,754}; //
uint8_t pinIndices[numbMetallicChannels + numbCowbellChannels]  = {2,3,4,5,6,7,0,1};
uint16_t frequencies[numbMetallicChannels + numbCowbellChannels];

const uint16_t cowbellTableHigh[] PROGMEM = {
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 149, 161, 172, 184, 195, 207, 218, 230, 241, 253, 264, 276, 287, 299, 310, 322, 333, 345, 356, 368, 379, 391, 402, 414, 425, 437, 448, 460, 471, 483, 494, 506, 517, 529, 540, 552, 563, 575, 586, 598, 609, 621, 633, 644, 656, 667, 679, 690, 702, 713, 725, 736, 748, 759, 771, 782, 794, 805, 817, 828, 840, 851, 863, 874, 886, 897, 909, 920, 932, 943, 955, 966, 978, 989, 1001, 1012, 1024, 1035, 1047, 1058, 1070, 1081, 1093, 1104, 1116, 1128, 1139, 1151, 1162, 1174, 1185, 1197, 1208, 1220, 1231, 1243, 1254, 1266, 1277, 1289, 1300, 1312, 1323, 1335, 1346, 1358, 1369, 1381, 1392, 1404, 1415, 1427, 1438, 1450, 1461, 1473, 1484, 1496, 1507, 1519, 1530, 1542, 1553, 1565, 1576, 1
};

const uint16_t cowbellTableLow[] PROGMEM = {
		8, 8, 9, 10, 11, 11, 12, 13, 14, 14, 15, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 24, 25, 25, 26, 27, 28, 28, 29, 30, 31, 31, 32, 33, 34, 35, 35, 36, 37, 38, 38, 39, 40, 41, 42, 42, 43, 44, 45, 45, 46, 47, 48, 48, 49, 50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 58, 59, 59, 60, 61, 62, 62, 63, 64, 65, 66, 66, 67, 68, 69, 69, 70, 71, 72, 72, 73, 74, 75, 76, 76, 77, 78, 79, 79, 80, 81, 82, 83, 83, 84, 85, 86, 86, 87, 88, 89, 89, 90, 91, 92, 93, 93, 94, 95, 96, 96, 97, 98, 99, 100, 100, 101, 102, 103, 103, 104, 105, 106, 107, 115, 124, 133, 142, 151, 160, 169, 178, 187, 196, 205, 214, 223, 232, 241, 250, 259, 268, 276, 285, 294, 303, 312, 321, 330, 339, 348, 357, 366, 375, 384, 393, 402, 411, 420, 429, 437, 446, 455, 464, 473, 482, 491, 500, 509, 518, 527, 536, 545, 554, 563, 572, 581, 590, 598, 607, 616, 625, 634, 643, 652, 661, 670, 679, 688, 697, 706, 715, 724, 733, 742, 751, 759, 768, 777, 786, 795, 804, 813, 822, 831, 840, 849, 858, 867, 876, 885, 894, 903, 912, 920, 929, 938, 947, 956, 965, 974, 983, 992, 1001, 1010, 1019, 1028, 1037, 1046, 1055, 1064, 1073, 1081, 1090, 1099, 1108, 1117, 1126, 1135, 1144, 1153, 1162, 1171, 1180, 1189, 1198, 1207, 1216, 1225, 1234, 1243
};




void setup() {


	TIMSK2 = 0;

	#ifdef DEBUG
	Serial.begin(115200);
	Serial.println("start");
	#endif


	oscil.init(pinIndices);
	oscil.setFrequencies(minFrequencies);

	oscil.start();

	digiNoise.init();
	digiNoise.setTopFreq(1000);

	hardware.init();


}



void loop() {


	// get new CV and knob values
	hardware.update();

	digiNoise.checkForBitFlip();

	oscil.fillBuffer();


	// Read basic knobs
	uint8_t digiNoiseIn = 255- hardware.getKnobValue(0);
	uint8_t metallicIn = 255-hardware.getKnobValue(1);
	uint8_t cowbellIn = 255-hardware.getKnobValue(2);

	// Add CV
	//uint16_t tmp = cowbellIn;// + hardware.getCVValue(0);
	//if (tmp > 253) tmp = 253; //dirty fix: better change lookup table
	//cowbellIn = tmp;

	if (cowbellIn > 253) cowbellIn = 253;
/*
	if (hardware.getSwitchState()) {
		uint16_t tmp = digiNoiseIn + (hardware.getCVValue(1)>>1);
		if (tmp > 255) tmp = 255;
		digiNoiseIn = tmp;
	} else {
		uint16_t tmp = metallicIn + hardware.getCVValue(1);
		if (tmp > 255) tmp = 255;
		metallicIn = tmp;
	}
*/
	digiNoise.checkForBitFlip();

	//** Set metallic frequencies


	for (uint8_t index=0; index<numbMetallicChannels; index++) {
		frequencies[index] = minFrequencies[index]/8 + (((uint32_t)minFrequencies[index]*metallicIn)>>9);
		digiNoise.checkForBitFlip();
	}


	//** Set cowbell frequencies
	frequencies[7] = mapProgmemU8U16(cowbellIn,cowbellTableHigh);
	frequencies[6] = mapProgmemU8U16(cowbellIn,cowbellTableLow);

	digiNoise.checkForBitFlip();
	oscil.setFrequencies(frequencies);
	digiNoise.checkForBitFlip();


	//** Set digital noise top frequency
	digiNoise.setTopFreq((digiNoiseIn<<5)+62);


	digiNoise.checkForBitFlip();

}




