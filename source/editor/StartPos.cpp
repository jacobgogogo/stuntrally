#include "pch.h"
#include "../ogre/common/Defines.h"
#include "OgreApp.h"
#include "../road/Road.h"
#include "../ogre/common/RenderConst.h"
#include "../ogre/common/SceneXml.h"
using namespace Ogre;


bool App::LoadStartPos(std::string path1, bool tool)
{
	vStartPos.clear();  // clear
	vStartRot.clear();
	std::string path = path1+"track.txt";
	CONFIGFILE param;
	if (!param.Load(path))
		return false;

	int sp_num = 0;
	float f3[3], f1;
	QUATERNION <float> fixer;  fixer.Rotate(3.141593, 0,0,1);
	
	while (param.GetParam("start position "+toStr(sp_num), f3))
	{
		MATHVECTOR <float, 3> pos(f3[2], f3[0], f3[1]);

		if (!param.GetParam("start orientation-xyz "+toStr(sp_num), f3))
			return false;

		if (!param.GetParam("start orientation-w "+toStr(sp_num), f1))
			return false;

		QUATERNION <float> orient(f3[2], f3[0], f3[1], f1);
		orient = fixer * orient;

		vStartPos.push_back(pos);  // add
		vStartRot.push_back(orient);
		sp_num++;
	}
	
	if (!tool)
		UpdStartPos();
	return true;
}

bool App::SaveStartPos(std::string path)
{
	CONFIGFILE param;
	if (!param.Load(path))
		return false;
		
	QUATERNION <float> fixer;  fixer.Rotate(-3.141593, 0,0,1);
	for (int i=0; i < 4; ++i)
	{
		int n = 0;  // 0- all same  i- edit 4
		//  pos
		float p3[3] = {vStartPos[n][1], vStartPos[n][2], vStartPos[n][0]};
		param.SetParam("start position "+toStr(i), p3);
		
		//  rot
		QUATERNION <float> orient = vStartRot[n];
		orient = fixer * orient;
		float f3[3] = {orient.y(), orient.z(), orient.x()}, f1 = orient.w();

		param.SetParam("start orientation-xyz "+toStr(i), f3);
		param.SetParam("start orientation-w "+toStr(i), f1);
	}
	return param.Write();
}


void App::UpdStartPos()
{
	if (!ndCar)
	{ 	//  car for start pos
 		ndCar = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		entCar = mSceneMgr->createEntity("car.mesh");
		entCar->setVisibilityFlags(RV_Hud);  ndCar->setPosition(Vector3(20000,0,0));
		ndCar->attachObject(entCar);
	}
	if (!ndStBox)
	{ 	//  start pos box
		MaterialPtr mtr = Ogre::MaterialManager::getSingleton().getByName("start_box");
		if (!mtr.isNull())
 		{	ndStBox = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			entStBox = mSceneMgr->createEntity("cube.mesh");
			entStBox->setVisibilityFlags(RV_Hud);  ndStBox->setPosition(Vector3(20000,0,0));
				entStBox->setCastShadows(true);  //`
				entStBox->setMaterial(mtr);  entStBox->setRenderQueueGroup(RQG_CarGlass);  // after road
			ndStBox->attachObject(entStBox);
	}	}
	if (!ndFluidBox)
	{ 	//  fluid edit box
		MaterialPtr mtr = Ogre::MaterialManager::getSingleton().getByName("fluid_box");
		if (!mtr.isNull())
 		{	ndFluidBox = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			entFluidBox = mSceneMgr->createEntity("box_fluids.mesh");
			entFluidBox->setVisibilityFlags(RV_Hud);  ndFluidBox->setPosition(Vector3(0,0,0));
				entFluidBox->setCastShadows(false);  //`
				entFluidBox->setMaterial(mtr);  entFluidBox->setRenderQueueGroup(RQG_CarGlass);
			ndFluidBox->attachObject(entFluidBox);
			ndFluidBox->setVisible(false);
	}	}
	if (!ndObjBox)
	{ 	//  picked object box
		MaterialPtr mtr = Ogre::MaterialManager::getSingleton().getByName("object_box");
		if (!mtr.isNull())
 		{	ndObjBox = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			entObjBox = mSceneMgr->createEntity("box_obj.mesh");
			entObjBox->setVisibilityFlags(RV_Hud);  ndObjBox->setPosition(Vector3(0,0,0));
				entObjBox->setCastShadows(false);  //`
				entObjBox->setMaterial(mtr);  entObjBox->setRenderQueueGroup(RQG_CarGlass);
			ndObjBox->attachObject(entObjBox);
			ndObjBox->setVisible(false);
	}	}
	if (vStartPos.size() < 4 || vStartRot.size() < 4)  return;

	float* pos = &vStartPos[0][0];
	float* rot = &vStartRot[0][0];

	Vector3 p1 = Vector3(pos[0],pos[2],-pos[1]);

	Quaternion q(rot[0],rot[1],rot[2],rot[3]);
	Radian rad;  Vector3 axi;  q.ToAngleAxis(rad, axi);

	Vector3 vrot(axi.z, -axi.x, -axi.y);
		QUATERNION <double> fix;  fix.Rotate(PI_d, 0, 1, 0);
		Quaternion qr;  qr.w = fix.w();  qr.x = fix.x();  qr.y = fix.y();  qr.z = fix.z();
	Quaternion q1;  q1.FromAngleAxis(-rad, vrot);  q1 = q1 * qr;
	//Vector3 vcx,vcy,vcz;  q1.ToAxes(vcx,vcy,vcz);

	ndCar->setPosition(p1);    ndCar->setOrientation(q1);

	ndStBox->setPosition(p1);  ndStBox->setOrientation(q1);
	if (road)
	ndStBox->setScale(Vector3(1,road->vStBoxDim.y,road->vStBoxDim.z));
	ndStBox->setVisible(edMode == ED_Start && bEdit());
}



///...........................................................................................................................
//  check track, and report warnings
///...........................................................................................................................

const static String clrWarn[5] = {"#FF4040","#FFA040","#E0E040","#80F040","#60A0E0"};
const static String strWarn[5] = {"ERR   ","WARN  ","Info  ","Note  ","Txt   "};
void App::Warn(eWarn type, String text)
{
	if (logWarn)
		LogO(strWarn[type]+text);
	else
		edWarn->addText(clrWarn[type]+text+"\n");
	if (type == ERR || type == WARN)  ++cntWarn;  // count serious only
}
	
void App::WarningsCheck(const Scene* sc, const SplineRoad* road)
{
	if (!edWarn && !logWarn)  return;
	cntWarn = 0;
	if (!logWarn)
		edWarn->setCaption("");
	
	bool hqTerrain=0, hqGrass=0, hqVeget=0, hqRoad=0;  // high quality

	if (road && road->getNumPoints() > 2)
	{
		///-  start  -------------
		int cnt = road->getNumPoints();
		const float* pos = &vStartPos[0][0], *rot = &vStartRot[0][0];
		Vector3 stPos = Vector3(pos[0],pos[2],-pos[1]);

		Quaternion q(rot[0],rot[1],rot[2],rot[3]);
		Radian rad;  Vector3 axi;  q.ToAngleAxis(rad, axi);
		Vector3 vrot(axi.z, -axi.x, -axi.y);
			QUATERNION <double> fix;  fix.Rotate(PI_d, 0, 1, 0);
			Quaternion qr;  qr.w = fix.w();  qr.x = fix.x();  qr.y = fix.y();  qr.z = fix.z();
		Quaternion q1;  q1.FromAngleAxis(-rad, vrot);  q1 = q1 * qr;
		Vector3 vx,vy,vz;  q1.ToAxes(vx,vy,vz);  Vector3 stDir = -vx;
		Plane p(stDir, stPos);

		if (road->iP1 >= 0 && road->iP1 < cnt  && road->mP[road->iP1].chkR >= 1.f)
		{
			Vector3 ch0 = road->mP[road->iP1].pos;
			float d = p.getDistance(ch0);
			Warn(TXT,"Car start to 1st check distance: "+fToStr(d,2,4));
			if (d < 0.f)
				Warn(WARN,"Car start isn't facing first checkpoint\n (wrong direction or first checkpoint), distance: "+fToStr(d,2,4));
			//Warn(NOTE,"check0 pos "+fToStr(ch0.x,2,5)+" "+fToStr(ch0.y,2,5)+" "+fToStr(ch0.z,2,5));
		}
		//Warn(TXT,"Start pos "+fToStr(stPos.x,2,5)+" "+fToStr(stPos.y,2,5)+" "+fToStr(stPos.z,2,5));
		//Warn(TXT,"Start dir "+fToStr(vx.x,3,5)+" "+fToStr(vx.y,3,5)+" "+fToStr(vx.z,3,5));


		//-  start pos  ----
		float tws = 0.5f * sc->td.fTerWorldSize;
		if (stPos.x < -tws || stPos.x > tws || stPos.z < -tws || stPos.z > tws)
			Warn(ERR,"Car start outside track area  Whoa :o");
		
		if (terrain)  // won't work in tool..
		{	float yt = terrain->getHeightAtWorldPosition(stPos), yd = stPos.y - yt - 0.5f;
			//Warn(TXT,"Car start to terrain distance "+fToStr(yd,1,4));
			if (yd < 0.f)   Warn(ERR, "Car start below terrain  Whoa :o");
			if (yd > 0.3f)  Warn(INFO,"Car start far above terrain\n (skip this if on bridge or in pipe), distance: "+fToStr(yd,1,4));
		}
		

		//-  other start places inside terrain (split screen)  ----
		if (terrain)  // won't work in tool..
		for (int i=1; i<4; ++i)
		{
			Vector3 p = stPos + i * stDir * 6.f;  //par dist
			float yt = terrain->getHeightAtWorldPosition(p), yd = p.y - yt - 0.5f;
			String si = toStr(i);
							Warn(TXT, "Car "+si+" start to ter dist "+fToStr(yd,1,4));
			if (yd < 0.f)   Warn(WARN,"Car "+si+" start below terrain !");
			if (yd > 0.3f)  Warn(INFO,"Car "+si+" start far above terrain\n (skips bridge/pipe), distance: "+fToStr(yd,1,4));
		}
		
		
		//-  first chk  ----
		if (road->iP1 < 0 || road->iP1 >= cnt)
			Warn(ERR,"First checkpoint not set  (use ctrl-0)");
		else
		if (road->mP[road->iP1].chkR < 0.1f)
			Warn(ERR,"First checkpoint not set  (use ctrl-0)");

		
		///-  road, checkpoints  -------------
		int numChks = 0, iClosest=-1;  float stD = FLT_MAX;
		bool mtrUsed[4]={0,0,0,0};
		for (int i=0; i < road->mP.size(); ++i)
		{
			const SplinePoint& p = road->mP[i];
			if (p.chkR > 0.f && p.chkR < 1.f)
				Warn(WARN,"Too small checkpoint at road point "+toStr(i+1)+", chkR = "+fToStr(p.chkR,1,3));
			//.. in pipe > 2.f on bridge = 1.f
			
			if (p.chkR >= 1.f)
			{	++numChks;
				float d = stPos.squaredDistance(p.pos);
				if (d < stD) {  stD = d;  iClosest = i;  }
			}
			if (p.idMtr >= 0 && p.idMtr < 4)
				mtrUsed[p.idMtr] = true;
		}
		if (numChks==0)
			Warn(ERR,"No checkpoints set  (use K,L on some road points)");
		if (numChks < 3)
			Warn(INFO,"Too few checkpoints (add more), count "+toStr(numChks));

			
		//-  road materials used  ----
		int rdm = 0;
		for (int i=0; i<4; ++i)
			if (mtrUsed[i])  ++rdm;
		
		Warn(TXT,"Road materials used "+toStr(rdm));
		hqRoad = rdm >= 3;
		if (hqTerrain) Warn(INFO,"HQ Road");
		//if (rdm >= 4)  Warn(WARN,"Too many terrain layers used, not recommended");
		if (rdm <= 1)  Warn(INFO,"Too few road materials used");
		

		//-  start width, height  ----
		float width = road->vStBoxDim.z, height = road->vStBoxDim.y;

		float rdW = 100.f;  if (iClosest >= 0)  rdW = road->mP[iClosest].width;
		Warn(TXT,"Closest road point width: "+fToStr(rdW,1,4)+",  distance "+fToStr(stPos.distance(road->mP[iClosest].pos),0,3));
		
		if (width < 8.f || width < rdW * 1.4f)
			Warn(WARN,"Car start width small "+fToStr(width,0,2));
		if (height < 4.5f)
			Warn(WARN,"Car start height small "+fToStr(height,0,2));

		//-  rd, chk cnt  ----
		float ratio = float(numChks)/cnt;
		Warn(TXT,"Road points to checkpoints ratio: "+fToStr(ratio,2,4));
		if (ratio < 1.f/10.f)  //par
			Warn(WARN,"Extremely low checkpoints ratio, add more");
		else if (ratio < 1.f/5.f)  //par  1 chk for 5 points
			Warn(WARN,"Very few checkpoints ratio, add more");
		
		//..  road points too far?
		//..  big road, merge len
		//..  road->iDir == 1
	}
	
	///-  heightmap  -------------
	int sz = sc->td.iVertsX * sc->td.iVertsX * sizeof(float) / 1024/1024;
	if (sc->td.iVertsX > 2000)
		Warn(ERR,"Using too big heightmap "+toStr(sc->td.iVertsX)+", file size is "+toStr(sz)+" MB");
	else
	if (sc->td.iVertsX > 1000)
		Warn(INFO,"Using big heightmap "+toStr(sc->td.iVertsX)+", file size is "+toStr(sz)+" MB");

	if (sc->td.iVertsX < 200)
		Warn(INFO,"Using too small heightmap "+toStr(sc->td.iVertsX));

	//-  tri size  ----
	if (sc->td.fTriangleSize < 0.9f)
		Warn(INFO,"Terrain triangle size is small "+fToStr(sc->td.fTriangleSize,2,4));

	if (sc->td.fTriangleSize > 1.9f)
		Warn(INFO,"Terrain triangle size is big "+fToStr(sc->td.fTriangleSize,2,4)+", not recommended");

		
	///-  ter layers  -------------
	int lay = sc->td.layers.size();  //SetUsedStr
	Warn(NOTE,"Terrain layers used: "+toStr(lay));
	hqTerrain = lay >= 4;
	if (hqTerrain) Warn(INFO,"HQ Terrain");
	if (lay >= 5)  Warn(WARN,"Too many terrain layers used, not recommended");
	if (lay <= 2)  Warn(INFO,"Too few terrain layers used");

	
	///-  vegetation  -------------
	int veg = sc->pgLayers.size();
	Warn(NOTE,"Vegetation models used: "+toStr(veg));
	hqVeget = veg >= 5;
	if (hqVeget)   Warn(INFO,"HQ Vegetation");
	if (veg >= 7)  Warn(WARN,"Too many models used, not recommended");
	if (veg <= 2)  Warn(INFO,"Too few models used");
	
	//-  density  ----
	if (sc->densTrees > 3.1f)
		Warn(ERR,"Vegetation use is huge, trees density is "+fToStr(sc->densTrees,1,3));
	else
	if (sc->densTrees > 2.f)
		Warn(WARN,"Using a lot of vegetation, trees density is "+fToStr(sc->densTrees,1,3));
	
	if (sc->grDensSmooth > 10)
		Warn(WARN,"Smooth grass density is high "+toStr(sc->grDensSmooth)+" saving will take long time");

	//-  grass  ----
	int gr=0;
	if (sc->densGrass > 0.01)  for (int i=0; i < sc->ciNumGrLay; ++i)  if (sc->grLayersAll[i].on)  ++gr;
	Warn(NOTE,"Grass layers used: "+toStr(gr));
	hqGrass = gr >= 4;
	if (hqGrass)  Warn(INFO,"HQ Grass");
	if (gr >= 5)  Warn(WARN,"Too many grasses used, not recommended");
	if (gr <= 2)  Warn(INFO,"Too few grasses used");

	//..  page size small, dist big
	

	///-  quality (optym, fps drop)  --------
	int hq=0;
	if (hqTerrain) ++hq;  if (hqGrass) ++hq;  if (hqVeget) ++hq;  if (hqRoad) ++hq;
	Warn(NOTE,"HQ Overall: "+toStr(hq));
	if (hq > 3)
		Warn(INFO,"Quality too high (possibly low Fps), try to reduce densities or layers/models/grasses count");
	else if (hq > 2)
		Warn(INFO,"Great quality, but don't forget about some optimisations");
	else if (hq == 0)
		Warn(INFO,"Low quality (ignore for deserts), try to add some layers/models/grasses");
	
	//..  scID light diff ?
	//..  objects count
	

	///-  end  ----------------
	if (logWarn)
	{
		LogO("Warnings: "+toStr(cntWarn)+"\n");
		return;
	}
	if (cntWarn == 0)
	{
		Warn(NOTE,"#A0C0FF""No warnings.");
		txWarn->setVisible(false);
	}else
	{	//  show warn overlay
		txWarn->setVisible(true);
		txWarn->setCaption(TR("#{Warnings}: ")+toStr(cntWarn)+"  (alt-J)");
	}
}
