#include <iostream>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/stitching/stitcher.hpp>
#include <cstdlib>

using namespace std;
using namespace cv;

bool try_use_gpu = false;
bool enableLog = false;
int img_num = 0;
int img_result_num = 0;
vector<Mat> imgs;

void printUsage();
int parseCmdArgs(int argc, char** argv);
int getLengthOfInt(int value);

int main(int argc, char* argv[])
{
    int retval = parseCmdArgs(argc, argv);
    if (retval) return -1;

    Stitcher stitcher = Stitcher::createDefault(try_use_gpu);
    int len = getLengthOfInt(img_result_num);
    int64 total = 0;
    Mat pano;

    for (int i = 0; i < img_result_num; i++) {

      LOGLN("=====> num : " << i);
      int64 t;

      for (int j = 0; j < img_num; j++) {
        char inputName[64] = "";
        memset(inputName, 0, 64);
        sprintf(inputName, "/root/Videos/jpg%d_%d.jpg", j, i);
        LOGLN("inputName : " << inputName); 
        Mat img = imread(inputName);
        if (img.empty())
        {
            cout << "Can't read image '" << inputName << "'\n";
            return -1;
        }
        imgs.push_back(img);
      }

      if (i == 0) {
        if (stitcher.estimateTransform(imgs) != Stitcher::OK)
        {
          //cout << "Can't stitch images, error code = " << int(status) << endl;
          return -1;
        }
      }

      if (enableLog)
        t = getTickCount();

      Stitcher::Status status = stitcher.composePanorama(imgs, pano);
      if (status != Stitcher::OK)
      {
          cout << "Can't stitch images, error code = " << int(status) << endl;
          return -1;
      }

      if (enableLog) {
          int64 currentTime = getTickCount() - t;
          LOGLN("Compositing time(" << i << ")  : " << (currentTime / getTickFrequency()) << " sec");
          total += currentTime;
      }

      char outputName[64];
      memset(outputName, 0, 64);
      char format[64];
      sprintf(format, "/home/zhongcong/video/intel/%%0%dd.jpg", len);
      LOGLN("format : " << format);
      sprintf(outputName, format, i);
      LOGLN("outputName : " << outputName);
      imwrite(outputName, pano);

      for (int j = 0; j < img_num; j++)
        imgs.pop_back();

    }

    if (enableLog)
        LOGLN("Compositing average time : " << ((total / (double)img_result_num) / getTickFrequency()) << " sec");

    return 0;
}


void printUsage()
{
    cout <<
        "Rotation model images stitcher.\n\n"
        "stitching --try_use_gpu <yes|no> --img_num <number> --output <result_img>\n\n"
        "Flags:\n"
        "  --try_use_gpu (yes|no)\n"
        "      Try to use GPU. The default value is 'no'. All default values\n"
        "      are for CPU mode.\n"
        "  --img_num <number of images wait for stitching each time>\n"
        "      The default is 0.\n"
        "  --img_result_num <stitch times>\n"
        "      The default is 0.\n"
        "  --output <result_img>\n"
        "      The default is 'result.jpg'.\n";
        "  --enable_log <1|0>\n"
        "      The default is 0.\n";
}


int parseCmdArgs(int argc, char** argv)
{
    if (argc == 1)
    {
        printUsage();
        return -1;
    }
    for (int i = 1; i < argc; ++i)
    {
        if (string(argv[i]) == "--help" || string(argv[i]) == "/?")
        {
            printUsage();
            return -1;
        }
        else if (string(argv[i]) == "--try_use_gpu")
        {
            if (string(argv[i + 1]) == "no")
                try_use_gpu = false;
            else if (string(argv[i + 1]) == "yes")
                try_use_gpu = true;
            else
            {
                cout << "Bad --try_use_gpu flag value\n";
                return -1;
            }
            i++;
        }
        else if (string(argv[i]) == "--img_num")
        {
            img_num = atoi(argv[i + 1]);
            i++;
        }
        else if (string(argv[i]) == "--img_result_num")
        {
            img_result_num = atoi(argv[i + 1]);
            i++;
        }
        else if (string(argv[i]) == "--enable_log")
        {
            enableLog = atoi(argv[i + 1]);
            i++;
        }
#if 0
        else if (string(argv[i]) == "--output")
        {
            result_name = argv[i + 1];
            i++;
        }
#endif
        else
        {
#if 0
            Mat img = imread(argv[i]);
            if (img.empty())
            {
                cout << "Can't read image '" << argv[i] << "'\n";
                return -1;
            }
            imgs.push_back(img);
#endif
        }
    }
    return 0;
}

int getLengthOfInt(int value) {
  int i = 0;
  do {
    value /= 10;
    i++;
  } while (value);

  return i;
}
