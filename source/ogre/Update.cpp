#include "pch.h"
#include "common/Defines.h"
#include "OgreGame.h"
#include "FollowCamera.h"
#include "../road/Road.h"
#include "../vdrift/game.h"
#include "../vdrift/quickprof.h"
#include "../paged-geom/PagedGeometry.h"
#include "../network/masterclient.hpp"
#include "../network/gameclient.hpp"
#include "LinearMath/btDefaultMotionState.h"

#include <OgreParticleSystem.h>
#include <OgreManualObject.h>
#include <OgreMaterialManager.h>
#include "common/Gui_Def.h"
#include "common/MultiList2.h"
#include "common/Slider.h"
#include "SplitScreen.h"
#include <MyGUI.h>
using namespace Ogre;
using namespace MyGUI;

#include "../shiny/Main/Factory.hpp"

#include "../sdl4ogre/sdlinputwrapper.hpp"


#define isKey(a)  mInputWrapper->isKeyDown(a)


//  simulation (2nd) thread
//---------------------------------------------------------------------------------------------------------------

void App::UpdThr()
{
	while (!mShutDown)
	{
		///  step Game  **

		//  separate thread
		pGame->qtim.update();
		double dt = pGame->qtim.dt;
		
		if (pSet->multi_thr == 1 && !bLoading)
		{
			bSimulating = true;
			bool ret = pGame->OneLoop(dt);
			if (!ret)
				mShutDown = true;

			DoNetworking();
			bSimulating = false;
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(pSet->thread_sleep));
	}
}

		
void App::DoNetworking()
{
	bool doNetworking = (mClient && mClient->getState() == P2PGameClient::GAME);
	// Note that there is no pause when in networked game
	bool gui = isFocGui || isTweak();
	pGame->pause = bRplPlay ? (bRplPause || gui) : (gui && !doNetworking);

	//  handle networking stuff
	if (doNetworking)
	{
		PROFILER.beginBlock("-network");

		//  update the local car's state to the client
		protocol::CarStatePackage cs;  // FIXME: Handles only one local car
		for (CarModels::const_iterator it = carModels.begin(); it != carModels.end(); ++it)
		{
			if ((*it)->eType == CarModel::CT_LOCAL)
			{
				cs = (*it)->pCar->GetCarStatePackage();
				cs.trackPercent = uint8_t( (*it)->trackPercent / 100.f * 255.f);  // pack to uint8
				break;
			}
		}
		mClient->setLocalCarState(cs);

		//  check for new car states
		protocol::CarStates states = mClient->getReceivedCarStates();
		for (protocol::CarStates::const_iterator it = states.begin(); it != states.end(); ++it)
		{
			int8_t id = it->first;  // Car number  // FIXME: Various places assume carModels[0] is local
			if (id == 0)  id = mClient->getId();
			
			CarModel* cm = carModels[id];
			if (cm && cm->pCar)
			{
				cm->pCar->UpdateCarState(it->second);
				cm->trackPercent = cm->pCar->trackPercentCopy;  // got from client
			}
		}
		PROFILER.endBlock("-network");
	}
}


//  Frame Start
//---------------------------------------------------------------------------------------------------------------

bool App::frameStart(Real time)
{
	PROFILER.beginBlock(" frameSt");
	fLastFrameDT = time;

	for (int i=0; i<4; ++i)
	{
		boost::lock_guard<boost::mutex> lock(mPlayerInputStateMutex);
		for (int a = 0; a<NumPlayerActions; ++a)
		{
			mPlayerInputState[i][a] = mInputCtrlPlayer[i]->getChannel(a)->getValue();
		}
	}

	if (imgBack && pGame)  // show/hide background image
	{
		bool backImgVis = !bLoading && pGame->cars.empty();
		imgBack->setVisible(backImgVis);
	}


	//  multi thread
	if (pSet->multi_thr == 1 && pGame && !bLoading)
	{
		updatePoses(time);
	}

	///  graphs update  -._/\_-.
	if (pSet->show_graphs && graphs.size() > 0)
	{
		GraphsNewVals();
		UpdateGraphs();
	}

	//...................................................................
	///* tire edit */
	if (pSet->graphs_type == Gh_TireEdit && carModels.size() > 0)
	{
		int k = (isKey(SDL_SCANCODE_1) || isKey(SDL_SCANCODE_KP_DIVIDE)  ? -1 : 0)
			  + (isKey(SDL_SCANCODE_2) || isKey(SDL_SCANCODE_KP_MULTIPLY) ? 1 : 0);
		if (k)
		{
			double mul = shift ? 0.2 : (ctrl ? 4.0 : 1.0);
			mul *= 0.005;  // par

			CARDYNAMICS& cd = carModels[0]->pCar->dynamics;
			CARTIRE* tire = cd.GetTire(FRONT_LEFT);
			if (iEdTire == 1)  // longit |
			{
				Dbl& val = tire->longitudinal[iCurLong];  // modify 1st
				val += mul*k * (1 + abs(val));
				for (int i=1; i<4; ++i)
					cd.GetTire(WHEEL_POSITION(i))->longitudinal[iCurLong] = val;  // copy for rest
			}
			else if (iEdTire == 0)  // lateral --
			{
				Dbl& val = tire->lateral[iCurLat];
				val += mul*k * (1 + abs(val));
				for (int i=1; i<4; ++i)
					cd.GetTire(WHEEL_POSITION(i))->lateral[iCurLat] = val;
			}
			else  // align o
			{
				Dbl& val = tire->aligning[iCurAlign];
				val += mul*k * (1 + abs(val));
				for (int i=1; i<4; ++i)
					cd.GetTire(WHEEL_POSITION(i))->aligning[iCurAlign] = val;
			}

			//  update hat, 1st
			tire->CalculateSigmaHatAlphaHat();
			for (int i=1; i<4; ++i)  // copy for rest
			{	cd.GetTire(WHEEL_POSITION(i))->sigma_hat = tire->sigma_hat;
				cd.GetTire(WHEEL_POSITION(i))->alpha_hat = tire->alpha_hat;
			}
			iUpdTireGr = 1;
		}
	}
	//...................................................................

	UnfocusLists();


	if (bGuiReinit)  // after language change from combo
	{	bGuiReinit = false;

		mGUI->destroyWidgets(vwGui);  bnQuit=0;mWndOpts=0;  //todo: rest too..
		InitGui();
		bWindowResized = true;
		mWndTabsOpts->setIndexSelected(3);  // switch back to view tab
	}

	if (bWindowResized)
	{	bWindowResized = false;
		ResizeOptWnd();
		SizeGUI();
		updTrkListDim();  updChampListDim();  // resize lists
		bSizeHUD = true;
		
		if (mSplitMgr)  //  reassign car cameras from new viewports
		{	std::list<Camera*>::iterator it = mSplitMgr->mCameras.begin();
			for (int i=0; i < carModels.size(); ++i)
				if (carModels[i]->fCam && it != mSplitMgr->mCameras.end())
				{	carModels[i]->fCam->mCamera = *it;  ++it;  }
		}
		if (!mSplitMgr->mCameras.empty())
		{
			Camera* cam1 = *mSplitMgr->mCameras.begin();
			mWaterRTT.setViewerCamera(cam1);
			if (grass)  grass->setCamera(cam1);
			if (trees)  trees->setCamera(cam1);
		}
	}
		
	///  sort trk list
	if (trkList && trkList->mSortColumnIndex != trkList->mSortColumnIndexOld
		|| trkList->mSortUp != trkList->mSortUpOld)
	{
		trkList->mSortColumnIndexOld = trkList->mSortColumnIndex;
		trkList->mSortUpOld = trkList->mSortUp;

		pSet->tracks_sort = trkList->mSortColumnIndex;  // to set
		pSet->tracks_sortup = trkList->mSortUp;
		TrackListUpd(false);
	}

	///  sort car list
	if (carList && carList->mSortColumnIndex != carList->mSortColumnIndexOld
		|| carList->mSortUp != carList->mSortUpOld)
	{
		carList->mSortColumnIndexOld = carList->mSortColumnIndex;
		carList->mSortUpOld = carList->mSortUp;

		pSet->cars_sort = carList->mSortColumnIndex;  // to set
		pSet->cars_sortup = carList->mSortUp;
		CarListUpd(false);
	}

	if (bLoading)
	{
		NewGameDoLoad();
		PROFILER.endBlock(" frameSt");
		return true;
	}
	else 
	{
		///  loading end  ------
		const int iFr = 3;
		if (iLoad1stFrames >= 0)
		{	++iLoad1stFrames;
			if (iLoad1stFrames == iFr)
			{
				LoadingOff();  // hide loading overlay
				mSplitMgr->mGuiViewport->setClearEveryFrame(true, FBT_DEPTH);
				ChampLoadEnd();
				bLoadingEnd = true;
				iLoad1stFrames = -1;  // for refl
			}
		}else if (iLoad1stFrames >= -1)
		{
			--iLoad1stFrames;  // -2 end

			imgLoad->setVisible(false);  // hide back imgs
			//if (imgBack)
			//imgBack->setVisible(false);
		}
		
		
		bool bFirstFrame = !carModels.empty() && carModels.front()->bGetStPos;
		
		if (isFocGui && mWndTabsOpts->getIndexSelected() == 4 && pSet->inMenu == MNU_Options && !pSet->isMain)
			UpdateInputBars();
		
		//  keys up/dn, for lists
		static float dirU = 0.f,dirD = 0.f;
		if (isFocGui && !pSet->isMain && !isTweak())
		{
			if (isKey(SDL_SCANCODE_UP)  ||isKey(SDL_SCANCODE_KP_8))	dirD += time;  else
			if (isKey(SDL_SCANCODE_DOWN)||isKey(SDL_SCANCODE_KP_2))	dirU += time;  else
			{	dirU = 0.f;  dirD = 0.f;  }
			int d = ctrl ? 4 : 1;
			if (dirU > 0.0f) {  LNext( d);  dirU = -0.2f;  }
			if (dirD > 0.0f) {  LNext(-d);  dirD = -0.2f;  }
		}
		
		///  Gui updates from networking
		//  We do them here so that they are handled in the main thread as MyGUI is not thread-safe
		if (isFocGui)
		{
			if (mMasterClient) {
				std::string error = mMasterClient->getError();
				if (!error.empty())
					Message::createMessageBox("Message", TR("#{Error}"), error,
						MessageBoxStyle::IconError | MessageBoxStyle::Ok);
			}
			boost::mutex::scoped_lock lock(netGuiMutex);
			if (bRebuildGameList) {  rebuildGameList();  bRebuildGameList = false;  }
			if (bRebuildPlayerList) {  rebuildPlayerList();  bRebuildPlayerList = false;  }
			if (bUpdateGameInfo) {  updateGameInfo();  bUpdateGameInfo = false;  }
			if (bUpdChat)  {  edNetChat->setCaption(sChatBuffer);  bUpdChat = false;  }
			if (bStartGame)
			{
				mClient->startGame();
				btnNewGameStart(NULL);
				bStartGame = false;
			}
		}

		//  replay forward,backward keys
		if (bRplPlay)
		{
			isFocRpl = ctrl;
			bool le = isKey(SDL_SCANCODE_LEFTBRACKET), ri = isKey(SDL_SCANCODE_RIGHTBRACKET), ctrlN = ctrl && (le || ri);
			int ta = ((le || bRplBack) ? -2 : 0) + ((ri || bRplFwd) ? 2 : 0);
			if (ta)
			{	double tadd = ta;
				tadd *= (shift ? 0.2 : 1) * (ctrlN ? 4 : 1) * (alt ? 8 : 1);  // multipliers
				if (!bRplPause)  tadd -= 1;  // play compensate
				double t = pGame->timer.GetReplayTime(0), len = replay.GetTimeLength();
				t += tadd * time;  // add
				if (t < 0.0)  t += len;  // cycle
				if (t > len)  t -= len;
				pGame->timer.SetReplayTime(0, t);
			}
		}

		if (!pGame)
		{
			PROFILER.endBlock(" frameSt");
			return false;
		}



		if (pSet->multi_thr == 0)
			DoNetworking();


		//  single thread, sim on draw
		bool ret = true;
		if (pSet->multi_thr == 0)
		{
			ret = pGame->OneLoop(time);
			if (!ret)  mShutDown = true;
			updatePoses(time);
		}
		
		// align checkpoint arrow
		// move in front of camera
		if (pSet->check_arrow && arrowNode && !bRplPlay)
		{
			Vector3 camPos = carModels.front()->fCam->mCamera->getPosition();
			Vector3 dir = carModels.front()->fCam->mCamera->getDirection();
			dir.normalise();
			Vector3 up = carModels.front()->fCam->mCamera->getUp();
			up.normalise();
			Vector3 arrowPos = camPos + 10.0f * dir + 3.5f*up;
			arrowNode->setPosition(arrowPos);
			
			// animate
			if (bFirstFrame) // 1st frame: dont animate
				arrowAnimCur = arrowAnimEnd;
			else
				arrowAnimCur = Quaternion::Slerp(time*5, arrowAnimStart, arrowAnimEnd, true);
			arrowRotNode->setOrientation(arrowAnimCur);
			
			// look down -y a bit so we can see the arrow better
			arrowRotNode->pitch(Degree(-20), SceneNode::TS_LOCAL); 
		}

		for (std::vector<CarModel*>::iterator it=carModels.begin();
			it!=carModels.end(); ++it)
		{
			if ( (*it)->fCam)
				(*it)->fCam->updInfo(time);
		}

		//  update all cube maps
		PROFILER.beginBlock("g.refl");
		for (std::vector<CarModel*>::iterator it=carModels.begin(); it!=carModels.end(); it++)
		if (!(*it)->isGhost() && (*it)->pReflect)
			(*it)->pReflect->Update(iLoad1stFrames == -1);
		PROFILER.endBlock("g.refl");

		//  trees
		PROFILER.beginBlock("g.veget");
		if (road) {
			if (grass)  grass->update();
			if (trees)  trees->update();  }
		PROFILER.endBlock("g.veget");

		//  road upd lods
		if (road)
		{
			//PROFILER.beginBlock("g.road");  // below 0.0 ms

			//  more than 1: in pre viewport, each frame
			if (mSplitMgr->mNumViewports == 1)
			{
				roadUpdTm += time;
				if (roadUpdTm > 0.1f)  // interval [sec]
				{
					roadUpdTm = 0.f;
					road->UpdLodVis(pSet->road_dist);
				}
			}
			//PROFILER.endBlock("g.road");
		}

		//**  bullet bebug draw
		if (dbgdraw)  {							// DBG_DrawWireframe
			dbgdraw->setDebugMode(pSet->bltDebug ? 1 /*+(1<<13) 255*/ : 0);
			dbgdraw->step();  }


		///  terrain mtr from blend maps
		// now in CarModel::Update
		//UpdWhTerMtr(pCar);
		
		// stop rain/snow when paused
		if (pr && pr2 && pGame)
		{
			if (pGame->pause)
				{	 pr->setSpeedFactor(0.f);	 pr2->setSpeedFactor(0.f);	}
			else{	 pr->setSpeedFactor(1.f);	 pr2->setSpeedFactor(1.f);	}
		}

		
		//  update shader time
		mTimer += time;
		mFactory->setSharedParameter("windTimer",  sh::makeProperty <sh::FloatValue>(new sh::FloatValue(mTimer)));
		mFactory->setSharedParameter("waterTimer", sh::makeProperty <sh::FloatValue>(new sh::FloatValue(mTimer)));


		///()  grass sphere pos
		if (!carModels.empty())
		{			//par
			Real r = 1.7;  r *= r;
			const Vector3* p = &carModels[0]->posSph[0];
			mFactory->setSharedParameter("posSph0", sh::makeProperty <sh::Vector4>(new sh::Vector4(p->x,p->y,p->z,r)));
			p = &carModels[0]->posSph[1];
			mFactory->setSharedParameter("posSph1", sh::makeProperty <sh::Vector4>(new sh::Vector4(p->x,p->y,p->z,r)));
		}else
		{	mFactory->setSharedParameter("posSph0", sh::makeProperty <sh::Vector4>(new sh::Vector4(0,0,500,-1)));
			mFactory->setSharedParameter("posSph1", sh::makeProperty <sh::Vector4>(new sh::Vector4(0,0,500,-1)));
		}


		//  Signal loading finished to the peers
		if (mClient && bLoadingEnd)
		{
			bLoadingEnd = false;
			mClient->loadingFinished();
		}
		
		PROFILER.endBlock(" frameSt");

		return ret;
	}
	PROFILER.endBlock(" frameSt");
}

bool App::frameEnd(Real time)
{
	return true;
}
