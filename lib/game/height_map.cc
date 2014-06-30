#include "height_map.h"
#include "terrain.h"
#include <ag_serial.h>
#include <ag_main.h>
#include <ag_rand.h>
#include "terrain_complex.h"

//////////////////////////////////////////////////////////////////////////
// HeightMap
//////////////////////////////////////////////////////////////////////////

char TerrainNames[][20]= {"water","sand","earth","grass","grass2","forest","rock","rock2"};

std::vector<float> genSomeHeights ( int mW,int mH,float mMaxHeight );

HeightMap::HeightMap ( Scene *pScene,int w,int h ) :
  sigMapChanged ( this,"mapChanged" ),
  sigMapChangedComplete ( this,"mapChangedComplete" ),
  mTerrainTypes ( LASTTERRAIN+1 ),
  mW ( w ),mH ( h ),mChangeRect ( AGVector2(),AGVector2() ),mChanges ( 0 ),
  mScene ( pScene )

{
  mHeights=genSomeHeights ( w+2,h+2,5 );

  for ( int t=FIRSTTERRAIN; t<LASTTERRAIN; t++ )
    mTerrainTypes[TerrainType ( t )]=genSomeHeights ( w+2,h+2,1 );

  mTerrain=0;

  setTerrainScale ( WATER,0 );
  setTerrainScale ( SAND,8/32.0 );
  setTerrainScale ( EARTH,12/32.0 );
  setTerrainScale ( GRASS,16/32.0 );
  setTerrainScale ( GRASS2,20/32.0 );
  setTerrainScale ( FOREST,23/32.0 );
  setTerrainScale ( ROCK,31/32.0 );
  setTerrainScale ( ROCK2,31/32.0 );

  checkTerrain();
}

HeightMap::~HeightMap() throw()
{
  if ( mTerrain )
    checkedDelete ( mTerrain );
}

void HeightMap::initTerrainMesh()
{
  if ( videoInited() && mScene ) {
    if(false)
      mTerrain=new AntTerrainComplex(mScene,*this);
    else
      mTerrain=new Terrain ( mScene,*this );
  }
}

void HeightMap::setTerrain ( TerrainBase *pTerrain )
{
  assert ( pTerrain );
  assert ( !mTerrain );
  mTerrain=pTerrain;
}


void HeightMap::setHeight ( float height )
{
  for ( size_t y=0; y<mH+2; y++ )
    for ( size_t x=0; x<mW+2; x++ )
    {
      set ( x,y,height );
      setTerrain ( x,y,EARTH,1 );
    }

  mapChanged();
}

void HeightMap::addChange ( const AGVector2 &v )
{
  mChanges++;
  if ( mChanges==1 )
  {
    mChangeRect=AGRect2 ( v,v );
  }
  else
    mChangeRect.include ( v );
}

AGRect2 HeightMap::getChangeRect() const
{
  return mChangeRect;
}


void HeightMap::setTerrain ( size_t x,size_t y,TerrainType t,float v )
{
  size_t p=x+y* ( mW+2 );

  //  cdebug(p<<"  "<<EARTH<<" "<<mTerrainTypes[EARTH].size());
  assert ( mTerrainTypes[EARTH].size() >p );
  mTerrainTypes[t][p]=v;

  float sum=0;
  for ( int i=FIRSTTERRAIN; i<LASTTERRAIN; i++ )
  {
    assert ( mTerrainTypes[TerrainType ( i )].size() >p );
    if ( t!=i )
      sum+=mTerrainTypes[TerrainType ( i )][p];
  }

  for ( int i=FIRSTTERRAIN; i<LASTTERRAIN; i++ )
    if ( t!=i )
    {
      if ( sum==0 || v==1 )
        mTerrainTypes[TerrainType ( i )][p]=0;
      else
        mTerrainTypes[TerrainType ( i )][p]=mTerrainTypes[TerrainType ( i )][p]/sum* ( 1-v ); // so sum(mTerraintypes[x][.]) == 1
    }
  addChange ( AGVector2 ( x,y ) );
}

float HeightMap::getTerrain ( size_t x,size_t y,TerrainType t ) const
{
  size_t p=x+y* ( mW+2 );
  return const_cast<HeightMap*> ( this )->mTerrainTypes[t][p];
}




void HeightMap::set ( size_t x,size_t y,float height )
{
  assert ( x>=0 );
  assert ( y>=0 );
  assert ( x<mW+2 );
  assert ( y<mH+2 );
  mHeights[x+y* ( mW+2 )]=height;
  addChange ( AGVector2 ( x,y ) );
}

void HeightMap::loadBinary ( BinaryIn &is )
{
  float f;

  Uint32 tw,th; // temporary sizes for AMD64
  Uint32 version;
  is>>tw>>th;
  mW=tw;
  mH=th;

  if ( tw > 0 && th>0 )
  {
    version=0;
  }
  else
  {
    is>>version;
  }
  if ( version==0 )
  {

    cdebug ( "mw:"<<mW<<"  "<<mH );
    //  cdebug("pos:"<<is.pos());

    assert ( mW<10000 && mH<10000 );


    mHeights=std::vector<float> ( mW*mH*4 );
    for ( size_t y=0; y<mH+2; y++ )
    {
      for ( size_t x=0; x<mW+2; x++ )
      {
        is>>f;
        mHeights[x+y* ( mW+2 )]=f;
      }
    }

    for ( int i=FIRSTTERRAIN; i<LASTTERRAIN; i++ )
    {
      mTerrainTypes[TerrainType ( i )]=std::vector<float> ( mW*mH*4 );

      for ( size_t y=0; y<mH+2; y++ )
      {
        for ( size_t x=0; x<mW+2; x++ )
        {
          is>>f;
          mTerrainTypes[TerrainType ( i )][x+y* ( mW+2 )]=f;
          addChange ( AGVector2 ( x,y ) );
        }
      }
    }
  }
  checkTerrain();
}

void HeightMap::saveBinary ( BinaryOut &os,int version ) const
{
  if ( version==0 )
  {
    os<< ( Uint32 ) mW<< ( Uint32 ) mH;

    for ( size_t y=0; y<mH+2; y++ )
    {
      for ( size_t x=0; x<mW+2; x++ )
      {
        os<<mHeights[x+y* ( mW+2 )];
      }
    }


    for ( int i=FIRSTTERRAIN; i<LASTTERRAIN; i++ )
    {
      for ( size_t y=0; y<mH+2; y++ )
      {
        for ( size_t x=0; x<mW+2; x++ )
        {
          os<<const_cast<HeightMap*> ( this )->mTerrainTypes[TerrainType ( i )][x+y* ( mW+2 )]; // trust me - I'm const
        }
      }
    }
  }
  else if ( version==1 )
  {
    os<< ( Uint32 ) 0;
    os<< ( Uint32 ) 0;
    os<< ( Uint32 ) version;
    os<< ( Uint32 ) mW<< ( Uint32 ) mH;
    for ( size_t y=0; y<mH+2; y++ )
    {
      for ( size_t x=0; x<mW+2; x++ )
      {
        os<<mHeights[x+y* ( mW+2 )];
      }
    }


    for ( int i=FIRSTTERRAIN; i<LASTTERRAIN; i++ )
    {
      for ( size_t y=0; y<mH+2; y++ )
      {
        for ( size_t x=0; x<mW+2; x++ )
        {
          os<<const_cast<HeightMap*> ( this )->mTerrainTypes[TerrainType ( i )][x+y* ( mW+2 )]; // trust me - I'm const
        }
      }
    }

  }
}

bool HeightMap::loadXML ( const Node &node )
{
  AGString filename=node.get ( "filename" );
  if ( filename.length() )
  {
    filename=findFile ( filename );
    BinaryFileIn is ( filename );
    loadBinary ( is );
  }
  else
  {
    Node::NodeVector nv=node.getChildren ( "data" );
    if ( nv.size() )
    {
      AGString c=nv[0]->getContent();
      BinaryStringIn is ( hexToBinary ( c ) );
      loadBinary ( is );
    }
    else
    {
      mW=node.get ( "width" ).toInt();
      mH=node.get ( "height" ).toInt();

      mHeights=std::vector<float> ( mW*mH*4 );

      for ( int i=FIRSTTERRAIN; i<LASTTERRAIN; i++ )
      {
        mTerrainTypes[TerrainType ( i )]=std::vector<float> ( mW*mH*4 );
        cdebug ( mTerrainTypes[TerrainType ( i )].size() );
        Node::NodeVector gv=node.getChildren ( TerrainNames[i] );
        if ( gv.size() ==0 )
          continue;
        Node &g=**gv.begin();

        std::istringstream is ( g.getContent() );

        float f;
        for ( size_t y=0; y<mH+2; y++ )
        {
          for ( size_t x=0; x<mW+2; x++ )
          {
            is>>f;
            mTerrainTypes[TerrainType ( i )][x+y* ( mW+2 )]=f;
            addChange ( AGVector2 ( x,y ) );
          }
        }
      }


      Node::NodeVector hv=node.getChildren ( "height" );

      if ( hv.size() ==0 ) // || gv.size()==0)
        return false;
      assert ( hv.size() ==1 );
      Node &h=**hv.begin();


      std::istringstream ish ( h.getContent() );

      float f;
      for ( size_t y=0; y<mH+2; y++ )
      {
        for ( size_t x=0; x<mW+2; x++ )
        {
          ish>>f;
          mHeights[x+y* ( mW+2 )]=f;
        }
      }
    }
  }

  checkTerrain();

  // compete change
  if ( mTerrain )
    mTerrain->mapChangedComplete();
  //  mTerrain->addToScenes();
  mChanges=0;
  mChangeRect=AGRect2 ( AGVector2(),AGVector2() );


  sigMapChangedComplete ( new AGEvent ( this,"mapChangedComplete" ) );
  return true;
}

void HeightMap::newMap ( int w,int h )
{
  mW=w;
  mH=h;

  mHeights=genSomeHeights ( w+2,h+2,5 );

  for ( int t=FIRSTTERRAIN; t<LASTTERRAIN; t++ )
    mTerrainTypes[TerrainType ( t )]=genSomeHeights ( w+2,h+2,1 );

  checkTerrain();

  // compete change
  if ( mTerrain )
    mTerrain->mapChangedComplete();
  //  mTerrain->addToScenes();
  mChanges=0;
  mChangeRect=AGRect2 ( AGVector2(),AGVector2() );

}


void HeightMap::mapChanged()
{
  sigMapChanged ( new AGEvent ( this,"mapChanged" ) );
  mChanges=0;
  mChangeRect=AGRect2 ( AGVector2(),AGVector2() );
}


void HeightMap::saveXML ( Node &node ) const
{
  int version=0;
  node.set ( "width",AGString ( mW ) );
  node.set ( "height",AGString ( mH ) );

  if ( false ) //(mW<=64 && mH<=64) || mName.length()==0)
  {
    AGStringStream osh;
    osh.precision ( 2 );

    for ( size_t y=0; y<mH+2; y++ )
    {
      for ( size_t x=0; x<mW+2; x++ )
      {
        osh<<get ( x,y ) <<" ";
      }
      osh<<"\n";
    }
    node.addChild ( "height" ).setContent ( osh.str() );

    for ( int i=FIRSTTERRAIN; i<LASTTERRAIN; i++ )
    {
      std::ostringstream os;
      os.precision ( 2 );
      for ( size_t y=0; y<mH+2; y++ )
      {
        for ( size_t x=0; x<mW+2; x++ )
        {
          os<<getTerrain ( x,y,TerrainType ( i ) ) <<" ";
        }
        os<<std::endl;
      }
      node.addChild ( TerrainNames[i] ).setContent ( AGString ( os.str() ) );

    }
  }
  else if ( false )
  {
    std::string name=mName.replace ( ".antlvl",".hmap" );
    BinaryFileOut os ( name );
    saveBinary ( os ,version );
    node.set ( "filename",AGString ( name ) );
  }
  else
  {
    BinaryStringOut os;
    saveBinary ( os ,version);
    Node &n=node.addChild ( "data" );
    n.setContent ( AGString ( binaryToHex ( os.getString() ) ) );
  }
}


std::vector<float> genSomeHeights ( int mW,int mH,float mMaxHeight )
{
  std::vector<float> h ( mW*mH );

  for ( int i=0; i<mW*mH; i++ )
    h[i]=0;
  return h;

  for ( int i=0; i<mW*mH; i++ )
    h[i]=agRand ( mMaxHeight );

  int x,y;
  // blur

  for ( int j=0; j<2; j++ ) // once
    for ( x=0; x<mW-1; x++ )
      for ( y=0; y<mH-1; y++ )
        h[x+y*mW]= ( h[x+y*mW]+h[x+y*mW+1]+h[x+ ( y+1 ) *mW]+h[x+1+ ( y+1 ) *mW] ) /4;


  return h;
}

float HeightMap::get ( size_t x,size_t y ) const
{
  assert ( x>=0 );
  assert ( y>=0 );
  if ( x>=mW+2 )
    cdebug ( "x:"<<x<<"  mW:"<<mW );
  assert ( x<mW+2 );
  if ( y>=mH+2 )
    cdebug ( "y:"<<y );
  assert ( y<mH+2 );
  return mHeights[x+y* ( mW+2 )];
}

AGVector4 HeightMap::getVertex ( int x,int y )
{
  return AGVector4 ( x,y,getHeight ( x,y ),1 );
}

float HeightMap::getHeight ( float x,float y ) const
{
  int mx= ( int ) ( x );
  int my= ( int ) ( y );

  float dx=x-mx;
  float dy=y-my;

  float a0=get ( mx,my );
  float a1=get ( mx+1,my );
  float a2=get ( mx,my+1 );
  float a3=get ( mx+1,my+1 );

  float h= ( a0* ( 1-dx ) +a1*dx ) * ( 1-dy ) + ( a2* ( 1-dx ) +a3*dx ) *dy;
  return h;

}

AGVector3 HeightMap::getNormalF ( float x,float y ) const
{
  AGVector4 v1,v2;
  if ( x>2 )
    v1=AGVector4 ( 1,0,getHeight ( x,y )-getHeight ( x-1,y ),0 );
  else
    v1=AGVector4 ( 1,0,getHeight ( x+1,y )-getHeight ( x,y ),0 );

  if ( y>2 )
    v2=AGVector4 ( 0,1,getHeight ( x,y )-getHeight ( x,y-1 ),0 );
  else
    v2=AGVector4 ( 0,1,getHeight ( x,y+1 )-getHeight ( x,y ),0 );

  AGVector4 v3=v1%v2;
  v3.normalize3();

  return AGVector3 ( v3[0],v3[1],v3[2] );
}


AGVector3 HeightMap::getNormal ( int x,int y ) const
{
  AGVector4 v1=AGVector4 ( 1,0,get ( x+1,y )-get ( x,y ),0 );
  AGVector4 v2=AGVector4 ( 0,1,get ( x,y+1 )-get ( x,y ),0 );

  AGVector4 v3=v1%v2;
  v3.normalize3();
  //  v3*=-1;
  return AGVector3 ( v3[0],v3[1],v3[2] );
}

AGVector2 HeightMap::truncPos ( const AGVector2 &p ) const
{
  AGVector2 maxPos ( mW+0.5,mH+0.5 );
  return AGVector2 ( std::max ( 0.0f,std::min ( maxPos[0],p[0] ) ),
      std::max ( 0.0f,std::min ( maxPos[1],p[1] ) ) );
}

AGVector3 HeightMap::truncPos ( const AGVector3 &p ) const
{
  AGVector2 maxPos ( mW+0.5,mH+0.5 );
  return AGVector3 ( std::max ( 0.0f,std::min ( maxPos[0],p[0] ) ),
      std::max ( 0.0f,std::min ( maxPos[1],p[1] ) ),
      p[2] );
}

void HeightMap::setTerrainScale ( TerrainType t,float s )
{
  mTerrainScale[t]=s;
}

float HeightMap::getTerrainValue ( float x,float y,TerrainType t )
{
  int mx= ( int ) ( x );
  int my= ( int ) ( y );

  float dx=x-mx;
  float dy=y-my;

  float a0=getTerrain ( mx,my,t );
  float a1=getTerrain ( mx+1,my,t );
  float a2=getTerrain ( mx,my+1,t );
  float a3=getTerrain ( mx+1,my+1,t );

  float h= ( a0* ( 1-dx ) +a1*dx ) * ( 1-dy ) + ( a2* ( 1-dx ) +a3*dx ) *dy;
  return h;
}

TerrainType HeightMap::getTerrain ( float x,float y )
{
  TerrainType t=FIRSTTERRAIN;
  float mmax=0.0f;
  for ( int i=FIRSTTERRAIN; i<LASTTERRAIN; i++ )
  {
    float v=getTerrainValue ( x,y,TerrainType ( i ) );
    if ( v>mmax )
    {
      t=TerrainType ( i );
      mmax=v;
    }
  }
  return t;
}

// mean value
float HeightMap::getTerrainWeight ( float x,float y )
{
  float mean=0;
  for ( int i=FIRSTTERRAIN; i<LASTTERRAIN; i++ )
  {
    float v=getTerrainValue ( x,y,TerrainType ( i ) );
    mean+=v*i;
  }
  return mean/ ( LASTTERRAIN-FIRSTTERRAIN );
}

float HeightMap::getMean ( float x,float y )
{
  float mean=0;
  for ( int i=FIRSTTERRAIN; i<LASTTERRAIN; i++ )
  {
    float v=getTerrainValue ( x,y,TerrainType ( i ) );
    mean+=v*mTerrainScale[TerrainType ( i )];
  }
  return mean;

}


float HeightMap::getTerrainScale ( float x,float y )
{
  TerrainType t=getTerrain ( x,y ),t2;

  float mean=getTerrainWeight ( x,y );

  if ( mean<t )
  {
    t2=TerrainType ( t-1 );
  }
  else
    t2=TerrainType ( t+1 );

  float s1=mTerrainScale[t];
  float s2=mTerrainScale[t2];

  mean=fabs ( std::max ( std::min ( mean,1.0f ),-1.0f ) );

  //  cdebug(mean);
  float w=0;
  return getMean ( x,y ) * ( 1-w ) +s1*w;

  return s1* ( 1-mean ) +s2*mean;
}

Scene *HeightMap::getScene()
{
  return mScene;
}

AGVector2 HeightMap::getNextPlaceAbove ( const AGVector2 &p,float height ) const
{
  int x= ( int ) p[0];
  int y= ( int ) p[1];
  int d;
  float maxh=height;
  AGVector2 found ( x,y );

  for ( d=0; d<5; d++ )
  {
    for ( int i=-d; i<=d; i++ )
    {
      std::vector<AGVector2> p;
      p.push_back ( AGVector2 ( i,y-d ) );
      p.push_back ( AGVector2 ( i,y+d ) );
      p.push_back ( AGVector2 ( x-d,i ) );
      p.push_back ( AGVector2 ( x+d,i ) );
      for ( std::vector<AGVector2>::iterator k=p.begin(); k!=p.end(); ++k )
      {
        if ( ( *k ) [0]<0 || ( *k ) [0]>mW || ( *k ) [1]<0 || ( *k ) [1]>mH )
          continue;
        cdebug ( ( *k ) [0]<<"  "<< ( *k ) [1]<<" "<<x<<" "<<y<<" "<<i<<" "<<d );
        float h=getHeight ( ( *k ) [0], ( *k ) [1] );
        if ( h>maxh )
        {
          maxh=h;
          found=*k;
        }
      }
    }

    if ( maxh>height )
      break;
  }
  return found;
}

void HeightMap::checkTerrain()
{
  if ( !mTerrain )
    initTerrainMesh();
}
/*
   std::string HeightMap::hash() const
   {
   BinaryStringOut s;
   for(std::vector<float>::const_iterator i=mHeights.begin();i!=mHeights.end();i++)
   s<<*i;

   return rubyHash(s.getString());

   }
   */
/**
 * returns the mesh (scene-node) of the height-map
 * FIXME: this is for testing-purpose only and shouldn't be used at all (?)
 * it could be, that HeightMap introduced more than 1 mesh at some time
 * */
TerrainBase *HeightMap::getTerrainMesh()
{
  return mTerrain;
}
