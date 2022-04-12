/*
 * Copyright (c) 2007 by David Kamphausen. All rights reserved.
 *
 * ag_rand.cc
 * by David Kamphausen (david.kamphausen@web.de)
 *
 * The "Antargis" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */

#include "ag_rand.h"
#include "ag_main.h"
#include "ag_profiler.h"
#include "rk_debug.h"

#include <sstream>

// static mt_state mState;

AGRandomizer::AGRandomizer(const std::string &pSeed) {
  std::istringstream is;
  is.str(pSeed);

  mState.stateptr = 0;
  mState.initialized = 0;

  is >> mState.stateptr;
  is >> mState.initialized;

  for (unsigned long i = 0; i < MT_STATE_SIZE; i++) {
    mState.statevec[i] = 0;
    is >> mState.statevec[i];
  }

  assert(mState.stateptr < MT_STATE_SIZE && mState.stateptr >= 0);
}

AGRandomizer::~AGRandomizer() throw() {}

float AGRandomizer::operator()(float f) {
  float d = (float)mts_drand(&mState);
  d *= f;

  return d;
}
int AGRandomizer::operator()(int i) {
  int r = mts_lrand(&mState);
  r %= i;
  return r;
}

std::string AGRandomizer::stateToString() const {
  std::ostringstream os;
  os << mState.stateptr << " " << mState.initialized << " ";

  for (unsigned long i = 0; i < MT_STATE_SIZE; i++)
    os << mState.statevec[i] << " ";

  return os.str();
}

int agRand(int i) {
  AGRandomizerBase *r = getMain()->getRand();
  if (!r)
    throw std::runtime_error("Randomizer not set!");
  return (*r)(i);
}
float agRand(float f) {
  AGRandomizerBase *r = getMain()->getRand();
  if (!r)
    throw std::runtime_error("Randomizer not set!");
  return (*r)(f);
}
