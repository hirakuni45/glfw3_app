#pragma once

#include "gl_fw/gl_info.hpp"
#include "LinearMath/btAlignedObjectArray.h"
#include "LinearMath/btVector3.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"

namespace bullet {

	class gl_shape_drawer
	{
	protected:
		struct ShapeCache
		{
			struct Edge {
				btVector3 n[2];
				int v[2];
			};
			ShapeCache(btConvexShape* s) : m_shapehull(s) {}
			btShapeHull					m_shapehull;
			btAlignedObjectArray<Edge>	m_edges;
		};

		//clean-up memory of dynamically created shape hulls
		btAlignedObjectArray<ShapeCache*>	m_shapecaches;
		unsigned int						m_texturehandle;
		bool								m_textureenabled;
		bool								m_textureinitialized;

		ShapeCache*							cache(btConvexShape*);

	public:
		gl_shape_drawer() : m_texturehandle(0), m_textureenabled(false), m_textureinitialized(false) { }
		~gl_shape_drawer() {
			for(int i = 0; i < m_shapecaches.size(); ++i) {
				m_shapecaches[i]->~ShapeCache();
				btAlignedFree(m_shapecaches[i]);
			}
			m_shapecaches.clear();
			if(m_textureinitialized) {
				glDeleteTextures(1, (const GLuint*)&m_texturehandle);
			}
		}

		///drawOpenGL might allocate temporary memoty, stores pointer in shape userpointer
		virtual void drawOpenGL(btScalar* m, const btCollisionShape* shape, const btVector3& color,int	debugMode,const btVector3& worldBoundsMin,const btVector3& worldBoundsMax);
		virtual void drawShadow(btScalar* m, const btVector3& extrusion,const btCollisionShape* shape,const btVector3& worldBoundsMin,const btVector3& worldBoundsMax);
		
		bool enable_texture(bool enable = true) {
			bool p = m_textureenabled;
			m_textureenabled=enable;
			return p;
		}

		bool has_texture_enabled() const {
			return m_textureenabled;
		}
		
		static void drawCylinder(float radius,float halfHeight, int upAxis);
		void drawSphere(btScalar r, int lats, int longs);
		static void drawCoordSystem();
	};

	void OGL_displaylist_register_shape(btCollisionShape* shape);
	void OGL_displaylist_clean();
}
