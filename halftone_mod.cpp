#include <Magick++.h>
#include <cxxopts.hpp>
#include <chrono>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <filesystem>



using namespace std;
using namespace Magick;
using namespace filesystem;

int whichAnglesYo()
{
	const int anglesList[4] = { 1,
								2,
								3,
								4 };
	return anglesList[rand() % 4];
}

int whichDotsYo()
{
	const int dotsList[3] = { 1,
							  2,
							  3	};
	return dotsList[rand() % 3];
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
	double c, double m, double y, double b, \
	string& ditherTypeThingy, string& croppingThing, float& solidPercent, int& randomNumber);
void dotsFunc(int DPI, int& dotsOfHalftone);
void Black_solid_dithered(int randomNumber, float solidPercent, Magick::Image& blackLines, Magick::Image& inputReScale, float poLarge, std::string& noiseSize, Magick::Image& solidBlackImage, Magick::Image& inputBlack, Magick::Image& noiseForDisplaceBlack);
void Jpeg_compression(int noise, int randomNumber, int poJPG, std::string& inFileEXT, int randomQuant, Magick::Image& rComposite, double randomFinalBlur, std::string& noiseSize, float solidPercent, Magick::Image& solidBlackImage, double finalGrain, int qJpeg, std::string& outFile);
void Dither_and_combine_channels(int noise, Magick::Image& inputCyan, Magick::Image& inputMagenta, Magick::Image& inputYellow, Magick::Image& inputBlack, std::vector<int>& anglesNum, std::string& ditherSize, std::string& cropChannelsNew, float& solidPercent, int& randomNumber, Magick::Image& noiseForDisplaceCyan, Magick::Image& noiseForDisplaceMagenta, Magick::Image& noiseForDisplaceYellow, Magick::Image& blackLines, Magick::Image& inputReScale, float poLarge, std::string& noiseSize, Magick::Image& solidBlackImage, Magick::Image& noiseForDisplaceBlack, int poJPG, std::string& inFileEXT, int randomQuant, double randomFinalBlur, double finalGrain, int qJpeg, std::string& outFile);
void Halftone_Function_Main(int noise, std::filesystem::path& inputDir, std::filesystem::path& outputDir, const int& quantMin, const int& quantMax, const int& qMin, const int& qMax, char** argv, int DPI, int angle, float& solidPercent, float poLarge, int poJPG);
//void min_max_DPI(int DPI, double& maxDPI, double& minDPI, double& maxNumc6x6w, double& minNumc6x6w, double& maxNumMultiplier, int& dotsOfHalftone);
void anglesFunc(int angle, int& angle_of_halftone);


void print(std::vector<int> const& anglesLaser)
{
	std::copy(anglesLaser.begin(), anglesLaser.end(), std::ostream_iterator<int>(cout, " "));
}

// Random Final Blur
const double finalBlurMin = 0.68;
const double finalBlurMax = 1.66;

const double finalGrainMin = 0.07;
const double finalGrainMax = 0.23;


// Main Function
int main(int argc, char** argv)
{
	// ("s,solid", "Do you want the outlines to be solid?", cxxopts::value<bool>()->default_value("false"))
	cxxopts::Options options(argv[0], "This is how you use this app");
	options.add_options()
		("i,input", "input directory", cxxopts::value<std::string>())
		("o,output", "output directory", cxxopts::value<std::string>())
		("n,noise", "Add noise, 1 = yes, 0 = no", cxxopts::value<int>()->default_value("1"))
		("a,angle", "Angle of Halftone Dots 1 - 5, 0 for random", cxxopts::value<int>()->default_value("0"))
		("j,jpeg", "Percentage of Jpeg", cxxopts::value<float>()->default_value("0.05"))
		("s,solid", "Do you want the outlines to be solid?", cxxopts::value<float>()->default_value("0.5"))
		("g,DPI", "DPI, 1 = 250 DPI, 2 = 300 DPI, 3 = 600 DPI, 4 = 900 DPI, 5 = 1200 DPI, 6 = 2400 DPI, 7 = 4800 DPI, 0 = Random", cxxopts::value<int>()->default_value("1"))
		("b,angles", "print angles")
		("h,help", "print help")
		;

	auto result = options.parse(argc, argv);

	if (result.count("help"))
	{
		cout << options.help() << endl;
		exit(0);
	}
	if (result.count("angles"))
	{
		cout << "1 = Cyan:15, Magenta:75, Yellow:0, Black:45" << endl;
		cout << "2 = Cyan:105, Magenta:75, Yellow:90, Black:15" << endl;
		cout << "3 = Cyan:15, Magenta:45, Yellow:0, Black:75" << endl;
		cout << "4 = Cyan:165, Magenta:45, Yellow:90, Black:105" << endl;
		cout << "5 = Cyan:95, Magenta:65, Yellow:15, Black:35" << endl;
		exit(0);
	}


	string input;
	string output;
	int noise = 1;
	int angle = 0;
	float large = 0.05;
	float jpeg = 0.05;
	float solid = 0.5;
	int DPI = 1;

	if (result.count("input"))
		input = result["input"].as<std::string>();

	if (result.count("output"))
		output = result["output"].as<std::string>();

	if (result.count("noise"))
		noise = result["noise"].as<int>();

	if (result.count("angle"))
		angle = result["angle"].as<int>();

	if (result.count("large"))
		large = result["large"].as<float>();

	if (result.count("jpeg"))
		jpeg = result["jpeg"].as<float>();

	if (result.count("solid"))
		solid = result["solid"].as<float>();

	if (result.count("DPI"))
		DPI = result["DPI"].as<int>();

	//bool solid = result["solid"].as<bool>();

	srand(time(NULL));
	filesystem::path inputDir(input);
	filesystem::path outputDir(output);

	// Minimum and Maximum JPEG Compression values
	const int qMin = 50;
	const int qMax = 94;

	// Minimum and Maximum number of colours for Quantize
	const int quantMin = 8;
	const int quantMax = 512;

	// Percentage of JPEG
	// Eg: 0.1 = 10% chance of jpeg
	int poJPG = 200 * jpeg;

	// Percentage of larger dither
	// Eg: 0.02 = 2% chance of larger dither
	float poLarge = 200 * large;

	float solidPercent = 200 * solid;

	if (filesystem::create_directory(outputDir)) {
		cout << "Output Directory has been created" << endl;
	}
	Halftone_Function_Main(noise, inputDir, outputDir, quantMin, quantMax, qMin, qMax, argv, DPI, angle, solidPercent, poLarge, poJPG);
}

void Halftone_Function_Main(int noise, std::filesystem::path& inputDir, std::filesystem::path& outputDir, const int& quantMin, const int& quantMax, const int& qMin, const int& qMax, char** argv, int DPI, int angle, float& solidPercent, float poLarge, int poJPG)
{
	for (const auto& entry : filesystem::directory_iterator(inputDir)) {
		if (is_regular_file(entry.path()))
		{
			// Get Input Directory
			filesystem::path inputFile(inputDir);

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

			// Random JPEG quality
			uniform_int_distribution<int> seq2(qMin, qMax);
			int qJpeg = seq2(randomNumberGen01);

			// Chance of Solid black channel
			uniform_int_distribution<int> sequence_solid(1, 200);
			int randomNumber = sequence_solid(randomNumberGen01);
			string percentage_Solid = to_string(randomNumber);

			// Random blur
			uniform_real_distribution<double> sequence02(finalBlurMin, finalBlurMax);
			double randomBlurScale = sequence02(randomNumberGen01);
			double randomFinalBlur = randomBlurScale;

			// Random grain
			uniform_real_distribution<double> sequence03(finalGrainMin, finalGrainMax);
			double finalGrain = sequence03(randomNumberGen01);

			//uniform_int_distribution<int> dpiScale_01(127, 137);
			//int six_Hundred_Scaler = dpiScale_01(randomNumberGen01);

			//uniform_int_distribution<int> dpiScale_02(145, 155);
			//int nine_Hundred_Scaler = dpiScale_02(randomNumberGen01);

			//uniform_int_distribution<int> dpiScale_03(200, 220);
			//int twelve_Hundred_Scaler = dpiScale_03(randomNumberGen01);

			//uniform_int_distribution<int> dpiScale_04(170, 180);
			//int fortyEight_Hundred_Scaler = dpiScale_04(randomNumberGen01);


			///////////////////////////////////////////////////////////////
			///							DPI SCALE						///
			///////////////////////////////////////////////////////////////
			int scalerOffset = 0;

			uniform_int_distribution<int> dpiScale_01(105, 107);
			int two_Hundred_Fifty_Scaler = dpiScale_01(randomNumberGen01);

			uniform_int_distribution<int> dpiScale_02(108, 113);
			int three_Hundred_Scaler = dpiScale_02(randomNumberGen01);

			uniform_int_distribution<int> dpiScale_03(107, 113);
			int six_Hundred_Scaler = dpiScale_03(randomNumberGen01);

			uniform_int_distribution<int> dpiScale_04(145, 155);
			int nine_Hundred_Scaler = dpiScale_04(randomNumberGen01);

			uniform_int_distribution<int> dpiScale_05(200, 220);
			int twelve_Hundred_Scaler = dpiScale_05(randomNumberGen01);

			uniform_int_distribution<int> dpiScale_06(181, 186);
			int twentyFour_Hundred_Scaler = dpiScale_06(randomNumberGen01);

			uniform_int_distribution<int> dpiScale_07( 162, 178 );
			int fortyEight_Hundred_Scaler = dpiScale_07( randomNumberGen01 );




			/// RANDOMISED LASERSCHWERT ANGLES BY 10% ///
			uniform_int_distribution<int> one_angle(92, 97);
			int angle_first = one_angle(randomNumberGen01);

			uniform_int_distribution<int> two_angle(62, 67);
			int angle_second = two_angle(randomNumberGen01);

			uniform_int_distribution<int> three_angle(12, 17);
			int angle_third = three_angle(randomNumberGen01);

			uniform_int_distribution<int> four_angle(32, 37);
			int angle_fourth = four_angle(randomNumberGen01);

			uniform_int_distribution<int> DPIr(1, 7);
			int randomDPI = DPIr(randomNumberGen01);

			vector<int> anglesLaser;
			anglesLaser.push_back(angle_first);
			anglesLaser.push_back(angle_second);
			anglesLaser.push_back(angle_third);
			anglesLaser.push_back(angle_fourth);


			try {
				InitializeMagick(*argv);
				// Read Original Image file
				Image inputReScale, inputCyan, inputMagenta, inputYellow, inputBlack, blackLines, solidBlack;
				inputReScale.read(inFileEXT);

				// Get Image Dimensions
				size_t width = inputReScale.baseColumns();
				size_t height = inputReScale.baseRows();

				auto DPI_scaler = 0;

				int dotsOfHalftone = 0;
				int dotsChooser = DPI;
				//dotsFunc(DPI, dotsOfHalftone);
				string dotsNum;

				switch (dotsChooser) {
				case 0:
					dotsOfHalftone = randomDPI; break;
				case 1:
					dotsOfHalftone = DPI; break;
				case 2:
					dotsOfHalftone = DPI; break;
				case 3:
					dotsOfHalftone = DPI; break;
				case 4:
					dotsOfHalftone = DPI; break;
				case 5:
					dotsOfHalftone = DPI; break;
				case 6:
					dotsOfHalftone = DPI; break;
				case 7:
					dotsOfHalftone = DPI; break;
				}

				switch (dotsOfHalftone)
				{
				case 1:
					dotsNum = "c5x5w";
					DPI_scaler = two_Hundred_Fifty_Scaler;
					cout << "\nDPI 250 " << "\n";
					cout << "Halftone Dot Pattern: c5x5w " << endl;
					break;
				case 2:
					dotsNum = "c5x5w";
					DPI_scaler = three_Hundred_Scaler;
					cout << "\nDPI 300 " << "\n";
					cout << "Halftone Dot Pattern: c5x5w " << endl;
					break;
				case 3:
					dotsNum = "c6x6w";
					DPI_scaler = six_Hundred_Scaler;
					cout << "\nDPI 600 " << "\n";
					cout << "Halftone Dot Pattern: c6x6w " << endl;
					break;
				case 4:
					dotsNum = "h8x8o";
					DPI_scaler = nine_Hundred_Scaler;
					cout << "\nDPI 900 " << "\n";
					cout << "Halftone Dot Pattern: h8x8o " << endl;
					break;
				case 5:
					dotsNum = "h16x16o";
					DPI_scaler = twelve_Hundred_Scaler;
					cout << "\nDPI 1200 " << "\n";
					cout << "Halftone Dot Pattern: h16x16o " << endl;
					break;
				case 6:
					dotsNum = "c21x21w";
					DPI_scaler = twentyFour_Hundred_Scaler;
					cout << "\nDPI 2400 " << "\n";
					cout << "Halftone Dot Pattern: c21x21w " << endl;
					break;
				case 7:
					dotsNum = "c21x21w";
					DPI_scaler = fortyEight_Hundred_Scaler;
					cout << "\nDPI 4800 " << "\n";
					cout << "Halftone Dot Pattern: c21x21w " << endl;
					break;
				}
				
				string DPI_scaler_percent = to_string(DPI_scaler) + "%";


				// Calculate size of Crop after Upscale and Distort
				int widthPercent = width * DPI_scaler / 100;
				int heightPercent = height * DPI_scaler / 100;
				string cropChannelsNew = to_string(widthPercent) + "x" + to_string(heightPercent);
				//cout << cropChannelsNew << endl;


				/////////////////////////////////////////////////
				// Generate Noise Map for Displace PER CHANNEL
				/////////////////////////////////////////////////
				string noiseSize = to_string(width) + "x" + to_string(height);

				Image noiseForDisplaceCyan(Geometry(noiseSize), "gray"), noiseForDisplaceMagenta(Geometry(noiseSize), "gray"), noiseForDisplaceYellow(Geometry(noiseSize), "gray"), noiseForDisplaceBlack(Geometry(noiseSize), "gray"), solidBlackImage(Geometry(noiseSize), "black");

				noiseForDisplaceCyan.colorSpace(sRGBColorspace);
				noiseForDisplaceCyan.addNoise(GaussianNoise);
				noiseForDisplaceCyan.colorSpace(GRAYColorspace);
				noiseForDisplaceCyan.linearStretch(QuantumRange / 0.26, QuantumRange / 0.26);
				noiseForDisplaceCyan.blur(0, 0.9);


				noiseForDisplaceMagenta.colorSpace(sRGBColorspace);
				noiseForDisplaceMagenta.addNoise(GaussianNoise);
				noiseForDisplaceMagenta.colorSpace(GRAYColorspace);
				noiseForDisplaceMagenta.linearStretch(QuantumRange / 0.26, QuantumRange / 0.26);
				noiseForDisplaceMagenta.blur(0, 0.86);


				noiseForDisplaceYellow.colorSpace(sRGBColorspace);
				noiseForDisplaceYellow.addNoise(GaussianNoise);
				noiseForDisplaceYellow.colorSpace(GRAYColorspace);
				noiseForDisplaceYellow.linearStretch(QuantumRange / 0.26, QuantumRange / 0.26);
				noiseForDisplaceYellow.blur(0, 1.0);


				noiseForDisplaceBlack.colorSpace(sRGBColorspace);
				noiseForDisplaceBlack.addNoise(GaussianNoise);
				noiseForDisplaceBlack.colorSpace(GRAYColorspace);
				noiseForDisplaceBlack.linearStretch(QuantumRange / 0.26, QuantumRange / 0.26);
				noiseForDisplaceBlack.blur(0, 1.2);


				// Resize Input Image and adjust gamma in Memory before Splitting
				inputReScale.colorSpace(CMYKColorspace);
				inputReScale.filterType(HanningFilter);
				inputReScale.resize(DPI_scaler_percent);
				inputReScale.gaussianBlur(0, 0.6);
				noiseForDisplaceCyan.resize(DPI_scaler_percent);
				noiseForDisplaceMagenta.resize(DPI_scaler_percent);
				noiseForDisplaceYellow.resize(DPI_scaler_percent);
				noiseForDisplaceBlack.resize(DPI_scaler_percent);

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

				string ditherSize = dotsNum;
				string cropThis;
				string forPrint;

				cout << inFileNoEXT << " Is being processed: " << endl << "UpScale: " << DPI_scaler_percent << endl;

				int angle_of_halftone = 1;
				anglesFunc(angle, angle_of_halftone);
				vector<int> anglesNum;
				switch (angle_of_halftone)
				{
				case 1:
					anglesNum = { 15, 75, 0, 45 };
					cout << "Angles number One: ";
					print(anglesNum);
					cout << endl;
					break;
				case 2:
					anglesNum = { 105, 75, 90, 15 };
					cout << "Angles number Two: ";
					print(anglesNum);
					cout << endl;
					break;
				case 3:
					anglesNum = { 15, 45, 0, 75 };
					cout << "Angles number Three: ";
					print(anglesNum);
					cout << endl;
					break;
				case 4:
					anglesNum = { 165, 45, 90, 105 };
					cout << "Angles number Four: ";
					print(anglesNum);
					cout << endl;
					break;
				case 5:
					anglesNum = anglesLaser;
					cout << "Angles number Laser: ";
					print(anglesNum);
					cout << endl;
				}


				auto t1 = std::chrono::high_resolution_clock::now();
				Dither_and_combine_channels(noise, inputCyan, inputMagenta, inputYellow, inputBlack, anglesNum, ditherSize, cropChannelsNew, solidPercent, randomNumber, noiseForDisplaceCyan, noiseForDisplaceMagenta, noiseForDisplaceYellow, blackLines, inputReScale, poLarge, noiseSize, solidBlackImage, noiseForDisplaceBlack, poJPG, inFileEXT, randomQuant, randomFinalBlur, finalGrain, qJpeg, outFile);
				auto t2 = std::chrono::high_resolution_clock::now();

				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
				cout << "Time taken for this image: " << duration / 1000.0 << " seconds" << "\n" << endl;


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

void Dither_and_combine_channels(int noise, Magick::Image& inputCyan, Magick::Image& inputMagenta, Magick::Image& inputYellow, Magick::Image& inputBlack, std::vector<int>& anglesNum, std::string& ditherSize, std::string& cropChannelsNew, float& solidPercent, int& randomNumber, Magick::Image& noiseForDisplaceCyan, Magick::Image& noiseForDisplaceMagenta, Magick::Image& noiseForDisplaceYellow, Magick::Image& blackLines, Magick::Image& inputReScale, float poLarge, std::string& noiseSize, Magick::Image& solidBlackImage, Magick::Image& noiseForDisplaceBlack, int poJPG, std::string& inFileEXT, int randomQuant, double randomFinalBlur, double finalGrain, int qJpeg, std::string& outFile)
{
	string artARGHor = "3x0";
	string artARGVer = "0x3";

	// Output Cyan Dithered Image
	if (noise == 1) {
		inputCyan.addNoise(GaussianNoise, 0.07);
		cout << "Adding grain\n";
	}
	else {
		cout << "No grain added\n";
	}
	inputCyan.modulate(100, 0, 100);
	case_One(inputCyan, inputMagenta, inputYellow, inputBlack, anglesNum[0], anglesNum[1], anglesNum[2], anglesNum[3], ditherSize, cropChannelsNew, solidPercent, randomNumber);
	inputCyan.artifact("compose:args", artARGHor);
	inputCyan.composite(noiseForDisplaceCyan, 0, 0, DisplaceCompositeOp);
	inputCyan.artifact("compose:args", artARGVer);
	inputCyan.composite(noiseForDisplaceCyan, 0, 0, DisplaceCompositeOp);
	//inputCyan.write("_cyan.png");

	// Output Magenta Dithered Image
	if (noise == 1) {
		inputMagenta.addNoise(GaussianNoise, 0.07);
	}
	inputMagenta.modulate(100, 0, 100);
	inputMagenta.artifact("compose:args", artARGHor);
	inputMagenta.composite(noiseForDisplaceMagenta, 0, 0, DisplaceCompositeOp);
	inputMagenta.artifact("compose:args", artARGVer);
	inputMagenta.composite(noiseForDisplaceMagenta, 0, 0, DisplaceCompositeOp);
	//inputMagenta.write("_Magenta.png");

	// Output Yellow Dithered Image
	if (noise == 1) {
		inputYellow.addNoise(GaussianNoise, 0.05);
	}
	inputYellow.modulate(100, 0, 100);
	inputYellow.artifact("compose:args", artARGHor);
	inputYellow.composite(noiseForDisplaceYellow, 0, 0, DisplaceCompositeOp);
	inputYellow.artifact("compose:args", artARGVer);
	inputYellow.composite(noiseForDisplaceYellow, 0, 0, DisplaceCompositeOp);
	//inputYellow.write("_Yellow.png");

	Black_solid_dithered(randomNumber, solidPercent, blackLines, inputReScale, poLarge, noiseSize, solidBlackImage, inputBlack, noiseForDisplaceBlack);

	// Output Combined Channels to single Colour Image
	std::vector<Image> imageList;
	imageList.push_back(inputCyan);
	imageList.push_back(inputMagenta);
	imageList.push_back(inputYellow);
	imageList.push_back(inputBlack);

	Image rComposite;
	Magick::combineImages(&rComposite, imageList.begin(), imageList.end(), AllChannels, CMYKColorspace);

	Jpeg_compression(noise, randomNumber, poJPG, inFileEXT, randomQuant, rComposite, randomFinalBlur, noiseSize, solidPercent, solidBlackImage, finalGrain, qJpeg, outFile);
}

void Jpeg_compression(int noise, int randomNumber, int poJPG, std::string& inFileEXT, int randomQuant, Magick::Image& rComposite, double randomFinalBlur, std::string& noiseSize, float solidPercent, Magick::Image& solidBlackImage, double finalGrain, int qJpeg, std::string& outFile)
{
	if (randomNumber <= poJPG) {
		cout << inFileEXT << endl << "Quantized: " << randomQuant << endl << "and compressed " << endl;
		rComposite.blur(0, randomFinalBlur);
		rComposite.filterType(CubicFilter);
		rComposite.resize(noiseSize);
		rComposite.colorSpace(sRGBColorspace);

		if (randomNumber <= solidPercent) {
			rComposite.alpha(false);
			rComposite.composite(solidBlackImage, 0, 0, OverCompositeOp);
		}
		else {

		}

		if (noise == 1) {
			rComposite.addNoise(LaplacianNoise, finalGrain);
		}
		rComposite.quantizeDither(false);
		rComposite.quantizeColors(randomQuant);
		rComposite.quantize(true);
		rComposite.defineValue("JPEG", "sampling-factor", samplesYo());
		rComposite.quality(qJpeg);
		rComposite.magick("JPEG");
		rComposite.write(outFile);
	}
	else {
		rComposite.gaussianBlur(0, randomFinalBlur);
		rComposite.filterType(MitchellFilter);
		rComposite.resize(noiseSize);
		rComposite.colorSpace(sRGBColorspace);

		if (randomNumber <= solidPercent) {
			rComposite.alpha(false);
			rComposite.composite(solidBlackImage, 0, 0, OverCompositeOp);
		}
		else {

		}
		if (noise == 1) {
			rComposite.addNoise(LaplacianNoise, finalGrain);
		}
		rComposite.write(outFile);
	}
}

void Black_solid_dithered(int randomNumber, float solidPercent, Magick::Image& blackLines, Magick::Image& inputReScale, float poLarge, std::string& noiseSize, Magick::Image& solidBlackImage, Magick::Image& inputBlack, Magick::Image& noiseForDisplaceBlack)
{
	string artARGHor = "2x0";
	string artARGVer = "0x2";
	// Output Black Dithered/solid Image
	if (randomNumber <= solidPercent) {
		

		// Get black channel and find the edges
		blackLines = inputReScale;
		blackLines.channel(BlackChannel);
		blackLines.sharpen(0.6, 1.0);
		blackLines.morphology(ConvolveMorphology, DoGKernel, "6,1,4");

		if (randomNumber <= poLarge) {
			blackLines.morphology(DilateMorphology, DiamondKernel, "1x3");
		}
		else {
			blackLines.morphology(ErodeMorphology, DiamondKernel, "1x2");
		}

		blackLines.colorSpace(GRAYColorspace);
		blackLines.linearStretch(QuantumRange, QuantumRange);
		blackLines.resize(noiseSize);
		solidBlackImage.composite(blackLines, 0, 0, CopyAlphaCompositeOp);
		solidBlackImage.colorSpace(sRGBColorspace);

		inputBlack.modulate(100, 0, 100);
		inputBlack.artifact("compose:args", artARGHor);
		inputBlack.composite(noiseForDisplaceBlack, 0, 0, DisplaceCompositeOp);
		inputBlack.artifact("compose:args", artARGVer);
		inputBlack.composite(noiseForDisplaceBlack, 0, 0, DisplaceCompositeOp);
	}
	else {

		inputBlack.modulate(100, 0, 100);
		inputBlack.level(0, QuantumRange, 0.75);
		inputBlack.artifact("compose:args", artARGHor);
		inputBlack.composite(noiseForDisplaceBlack, 0, 0, DisplaceCompositeOp);
		inputBlack.artifact("compose:args", artARGVer);
		inputBlack.composite(noiseForDisplaceBlack, 0, 0, DisplaceCompositeOp);
	}
}

void dotsFunc(int DPI, int& dotsOfHalftone)
{
	switch (DPI) {
	case 0:
		dotsOfHalftone = whichDotsYo();
		break;
	case 1:
		dotsOfHalftone = 1;
		break;
	case 2:
		dotsOfHalftone = 2;
		break;
	case 3:
		dotsOfHalftone = 3;
		break;
	}
}

void anglesFunc(int angle, int& angle_of_halftone)
{
	switch (angle) {
	case 0:
		angle_of_halftone = whichAnglesYo();
		break;
	case 1:
		angle_of_halftone = 1;
		break;
	case 2:
		angle_of_halftone = 2;
		break;
	case 3:
		angle_of_halftone = 3;
		break;
	case 4:
		angle_of_halftone = 4;
		break;
	case 5:
		angle_of_halftone = 5;
		break;
	}
}

void case_One(Image& inputCyan, Image& inputMagenta, Image& inputYellow, Image& inputBlack, \
	double c, double m, double y, double b, string& ditherTypeThingy, \
	string& croppingThing, float& solidPercent, int& randomNumber)
{
	double cyanArgsFirst[2] = { c,1 };
	double cyanArgsSecond[2] = { -c,1 };
	double magentaArgsFirst[2] = { m,1 };
	double magentaArgsSecond[2] = { -m,1 };
	double yellowArgsFirst[2] = { y,1 };
	double yellowArgsSecond[2] = { -y,1 };
	double blackArgsFirst[2] = { b,1 };
	double blackArgsSecond[2] = { -b,1 };


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
	if (randomNumber <= solidPercent) {
		inputBlack.blur(0, 0.1);
	}
	else {
		inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsFirst, Magick::MagickTrue);
		inputBlack.orderedDither(ditherTypeThingy);
		inputBlack.distort(ScaleRotateTranslateDistortion, 1, blackArgsSecond, Magick::MagickTrue);
		inputBlack.crop(croppingThing);
	}
}

