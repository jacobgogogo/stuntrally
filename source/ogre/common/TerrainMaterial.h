#ifndef _TERRAINMATERIAL_H
#define _TERRAINMATERIAL_H

#include "OgreTerrainPrerequisites.h"
#include "OgreTerrainMaterialGenerator.h"
#include "OgreGpuProgramParams.h"

namespace sh
{
	class MaterialInstance;
}


class TerrainMaterial : public Ogre::TerrainMaterialGenerator
{
public:

	class Profile : public Ogre::TerrainMaterialGenerator::Profile
	{
	public:
		Profile(Ogre::TerrainMaterialGenerator* parent, const Ogre::String& name, const Ogre::String& desc);
		virtual ~Profile();

		virtual bool isVertexCompressionSupported() const { return false; }

		virtual Ogre::MaterialPtr generate(const Ogre::Terrain* terrain);

		virtual Ogre::MaterialPtr generateForCompositeMap(const Ogre::Terrain* terrain);

		virtual Ogre::uint8 getMaxLayers(const Ogre::Terrain* terrain) const;

		virtual void updateParams(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain);

		virtual void updateParamsForCompositeMap(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain);

		virtual void requestOptions(Ogre::Terrain* terrain);

	private:
		sh::MaterialInstance* mMaterial;
		Ogre::String mMatName, mMatNameComp;

		void createMaterial (const Ogre::String& matName, const Ogre::Terrain* terrain, bool renderCompositeMap);

	};

	TerrainMaterial();
};


#endif
