#ifndef __AG_REACTOR_H
#define __AG_REACTOR_H

class AGReactor;

template<class R,class P0=void,class P1=void,class P2=void>
class AGSignal2 {
  public:
    AGSignal2(AGReactor *reactor):
      mReactor(reactor) {
      }

    R operator()() {
      return operator()(0,0,0);
    }

    R operator()(P0 p0) {
      return operator()(p0,0,0);
    }

    R operator()(P0 p0,P1 p1) {
      return operator()(p0,p1,0);
    }

    R operator()(P0 p0,P1 p1,P2 p2);
  private:
    AGReactor *mReactor;
};

class AGReactor {
};




template<class R,class P0,class P1,class P2>
inline R AGSignal2<R,P0,P1,P2>::operator()(P0 p0,P1 p1,P2 p2) {

}


#endif

