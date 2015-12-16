/*
 * IFrames.cpp
 *
 *  Created on: Jun 12, 2015
 *      Author: yunlong
                Zhong Cong
 */

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "IFrames.h"

using namespace std;
void printUsage();
int parseCmdArgs(int argc, char** argv);

char* inputDir;
char* outputDir;
bool  tryUseMultiThread = false;
int videoNum = 0;
int frameNum = 0;

int main(int argc, char** argv) 
{
    int retval = parseCmdArgs(argc, argv);
    if (retval) return -1;

    int i;
    IFrames iframes;

    for (i = 0; i < videoNum; i++) {
        char videoFileName[64];
        sprintf(videoFileName, "%s/videoinput%d.mp4", inputDir, i);
        printf("videoFileName : %s\n", videoFileName);

        if (iframes.getIFrames(videoFileName, outputDir, 0, i, frameNum) == -1) {
            printf("process transcode error\n");
            return 0;
        }
    }

    return 0;
}

void printUsage()
{
    printf(
        "Transcode: Decode mp4 and encode to jpg.\n\n"
        "transcode \n\n"
        "  --try_use_multi_thread (yes|no)\n"
        "      Try to use multi-thread. The default value is 'no'. All default values\n"
        "      are for single-thread mode.\n"
        "  --input <input dir>\n"
        "      The default is '/root/Videos'.\n"
        "  --output <output dir>\n"
        "      The default is <input dir>.\n"
        "  --video_num <input video numbers>\n"
        "      The default is 0.\n"
        "  --frame_num <video frame numbers>\n"
        "      The default is 0.\n");
}

int parseCmdArgs(int argc, char** argv)
{
    if (argc == 1) {
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
        else if (string(argv[i]) == "--try_use_multi_threads")
        {
            if (string(argv[i + 1]) == "no")
                tryUseMultiThread = false;
            else if (string(argv[i + 1]) == "yes")
                tryUseMultiThread = true;
            else
            {
                cout << "Bad --try_use_multi_threads flag value\n";
                return -1;
            }
            i++;
        }
        else if (string(argv[i]) == "--input")
        {
            inputDir = argv[i + 1];
            i++;
        }
        else if (string(argv[i]) == "--output")
        {
            outputDir = argv[i + 1];
            i++;
        }
        else if (string(argv[i]) == "--video_num")
        {
            videoNum = atoi(argv[i + 1]);
            i++;
        }
        else if (string(argv[i]) == "--frame_num")
        {
            frameNum = atoi(argv[i + 1]);
            i++;
        }
        else
        {
        }
    }

    if (inputDir == NULL)
        inputDir = "/root/Videos";
    if (outputDir == NULL)
        outputDir = inputDir;

    return 0;
}
