#include <Magick++.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <filesystem>


using namespace std;
using namespace Magick;
namespace fs = filesystem;

// Rotation values for Dithering Operation
const double cyanArgsFirst[2] = { 15,1 };
const double cyanArgsSecond[2] = { -15,1 };
const double magentaArgsFirst[2] = { 75,1 };
const double magentaArgsSecond[2] = { -75,1 };
const double blackArgsFirst[2] = { 45,1 };
const double blackArgsSecond[2] = { -45,1 };


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



/*
// Thresholds
string thresholdsYo()
{
	const string thrshmp[4] = { "c6x6w",
								"c7x7w",
								"h8x8o",
								"h16x16o"};
	return thrshmp[rand() % 4];
};
*/

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

			string ditherType;
			if (randomSolid < 100) {
				cout << "Solid Black selected";
			}
			if (randomLarge <= poLarge) {
				cout << endl << inFileNoEXT << " Is being processed with these values: " << endl << "Dither Type: " << "h16x16o" << endl << "UpScale: " << randomScalePercentL << endl;
			}
			else {
				cout << endl << inFileNoEXT << " Is being processed with these values: " << endl << "Dither Type: " << "h8x8o" << endl << "UpScale: " << randomScalePercent << endl;
			}


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
				cout << "original Resolution: " << noiseSize << endl << endl;

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

				// Output Cyan Dithered Image
				if (randomLarge <= poLarge) {
					string artARG = "2x2";
					inputCyan.addNoise(GaussianNoise, 0.21);
					inputCyan.modulate(100, 0, 100);
					inputCyan.distort(ScaleRotateTranslateDistortion, 1, cyanArgsFirst, Magick::MagickTrue);
					inputCyan.orderedDither("h16x16o");
					inputCyan.distort(ScaleRotateTranslateDistortion, 1, cyanArgsSecond, Magick::MagickTrue);
					inputCyan.crop(cropChannelsNewL);
					inputCyan.artifact("compose:args", artARG);
					inputCyan.composite(noiseForDisplaceCyan, 0, 0, DisplaceCompositeOp);
				}
				else {
					string artARG = "2x2";
					inputCyan.addNoise(GaussianNoise, 0.11);
					inputCyan.modulate(100, 0, 100);
					inputCyan.distort(ScaleRotateTranslateDistortion, 1, cyanArgsFirst, Magick::MagickTrue);
					inputCyan.orderedDither("h8x8o");
					inputCyan.distort(ScaleRotateTranslateDistortion, 1, cyanArgsSecond, Magick::MagickTrue);
					inputCyan.crop(cropChannelsNew);
					inputCyan.artifact("compose:args", artARG);
					inputCyan.composite(noiseForDisplaceCyan, 0, 0, DisplaceCompositeOp);
				}
				//inputCyan.write("_cyan.png");

				// Output Magenta Dithered Image
				if (randomLarge <= poLarge) {
					string artARG = "2x2";
					inputMagenta.addNoise(GaussianNoise, 0.13);
					inputMagenta.modulate(100, 0, 100);
					inputMagenta.distort(ScaleRotateTranslateDistortion, 1, magentaArgsFirst, Magick::MagickTrue);
					inputMagenta.orderedDither("h16x16o");
					inputMagenta.distort(ScaleRotateTranslateDistortion, 1, magentaArgsSecond, Magick::MagickTrue);
					inputMagenta.crop(cropChannelsNewL);
					inputMagenta.artifact("compose:args", artARG);
					inputMagenta.composite(noiseForDisplaceMagenta, 0, 0, DisplaceCompositeOp);
				}
				else {
					string artARG = "2x2";
					inputMagenta.addNoise(GaussianNoise, 0.13);
					inputMagenta.modulate(100, 0, 100);
					inputMagenta.distort(ScaleRotateTranslateDistortion, 1, magentaArgsFirst, Magick::MagickTrue);
					inputMagenta.orderedDither("h8x8o");
					inputMagenta.distort(ScaleRotateTranslateDistortion, 1, magentaArgsSecond, Magick::MagickTrue);
					inputMagenta.crop(cropChannelsNew);
					inputMagenta.artifact("compose:args", artARG);
					inputMagenta.composite(noiseForDisplaceMagenta, 0, 0, DisplaceCompositeOp);
				}
				//inputMagenta.write("_Magenta.png");

				// Output Yellow Dithered Image
				if (randomLarge <= poLarge) {
					string artARG = "2x2";
					inputYellow.addNoise(GaussianNoise, 0.12);
					inputYellow.modulate(100, 0, 100);
					inputYellow.orderedDither("h16x16o");
					inputYellow.crop(cropChannelsNewL);
					inputYellow.artifact("compose:args", artARG);
					inputYellow.composite(noiseForDisplaceYellow, 0, 0, DisplaceCompositeOp);
				}
				else {
					string artARG = "2x2";
					inputYellow.addNoise(GaussianNoise, 0.12);
					inputYellow.modulate(100, 0, 100);
					inputYellow.orderedDither("h8x8o");
					inputYellow.crop(cropChannelsNew);
					inputYellow.artifact("compose:args", artARG);
					inputYellow.composite(noiseForDisplaceYellow, 0, 0, DisplaceCompositeOp);
				}
				//inputYellow.write("_Yellow.png");

				// Output Black Dithered/solid Image
				if (randomSolid < 100) {
					string artARG = "2x2";
					if (randomLarge <= poLarge) {
						inputBlack.addNoise(GaussianNoise, 0.09);
						inputBlack.modulate(100, 0, 100);
						inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsFirst, Magick::MagickTrue);
						inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsSecond, Magick::MagickTrue);
						inputBlack.crop(cropChannelsNewL);
						inputBlack.artifact("compose:args", artARG);
						inputBlack.composite(noiseForDisplaceBlack, 0, 0, DisplaceCompositeOp);
					}
					else {
						string artARG = "2x2";
						inputBlack.addNoise(GaussianNoise, 0.09);
						inputBlack.modulate(100, 0, 100);
						inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsFirst, Magick::MagickTrue);
						inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsSecond, Magick::MagickTrue);
						inputBlack.crop(cropChannelsNew);
						inputBlack.artifact("compose:args", artARG);
						inputBlack.composite(noiseForDisplaceBlack, 0, 0, DisplaceCompositeOp);
					}

				}
				else {
					if (randomLarge <= poLarge) {
						string artARG = "2x2";
						inputBlack.addNoise(GaussianNoise, 0.09);
						inputBlack.modulate(100, 0, 100);
						inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsFirst, Magick::MagickTrue);
						inputBlack.orderedDither("h16x16o");
						inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsSecond, Magick::MagickTrue);
						inputBlack.crop(cropChannelsNewL);
						inputBlack.artifact("compose:args", artARG);
						inputBlack.composite(noiseForDisplaceBlack, 0, 0, DisplaceCompositeOp);
					}
					else {
						string artARG = "2x2";
						inputBlack.addNoise(GaussianNoise, 0.09);
						inputBlack.modulate(100, 0, 100);
						inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsFirst, Magick::MagickTrue);
						inputBlack.orderedDither("h8x8o");
						inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsSecond, Magick::MagickTrue);
						inputBlack.crop(cropChannelsNew);
						inputBlack.artifact("compose:args", artARG);
						inputBlack.composite(noiseForDisplaceBlack, 0, 0, DisplaceCompositeOp);
					}
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
					cout << inFileEXT << endl << "Quantized: " << randomQuant << endl << " and compressed " << endl << endl;
					cout << inFileEXT << endl << "has been processed and compressed " << endl << endl;
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