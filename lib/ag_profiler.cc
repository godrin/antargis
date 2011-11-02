#include <time.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <stack>
#include <list>
#include <set>

#include "ag_profiler.h"

#include "rk_debug.h"

#include <SDL.h>

#ifdef MPROFILE

#define RDTSC_TIMER

double
volatile Uint64 get_time_unit()
  {
    unsigned int a = 0, b = 0;
#ifdef RDTSC_TIMER
#  if defined(__GNUC__)
    asm ("RDTSC": "=a" (a), "=d" (b) : );
#  elif defined(_MSC_VER)
    __asm RDTSC
    __asm mov a, eax
    __asm mov b, edx
#  endif
#endif
    return ( (Uint64)a + ((Uint64)b<<32));
  }

Uint64 convert_time(Uint64 t)
  {
    static double period = 0;
    if(period)
      return (Uint64)(t*period);
    Uint64 b = get_time_unit();
#ifdef WIN32
    SDL_Delay(50);
    period = 50000.0/((get_time_unit()-b));
#else
    usleep(1000000);
#ifdef RDTSC_TIMER
    period = 1000.0/((get_time_unit()-b));
#else
    period = 1;
#endif
#endif
    return convert_time(t);
  }

struct StackTraceEnter
{
  SOURCE_LINE* mSrcline;
  Uint64 mStartTime;
};

static std::stack<StackTraceEnter> call_stack;
static std::set<SOURCE_LINE*> used_functions;

static std::string getStackNodeString(const SOURCE_LINE* stNode)
  {
    std::string strName;
    if(stNode->name!=NULL)
      strName = std::string(" ") + stNode->name;
    std::stringstream s;
    s<<stNode->file<<", line "<<stNode->line<<" fn: "<<stNode->funct+strName<<" time: "<<convert_time(stNode->time)<<" count: "<<stNode->count;
    if(stNode->count>0)
      s<<" per call:"<<convert_time(stNode->time)/double(stNode->count);
    s<<std::endl;
    return s.str();
  }

UserStackTraceHelper::UserStackTraceHelper( SOURCE_LINE* srcline)
  {
    StackTraceEnter st;
    st.mStartTime = get_time_unit();
    st.mSrcline = srcline;
    srcline->count++;
    call_stack.push(st);
    used_functions.insert(srcline);
  }

UserStackTraceHelper::~UserStackTraceHelper()
  {
    StackTraceEnter st=call_stack.top();
    call_stack.pop();
    st.mSrcline->time += get_time_unit() - st.mStartTime;
  }


std::string UserStackTraceHelper::getStackTraceString()
  {
    std::string stack_string = "";

    std::stack<StackTraceEnter> tmp;
    tmp = call_stack;
    while(!tmp.empty())
      {
        StackTraceEnter stNode = tmp.top();
        stack_string += getStackNodeString(stNode.mSrcline);
        tmp.pop();
      }
    return stack_string;
  }

class _getProfilerString
{
  std::string mAll;
public:
  _getProfilerString(){mAll = "";};

  void operator()(const SOURCE_LINE* l)
  {
    mAll+= getStackNodeString(l);
  }
  operator std::string()
    {
      return mAll;
    }
};

static bool cmpTime(const SOURCE_LINE* a, const SOURCE_LINE* b)
  {
    return a->time<b->time;
  }

std::string UserStackTraceHelper::getProfilerString()
  {
    std::list<SOURCE_LINE*> lst;
    std::copy(used_functions.begin(), used_functions.end(), std::back_inserter(lst));
    lst.sort(cmpTime);
    return std::for_each(lst.begin(), lst.end(), _getProfilerString());
  }


class AGProfilerWriter
{
public:
  ~AGProfilerWriter()
    {
      CTRACE;
      std::ofstream os("prof.txt");
      os<<UserStackTraceHelper::getProfilerString()<<std::endl;
      os<<UserStackTraceHelper::getStackTraceString()<<std::endl;
    }
};
AGProfilerWriter gProfilerWriter;

#ifdef RUN_PROFILER_TEST

//  Test code
#include <stdlib.h>

class A
{
public:
  A(){STACKTRACE sleep(10);}
  ~A(){STACKTRACE}
  void f()
    {
      STACKTRACE
      for(int i=0; i<30000000; i++)
        {
          if(i % 5 > random())
            i++;
        }
    }
  void f2()
    {
      STACKTRACE
      for(int i=0; i<300; i++)
        {
          if(i % 5 > random())
            i++;
        }
    }
};


int main()
  {
      {
        STACKTRACE
        A* b;
        b = new A();
        b->f();
        for(int i=1;i<5;i++)
          b->f2();
      }

      printf("profile:\n\n %s\n", UserStackTraceHelper::getProfilerString().c_str());
      printf("call stack:\n\n %s\n", UserStackTraceHelper::getStackTraceString().c_str());
      return 0;
  }
#endif

#endif
