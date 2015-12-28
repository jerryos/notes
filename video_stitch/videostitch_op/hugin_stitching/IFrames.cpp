/*
 * IFrames.cpp
 *
 *  Created on: Jun 12, 2015
 *      Author: yunlong
                Zhong Cong
 */

#include <stdio.h>
#include <iostream>
#include <iosfwd>
#include <fstream>
#include <cstring>
using namespace std;
#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/mem.h>
#include <libavutil/pixfmt.h>
}

#include <jpeglib.h>

#include "IFrames.h"

static void SaveJpeg(const char * outpath, AVFrame* pFrame, int width, int height, int iFrame, int cameraId);
static void SaveFrame(const char * outpath, AVFrame *pFrames, int width, int height, int iFrame);
static void SaveAsBMP(const char * outpath, AVFrame *pFrames, int width, int height, int iFrame);

IFrames::IFrames() {
	// TODO Auto-generated constructor stub

}


IFrames::~IFrames() {
	// TODO Auto-generated destructor stub
}



/**
 * decode video file, store decoded frames to specified path.
 *
 * image files are named as 0001.jpg to xxxx.jpg
 */
int IFrames::getIFrames(const char * videoFile, const char * outputPath, bool getKeyFrame, int cameraId, int frameNum) {
        if (getKeyFrame)
          printf("get key frames\n");
        else
          printf("get all frames\n");

	struct SwsContext *pSWSCtx;

	AVFormatContext *pFormatCtx=NULL;  
	int i, videoStream;                   
	AVCodecContext *pCodecCtx=NULL;   
	AVCodec *pCodec;             
	AVFrame *pFrame;            
	AVFrame *pFrameRGB;          
	AVPacket packet;             
	int     numBytes,frameFinished;
	uint8_t *buffer;

	av_register_all();

	if(avformat_open_input(&pFormatCtx,videoFile, NULL, NULL)!=0) {
        cout << "Can not open input : " << videoFile << endl;
        return -1;
    }

	if(avformat_find_stream_info(pFormatCtx,NULL)<0)
		return -1;                                     

	av_dump_format(pFormatCtx, 0, videoFile, 0);// handle debugging function

	videoStream=-1;

	// find video stream
	for(i=0; i< pFormatCtx->nb_streams; i++)

		if (pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)//CODEC_TYPE_VIDEO
		{
			videoStream=i;
			break;
		}
	if(videoStream==-1)
	{
		cout<<"can not find video stream"<<endl;
		return -1;                     

	}

	// find video decoder, "pCodec" is decoder
	pCodecCtx=pFormatCtx->streams[videoStream]->codec;

	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL)
		return -1;                       

	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0)
		return -1;                       


	pFrame=av_frame_alloc();
	pFrameRGB = av_frame_alloc();
	numBytes=avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,pCodecCtx->height);

	buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
	avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,pCodecCtx->width, pCodecCtx->height);

	i=0;

	while(av_read_frame(pFormatCtx,&packet)>=0 && i < frameNum) 
	{
		if(packet.stream_index==videoStream)  
		{
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,&packet); 
			pSWSCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
			if(frameFinished)
			{
			    if(!getKeyFrame || (getKeyFrame && pFrame->key_frame==1))
			    {
                              printf("save frame(%s) %d\n", pFrame->key_frame ? "key frame" : "not key frame", i);
                              sws_scale(pSWSCtx, pFrame->data, pFrame->linesize,0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                              //SaveFrame(outputPath,pFrameRGB,pCodecCtx->width,pCodecCtx->height,i);
                              SaveJpeg(outputPath, pFrameRGB, pCodecCtx->width, pCodecCtx->height, cameraId, i);
                              //SaveAsBMP(outputPath,pFrameRGB,pCodecCtx->width,pCodecCtx->height,i);
                              i++;
                            }
			}
		}
		av_free_packet(&packet);
	}


	av_free(pFrameRGB);
	av_free(pFrame);
	sws_freeContext(pSWSCtx);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return i;

}

void SaveJpeg(const char * outpath, AVFrame* pFrame, int width, int height, int cameraId, int iFrame)
{
    FILE *pFile;
    char szFilename[128];
    memset(szFilename, 0, 128);
    int y;
    //open file
    sprintf(szFilename,"%s/jpg%d_%d.jpg",outpath, cameraId, iFrame);
    printf("save to %s\n", szFilename);

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride;
    uint8_t *buffer;
    FILE *fp;

    buffer = pFrame->data[0];

    fp = fopen(szFilename, "wb");

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);

    jpeg_set_quality(&cinfo, 80, true);

    jpeg_start_compress(&cinfo, TRUE);

    row_stride = width * 3;
    while (cinfo.next_scanline < height)
    {
        row_pointer[0] = &buffer[cinfo.next_scanline * row_stride];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(fp);
    jpeg_destroy_compress(&cinfo);
    return;
}

void SaveFrame(const char * outpath, AVFrame *pFrames, int width, int height, int iFrame)
{
	FILE *pFile;
	char szFilename[128];
	memset(szFilename, 0, 128);
	int y;
	//open file
	sprintf(szFilename,"%s/frame%d.ppm",outpath, iFrame);

	pFile=fopen(szFilename,"w");
	if (!pFile)

		return;
	// Write header

	fprintf(pFile, "p6\n%d %d\n255\n", width, height);

	for (y=0;y<height;y++)

		fwrite(pFrames->data[0]+y*pFrames->linesize[0],1,width*3,pFile);

	fclose(pFile);
}
typedef struct tagBITMAPFILEHEADER {
        uint16_t    bfType;
        uint32_t   bfSize;
        uint16_t    bfReserved1;
        uint16_t    bfReserved2;
        uint32_t   bfOffBits;
} BITMAPFILEHEADER, FAR *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER{
        uint32_t      biSize;
        long       biWidth;
        long       biHeight;
        uint16_t       biPlanes;
        uint16_t       biBitCount;
        uint32_t      biCompression;
        uint32_t      biSizeImage;
        long       biXPelsPerMeter;
        long       biYPelsPerMeter;
        uint32_t      biClrUsed;
        uint32_t      biClrImportant;
} BITMAPINFOHEADER, FAR *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

 
void SaveAsBMP(const char *outpath,AVFrame *pFrameRGB,int width,int height,int iFrame)
{

	FILE *fp;
	char szFilename[128];
	memset(szFilename, 0, 128);
	int y;
	//open file
	sprintf(szFilename,"%s/frame%d.bmp",outpath, iFrame);
	printf("save to %s\n", szFilename);
 BITMAPFILEHEADER bmpheader;
 BITMAPINFOHEADER bmpinfo;
 printf(szFilename,"%s/frame%d.bmp",outpath, iFrame);
 if ( (fp=fopen(szFilename,"w")) == NULL )
  {
   printf ("open file failed!\n");
   return;
  }
 bmpheader.bfType = 0x4d42;
 bmpheader.bfReserved1 = 0;
 bmpheader.bfReserved2 = 0;
 bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
 bmpheader.bfSize = bmpheader.bfOffBits + width*height*24/8;
 bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
 bmpinfo.biWidth = width;
 bmpinfo.biHeight = height;
 bmpinfo.biPlanes = 1;
 bmpinfo.biBitCount = 24;
 bmpinfo.biCompression = 0;
 bmpinfo.biSizeImage = (width*24+31)/32*4*height;
 bmpinfo.biXPelsPerMeter = 100;
 bmpinfo.biYPelsPerMeter = 100;
 bmpinfo.biClrUsed = 0;
 bmpinfo.biClrImportant = 0;
 fwrite (&bmpheader, sizeof(bmpheader), 1, fp);
 fwrite (&bmpinfo, sizeof(bmpinfo), 1, fp);
 fwrite (pFrameRGB->data[0], width*height*24/8, 1, fp);
 fclose(fp);
}
