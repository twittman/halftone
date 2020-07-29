#include <Magick++.h>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <filesystem>


using namespace std;
using namespace Magick;
namespace fs = filesystem;

int whichAnglesYo()
{
	const int anglesList[4] = {  1,
								 2,
								 3,
								 4 };
	return anglesList[rand() % 4];
}

string samplesYo()
{
	const string samplesList[6] = { "4:1:0",
									"4:1:1",
									"4:2:0",
									"4:2:2",
									"4:4:0",
									"4:4:4" };
	return samplesList[rand() % 6];
}



double magentaArgsFirst[2];
double magentaArgsSecond[2];
double blackArgsFirst[2];
double blackArgsSecond[2];
double cyanArgsFirst[2];
double cyanArgsSecond[2];
double yellowArgsFirst[2];
double yellowArgsSecond[2];


void case_One(Image& inputCyan, Image& inputMagenta, Image& inputBlack, Image& inputYellow, \
	double c, double m, double y, double b,\
	string& ditherTypeThingy, string& croppingThing);

// Random Resize Values for small dither
const int reSizeMin = 140;
const int reSizeMax = 360;

// Random Resize Values for large dither
const int reSizeMinL = 200;
const int reSizeMaxL = 400;

// Random Final Blur
const double finalBlurMin = 0.24;
const double finalBlurMax = 0.76;

const double finalGrainMin = 0.05;
const double finalGrainMax = 0.40;


// Main Function
int main(int argc, char** argv)
{
	srand(time(NULL));
	const fs::path inputDir{ argc >= 2 ? argv[1] : fs::current_path() };
	fs::path outputDir(argv[2]);

	// Min and Max Gamma adjustment
	//const double gammaMin = 0.9;
	//const double gammaMax = 1.2;


	// Minimum and Maximum JPEG Compression values
	// you can change these for sure.
	const int qMin = 50;
	const int qMax = 90;

	// Minimum and Maximum number of colours for Quantize
	const int quantMin = 8;
	const int quantMax = 512;

	// Percentage of JPEG
	// Eg: 0.1 = 10% chance of jpeg
	int poJPG = 200 * stod(argv[3]);

	// Percentage of larger dither
	// Eg: 0.02 = 2% chance of larger dither
	int poLarge = 400 * stod(argv[4]);


	if (fs::create_directory(outputDir)) {
		cout << "Output Directory has been created" << endl;
	}
	for (const auto& entry : fs::directory_iterator(inputDir)) {
		if (is_regular_file(entry.path()))
		{
			// Get Input Directory
			fs::path inputFile(inputDir);

			// Get Filename with Extension
			string inFileEXT = entry.path().string();
			const __int64 index1 = inFileEXT.find_last_of("/\\");
			string fileNoPathEXT = inFileEXT.substr(index1 + 1);

			// Get Filename without Extension
			string inFileNoEXT = entry.path().stem().string();
			const __int64 index2 = inFileNoEXT.find_last_of("/\\");
			string fileNoPathNoEXT = inFileNoEXT.substr(index2 + 1);

			// Get Output Filename
			string outFileDir = outputDir.string();
			string outFile = outFileDir + '\\' + fileNoPathEXT;


			random_device ranDev;
			mt19937 randomNumberGen01(ranDev());

			uniform_int_distribution<int> seq6(quantMin, quantMax);
			int randomQuant = seq6(randomNumberGen01);

			// random gamma
			//uniform_real_distribution<double> seq_01(gammaMin, gammaMax);
			//double gammaRandom = seq_01(randomNumberGen01);


			// Random JPEG quality
			uniform_int_distribution<int> seq2(qMin, qMax);
			int qJpeg = seq2(randomNumberGen01);

			// small dither resize
			uniform_int_distribution<int> sequence01(reSizeMin, reSizeMax);
			int randomScale = sequence01(randomNumberGen01);
			string randomScalePercent = to_string(randomScale) + "%";

			// large dither resize
			uniform_int_distribution<int> sequence010(reSizeMinL, reSizeMaxL);
			int randomScaleL = sequence010(randomNumberGen01);
			string randomScalePercentL = to_string(randomScaleL) + "%";


			uniform_int_distribution<int> sequence_large(1, 400);
			int randomLarge = sequence_large(randomNumberGen01);

			uniform_int_distribution<int> sequence_solid(1, 200);
			int randomSolid = sequence_solid(randomNumberGen01);
			string percentage_Solid = to_string(randomSolid);

			uniform_real_distribution<double> sequence02(finalBlurMin, finalBlurMax);
			double randomBlurScale = sequence02(randomNumberGen01);
			double randomFinalBlur = randomBlurScale;

			uniform_real_distribution<double> sequence03(finalGrainMin, finalGrainMax);
			double finalGrain = sequence03(randomNumberGen01);

			//string ditherType;
			


			try {
				InitializeMagick(*argv);
				// Read Original Image file
				Image inputReScale, inputCyan, inputMagenta, inputYellow, inputBlack;
				inputReScale.read(inFileEXT);

				// Get Image Dimensions
				size_t width = inputReScale.baseColumns();
				size_t height = inputReScale.baseRows();

				// Calculate size of Crop after Upscale and Distort
				double widthPercent = width * stoi(randomScalePercent) / 100;
				double heightPercent = height * stoi(randomScalePercent) / 100;
				string cropChannelsNew = to_string(widthPercent) + "x" + to_string(heightPercent);

				// Calculate size of Crop after Upscale and Distort
				double widthPercentL = width * stoi(randomScalePercentL) / 100;
				double heightPercentL = height * stoi(randomScalePercentL) / 100;
				string cropChannelsNewL = to_string(widthPercentL) + "x" + to_string(heightPercentL);



				/////////////////////////////////////////////////
				// Generate Noise Map for Displace PER CHANNEL
				/////////////////////////////////////////////////
				string noiseSize = to_string(width) + "x" + to_string(height);
				cout << "original Resolution: " << noiseSize << endl;

				Image noiseForDisplaceCyan(Geometry(noiseSize), "gray"), noiseForDisplaceMagenta(Geometry(noiseSize), "gray"), noiseForDisplaceYellow(Geometry(noiseSize), "gray"), noiseForDisplaceBlack(Geometry(noiseSize), "gray");

				noiseForDisplaceCyan.colorSpace(sRGBColorspace);
				noiseForDisplaceCyan.addNoise(GaussianNoise);
				noiseForDisplaceCyan.colorSpace(GRAYColorspace);
				noiseForDisplaceCyan.linearStretch(QuantumRange / 0.26, QuantumRange / 0.26);
				noiseForDisplaceCyan.gaussianBlur(0, 0.9);


				noiseForDisplaceMagenta.colorSpace(sRGBColorspace);
				noiseForDisplaceMagenta.addNoise(GaussianNoise);
				noiseForDisplaceMagenta.colorSpace(GRAYColorspace);
				noiseForDisplaceMagenta.linearStretch(QuantumRange / 0.26, QuantumRange / 0.26);
				noiseForDisplaceMagenta.gaussianBlur(0, 0.86);


				noiseForDisplaceYellow.colorSpace(sRGBColorspace);
				noiseForDisplaceYellow.addNoise(GaussianNoise);
				noiseForDisplaceYellow.colorSpace(GRAYColorspace);
				noiseForDisplaceYellow.linearStretch(QuantumRange / 0.26, QuantumRange / 0.26);
				noiseForDisplaceYellow.gaussianBlur(0, 1.0);


				noiseForDisplaceBlack.colorSpace(sRGBColorspace);
				noiseForDisplaceBlack.addNoise(GaussianNoise);
				noiseForDisplaceBlack.colorSpace(GRAYColorspace);
				noiseForDisplaceBlack.linearStretch(QuantumRange / 0.26, QuantumRange / 0.26);
				noiseForDisplaceBlack.gaussianBlur(0, 1.2);


				// Resize Input Image and adjust gamma in Memory before Splitting
				if (randomLarge <= poLarge) {
					//inputReScale.gamma(gammaRandom);
					//inputReScale.modulate(97, 99, 100);
					inputReScale.colorSpace(CMYKColorspace);
					inputReScale.filterType(HanningFilter);
					inputReScale.resize(randomScalePercentL);
					inputReScale.gaussianBlur(0, 1.0);
					noiseForDisplaceCyan.resize(randomScalePercentL);
					noiseForDisplaceMagenta.resize(randomScalePercentL);
					noiseForDisplaceYellow.resize(randomScalePercentL);
					noiseForDisplaceBlack.resize(randomScalePercentL);
				}
				else {
					//inputReScale.gamma(gammaRandom);
					//inputReScale.modulate(97, 99, 100);
					inputReScale.colorSpace(CMYKColorspace);
					inputReScale.filterType(HanningFilter);
					inputReScale.resize(randomScalePercent);
					inputReScale.gaussianBlur(0, 0.7);
					noiseForDisplaceCyan.resize(randomScalePercent);
					noiseForDisplaceMagenta.resize(randomScalePercent);
					noiseForDisplaceYellow.resize(randomScalePercent);
					noiseForDisplaceBlack.resize(randomScalePercent);
				}

				//noiseForDisplaceCyan.write("_Noise_Cyan.png");
				//noiseForDisplaceMagenta.write("_Noise_Magenta.png");
				//noiseForDisplaceYellow.write("_Noise_Yellow.png");
				//noiseForDisplaceBlack.write("_Noise_Black.png");

				inputCyan = inputReScale;
				inputCyan.channel(CyanChannel);

				inputMagenta = inputReScale;
				inputMagenta.channel(MagentaChannel);

				inputYellow = inputReScale;
				inputYellow.channel(YellowChannel);

				inputBlack = inputReScale;
				inputBlack.channel(BlackChannel);

				string ditherSize;
				string cropThis;
				string forPrint;

				if (randomLarge <= poLarge) {
					ditherSize = "h16x16o";
					cropThis = cropChannelsNewL;
					forPrint = "h16x16o dither selected";
				}
				else {
					ditherSize = "h8x8o";
					cropThis = cropChannelsNew;
					forPrint = "h8x8o dither selected";
				}

				if (randomSolid < 100) {
					cout << "\nSolid Black has been selected for the Image below";
					cout << endl << inFileNoEXT << " Is being processed with these values: " << endl << "UpScale: " << randomScalePercent << endl;
				}
				else {
					cout << endl << inFileNoEXT << " Is being processed with these values: " << endl << "UpScale: " << randomScalePercent << endl;
				}

				switch (whichAnglesYo())
				{
				case 1:
					// One	
					case_One(inputCyan, inputMagenta, inputYellow, inputBlack, 15, 75, 0, 45, ditherSize, cropThis);
					cout << "\nDither angles #1" << endl;
					break;
				case 2:
					// Two
					case_One(inputCyan, inputMagenta, inputYellow, inputBlack, 105, 75, 90, 15, ditherSize, cropThis);
					cout << "\nDither angles #2" << endl;
					break;
				case 3:
					// Three
					case_One(inputCyan, inputMagenta, inputYellow, inputBlack, 15, 45, 0, 75, ditherSize, cropThis);
					cout << "\nDither angles #3" << endl;
					break;
				case 4:
					// Four
					case_One(inputCyan, inputMagenta, inputYellow, inputBlack, 165, 45, 90, 105, ditherSize, cropThis);
					cout << "\nDither angles #4" << endl;
					break;
				}

				string artARG = "2x2";

				// Output Cyan Dithered Image
				inputCyan.addNoise(GaussianNoise, 0.15);
				inputCyan.modulate(100, 0, 100);
				case_One;
				inputCyan.artifact("compose:args", artARG);
				inputCyan.composite(noiseForDisplaceCyan, 0, 0, DisplaceCompositeOp);
				//inputCyan.write("_cyan.png");

				// Output Magenta Dithered Image
				inputMagenta.addNoise(GaussianNoise, 0.13);
				inputMagenta.modulate(100, 0, 100);
				case_One;
				inputMagenta.artifact("compose:args", artARG);
				inputMagenta.composite(noiseForDisplaceMagenta, 0, 0, DisplaceCompositeOp);
				//inputMagenta.write("_Magenta.png");

				// Output Yellow Dithered Image
				inputYellow.addNoise(GaussianNoise, 0.12);
				inputYellow.modulate(100, 0, 100);
				case_One;
				inputYellow.artifact("compose:args", artARG);
				inputYellow.composite(noiseForDisplaceYellow, 0, 0, DisplaceCompositeOp);
				//inputYellow.write("_Yellow.png");

				// Output Black Dithered/solid Image
				if (randomSolid < 100) {
					string artARG = "2x2";
					if (randomLarge <= poLarge) {
						inputBlack.addNoise(GaussianNoise, 0.09);
						inputBlack.modulate(100, 0, 100);
						inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsFirst, Magick::MagickTrue);
						inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsSecond, Magick::MagickTrue);
						inputBlack.crop(cropThis);
						inputBlack.artifact("compose:args", artARG);
						inputBlack.composite(noiseForDisplaceBlack, 0, 0, DisplaceCompositeOp);
					}
					else {
						string artARG = "2x2";
						inputBlack.addNoise(GaussianNoise, 0.09);
						inputBlack.modulate(100, 0, 100);
						inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsFirst, Magick::MagickTrue);
						inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsSecond, Magick::MagickTrue);
						inputBlack.crop(cropThis);
						inputBlack.artifact("compose:args", artARG);
						inputBlack.composite(noiseForDisplaceBlack, 0, 0, DisplaceCompositeOp);
					}

				}
				else {
					string artARG = "2x2";
					inputBlack.addNoise(GaussianNoise, 0.09);
					inputBlack.modulate(100, 0, 100);
					case_One;
					inputBlack.artifact("compose:args", artARG);
					inputBlack.composite(noiseForDisplaceBlack, 0, 0, DisplaceCompositeOp);
				}
				//inputBlack.write("_Black.png");

				// Output Combined Channels to single Colour Image
				std::vector<Image> imageList;
				imageList.push_back(inputCyan);
				imageList.push_back(inputMagenta);
				imageList.push_back(inputYellow);
				imageList.push_back(inputBlack);

				//////////////////////////////////////////////////////////////////////////////////// string artARG = "2x2";
				Image rComposite;
				Magick::combineImages(&rComposite, imageList.begin(), imageList.end(), AllChannels, CMYKColorspace);

				if (randomSolid <= poJPG) {
					cout << inFileEXT << endl << "Quantized: " << randomQuant << endl << " and compressed " << endl;
					cout << inFileEXT << endl << "has been processed and compressed " << endl;
					rComposite.gaussianBlur(0, randomFinalBlur);
					rComposite.filterType(CubicFilter);
					rComposite.resize(noiseSize);
					rComposite.colorSpace(sRGBColorspace);
					rComposite.addNoise(LaplacianNoise, finalGrain);
					rComposite.quantizeDither(false);
					rComposite.quantizeColors(randomQuant);
					rComposite.quantize(true);
					//////////////////////////////////////////////////////////////////////////////    rComposite.artifact("compose:args", artARG);
					//////////////////////////////////////////////////////////////////////////////    rComposite.composite(noiseForDisplace, 0, 0, DisplaceCompositeOp);
					rComposite.defineValue("JPEG", "sampling-factor", samplesYo());
					rComposite.quality(qJpeg);
					rComposite.magick("JPEG");
					rComposite.write(outFile);
				}
				else {
					rComposite.gaussianBlur(0, randomFinalBlur);
					rComposite.filterType(CubicFilter);
					rComposite.resize(noiseSize);
					rComposite.colorSpace(sRGBColorspace);
					rComposite.addNoise(LaplacianNoise, finalGrain);
					/////////////////////////////////////////////////////////////////////////////    rComposite.artifact("compose:args", artARG);
					/////////////////////////////////////////////////////////////////////////////    rComposite.composite(noiseForDisplace, 0, 0, DisplaceCompositeOp);
					rComposite.write(outFile);
				}
			}
			catch (Exception& error_)
			{
				cout << "Caught exception: " << error_.what() << endl;
				cout << "The falure is yours" << endl;
				break;
			}
		}
	}
}



void case_One(Image& inputCyan, Image& inputMagenta, Image& inputBlack, Image& inputYellow, \
	double c, double m, double y, double b, string& ditherTypeThingy, \
	string& croppingThing)
{
	double cyanArgsFirst[2] = { c,1 };
	double cyanArgsSecond[2] = { -c,1 };
	double magentaArgsFirst[2] = { m,1 };
	double magentaArgsSecond[2] = { -m,1 };
	double yellowArgsFirst[2] = { y,1 };
	double yellowArgsSecond[2] = { -y,1 };
	double blackArgsFirst[2] = { b,1 };
	double blackArgsSecond[2] = { -b,1 };

	cout << "Dither type: " << ditherTypeThingy << endl;

	// Cyan
	inputCyan.distort(ScaleRotateTranslateDistortion, 1, cyanArgsFirst, Magick::MagickTrue);
	inputCyan.orderedDither(ditherTypeThingy);
	inputCyan.distort(ScaleRotateTranslateDistortion, 1, cyanArgsSecond, Magick::MagickTrue);
	inputCyan.crop(croppingThing);

	// Magenta
	inputMagenta.distort(ScaleRotateTranslateDistortion, 1, magentaArgsFirst, Magick::MagickTrue);
	inputMagenta.orderedDither(ditherTypeThingy);
	inputMagenta.distort(ScaleRotateTranslateDistortion, 1, magentaArgsSecond, Magick::MagickTrue);
	inputMagenta.crop(croppingThing);

	// Yellow
	inputYellow.distort(ScaleRotateTranslateDistortion, 1, yellowArgsFirst, Magick::MagickTrue);
	inputYellow.orderedDither(ditherTypeThingy);
	inputYellow.distort(ScaleRotateTranslateDistortion, 1, yellowArgsSecond, Magick::MagickTrue);
	inputYellow.crop(croppingThing);

	// Black
	inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsFirst, Magick::MagickTrue);
	inputBlack.orderedDither(ditherTypeThingy);
	inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsSecond, Magick::MagickTrue);
	inputBlack.crop(croppingThing);
}

