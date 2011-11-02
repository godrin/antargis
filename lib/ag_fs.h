/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * fs.h
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

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#ifndef __FS_H
#define __FS_H

#include <rk_string.h>
#include <rk_base.h>

#include <vector>

struct FileNotFound
  {
    FileNotFound(const std::string &f):filename(f){}
    std::string filename;
  };

AGEXPORT void initFS(const char *argv0);
AGEXPORT AGString loadFile(const AGString &pFilename);
AGEXPORT bool saveFile(const AGString &pFilename,const AGString &pContent);

AGEXPORT bool fileExists(const AGString &pName);

AGEXPORT std::vector<AGString> getDirectory(const AGString &pDir);
AGEXPORT std::vector<AGString> getLoadPaths();

AGEXPORT AGString checkFileName(AGString s);
AGEXPORT AGString getWriteDir();
AGEXPORT AGString findFile(const AGString &pName);

AGEXPORT AGString getDocumentsDir();

AGEXPORT void addPath(const AGString &pName);
AGEXPORT void addPathFront(const AGString &pName);

AGEXPORT AGString compress(const AGString &pString);
AGEXPORT AGString uncompress(const AGString &pString);

AGEXPORT AGString getDirSep();

#endif
