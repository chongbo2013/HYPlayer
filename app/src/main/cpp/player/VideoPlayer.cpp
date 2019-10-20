//
// Created by templechen on 2019-10-18.
//

#include "VideoPlayer.h"

VideoPlayer::VideoPlayer(const char *path, bool usingMediaCodec) {
    packetQueue = new circle_av_packet_queue();
    frameQueue = new circle_av_frame_queue();
    demuxLooper = new DemuxLooper(packetQueue, false);
    decodeLooper = new DecodeLooper(frameQueue, packetQueue, false, usingMediaCodec);
    demuxLooper->sendMessage(demuxLooper->kMsgDemuxCreated, (void *) path);
    decodeLooper->sendMessage(decodeLooper->kMsgDecodeCreated);
    glVideoLooper = new GLVideoLooper(frameQueue);
    glVideoLooper->sendMessage(glVideoLooper->kMsgSurfaceCreated);
}

VideoPlayer::~VideoPlayer() {

}

void VideoPlayer::start() {
    if (glVideoLooper != nullptr) {
        glVideoLooper->sendMessage(glVideoLooper->kMsgSurfaceStart);
    }
}

void VideoPlayer::pause() {
    if (glVideoLooper != nullptr) {
        glVideoLooper->sendMessage(glVideoLooper->kMsgSurfacePause);
    }
}

void VideoPlayer::seek(long pos) {
//    if (slPlayerLooper != nullptr) {
//        slPlayerLooper->sendMessage(slPlayerLooper->kMsgSLPlayerSeek);
//    }
    if (demuxLooper != nullptr) {
        demuxLooper->demux->isDemuxing = false;
        demuxLooper->sendMessage(demuxLooper->kMsgDemuxSeek, reinterpret_cast<void *>(pos));
    }
}

void VideoPlayer::release() {
//    if (slPlayerLooper != nullptr) {
//        slPlayerLooper->sendMessage(slPlayerLooper->kMsgSLPlayerRelease);
//    }
    if (demuxLooper != nullptr) {
        demuxLooper->demux->isDemuxing = false;
        demuxLooper->demux->isOver = true;
        demuxLooper->sendMessage(demuxLooper->kMsgDemuxRelease);
    }
    if (decodeLooper != nullptr) {
        decodeLooper->sendMessage(decodeLooper->kMsgDecodeRelease);
    }
    delete this;
}

// thread unsafe
long VideoPlayer::getTotalDuration() {
    if (demuxLooper != nullptr) {
        return demuxLooper->getTotalDuration();
    }
    return 0;
}

//thread unsafe
long VideoPlayer::getCurrentDuration() {
//    if (slPlayerLooper != nullptr) {
//        return slPlayerLooper->getCurrentPos();
//    }
    return 0;
}

void VideoPlayer::setNativeWindowCreated(ANativeWindow *nativeWindow) {
    if (glVideoLooper != nullptr) {
        glVideoLooper->sendMessage(glVideoLooper->kMsgSurfaceCreated, nativeWindow);
    }
}

void VideoPlayer::setNativeWindowDestroyed() {
    if (glVideoLooper != nullptr) {
        glVideoLooper->sendMessage(glVideoLooper->kMsgSurfaceDestroyed);
        glVideoLooper->quit();
    }
}

void VideoPlayer::setNativeWindowChanged(int width, int height) {
    if (glVideoLooper != nullptr) {
        glVideoLooper->sendMessage(glVideoLooper->kMsgSurfaceChanged, width, height);
    }
}

void VideoPlayer::doFrame() {
    if (glVideoLooper != nullptr) {
        glVideoLooper->sendMessage(glVideoLooper->kMsgSurfaceDoFrame);
    }
}
