#include <ag_video_base.h>

AGVideoBase::~AGVideoBase() throw() {}

int AGVideoBase::width() const { return 1; }

int AGVideoBase::height() const { return 1; }

int AGVideoBase::realWidth() const { return 1; }
int AGVideoBase::realHeight() const { return 1; }

void AGVideoBase::initVideo(int w, int h, int d, bool fs, bool gl, int vw,
                            int vh) {}
