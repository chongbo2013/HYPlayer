//
// Created by templechen on 2019-08-18.
//

#include "DemuxLooper.h"
#include "../base/native_log.h"
#include "FFDemux.h"

DemuxLooper::DemuxLooper(circle_av_packet_queue *queue) {
    this->queue = queue;
}

DemuxLooper::~DemuxLooper() {

}

void DemuxLooper::handleMessage(Looper::LooperMessage *msg) {
    switch (msg->what) {
        case kMsgDemuxCreated: {
            demux = new FFDemux();
            demux->packetQueue = queue;
            demux->init((const char *) msg->obj);
            demux->isDemuxing = true;
            demux->start();
            break;
        };
        case kMsgDemuxSeek: {
            demux->seek((long) msg->obj);
            break;
        }
        case kMsgDemuxRelease: {
            demux->release();
            quit();
            break;
        }
        default: {
            ALOGE("unknown msg type %d in DemuxLooper", msg->what)
            break;
        }
    }
}

long DemuxLooper::getTotalDuration() {
    if (demux != nullptr) {
        return static_cast<long>(demux->totalDuration);
    }
    return 0L;
}

void DemuxLooper::pthreadExit() {
    delete this;
}
