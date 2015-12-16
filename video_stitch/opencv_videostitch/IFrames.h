/*
 * IFrames.h
 *
 *  Created on: Jun 12, 2015
 *      Author: yunlong
 */

#ifndef DECODER_IFRAMES_H_
#define DECODER_IFRAMES_H_

class IFrames {
public:
	IFrames();
	~IFrames();

public:
	int getIFrames(const char * videoFile, const char * outputPath, bool getKeyFrame, int cameraId, int frameNum);
};

#endif /* DECODER_IFRAMES_H_ */
