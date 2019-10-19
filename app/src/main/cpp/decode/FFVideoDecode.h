//
// Created by templechen on 2019-10-18.
//

#ifndef HYPLAYER_FFVIDEODECODE_H
#define HYPLAYER_FFVIDEODECODE_H


#include "FFDecode.h"

class FFVideoDecode : public IDecode{

public:

    virtual ~FFVideoDecode();

    virtual bool init();

    virtual bool start();

    virtual bool pause();

    virtual void release();

private:

    AVCodecParameters *parameters;

    AVCodec *codec;

    AVCodecContext *codecContext;

    double timeBase;

};


#endif //HYPLAYER_FFVIDEODECODE_H