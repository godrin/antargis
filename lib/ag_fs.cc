/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * fs.cpp
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

#include <string>
#include <list>
#include <assert.h>
#include <ag_fs.h>
#include <ag_config.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#ifdef WIN32
#include <windows.h>
#include <shlobj.h>
#endif

#include "rk_debug.h"
#include "rk_tools.h"

#include "ag_main.h"
#include "ag_serial.h"
#include <zlib.h>

static std::list<AGString> gFilesystemPathes;

void addPath(const AGString &pName)
  {
    gFilesystemPathes.push_back(pName);
    updateConfig();
  }

void addPathFront(const AGString &pName)
  {
    gFilesystemPathes.push_front(pName);
    updateConfig();
  }


void initFS(const char *argv0)
  {
    addPath("data");
    addPath("data/fonts");
#ifndef WIN32
    addPath("/usr/local/share/antargisgui/pics");
    addPath("/usr/local/share/antargisgui");
    addPath("/usr/local/share/antargis");
    addPath("/usr/share/antargis");
    addPath("/usr/share/antargis/data");
    addPath("/usr/share/fonts/truetype/freefont/");
#endif
    addPath(getWriteDir());
#ifdef WIN32
    addPath("c:/Windows/Fonts/");
#endif
  }

void checkDir(const std::string &s)
  {
#ifdef WIN32
    DWORD rc = CreateDirectory(s.c_str(), NULL);
    DWORD le;
    if(rc==0)
      {
        le=GetLastError();
        if(le==ERROR_ALREADY_EXISTS)
          {
            // dir exists -ok
          }
        else
          {
            dbout(0,"could not create dir:"<<s);
            dbout(0,"rc:"<<rc);
            throw std::runtime_error("could not create dir");
          }
      }
#else
    int rc;
    errno = 0;
    rc = mkdir(s.c_str(), S_IRWXU);
    if(rc==-1)
      {
        switch(errno)
        {
        case EEXIST:
          return; // everything's fine - directory exists already;
        case EACCES:
          return; // probably exists - we don't have access
        default:
          dbout(0,"could not create dir:"<<s);
          throw std::runtime_error("could not create dir");
        }
      }
#endif
  }

void checkParentDirs(const std::string &s)
  {
#ifdef WIN32
    std::string sep="\\";
#else
    std::string sep="/";
#endif
    std::vector<std::string> a=split(sep,s);

    a.pop_back();

    std::ostringstream os;
    for(std::vector<std::string>::iterator i=a.begin();i!=a.end();++i)
      {
        if(i->length()==0)
          continue;
        if(i!=a.begin())
          os<<sep;
        os<<*i;
        if(os.str().find(sep)!=std::string::npos)
          checkDir(os.str());
      }
  }

AGString checkFileName(AGString s)
  {
#ifdef WIN32
    if(s.length()>300)
      throw std::runtime_error("possible segfault???");
    s=replace(s,"/","\\");
    s=replace(s,"\\\\","\\"); // prevent windows from searching on network
#endif
    return s;
  }

std::string directLoad(const std::string &pName)
  {
    std::string fn=checkFileName(pName);
    if(!fileExists(fn))
      return "";

    FILE *f=fopen(fn.c_str(),"rb");
    if(!f)
      return "";
    fseek(f,0,SEEK_END);
    long len=ftell(f);
    fseek(f,0,SEEK_SET);
    char *buffer=new char[len+2];
    fread(buffer,1,len,f);

    fclose(f);

    std::string r(buffer,len);
    checkedDeleteArray(buffer);
    return r;
  }

AGString findFile(const AGString &pName)
  {
    if(fileExists(pName))
      return pName;
    for(std::list<AGString>::iterator i=gFilesystemPathes.begin();i!=gFilesystemPathes.end();i++)
      {
        std::string n=*i+"/"+pName;
        n=checkFileName(n);
        if(fileExists(n))
          return n;
      }


    if(pName.length()>5)
      {
        if(pName.substr(pName.length()-4,3)=="png")
          return findFile(pName.substr(0,pName.length()-3)+"jpg");
      }

    return "";
  }

std::string loadFromPath(const std::string &pName)
  {
    std::string r;
    r=directLoad(pName);
    if(r.length())
      return r;

    for(std::list<AGString>::iterator i=gFilesystemPathes.begin();i!=gFilesystemPathes.end();i++)
      {
        r=directLoad(*i+"/"+pName);
        if(r.length())
          return r;
      }

    for(std::list<AGString>::iterator i=gFilesystemPathes.begin();i!=gFilesystemPathes.end();i++)
      cdebug("path:"<<*i);

    cdebug("LOAD FAILED:"<<pName);

    return r;
  }

AGString loadFile(const AGString &pName)
  {
    return loadFromPath(checkFileName(pName));
  }

#ifdef WIN32
/* GetUserProfileDirectory() is only available on >= NT4 (no 9x/ME systems!) */
typedef BOOL (STDMETHODCALLTYPE FAR * LPFNGETUSERPROFILEDIR) (
    HANDLE hToken,
    LPTSTR lpProfileDir,
    LPDWORD lpcchSize);
#endif


std::string gUserDir;
std::string getUserDir()
  {
    if(gUserDir=="")
      {
#ifdef WIN32
        char *userDir=0;

        DWORD psize = 0;
        char dummy[1];
        BOOL rc = 0;
        HANDLE processHandle;            /* Current process handle */
        HANDLE accessToken = NULL;       /* Security handle to process */
        LPFNGETUSERPROFILEDIR GetUserProfileDirectory;
        HMODULE lib;

        assert(userDir == 0);

        /*
         * GetUserProfileDirectory() is only available on NT 4.0 and later.
         *  This means Win95/98/ME (and CE?) users have to do without, so for
         *  them, we'll default to the base directory when we can't get the
         *  function pointer.
         */

        lib = LoadLibrary("userenv.dll");
        if (lib)
          {
            /* !!! FIXME: Handle Unicode? */
            GetUserProfileDirectory = (LPFNGETUSERPROFILEDIR)
            GetProcAddress(lib, "GetUserProfileDirectoryA");
            if (GetUserProfileDirectory)
              {
                processHandle = GetCurrentProcess();
                if (OpenProcessToken(processHandle, TOKEN_QUERY, &accessToken))
                  {
                    /*
                     * Should fail. Will write the size of the profile path in
                     *  psize. Also note that the second parameter can't be
                     *  NULL or the function fails.
                     */
                    rc = GetUserProfileDirectory(accessToken, dummy, &psize);
                    assert(!rc);  /* success?! */

                    /* Allocate memory for the profile directory */
                    userDir = new char[psize+1];
                    if (userDir != NULL)
                      {
                        if (!GetUserProfileDirectory(accessToken, userDir, &psize))
                          {
                            checkedDeleteArray(userDir);
                            userDir = NULL;
                          } /* if */
                      } /* else */
                  } /* if */

                CloseHandle(accessToken);
              } /* if */

            FreeLibrary(lib);
          } /* if */

        if (userDir == NULL)  /* couldn't get profile for some reason. */
          {
            /* Might just be a non-NT system; resort to the basedir. */
            userDir = ".";
            std::cerr<<"sorry, this game doesn't run correctly on non-nt systems (win98/me)"<<std::endl;
          } /* if */
        gUserDir=userDir;

#else
        gUserDir=getenv("HOME");
#endif
      }
    return gUserDir;
  }


#ifdef WIN32

#define SHGFP_TYPE_CURRENT 0

AGString getDocumentsDir()
  {
    CHAR wszPath[MAX_PATH];

    HWND hWnd=0;

    SHGetFolderPath( hWnd, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, wszPath );

    AGString s(wszPath);

    return s;

  }

#else

AGString getDocumentsDir()
  {
    return getUserDir()+"/Desktop";
  }


#endif


AGString getWriteDir()
  {
    return getUserDir()+"/."+getAppName();
  }

bool saveFile(const AGString &pName,const AGString &pContent)
  {
    std::string n=checkFileName(getWriteDir()+"/"+pName);

    checkParentDirs(n);


    FILE *f=fopen(n.c_str(),"wb");
    if(!f)
      return false;
    fwrite(pContent.c_str(),pContent.length(),1,f);
    fclose(f);

    return true;
  }

bool fileExists(const AGString &pName)
  {
#ifdef WIN32
    if(GetFileAttributes(pName.c_str()) == INVALID_FILE_ATTRIBUTES)
      return false;
    return true;
#endif

    FILE *f=fopen(pName.c_str(),"r");
    bool found=false;

    if(f)
      {
        found=true;
        fclose(f);
      }

    return found;
  }

std::vector<AGString> getDirectoryInternal(const AGString &pDir)
  {
    std::vector<AGString> v;

#ifdef WIN32
    WIN32_FIND_DATA ent;

    std::string dir=pDir+"\\*";

    dir=replace(dir,"\\\\","\\"); // remove doubles

    char path[dir.length()+20];
    strcpy(path,dir.c_str());

    HANDLE d=FindFirstFile(path,&ent);

    if(d)
      {
        do
          {
            v.push_back(ent.cFileName);
          }
        while(FindNextFile(d,&ent)!=0);

        FindClose(d);
      }

#else
    struct dirent *ent;
    DIR *dir;
    std::string dirname=pDir;//+"/*";
    dbout(0,"DIR:"<<dirname);
    dir=opendir(dirname.c_str());
    if(dir)
      {
        while((ent=readdir(dir)))
          {
            dbout(0,"found:"<<ent->d_name);
            v.push_back(ent->d_name);
          }
      }


#endif

    return v;
  }

std::vector<AGString> getDirectory(const AGString &pDir)
  {
    std::vector<AGString> v;
    std::list<AGString> ps=gFilesystemPathes;
    ps.push_front(""); // add current dir
    ps.push_front("."); // add current dir


    for(std::list<AGString>::iterator i=ps.begin();i!=ps.end();i++)
      {

        std::vector<AGString> a=getDirectoryInternal(*i+"/"+pDir);
        std::copy(a.begin(),a.end(),std::back_inserter(v));
      }

    return v;
  }




AGString compress(const AGString &pString)
  {
    BinaryStringOut o;
    o<<(Uint32)pString.length();

    uLongf destlen=pString.length()+1000;
    char *buf=new char[destlen];

    compress((Bytef*)buf,&destlen,(Bytef*)pString.c_str(),pString.length());

    std::string r=o.getString()+std::string(buf,destlen);
    checkedDeleteArray(buf);
    return r;
  }
AGString uncompress(const AGString &pString)
  {
    BinaryStringIn i(pString);
    uLongf orig;
    Uint32 o;
    i>>o;
    orig=o;


    char *buf=new char[orig+10];
    uncompress((Bytef*)buf,&orig,(Bytef*)pString.c_str()+4,pString.length()-4);
    std::string r(buf,orig);
    checkedDeleteArray(buf);

    return r;
  }

AGString getDirSep()
  {
#ifdef WIN32
    return "\\";
#else
    return "/";
#endif
  }

std::vector<AGString> getLoadPaths()
  {
    std::vector<AGString> a;
    std::copy(gFilesystemPathes.begin(),gFilesystemPathes.end(),std::back_inserter(a));

    return a;
  }
