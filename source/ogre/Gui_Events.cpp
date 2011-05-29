#include "stdafx.h"
#include "../vdrift/pathmanager.h"
#include "../vdrift/game.h"
#include "../road/Road.h"
#include "OgreGame.h"
#include "MyGUI_PointerManager.h"
#include <boost/filesystem.hpp>
#include "../oisb/OISB.h"
using namespace MyGUI;

#define res  1000000.f
#define Fmt  sprintf

///  Gui Events
//-----------------------------------------------------------------------------------------------------------

#define ChkEv(var)  \
	pSet->var = !pSet->var;  if (wp) {  \
	ButtonPtr chk = wp->castType<MyGUI::Button>(); \
    chk->setStateCheck(pSet->var);  }

//  [Input]

void App::controlBtnClicked(Widget* sender)
{
	sender->setCaption( TR("#{InputAssignKey}"));
	// activate key capture mode
	bAssignKey = true;
	pressedKeySender = sender;
	// hide mouse
	MyGUI::PointerManager::getInstance().setVisible(false);
}
void App::joystickBindChanged(Widget* sender, size_t val)
{
	// get action/schema this bind belongs too
	std::string actionName = Ogre::StringUtil::split(sender->getName(), "_")[1];
	std::string schemaName = Ogre::StringUtil::split(sender->getName(), "_")[2];
	
	Log(actionName);
	Log(schemaName);
	
	OISB::ActionSchema* schema = OISB::System::getSingleton().mActionSchemas[schemaName];
	OISB::Action* action = schema->mActions[actionName];
	if (action->mBindings.size() == 0) return;
	if (action->mBindings.size() == 1) action->createBinding();
	OISB::Binding* binding = action->mBindings[1];
	binding->mOptional = true;
	
	// get selected joystick
	// find selected oisb joystick for this tab (to get num axis & buttons)
	MyGUI::ComboBoxPtr jsMenu = mGUI->findWidget<ComboBox>("joystickSel_" + schemaName);
	std::string jsName;
	if (jsMenu->getIndexSelected() != MyGUI::ITEM_NONE)
		jsName = jsMenu->getItemNameAt( jsMenu->getIndexSelected() );
	else 
	{
		Log("Couldnt get selected joystick"); return;
	}
	Log(jsName);
		
	// get selected axis or button
	MyGUI::ComboBoxPtr box = static_cast<MyGUI::ComboBoxPtr> (sender);
	if (box->getItemCount() < box->getIndexSelected() || box->getIndexSelected() == MyGUI::ITEM_NONE)
	{
		Log("Invalid item value"); return;
	}
	std::string bindName = box->getItemNameAt(box->getIndexSelected());
	Log(bindName);
	
	// unbind old
	for (int i=0; i<binding->getNumBindables(); i++)
	{
		binding->unbind(binding->getBindable(i));
	}
	
	// bind new
	try {
		binding->bind(jsName + "/" + bindName); 
	}
	catch (OIS::Exception) {
		Log("Failed to bind '" + jsName + "/" + bindName + "'");
	}

}
void App::joystickSelectionChanged(Widget* sender, size_t val)
{
	UpdateJsButtons();
	
	// ----------------  update all binds with the new joystick  -----------------------------------------
	std::string actionSchemaName = Ogre::StringUtil::split(sender->getName(), "_")[1];
	
	OISB::ActionSchema* schema = mOISBsys->mActionSchemas[actionSchemaName];
		
	for (std::map<OISB::String, OISB::Action*>::const_iterator
		ait = schema->mActions.begin();
		ait != schema->mActions.end(); ait++)
	{
		MyGUI::WidgetPtr box;
		if ((*ait).second->getActionType() == OISB::AT_TRIGGER)
			box = mGUI->findWidget<Widget>("jsButtonSel_" + (*ait).first + "_" + actionSchemaName);
		else if ((*ait).second->getActionType() == OISB::AT_ANALOG_AXIS)
			box = mGUI->findWidget<Widget>("jsAxisSel_" + (*ait).first + "_" + actionSchemaName);
			
		joystickBindChanged(box, 0);
	}
	
}

//  [Setup]
//    [Car]
void App::chkAbs(WP wp){		ChkEv(abs);		if (pGame)  pGame->ProcessNewSettings();	}
void App::chkTcs(WP wp){		ChkEv(tcs);		if (pGame)  pGame->ProcessNewSettings();	}

void App::chkGear(WP wp){		ChkEv(autoshift);	if (pGame)  pGame->ProcessNewSettings();	}
void App::chkRear(WP wp){		ChkEv(autorear);	if (pGame)  pGame->ProcessNewSettings();	}
void App::chkClutch(WP wp){		ChkEv(autoclutch);	if (pGame)  pGame->ProcessNewSettings();	}
//    [Game]
void App::chkVegetCollis(WP wp){	ChkEv(veget_collis);	}
void App::btnNumPlayers(WP wp)
{
	if      (wp->getName() == "btnPlayers1")  pSet->local_players = 1;
	else if (wp->getName() == "btnPlayers2")  pSet->local_players = 2;
	else if (wp->getName() == "btnPlayers3")  pSet->local_players = 3;
	else if (wp->getName() == "btnPlayers4")  pSet->local_players = 4;
}
void App::chkSplitVert(WP wp)
{
	ChkEv(split_vertically); 
}


// game

//  [Graphics]

//  textures
void App::comboTexFilter(SL)
{
	TextureFilterOptions tfo;							
	switch (val)  {
		case 0:	 tfo = TFO_BILINEAR;	break;
		case 1:	 tfo = TFO_TRILINEAR;	break;
		case 2:	 tfo = TFO_ANISOTROPIC;	break;	}
	MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
}

void App::slAnisotropy(SL)
{
	MaterialManager::getSingleton().setDefaultAnisotropy(val);	pSet->anisotropy = val;
	if (valAnisotropy)	valAnisotropy->setCaption(toStr(val));
}

//  view dist
void App::slViewDist(SL)
{
	Real v = 50.f + 6950.f * powf(val/res, 2.f);
	Vector3 sc = v*Vector3::UNIT_SCALE;

	SceneNode* nskb = mSceneMgr->getSkyBoxNode();
	if (nskb)  nskb->setScale(sc*0.58);
	else  if (ndSky)  ndSky->setScale(sc);

	pSet->view_distance = v;
	if (valViewDist){	Fmt(s, "%4.1f km", v*0.001f);	valViewDist->setCaption(s);  }
	// Set new far clip distance for all cams
	mSplitMgr->UpdateCamDist();
}

//  ter detail
void App::slTerDetail(SL)
{
	Real v = 20.f * powf(val/res, 2.f);  pSet->terdetail = v;
	if (mTerrainGlobals)
		mTerrainGlobals->setMaxPixelError(v);
	if (valTerDetail){	Fmt(s, "%4.1f %%", v);	valTerDetail->setCaption(s);  }
}

//  ter dist
void App::slTerDist(SL)
{
	Real v = 1000.f * powf(val/res, 2.f);  pSet->terdist = v;
	if (mTerrainGlobals)
		mTerrainGlobals->setCompositeMapDistance(v);
	if (valTerDist){	Fmt(s, "%4.0f m", v);	valTerDist->setCaption(s);  }
}

//  road dist
void App::slRoadDist(SL)
{
	Real v = 4.f * powf(val/res, 2.f);  pSet->road_dist = v;
	if (valRoadDist){	Fmt(s, "%5.2f", v);	valRoadDist->setCaption(s);  }
}


//  trees/grass
void App::slTrees(SL)
{
	Real v = 4.f * powf(val/res, 2.f);  pSet->trees = v;
	if (valTrees){	Fmt(s, "%4.2f", v);	valTrees->setCaption(s);  }
}
void App::slGrass(SL)
{
	Real v = 4.f * powf(val/res, 2.f);  pSet->grass = v;
	if (valGrass){	Fmt(s, "%4.2f", v);	valGrass->setCaption(s);  }
}

void App::slTreesDist(SL)
{
	Real v = 0.5f + 6.5f * powf(val/res, 2.f);  pSet->trees_dist = v;
	if (valTreesDist){	Fmt(s, "%4.2f", v);	valTreesDist->setCaption(s);  }
}
void App::slGrassDist(SL)
{
	Real v = 0.5f + 6.5f * powf(val/res, 2.f);  pSet->grass_dist = v;
	if (valGrassDist){	Fmt(s, "%4.2f", v);	valGrassDist->setCaption(s);  }
}

void App::btnTrGrReset(WP wp)
{
	HScrollPtr sl;  size_t v;
	#define setSld(name)  sl##name(0,v);  \
		sl = (HScrollPtr)mLayout->findWidget(#name);  if (sl)  sl->setScrollPosition(v);
	v = res*powf(1.f /4.f, 0.5f);
	setSld(Trees);
	setSld(Grass);
	v = res*powf((1.f-0.5f) /6.5f, 0.5f);
	setSld(TreesDist);
	setSld(GrassDist);
}


//  particles/trails
void App::slParticles(SL)
{
	Real v = 4.f * powf(val/res, 2.f);  pSet->particles_len = v;
	if (valParticles){	Fmt(s, "%4.2f", v);	valParticles->setCaption(s);  }
}
void App::slTrails(SL)
{
	Real v = 4.f * powf(val/res, 2.f);  pSet->trails_len = v;
	if (valTrails){	Fmt(s, "%4.2f", v);	valTrails->setCaption(s);  }
}


//  view size
void App::slSizeGaug(SL)
{
	float v = 0.1f + 0.15f * val/res;	pSet->size_gauges = v;  SizeHUD(true);
	if (valSizeGaug){	Fmt(s, "%4.3f", v);	valSizeGaug->setCaption(s);  }
}
//  size minimap
void App::slSizeMinmap(SL)
{
	float v = 0.05f + 0.25f * val/res;	pSet->size_minimap = v;  SizeHUD(true);
	if (valSizeMinmap){	Fmt(s, "%4.3f", v);	valSizeMinmap->setCaption(s);  }
}


//  reflect
void App::slReflSkip(SL)
{
	int v = 1000.f * powf(val/res, 2.f);	pSet->refl_skip = v;
	if (valReflSkip)  valReflSkip->setCaption(toStr(v));
}
void App::slReflSize(SL)
{
	int v = max( 0.0f, min((float) ciShadowNumSizes-1, ciShadowNumSizes * val/res));	pSet->refl_size = v;
	if (valReflSize)  valReflSize->setCaption(toStr(ciShadowSizesA[v]));
}
void App::slReflFaces(SL)
{
	pSet->refl_faces = val;
	if (valReflFaces)  valReflFaces->setCaption(toStr(val));
}
void App::slReflDist(SL)
{
	float v = 20.f + 1480.f * powf(val/res, 2.f);	pSet->refl_dist = v;
	if (valReflDist){	Fmt(s, "%4.0f m", v);	valReflDist->setCaption(s);  }
}


void App::slShaders(SL)
{
	int v = val;  pSet->shaders = v;
	if (valShaders)
	{	if (v == 0)  valShaders->setCaption("Vertex");  else
		if (v == 1)  valShaders->setCaption("Pixel");  else
		if (v == 2)  valShaders->setCaption("Metal");  }
}

//  shadows
void App::btnShadows(WP){	changeShadows();	}

void App::slShadowType(SL)
{
	int v = val;	pSet->shadow_type = v;
	if (valShadowType)
	{	if (v == 0)  valShadowType->setCaption("None");  else
		if (v == 1)  valShadowType->setCaption("Old");  else
		if (v == 2)  valShadowType->setCaption("Normal");  else
		if (v == 3)  valShadowType->setCaption("Depth");  }
}

void App::slShadowCount(SL)
{
	int v = 2 + 2.f * val/res;	pSet->shadow_count = v;
	if (valShadowCount)  valShadowCount->setCaption(toStr(v));
}

void App::slShadowSize(SL)
{
	int v = max( 0.0f, min((float) ciShadowNumSizes-1, ciShadowNumSizes * val/res));	pSet->shadow_size = v;
	if (valShadowSize)  valShadowSize->setCaption(toStr(ciShadowSizesA[v]));
}

void App::slShadowDist(SL)
{
	Real v = 50.f + 4750.f * powf(val/res, 2.f);	pSet->shadow_dist = v;
	if (valShadowDist){  Fmt(s, "%4.1f km", v*0.001f);	valShadowDist->setCaption(s);  }
}


//  sound
void App::slVolMaster(SL)
{
	Real v = 1.6f * val/res;	pSet->vol_master = v;	pGame->ProcessNewSettings();
	if (valVolMaster){  Fmt(s, "%4.2f", v);	valVolMaster->setCaption(s);  }
}
void App::slVolEngine(SL)
{
	Real v = 1.4f * val/res;	pSet->vol_engine = v;
	if (valVolEngine){  Fmt(s, "%4.2f", v);	valVolEngine->setCaption(s);  }
}
void App::slVolTires(SL)
{
	Real v = 1.4f * val/res;	pSet->vol_tires = v;
	if (valVolTires){  Fmt(s, "%4.2f", v);	valVolTires->setCaption(s);  }
}
void App::slVolEnv(SL)
{
	Real v = 1.4f * val/res;	pSet->vol_env = v;
	if (valVolEnv){  Fmt(s, "%4.2f", v);	valVolEnv->setCaption(s);  }
}


//  car color
void App::slCarClrH(SL)
{
	Real v = val/res;  pSet->car_hue = v;
	if (valCarClrH){	Fmt(s, "%4.2f", v);	valCarClrH->setCaption(s);  }
	for (std::list<CarModel*>::iterator it=carModels.begin(); it!=carModels.end(); it++)
		(*it)->ChangeClr();
}
void App::slCarClrS(SL)
{
	Real v = -1.f + 2.f * val/res;  pSet->car_sat = v;
	if (valCarClrS){	Fmt(s, "%4.2f", v);	valCarClrS->setCaption(s);  }
	for (std::list<CarModel*>::iterator it=carModels.begin(); it!=carModels.end(); it++)
		(*it)->ChangeClr();
}
void App::slCarClrV(SL)
{
	Real v = -1.f + 2.f * val/res;  pSet->car_val = v;
	if (valCarClrV){	Fmt(s, "%4.2f", v);	valCarClrV->setCaption(s);  }
	for (std::list<CarModel*>::iterator it=carModels.begin(); it!=carModels.end(); it++)
		(*it)->ChangeClr();
}


//  [Game] 	. . . . . . . . . . . . . . . . . . . .    --- lists ----    . . . . . . . . . . . . . . . . . . . .

//  car
void App::listCarChng(List* li, size_t pos)
{
	size_t i = li->getIndexSelected();  if (i==ITEM_NONE)  return;
	const UString& sl = li->getItemNameAt(i);	sListCar = sl;
	
	if (imgCar)  imgCar->setImageTexture(sListCar+".jpg");
}
void App::btnChgCar(WP)
{
	if (valCar){  valCar->setCaption(TR("#{Car}: ") + sListCar);	pSet->car = sListCar;  }
}

//  track
void App::listTrackChng(List* li, size_t pos)
{
	if (!li)  return;
	size_t i = li->getIndexSelected();  if (i==ITEM_NONE)  return;

	const UString& sl = li->getItemNameAt(i);  String s = sl;
	s = StringUtil::replaceAll(s, "*", "");
	sListTrack = s;

	int u = *li->getItemDataAt<int>(i,false);
	bListTrackU = u;
	
	//  won't refresh if same-...  road dissapears if not found...
	if (imgPrv)  imgPrv->setImageTexture(sListTrack+".jpg");
	if (imgTer)  imgTer->setImageTexture(sListTrack+"_ter.jpg");
	if (imgMini)  imgMini->setImageTexture(sListTrack+"_mini.png");
	ReadTrkStats();
}

void App::btnChgTrack(WP)
{
	pSet->track = sListTrack;
	pSet->track_user = bListTrackU;
	if (valTrk)  valTrk->setCaption(TR("#{Track}: ") + sListTrack);
}

//  new game
void App::btnNewGame(WP)
{
	NewGame();  isFocGui = false;  // off gui
	if (mWndOpts)  mWndOpts->setVisible(isFocGui);
	if (mWndRpl)  mWndRpl->setVisible(false);//
	if (bnQuit)  bnQuit->setVisible(isFocGui);
	mGUI->setVisiblePointer(isFocGuiOrRpl());
	mToolTip->setVisible(false);
}
void App::btnNewGameStart(WP wp)
{
	btnChgTrack(wp);
	btnNewGame(wp);
}

void App::btnQuit(WP)
{
	mShutDown = true;
}


//  [View]  . . . . . . . . . . . . . . . . . . . .    ---- checks ----    . . . . . . . . . . . . . . . . . . . .

void App::chkDigits(WP wp){ 		ChkEv(show_digits); ShowHUD();   }

void App::chkReverse(WP wp){		ChkEv(trackreverse);	ReadTrkStats();  }

void App::chkParticles(WP wp)
{		
	ChkEv(particles);
	for (std::list<CarModel*>::iterator it=carModels.begin(); it!=carModels.end(); it++)
		(*it)->UpdParsTrails();
}
void App::chkTrails(WP wp)
{			
	ChkEv(trails);		
	for (std::list<CarModel*>::iterator it=carModels.begin(); it!=carModels.end(); it++)
		(*it)->UpdParsTrails();
}
void App::chkFps(WP wp){			ChkEv(show_fps);	if (pSet->show_fps)  mFpsOverlay->show();  else  mFpsOverlay->hide();	}

void App::chkGauges(WP wp){			ChkEv(show_gauges);	ShowHUD();	}
void App::chkMinimap(WP wp){		ChkEv(trackmap);	if (ndMap)  ndMap->setVisible(pSet->trackmap);	}
void App::chkTimes(WP wp){			ChkEv(show_times);	ShowHUD();	}

//void App::chkRacingLine(WP wp){		ChkEv(racingline);	if (ndLine)  ndLine->setVisible(pSet->racingline);	}
void App::chkCamInfo(WP wp){		ChkEv(show_cam);	ShowHUD();	}

void App::chkCarDbgBars(WP wp){		ChkEv(car_dbgbars);	ShowHUD();	}
void App::chkCarDbgTxt(WP wp){		ChkEv(car_dbgtxt);	ShowHUD();	}
void App::chkBltDebug(WP wp){		ChkEv(bltDebug);	}
void App::chkBltProfilerTxt(WP wp){	ChkEv(bltProfilerTxt);	}

void App::radKmh(WP wp){	bRkmh->setStateCheck(true);  bRmph->setStateCheck(false);  pSet->show_mph = false;  ShowHUD();  }
void App::radMph(WP wp){	bRkmh->setStateCheck(false);  bRmph->setStateCheck(true);  pSet->show_mph = true;   ShowHUD();  }

void App::comboLanguage(SL)
{
	if (val == MyGUI::ITEM_NONE)  return;
	MyGUI::ComboBoxPtr cmb = static_cast<MyGUI::ComboBoxPtr>(wp);
	std::string sel = cmb->getItemNameAt(val);
	
	for (std::map<std::string, std::string>::const_iterator it = supportedLanguages.begin();
		it != supportedLanguages.end(); it++)
	{
		if (it->second == sel)
			pSet->language = it->first;
	}
}

//  Startup
void App::chkOgreDialog(WP wp){		ChkEv(ogre_dialog);	}
void App::chkAutoStart(WP wp){		ChkEv(autostart);	}
void App::chkEscQuits(WP wp){		ChkEv(escquit);		}
void App::chkBltLines(WP wp){		ChkEv(bltLines);	}

void App::chkLoadPics(WP wp){		ChkEv(loadingbackground);	}


//  [Video]  . . . . . . . . . . . . . . . . . . . .    ---- ------ ----    . . . . . . . . . . . . . . . . . . . .

void App::btnResChng(WP)
{
	if (!resList)  return;
	if (resList->getIndexSelected() == MyGUI::ITEM_NONE) return;
	String mode = resList->getItem(resList->getIndexSelected());

	pSet->windowx = StringConverter::parseInt(StringUtil::split(mode, "x")[0]);
	pSet->windowy = StringConverter::parseInt(StringUtil::split(mode, "x")[1]);
	
	mWindow->resize(pSet->windowx, pSet->windowy);
	
	if (pSet->fullscreen)
		mWindow->setFullscreen(true, pSet->windowx, pSet->windowy);
	else
	{
	#ifdef _WIN32
		int sx = GetSystemMetrics(SM_CXSCREEN), sy = GetSystemMetrics(SM_CYSCREEN);
		int cx = max(0,(sx - pSet->windowx) / 2), cy = max(0,(sy - pSet->windowy) / 2);
		mWindow->reposition(cx,cy);
	#else
		//mWindow->reposition(0,0);  // center ?..
	#endif
	}
	bWindowResized = true;
}

void App::chkVidBloom(WP wp)
{		
	ChkEv(bloom);		
	refreshCompositor();		
}
void App::chkVidHDR(WP wp)
{			
	ChkEv(hdr);	
	refreshCompositor();
}
void App::chkVidBlur(WP wp)
{		
	ChkEv(motionblur);
	refreshCompositor();
}

void App::chkVidFullscr(WP wp){		ChkEv(fullscreen);
	mWindow->setFullscreen(pSet->fullscreen, pSet->windowx, pSet->windowy); mWindow->resize(pSet->windowx, pSet->windowy);
}
void App::chkVidVSync(WP wp)
{		
	ChkEv(vsync); 
	Ogre::Root::getSingleton().getRenderSystem()->setWaitForVerticalBlank(pSet->vsync);
}

void App::slBloomInt(SL)
{
	Real v = val/res;  pSet->bloomintensity = v;
	if (valBloomInt){	Fmt(s, "%4.2f", v);	valBloomInt->setCaption(s);  }
	refreshCompositor();
}
void App::slBloomOrig(SL)
{
	Real v = val/res;  pSet->bloomorig = v;
	if (valBloomOrig){	Fmt(s, "%4.2f", v);	valBloomOrig->setCaption(s);  }
	refreshCompositor();
}
void App::slBlurIntens(SL)
{
	Real v = val/res;  pSet->motionblurintensity = v;
	if (valBlurIntens){	Fmt(s, "%4.2f", v);	valBlurIntens->setCaption(s);  }
	refreshCompositor();
}


///  [Replay]  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void App::slRplPosEv(SL)  // change play pos
{
	if (!bRplPlay)  return;
	double oldt = pGame->timer.GetReplayTime();
	double v = val/res;  v = max(0.0, min(1.0, v));  v *= replay.GetTimeLength();
	pGame->timer.SetReplayTime(v);

	FollowCamera* fCam = (*carModels.begin())->fCam;
	fCam->first = true;  // instant change
	for (int i=0; i < 10; ++i)
		fCam->update(abs(v-oldt)/10.f);  //..?
}

void App::btnRplLoad(WP)  // Load
{
	//  from list
	int i = rplList->getIndexSelected();
	if (i == MyGUI::ITEM_NONE)  return;

	String name = rplList->getItemNameAt(i);
	string file = PATHMANAGER::GetReplayPath() + "/" + name + ".rpl";

	if (!replay.LoadFile(file))
	{
		Message::createMessageBox(  // #{transl ..
			"Message", "Load Replay", "Error: Can't load file.",
			MessageBoxStyle::IconWarning | MessageBoxStyle::Ok);
	}
	else  // car, track change
	{
		string car = replay.header.car, trk = replay.header.track;
		bool usr = replay.header.track_user == 1;

		pSet->car = car;
		pSet->track = trk;  pSet->track_user = usr;
		//bRplPlay = 1;
		btnNewGame(0);
		bRplPlay = 1;
	}
}

void App::btnRplSave(WP)  // Save
{
	String edit = edRplName->getCaption();
	String file = PATHMANAGER::GetReplayPath() + "/" + pSet->track + "_" + edit + ".rpl";
	///  save
	if (boost::filesystem::exists(file.c_str()))
	{
		Message::createMessageBox(  // #{..
			"Message", "Save Replay", "File already exists.",
			MessageBoxStyle::IconWarning | MessageBoxStyle::Ok);
		return;
	}
	if (!replay.SaveFile(file.c_str()))
	{
		Message::createMessageBox(  // #{..
			"Message", "Save Replay", "Error: Can't save file.",
			MessageBoxStyle::IconWarning | MessageBoxStyle::Ok);
	}
	updReplaysList();
}

//  list change
void App::listRplChng(List* li, size_t pos)
{
	size_t i = li->getIndexSelected();  if (i == ITEM_NONE)  return;
	String name = li->getItemNameAt(i);
	string file = PATHMANAGER::GetReplayPath() + "/" + name + ".rpl";
	if (!valRplName)  return;  valRplName->setCaption(name);
	if (!valRplInfo)  return;
	
	//  load replay header upd text descr
	Replay rpl;
	if (rpl.LoadFile(file,true))
	{
		String ss = String(TR("#{Track}: ")) + rpl.header.track + (rpl.header.track_user ? "  *user*" : "");
		valRplName->setCaption(ss);

		ss = String(TR("#{Car}: ")) + rpl.header.car +
			(rpl.header.numPlayers == 1 ? "" : "       Players: " + toStr(rpl.header.numPlayers)) +
			//(rpl.header.cars[0][0] != 0 ? " , " + rpl.header.cars[0] : "") +
			//(rpl.header.cars[0][1] != 0 ? " , " + rpl.header.cars[1] : "") +
			//(rpl.header.cars[0][2] != 0 ? " , " + rpl.header.cars[2] : "") +
			"\n" + TR("#{RplTime}: ") + GetTimeString(rpl.GetTimeLength());
		valRplInfo->setCaption(ss);

		int size = boost::filesystem::file_size(file);
		sprintf(s, "%5.2f", float(size)/1000000.f);
		ss = String(TR("#{RplFileSize}:")) + s + TR(" #{UnitMB}\n") +
			TR("#{RplVersion}: ") + toStr(rpl.header.ver) + "     " + toStr(rpl.header.frameSize) + "B";
		if (valRplInfo2)  valRplInfo2->setCaption(ss);
	}
	//edRplDesc  edRplName
}


void App::chkRplAutoRec(WP wp)
{
	bRplRec = !bRplRec;  // changes take effect next game start
	if (!wp)  return;
	ButtonPtr chk = wp->castType<MyGUI::Button>();
    chk->setStateCheck(bRplRec);
}

void App::chkRplChkGhost(WP wp)
{
	//ChkEv(rpl_play);
}


void App::btnRplCur(WP)
{
}

void App::btnRplAll(WP)
{
}

//  replay controls

void App::btnRplToStart(WP)
{
	pGame->timer.RestartReplay();
}

void App::btnRplToEnd(WP)
{
}

void App::btnRplBack(WP)
{
}

void App::btnRplForward(WP)
{
}

void App::btnRplPlay(WP)  // play / pause
{
	bRplPause = !bRplPause;
	UpdRplPlayBtn();
}

void App::UpdRplPlayBtn()
{
	String sign = bRplPause ? "|>" : "||";
	if (btRplPl)
		btRplPl->setCaption(sign);
}


void App::updReplaysList()
{
	if (!rplList)  return;
	rplList->removeAllItems();  int ii = 0;  bool bFound = false;

	strlist li;
	PATHMANAGER::GetFolderIndex(PATHMANAGER::GetReplayPath(), li, "rpl");
	for (strlist::iterator i = li.begin(); i != li.end(); ++i)
	if (StringUtil::endsWith(*i, ".rpl"))
	{
		String s = *i;
		s = StringUtil::replaceAll(s,".rpl","");
		rplList->addItem(s);
	}
}


//  Delete
void App::btnRplDelete(WP)
{
	size_t i = rplList->getIndexSelected();  if (i == ITEM_NONE)  return;
	String name = rplList->getItemNameAt(i);
	Message* message = Message::createMessageBox(
		"Message", "Delete Replay ?", name,
		MessageBoxStyle::IconQuest | MessageBoxStyle::Yes | MessageBoxStyle::No);
	message->eventMessageBoxResult = newDelegate(this, &App::msgRplDelete);
	//message->setUserString("FileName", fileName);
}
void App::msgRplDelete(Message* sender, MessageBoxStyle result)
{
	if (result != MessageBoxStyle::Yes)
		return;
	size_t i = rplList->getIndexSelected();  if (i == ITEM_NONE)  return;
	String name = rplList->getItemNameAt(i);
	
	string file = PATHMANAGER::GetReplayPath() +"/"+ name + ".rpl";
	if (boost::filesystem::exists(file))
		boost::filesystem::remove(file);
	updReplaysList();
}

//  Rename
void App::btnRplRename(WP)
{
	//if (boost::filesystem::exists(from.c_str()))
	//	boost::filesystem::rename(from.c_str(), to.c_str());
}


//-----------------------------------------------------------------------------------------------------------
//  Key pressed
//-----------------------------------------------------------------------------------------------------------
bool App::keyPressed( const OIS::KeyEvent &arg )
{
	// update all keystates
	OISB::System::getSingleton().process(0);
	
	#define action(s) mOISBsys->lookupAction(std::string("General/")+std::string(s))->isActive()

	if (!bAssignKey)
	{
		//  change gui tabs
		if (mWndTabs)
		{	int num = mWndTabs->getItemCount();
			if (isFocGui)  
			{
				if (action("PrevTab")) {
					mWndTabs->setIndexSelected( (mWndTabs->getIndexSelected() - 1 + num) % num ); return true;
				}
				else if (action("NextTab")) {
					mWndTabs->setIndexSelected( (mWndTabs->getIndexSelected() + 1) % num ); return true;
				}
		}	}
		
		//  gui on/off
		if (action("ShowOptions"))
		{	toggleGui();  return false;  }
	
		//  new game
		if (action("RestartGame"))
		{	NewGame();  return false;	}


		///  Cameras  ---------------------------------
		int iChgCam = 0;
		if (action("NextCamera"))  // Next
			iChgCam = 1;
		if (action("PrevCamera"))  // Prev
			iChgCam = -1;
		
		if (iChgCam)
		{
			if (ctrl)
				//  change current camera car index
				iCurCam = (iCurCam + iChgCam +pSet->local_players) % pSet->local_players;
			else
			{	int visMask = 255, i = 0;
				roadUpCnt = 0;

				for (std::list<CarModel*>::iterator it=carModels.begin(); it!=carModels.end(); it++, i++)
				if (i == iCurCam)
				{
					if ((*it)->fCam)
					{	(*it)->fCam->Next(iChgCam < 0, shift);
						if ((*it)->fCam->ca.mHideGlass)  visMask = 255-16;
						else        visMask = 255;
					}
				}
				for (std::list<Viewport*>::iterator it=mSplitMgr->mViewports.begin(); it!=mSplitMgr->mViewports.end(); it++)
					(*it)->setVisibilityMask(visMask);
			}
			return false;
		}
	}
	
	using namespace OIS;
	if (!bAssignKey)
	{
		switch (arg.key)
		{
			case KC_ESCAPE:
				if (pSet->escquit)
					mShutDown = true;	// quit
				else
					toggleGui();		// gui on/off
				return true;

			#if 0
			case KC_1:
			if (mSplitMgr)
			{	Ogre::Viewport* vp = *mSplitMgr->mViewports.begin();
				vp->setAutoUpdated(shift);	vp->setVisibilityMask(shift ? 255 : 0);
			}	return true;
			case KC_2:
			if (mSplitMgr)
			{	Ogre::Viewport* vp = *(++mSplitMgr->mViewports.begin());
				vp->setAutoUpdated(shift);	vp->setVisibilityMask(shift ? 255 : 0);
			}	return true;
			case KC_3:
			if (mSplitMgr)
			{	Ogre::Viewport* vp = *(--mSplitMgr->mViewports.end());
				vp->setAutoUpdated(shift);	vp->setVisibilityMask(shift ? 255 : 0);
			}	return true;
			#endif


			case KC_BACK:	// replay controls
				if (mWndRpl && !isFocGui)
				{	mWndRpl->setVisible(!mWndRpl->isVisible());
					return true;  }
				break;

			case KC_P:		// replay play/pause
				if (bRplPlay && !isFocGui)
				{	bRplPause = !bRplPause;  UpdRplPlayBtn();
					return true;  }
				break;
				

			case KC_F9:		// car debug text/bars
				if (shift)	{	WP wp = chDbgT;  ChkEv(car_dbgtxt);  ShowHUD();  }
				else		{	WP wp = chDbgB;  ChkEv(car_dbgbars);   ShowHUD();  }
				return true;

			case KC_F11:	//  fps
			if (!shift)
			{	WP wp = chFps;  ChkEv(show_fps); 
				if (pSet->show_fps)  mFpsOverlay->show();  else  mFpsOverlay->hide();
				return false;
			}	break;

			case KC_F10:	//  blt debug, txt
			if (shift)
			{	WP wp = chBltTxt;  ChkEv(bltProfilerTxt);  return false;  }
			else if (ctrl)
			{	WP wp = chBlt;  ChkEv(bltDebug);  return false;  }
			break;


			case KC_F7:		// Times
			{	WP wp = chTimes;  ChkEv(show_times);  ShowHUD();  }
				return false;
				
			case KC_F8:		// Minimap
			{	WP wp = chMinimp;  ChkEv(trackmap);  if (ndMap)  ndMap->setVisible(pSet->trackmap);
			}	return false;
			
			
			case KC_RETURN:	//  chng trk + new game  after pg up/dn
			if (isFocGui)
			if (mWndTabs->getIndexSelected() == 0)  // track
			{	btnChgTrack(0);
				btnNewGame(0);
			}else if (mWndTabs->getIndexSelected() == 1)  // car
			{	btnChgCar(0);
				btnNewGame(0);
			}else if (mWndTabs->getIndexSelected() == 3)  // replay
			{
				btnRplPlay(0);
			}
			return false;
		}
	}

	
	if (!BaseApp::keyPressed(arg))
		return true;

	return true;
}


void App::toggleGui()
{
	if (alt)  return;
	isFocGui = !isFocGui;
	if (mWndOpts)	mWndOpts->setVisible(isFocGui);
	if (bnQuit)  bnQuit->setVisible(isFocGui);
	if (mGUI)	mGUI->setVisiblePointer(isFocGuiOrRpl());
	if (!isFocGui)  mToolTip->setVisible(false);
}
