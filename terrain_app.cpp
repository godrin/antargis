#include <ag_vdebug.h>
#include <ag_texturecache.h>
#include <ag_rendercontext.h>
#include "vertex_array.h"
#include "height_map.h"
#include "grid_2d.h"
#include "gl_app.h"
#include "ag_video.h"
#include "ag_main.h"
#include "simulation_state.h"
#include "generator.h"

int getTerrainDownScale();
int getTerrainDownScaleZ();
int getTerrainTriangleSize();

class BasicHeightMap {
	public:
		Grid2D<float> water,sand,rock,grass;

		BasicHeightMap(size_t w,size_t h):
			water(w,h),
			sand(w,h),
			rock(w,h),
			grass(w,h)
	{

	}
};

template<class T>
class GridPiece2D {
	public:
		GridPiece2D(Grid2D<T> &_grid,int _x,int _y,int _w,int _h):
			grid(_grid),
			x(_x),
			y(_y),
			w(_w),
			h(_h) {
			}

		size_t width() const {
			return w;
		}
		size_t height() const {
			return h;
		}
		float operator()(int _x,int _y) const {
			return grid((_y+y)*4,(_x+x)*4)*0.2;
		}

		int left() const {
			return x;
		}
		int top() const {
			return y;
		}
	private:
		Grid2D<T> &grid;
		int x,y,w,h;
};

class TerrainPieceView:public SceneNode {
	AGTexture texture;
	VertexArray m3dArray;
	GridPiece2D<float> piece;
	float mScale;
	float texHeight;
	public:
	TerrainPieceView(Scene *scene,GridPiece2D<float> _piece,float _texHeight):
		SceneNode(scene,AGVector4(),AGBox3()),
		m3dArray(true),
		piece(_piece),
		texHeight(_texHeight)
	{
		mScale=0.25;
		texture=getTextureCache()->get3D("data/textures/terrain/new3d.png",getTerrainDownScale(),getTerrainDownScaleZ());
		init();
	}
	AGVector3 getNormal ( int x,int y ) const
	{
		AGVector4 v1=AGVector4 ( 1,0,piece ( x+1,y )-piece ( x,y ),0 );
		AGVector4 v2=AGVector4 ( 0,1,piece ( x,y+1 )-piece ( x,y ),0 );

		AGVector4 v3=v1%v2;
		v3.normalize3();
		return AGVector3 ( v3[0],v3[1],v3[2] );
	}
	void mapChanged(const AGRect2 &changedRect)
	{
		AGBox3 bb=bbox();

		// first check, if this piece was really affected
		if(bb.valid())
		{
			AGRect2 r=AGRect2(AGVector2(bb.base[0]-2,bb.base[1]-2),AGVector2(bb.base[0]+bb.dir[0]+4,bb.base[1]+bb.dir[1]+4));  // add some border (2)

			if(!r.collide(changedRect))
			{
				return;
			}

		}
		init();
	}
	void init() {
		AGBox3 bb=bbox();

		bb=AGBox3();
		m3dArray.clear();

		AGVector4 white(1,1,1,0.5);
		AGVector4 v;
		AGVector3 n;
		AGVector3 tp3;
		float texFactor3w=0.5;

		// add Vertices
		size_t x,y;
		for(y=0;y<piece.height()+1;y++) {
			for(x=0;x<piece.width()+1;x++) {
				float sx=x*mScale,sy=y*mScale;
				v=AGVector4(mScale*(x+piece.left()),mScale*(y+piece.top()),piece(x,y),1);

				n=getNormal(x,y);

				tp3=AGVector3(-v[0]*texFactor3w,-v[1]*texFactor3w,texHeight);
				m3dArray.addVertex(v,white,n,tp3);

				bb.include(v.dim3());
			}
		}

		for(y=0;y<piece.height();y++) {
			for(x=0;x<piece.width();x++) {
				int p0=x+y*(piece.width()+1);
				int p1=p0+1;
				int p2=p0+(piece.width()+1);
				int p3=p2+1;
				if(((x+y)%2)==0)
				{
					m3dArray.addTriangle(p2,p0,p1);
					m3dArray.addTriangle(p3,p2,p1);

				}
				else
				{
					m3dArray.addTriangle(p3,p0,p1);
					m3dArray.addTriangle(p3,p2,p0);
				}

			}
		}
		m3dArray.setColors(true);
		setBBox(bb);
	}
	void draw()
	{
		assertGL;

		AGRenderContext c;
		c.setLighting(true);
		c.setTexture(texture.glTexture());
		texture.setFilter(GL_LINEAR,GL_LINEAR);

		c.begin();

		m3dArray.draw();
		assertGL;
	}
};


class TerrainApp:public GLApp {
	// HeightMap *map;
	//BasicHeightMap map;
	std::list<TerrainPieceView *> pieceViews;
	Generator::Generator generator;
	public:
	TerrainApp():GLApp(1024,768) /*,map(256,256)*/ {
		// map=new HeightMap(&getScene(),32,32);
		for(int x=0;x<4;x++)
			for(int y=0;y<4;y++)
				pieceViews.push_back(new TerrainPieceView(&getScene(),GridPiece2D<float>(generator.terrain(),16*x,16*y,16,16),0.5));
	}
	bool eventFrame ( float pTime )
	{
		bool r=GLApp::eventFrame ( pTime );

		getScene().advance ( pTime );
		SDL_Delay ( 20 );
		for(int i=0;i<4;i++)
			generator.update();
		for(TerrainPieceView *pieceView:pieceViews) {
			pieceView->init();
		}

		return r;
	}
};


int main() {
	AGMain main;
	main.setVideo ( new AGVideoManager() );
	main.getVideo()->initVideo ( 1024,768,32,false,true );
	TerrainApp app;
	app.run();
	return 0;

}
