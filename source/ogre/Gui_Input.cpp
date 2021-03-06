#include "pch.h"
#include "common/Defines.h"
#include "../vdrift/game.h"
#include "OgreGame.h"
#include "common/Gui_Def.h"

#include <OgreRoot.h>
using namespace MyGUI;
using namespace Ogre;

#define ALIGN  Align::Default  // MyGUI 3.2 has no Align::Relative


namespace
{
std::string GetKeyName(SDL_Keycode key, bool omit = false)
{
	if (key == SDLK_UNKNOWN)
		return omit ? "" : TR("#{InputKeyUnassigned}");
	else
		return std::string(SDL_GetKeyName(key));
}
}

///  input events
//----------------------------------------------------------------------------------------------------------------------------------
void App::LoadInputDefaults()
{
	mInputActions.clear();
	mInputActions.push_back(InputAction(A_ShowOptions, "ShowOptions", SDLK_TAB, InputAction::Trigger));
	mInputActions.push_back(InputAction(A_PrevTab, "PrevTab", SDLK_F2, InputAction::Trigger));
	mInputActions.push_back(InputAction(A_NextTab, "NextTab", SDLK_F3, InputAction::Trigger));
	mInputActions.push_back(InputAction(A_RestartGame, "RestartGame", SDLK_F5, InputAction::Trigger));
	mInputActions.push_back(InputAction(A_ResetGame, "ResetGame", SDLK_F4, InputAction::Trigger));
	mInputActions.push_back(InputAction(A_Screenshot, "Screenshot", SDLK_F12, InputAction::Trigger));

	LoadInputDefaults(mInputActions, mInputCtrl);

	std::vector<InputAction>* ap = mInputActionsPlayer;
	ap[0].clear();
	ap[0].push_back(InputAction(A_Throttle, "Throttle", SDLK_UP, InputAction::HalfAxis));
	ap[0].push_back(InputAction(A_Brake, "Brake", SDLK_DOWN, InputAction::HalfAxis));
	ap[0].push_back(InputAction(A_Steering, "Steering", SDLK_LEFT, SDLK_RIGHT, InputAction::Axis));
	ap[0].push_back(InputAction(A_HandBrake, "HandBrake", SDLK_SPACE, InputAction::HalfAxis));
	ap[0].push_back(InputAction(A_Boost, "Boost", SDLK_LCTRL, InputAction::HalfAxis));
	ap[0].push_back(InputAction(A_Flip, "Flip", SDLK_q, SDLK_w, InputAction::Axis));
	ap[0].push_back(InputAction(A_ShiftUp, "ShiftUp", SDLK_a, InputAction::Trigger));
	ap[0].push_back(InputAction(A_ShiftDown, "ShiftDown", SDLK_z, InputAction::Trigger));
	ap[0].push_back(InputAction(A_PrevCamera, "PrevCamera", SDLK_x, InputAction::Trigger));
	ap[0].push_back(InputAction(A_NextCamera, "NextCamera", SDLK_c, InputAction::Trigger));
	ap[0].push_back(InputAction(A_LastChk, "LastChk", SDLK_0, InputAction::Trigger));
	ap[0].push_back(InputAction(A_Rewind, "Rewind", SDLK_BACKSPACE, InputAction::Trigger));

	ap[1].clear();
	ap[1].push_back(InputAction(A_Throttle, "Throttle", SDLK_u, InputAction::HalfAxis));
	ap[1].push_back(InputAction(A_Brake, "Brake", SDLK_m, InputAction::HalfAxis));
	ap[1].push_back(InputAction(A_Steering, "Steering", SDLK_h, SDLK_k, InputAction::Axis));
	ap[1].push_back(InputAction(A_HandBrake, "HandBrake", SDLK_n, InputAction::HalfAxis));
	ap[1].push_back(InputAction(A_Boost, "Boost", SDLK_j, InputAction::HalfAxis));
	ap[1].push_back(InputAction(A_Flip, "Flip", SDLK_y, SDLK_i, InputAction::Axis));
	ap[1].push_back(InputAction(A_ShiftUp, "ShiftUp", SDLK_UNKNOWN, InputAction::Trigger));
	ap[1].push_back(InputAction(A_ShiftDown, "ShiftDown", SDLK_UNKNOWN, InputAction::Trigger));
	ap[1].push_back(InputAction(A_PrevCamera, "PrevCamera", SDLK_UNKNOWN, InputAction::Trigger));
	ap[1].push_back(InputAction(A_NextCamera, "NextCamera", SDLK_UNKNOWN, InputAction::Trigger));
	ap[1].push_back(InputAction(A_LastChk, "LastChk", SDLK_UNKNOWN, InputAction::Trigger));
	ap[1].push_back(InputAction(A_Rewind, "Rewind", SDLK_UNKNOWN, InputAction::Trigger));

	ap[2].clear();
	ap[2].push_back(InputAction(A_Throttle, "Throttle", SDLK_r, InputAction::HalfAxis));
	ap[2].push_back(InputAction(A_Brake, "Brake", SDLK_v, InputAction::HalfAxis));
	ap[2].push_back(InputAction(A_Steering, "Steering", SDLK_d, SDLK_g, InputAction::Axis));
	ap[2].push_back(InputAction(A_HandBrake, "HandBrake", SDLK_b, InputAction::HalfAxis));
	ap[2].push_back(InputAction(A_Boost, "Boost", SDLK_f, InputAction::HalfAxis));
	ap[2].push_back(InputAction(A_Flip, "Flip", SDLK_e, SDLK_t, InputAction::Axis));
	ap[2].push_back(InputAction(A_ShiftUp, "ShiftUp", SDLK_UNKNOWN, InputAction::Trigger));
	ap[2].push_back(InputAction(A_ShiftDown, "ShiftDown", SDLK_UNKNOWN, InputAction::Trigger));
	ap[2].push_back(InputAction(A_PrevCamera, "PrevCamera", SDLK_UNKNOWN, InputAction::Trigger));
	ap[2].push_back(InputAction(A_NextCamera, "NextCamera", SDLK_UNKNOWN, InputAction::Trigger));
	ap[2].push_back(InputAction(A_LastChk, "LastChk", SDLK_UNKNOWN, InputAction::Trigger));
	ap[2].push_back(InputAction(A_Rewind, "Rewind", SDLK_UNKNOWN, InputAction::Trigger));

	ap[3].clear();
	ap[3].push_back(InputAction(A_Throttle, "Throttle", SDLK_p, InputAction::HalfAxis));
	ap[3].push_back(InputAction(A_Brake, "Brake", SDLK_SLASH, InputAction::HalfAxis));
	ap[3].push_back(InputAction(A_Steering, "Steering", SDLK_l, SDLK_QUOTE, InputAction::Axis));
	ap[3].push_back(InputAction(A_HandBrake, "HandBrake", SDLK_PERIOD, InputAction::HalfAxis));
	ap[3].push_back(InputAction(A_Boost, "Boost", SDLK_SEMICOLON, InputAction::HalfAxis));
	ap[3].push_back(InputAction(A_Flip, "Flip", SDLK_o, SDLK_LEFTBRACKET, InputAction::Axis));
	ap[3].push_back(InputAction(A_ShiftUp, "ShiftUp", SDLK_UNKNOWN, InputAction::Trigger));
	ap[3].push_back(InputAction(A_ShiftDown, "ShiftDown", SDLK_UNKNOWN, InputAction::Trigger));
	ap[3].push_back(InputAction(A_PrevCamera, "PrevCamera", SDLK_UNKNOWN, InputAction::Trigger));
	ap[3].push_back(InputAction(A_NextCamera, "NextCamera", SDLK_UNKNOWN, InputAction::Trigger));
	ap[3].push_back(InputAction(A_LastChk, "LastChk", SDLK_UNKNOWN, InputAction::Trigger));
	ap[3].push_back(InputAction(A_Rewind, "Rewind", SDLK_UNKNOWN, InputAction::Trigger));

	for (int i=0; i<4; ++i)
		LoadInputDefaults(ap[i], mInputCtrlPlayer[i]);
}

void App::LoadInputDefaults(std::vector<InputAction> &actions, ICS::InputControlSystem *pICS)
{
	for (std::vector<InputAction>::iterator it = actions.begin(); it != actions.end(); ++it)
	{
		ICS::Control* control;
		bool controlExists = (pICS->getChannel(it->mId)->getControlsCount() != 0);
		if (!controlExists)
		{
			if (it->mType == InputAction::Trigger)
				control = new ICS::Control(boost::lexical_cast<std::string>(it->mId), false, true, 0, ICS::ICS_MAX, ICS::ICS_MAX, false);
			else if (it->mType == InputAction::Axis)
				control = new ICS::Control(boost::lexical_cast<std::string>(it->mId), false, true, 0.5, 0.1, 30.0);
			else if (it->mType == InputAction::HalfAxis)
					control = new ICS::Control(boost::lexical_cast<std::string>(it->mId), false, true, 0.0, 0.1, 30.0);

			pICS->addControl(control);

			if (it->mDefaultIncrease != SDLK_UNKNOWN)
				pICS->addKeyBinding(control, it->mDefaultIncrease, ICS::Control::INCREASE);
			if (it->mDefaultDecrease != SDLK_UNKNOWN)
				pICS->addKeyBinding(control, it->mDefaultDecrease, ICS::Control::DECREASE);

			control->attachChannel(pICS->getChannel(it->mId), ICS::Channel::DIRECT);
			pICS->getChannel(it->mId)->update();
		}
		else
			control = pICS->getChannel(it->mId)->getAttachedControls().front().control;

		it->mICS = pICS;
		it->mControl = control;

		if (pICS == mInputCtrl)
			pICS->getChannel(it->mId)->addListener(this);
	}
}

///  Input caption  ---------------------
void App::UpdateInputButton(MyGUI::Button* button, const InputAction& action, int bind)
{
	std::string s, sAssign = TR("#FFA030#{InputAssignKey}");  // caption

	SDL_Keycode decKey = action.mICS->getKeyBinding(action.mControl, ICS::Control::DECREASE);
	SDL_Keycode incKey = action.mICS->getKeyBinding(action.mControl, ICS::Control::INCREASE);

	if (action.mType == InputAction::Axis)
	{
		if (bind == 1)
			s = sAssign;
		else
		{	s += GetKeyName(decKey,true);
			if (!s.empty())  s += " , ";
			if (bind == 2)
				s += sAssign;
			else
				s += GetKeyName(incKey,true);
		}
	}else
	{	if (bind == 1)
			s = sAssign;
		else
			s += GetKeyName(incKey, action.mType & InputAction::Axis);
	}

	if (bind == 0)
	{
		for (int j=0; j < SDL_NumJoysticks(); ++j)
		{
			int axis = action.mICS->getJoystickAxisBinding(action.mControl, j, ICS::Control::INCREASE);
			if (axis != ICS::InputControlSystem::UNASSIGNED)
			{
				if (!s.empty())  s += " / ";
				s += "J"+toStr(j) + ".Axis " + toStr(axis);
			}
			int btn = action.mICS->getJoystickButtonBinding(action.mControl, j, ICS::Control::INCREASE);
			if (btn != ICS::InputControlSystem::UNASSIGNED)
			{
				if (!s.empty())  s += " / ";
				s += "J"+toStr(j) + ".Button " + toStr(btn);
			}
		}
	}
	if (s.empty())  s = TR("#{InputKeyUnassigned}");
	button->setCaption(s);
}


///  Gui Init - Input tabs
//----------------------------------------------------------------------------------------------------------------------------------
void App::CreateInputTab(const std::string& title, bool playerTab, const std::vector<InputAction>& actions, ICS::InputControlSystem* ICS)
{
	if (!tabInput)  return;
	TabItemPtr tabitem = tabInput->addItem(TR(title));

	std::string sPlr = title;

	//  button size and columns positons
	const int sx = 150, sy = 24,
		x0 = 16, x1 = 140, x2 = 310, x3 = 454,
		yh = 20, ya = 14,  s0 = x1-x0-5;

	#define CreateText(x,y, w,h, name, text)  {  StaticTextPtr txt =  \
		tabitem->createWidget<TextBox>("TextBox", x,y+2, w,h, ALIGN, name);  \
		setOrigPos(txt, "OptionsWnd");  \
		txt->setCaption(text);  }


	///  Headers  action, binding, value
	CreateText(x0,yh, sx,sy, "hdrTxt1_"+sPlr, TR("#90B0F0#{InputHeaderTxt1}"));
	CreateText(x1,yh, sx,sy, "hdrTxt2_"+sPlr, TR("#A0C0FF#{InputHeaderTxt2}"));
	if (playerTab)  {
		CreateText(x2,yh, sx,sy, "hdrTxt3_"+sPlr, TR("#90B0F0#{InputHeaderTxt3}"));
		CreateText(x3,yh, sx,sy, "hdrTxt4_"+sPlr, TR("#80A0E0#{InputHeaderTxt4}"));  }

	//  spacing for add y
	std::map <std::string, int> yRow;
	//  player
	yRow["Throttle"] = 2;	yRow["Brake"] = 2;	yRow["Steering"] = 2 +1;
	yRow["HandBrake"] = 2;	yRow["Boost"] = 2;	yRow["Flip"] = 2 +2;
	yRow["ShiftUp"] = 2;	yRow["ShiftDown"] = 2 +1;
	yRow["PrevCamera"] = 2;	yRow["NextCamera"] = 2+1;
	yRow["LastChk"] = 2;   yRow["Rewind"] = 2;
	//  general
	yRow["ShowOptions"] = 2+1;
	yRow["PrevTab"] = 2;		yRow["NextTab"] = 2+1;
	yRow["RestartGame"] = 2;	yRow["ResetGame"] = 2+1;
	yRow["Screenshot"] = 2;


	///  Actions  ------------------------------------------------
	int i = 0, y = yh + 2*ya;
	for (std::vector<InputAction>::const_iterator it = actions.begin(); it != actions.end(); ++it)
	{
		std::string name = it->mName;

		//  description label  ----------------
		StaticTextPtr desc = tabitem->createWidget<TextBox>("TextBox",
			x0, y+3, s0, sy,  ALIGN);
		setOrigPos(desc, "OptionsWnd");
		desc->setCaption( TR("#{InputMap" + name + "}") );
		desc->setTextColour(Colour(0.86f,0.94f,1.f));

		//  bind info
		bool analog = it->mType & InputAction::Axis;
		bool twosided = it->mType == InputAction::Axis;

		//  binding button  ----------------
		ButtonPtr btn1 = tabitem->createWidget<Button>("Button",
			x1, y, sx, sy,  ALIGN);
		setOrigPos(btn1, "OptionsWnd");
		UpdateInputButton(btn1, *it);
		btn1->eventMouseButtonClick += newDelegate(this, &App::inputBindBtnClicked);
		btn1->eventMouseButtonPressed += newDelegate(this, &App::inputBindBtn2);
		btn1->setUserData(*it);
		Colour clr = !playerTab ? Colour(0.7f,0.85f,1.f) :
			(analog ? (twosided ? Colour(0.8f,0.8f,1.0f) : Colour(0.7f,0.8f,1.0f)) : Colour(0.7f,0.9f,0.9f));
		btn1->setColour(clr);
		btn1->setTextColour(clr);

		//  value bar  --------------
		if (playerTab)
		{
			StaticImagePtr bar = tabitem->createWidget<ImageBox>("ImageBox",
				x2 + (twosided ? 0 : 64), y+4, twosided ? 128 : 64, 16, ALIGN,
				"bar_" + toStr(i) + "_" + sPlr);
			setOrigPos(bar, "OptionsWnd");
			bar->setUserData(*it);
			bar->setImageTexture(String("input_bar.png"));  bar->setImageCoord(IntCoord(0,0,128,16));
		}

		//  detail btn  ----------------
		if (analog)
		{	btn1 = tabitem->createWidget<Button>("Button",
				x3, y, 32, sy,  ALIGN,
				"inputdetail_" + toStr(i) + "_" + sPlr + "_1");
			setOrigPos(btn1, "OptionsWnd");
			btn1->setCaption(">");
			btn1->setTextColour(Colour(0.6f,0.7f,0.8f));
			btn1->setColour(Colour(0.6f,0.8f,1.0f));
			btn1->setUserData(*it);
			btn1->eventMouseButtonClick += newDelegate(this, &App::inputDetailBtn);
		}
		++i;
		y += yRow[name] * ya;
	}

	if (playerTab)
	{	y+=ya;
		CreateText(x1,y, 500,24, "txtunb" + sPlr, TR("#80B0F0#{InputUnbind}"));  y+=ya;
	}

	///  General tab  --------
	if (!playerTab)
	{	y += 2*ya;  //  camera infos
		CreateText(20,y, 280,24, "txtcam1", TR("#A0D0F0#{InputMapNextCamera} / #{InputMapPrevCamera}"));  y+=2*ya;
		CreateText(40,y, 280,24, "txtcam2", TR("#A0D0F0#{InputCameraTxt1}"));  y+=3*ya;
		//  replay controls info text
		CreateText(20,y, 500,24, "txtrpl1", TR("#A0D0F0#{Replay}:"));  y+=2*ya;
		CreateText(40,y, 500,24, "txtrpl2", TR("#80B0F0#{InputRplCtrl1}"));  y+=2*ya;
		CreateText(40,y, 500,24, "txtrpl3", TR("#80B0F0#{InputRplCtrl2}"));  y+=2*ya;
		CreateText(40,y, 500,24, "txtrpl4", TR("#80B0F0#{InputRplCtrl3}"));  y+=2*ya;
		CreateText(40,y, 500,24, "txtrpl5", TR("#60A0D0#{InputRplCtrl4}"));  y+=2*ya;
	}
}

void App::InitInputGui()
{
	LoadInputDefaults();

	txtInpDetail = mGUI->findWidget<StaticText>("InputDetail");
	panInputDetail = mGUI->findWidget<Widget>("PanInputDetail");

	TabItemPtr inpTabAll = mGUI->findWidget<TabItem>("InputTabAll");  if (!inpTabAll)  return;
	Tab(tabInput, "InputTab", tabInputChg);
	if (!tabInput)  return;

	//  details edits
	ButtonPtr btn, bchk;
	Btn("InputInv", btnInputInv);  //Ed(InputMul, editInput);
	Ed(InputIncrease, editInput);  //Ed(InputReturn, editInput);
	Chk("OneAxisThrBrk", chkOneAxis, false);  chOneAxis = bchk;

	//  key emul presets combo
	ComboBoxPtr combo;
	Cmb(combo, "CmbInputKeysAllPreset", comboInputKeyAllPreset);
	if (combo)
	{	combo->removeAllItems();  combo->addItem("");
		combo->addItem(TR("#{InpSet_Slow}"));
		combo->addItem(TR("#{InpSet_Medium}"));
		combo->addItem(TR("#{InpSet_Fast}"));
	}
	
	//  button size and columns positon
	const int sx = 130, sy = 24,  x0 = 20, x1 = 140, x2 = 285, x3 = 430,  yh = 20,  s0 = x1-x0-5;


	///  insert a tab item for every schema (global, 4players)
	CreateInputTab("#80C0FF#{InputMapGeneral}", false, mInputActions, mInputCtrl);
	for (int i=0; i < 4; ++i)
		CreateInputTab(String("#FFF850") + (i==0 ? "#{Player} ":" ") +toStr(i+1), true, mInputActionsPlayer[i], mInputCtrlPlayer[i]);


	TabItemPtr tabitem = tabInput->addItem(TR("#C0C0FF#{Other}"));
	int y = 32, ya = 26 / 2, yb = 20 / 2,  xa = 20, xa1=xa+16, xb = 250, xb1=xb+16;
	CreateText(xa,y, 500,24, "txtoth1", TR("#A0D0FF#{InputOther1}"));  y+=2*ya;
	CreateText(xa,y, 500,24, "txtoth2", TR("#A0D0FF#{InputOther2}"));  y+=2*ya;
	//CreateText(xa,y, 500,24, "txtoth3", TR("#80B0F0#{InputOther3}"));  y+=2*ya;
	y+=2*ya;
	CreateText(xa,y, 500,24, "txttir0", TR("#B0C0D0#{TiresEdit}"));  y+=2*ya;
	CreateText(xa1,y, 500,24, "txttir1", TR("#A0B0C0#{TiresEdit1}"));  y+=2*ya;
	CreateText(xa1,y, 500,24, "txttir2", TR("#A0B0C0#{TiresEdit2}"));  y+=2*ya;
	CreateText(xa1,y, 500,24, "txttir3", TR("#A0B0C0#{TiresEdit3}"));  y+=2*ya;
	CreateText(xa1,y, 500,24, "txttir4", TR("#A0B0C0#{TiresEdit4}"));  y+=3*ya;
	CreateText(xa,y, 500,24, "txttir5", TR("#90B0D0#{InputMapPrevTab}/#{InputMapNextTab} - #{InputGraphsType}"));  y+=3*ya;

	CreateText(xa,y, 500,24, "txttwk1", TR("#A0B8D0#{TweakEdit1}"));  y+=2*ya;
	CreateText(xa,y, 500,24, "txttwk2", TR("#A0B8D0#{TweakEdit2}"));  y+=3*ya;
	CreateText(xa,y, 500,24, "txttwk3", TR("#90B0C8#{TweakEdit3}"));  y+=2*ya;


	y = 32;
	tabitem = tabInput->addItem(TR("#B0A0E0#{Shortcuts}"));
	EditBox* ed = tabitem->createWidget<EditBox>("EditBoxEmpty", xa,y, 360,36, ALIGN, "txtshc0");
	ed->setCaption("#A0C0E0"+TR("#{ShortcutsInfo}"));  setOrigPos(ed, "OptionsWnd");  y+=5*yb;
	ed->setEditReadOnly(1);  ed->setEditMultiLine(1);  ed->setEditWordWrap(1);

	CreateText(xa,y, 200,24, "txtshc1", "#60FF60"+TR("Q  #{Track}"));  y+=2*yb;
	CreateText(xa,y, 200,24, "txtshc2", "#FF6050"+TR("C  #{Car}"));  y+=3*yb;
	CreateText(xa,y,  200,24, "txtshc3", "#90A0A0"+TR("T  #{Setup}"));  y+=2*yb;
	CreateText(xa,y,  200,24, "txtshc3", "#C0C080"+TR("W  #{Game}"));  y+=3*yb;
	CreateText(xa,y, 200,24, "txtshc5", "#FFC060"+TR("J  #{Tutorial}"));  y+=2*yb;
	CreateText(xa,y, 200,24, "txtshc5", "#80C0FF"+TR("H  #{Championship}"));  y+=2*yb;
	CreateText(xa,y, 200,24, "txtshc5", "#80FFCC"+TR("L  #{Challenge}"));  y+=3*yb;

	CreateText(xa,y, 200,24, "txtshc4", "#A0A0FF"+TR("U  #{Multiplayer}"));  y+=3*yb;
	CreateText(xa,y, 200,24, "txtshc6", "#FFA050"+TR("R  #{Replay}"));  y+=4*yb;
	CreateText(xa,y, 200,24, "txtshc7", "#60D060"+TR("#{InputFocusFind}"));  y+=2*yb;

	y = 32 + 5*yb;
	CreateText(xb,y, 200,24, "txtshd1", "#C0E0FF"+TR("S  #{Screen}"));  y+=2*yb;
	CreateText(xb1,y, 200,24, "txtshd2", "#E0C080"+TR("E  #{Effects}"));  y+=2*yb;
	CreateText(xb,y, 200,24, "txtshd3", "#B0B0FF"+TR("G  #{Graphics}"));  y+=2*yb;
	CreateText(xb1,y, 200,24, "txtshd4", "#90FF30"+TR("N  #{Vegetation}"));  y+=3*yb;
	
	CreateText(xb,y, 200,24, "txtshd5", "#D0FFFF"+TR("V  #{View}"));  y+=2*yb;
	CreateText(xb1,y, 200,24, "txtshd6", "#60F8F8"+TR("M  #{Minimap}"));  y+=2*yb;
	CreateText(xb1,y, 200,24, "txtshd7", "#C0A0E0"+TR("O  #{Other}"));  y+=3*yb;
	CreateText(xb,y, 200,24, "txtshd8", "#FFFF60"+TR("I  #{Input}"));  y+=3*yb;
	CreateText(xb,y, 200,24, "txtshd8", "#B090E0"+TR("P  #{Sound}"));  y+=3*yb;
}


///  Bind Input
//----------------------------------------------------------------------------------------------------------------------------------

void App::inputBindBtn2(WP sender, int, int, MouseButton mb)
{
	if (mb == MouseButton::Right)
		inputUnbind(sender);
}

void App::inputBindBtnClicked(WP sender)
{
	sender->castType<MyGUI::Button>()->setCaption( TR("#FFA030#{InputAssignKey}"));

	InputAction* action = sender->getUserData<InputAction>();
	mBindingAction = action;
	mBindingSender = sender->castType<MyGUI::Button>();

	if (mBindingAction->mType == InputAction::Axis)
	{	// bind decrease (ie left) first
		action->mICS->enableDetectingBindingState(action->mControl, ICS::Control::DECREASE);
	}else
		action->mICS->enableDetectingBindingState(action->mControl, ICS::Control::INCREASE);

	UpdateInputButton(mBindingSender, *action, 1);

	// activate key capture mode
	bAssignKey = true;
	hideMouse();
}

void App::notifyInputActionBound(bool complete)
{	
	UpdateInputButton(mBindingSender, *mBindingAction, complete ? 0 : 2);
	if (complete)
	{	bAssignKey = false;

		// If a key was assigned that used to belong to another control, it will now be unassigned,
		// so we need to force-update button labels
		TabControl* inputTab = mGUI->findWidget<TabControl>("InputTab");  if (!inputTab)  return;
		TabItem* current = inputTab->getItemSelected();
		for (int i=0; i < current->getChildCount(); ++i)
		{
			MyGUI::Button* button = current->getChildAt(i)->castType<MyGUI::Button>(false);
			if (!button || button->getCaption() == ">") // HACK: we don't want the detail buttons
				continue;
			if (button->getUserData<InputAction>() != mBindingAction)
				UpdateInputButton(button, *button->getUserData<InputAction>());
		}
	}
}

void App::inputUnbind(WP sender)
{
	InputAction* action = sender->getUserData<InputAction>();
	mBindingAction = action;
	mBindingSender = sender->castType<MyGUI::Button>();

	SDL_Keycode key = action->mICS->getKeyBinding(action->mControl, ICS::Control::INCREASE);
	action->mICS->removeKeyBinding(key);

	key = action->mICS->getKeyBinding(action->mControl, ICS::Control::DECREASE);
	action->mICS->removeKeyBinding(key);

	for (int j=0; j < SDL_NumJoysticks(); ++j)
	{
		int axis = action->mICS->getJoystickAxisBinding(action->mControl, j, ICS::Control::INCREASE);
		if (axis != ICS::InputControlSystem::UNASSIGNED)
			action->mICS->removeJoystickAxisBinding(j, axis);
		
		int btn = action->mICS->getJoystickButtonBinding(action->mControl, j, ICS::Control::INCREASE);
		if (btn != ICS::InputControlSystem::UNASSIGNED)
			action->mICS->removeJoystickButtonBinding(j, btn);
	}
	UpdateInputButton(mBindingSender, *action);
}


///  edit details
//-------------------------------------------------------------------------------
void App::inputDetailBtn(WP sender)
{
	const InputAction& action = *sender->getUserData<InputAction>();
	if (txtInpDetail)  txtInpDetail->setCaptionWithReplacing(TR("#{InputDetailsFor}")+":  #{InputMap"+action.mName+"}");

	mBindingAction = sender->getUserData<InputAction>();
	if (panInputDetail)  panInputDetail->setVisible(false);

	Button* btnInputInv = mGUI->findWidget<Button>("InputInv");
	if (btnInputInv)  btnInputInv->setStateSelected(mBindingAction->mControl->getInverted());
	if (edInputIncrease)  edInputIncrease->setCaption(toStr(action.mControl->getStepSize() * action.mControl->getStepsPerSeconds()));
}

void App::editInput(MyGUI::EditPtr ed)
{
	Real vInc = s2r(edInputIncrease->getCaption());
	mBindingAction->mControl->setStepSize(0.1);
	mBindingAction->mControl->setStepsPerSeconds(vInc*10);
}

void App::btnInputInv(WP wp)
{
	ButtonPtr chk = wp->castType<MyGUI::Button>();
	chk->setStateSelected(!chk->getStateSelected());
	mBindingAction->mControl->setInverted(chk->getStateSelected());
}

void App::chkOneAxis(WP wp)
{
	int id=0;  if (!TabInputId(&id))  return;
	ButtonPtr chk = wp->castType<MyGUI::Button>();
	bool b = !mInputCtrlPlayer[id]->mbOneAxisThrottleBrake;
	mInputCtrlPlayer[id]->mbOneAxisThrottleBrake = b;
    chk->setStateSelected(b);
}

void App::tabInputChg(MyGUI::TabPtr tab, size_t val)
{
	int id=0;  bool vis = TabInputId(&id);
	chOneAxis->setVisible(vis);
	//txtInpDetail;  panInputDetail;
	//edInputIncrease;
	if (vis)
	{
		bool b = mInputCtrlPlayer[id]->mbOneAxisThrottleBrake;
		chOneAxis->setStateSelected(b);
	}
}

//  returns player id 0..3, false if not player tab
bool App::TabInputId(int* pId)
{
	if (!tabInput)  return false;
	int id = tabInput->getIndexSelected();  if (id == 0)  return false;
	id -= 1;  if (id > 3)  return false;
	*pId = id;  return true;
}

void App::comboInputKeyAllPreset(MyGUI::ComboBoxPtr cmb, size_t val)
{
	if (val == 0)  return;  cmb->setIndexSelected(0);
	int id=0;  if (!TabInputId(&id))  return;

	const int numActs = 6;  // these actions have key emul params (analog)
	int keyActs[numActs] = {A_Boost, A_Brake, A_Flip, A_HandBrake, A_Steering, A_Throttle};
	const Real speeds[3] = {2,3,4};
	Real vInc = speeds[val-1];

	for (int i=0; i < numActs; ++i)
	{
		ICS::Control* control = mInputCtrlPlayer[id]->getControl(keyActs[i]);

		control->setStepSize(0.1);
		control->setStepsPerSeconds(vInc*10);
	}
	if (edInputIncrease)  edInputIncrease->setCaption(toStr(vInc));
}


///  update input bars vis,dbg
//-------------------------------------------------------------------------------
void App::UpdateInputBars()
{
	TabControl* inputTab = mGUI->findWidget<TabControl>("InputTab");  if (!inputTab)  return;
	TabItem* current = inputTab->getItemSelected();
	for (int i=0; i<current->getChildCount(); ++i)
	{
		MyGUI::ImageBox* image = current->getChildAt(i)->castType<MyGUI::ImageBox>(false);
		if (!image)
			continue;

		const InputAction& action = *image->getUserData<InputAction>();
		float val = action.mICS->getChannel(action.mId)->getValue();

		const int wf = 128, w = 256;  int v = -val * 128, vf = -(val*2-1) * 64, s=512, s0=s/2;

		bool full = action.mType == InputAction::Axis;

		if (full)	image->setImageCoord(IntCoord(std::max(0, std::min(s-wf, vf + s0 -wf/2)), 0, wf, 16));
		else		image->setImageCoord(IntCoord(std::max(0, std::min(s-w, v + s0)), 0, w, 16));
	}
}


void App::mouseAxisBindingDetected(ICS::InputControlSystem* pICS, ICS::Control* control,
	ICS::InputControlSystem::NamedAxis axis, ICS::Control::ControlChangingDirection direction)
{
	// we don't want mouse movement bindings
	return;
}

void App::keyBindingDetected(ICS::InputControlSystem* pICS, ICS::Control* control,
	SDL_Keycode key, ICS::Control::ControlChangingDirection direction)
{
	ICS::DetectingBindingListener::keyBindingDetected(pICS, control, key, direction);
	
	if (direction == ICS::Control::DECREASE)
	{
		pICS->enableDetectingBindingState(control, ICS::Control::INCREASE);
		notifyInputActionBound(false); // second key still needs binding
	}else
		notifyInputActionBound(true); // done
}

void App::mouseButtonBindingDetected(ICS::InputControlSystem* pICS, ICS::Control* control,
	unsigned int button, ICS::Control::ControlChangingDirection direction)
{
	return;
}

void App::joystickAxisBindingDetected(ICS::InputControlSystem* pICS, ICS::Control* control,
	int deviceId, int axis, ICS::Control::ControlChangingDirection direction)
{
	ICS::DetectingBindingListener::joystickAxisBindingDetected(pICS, control, deviceId, axis, ICS::Control::INCREASE);
	std::string s = control->getName();
	//LogO("Control "+s);
	//  inverted throttle and brake by default
	bool inv = s != "2" && s != "5";  // only steering and flip normal
	control->setInverted(inv);
	notifyInputActionBound(true);
}

void App::joystickButtonBindingDetected(ICS::InputControlSystem* pICS, ICS::Control* control,
	int deviceId, unsigned int button, ICS::Control::ControlChangingDirection direction)
{
	// 2-sided axis can't be bound with a JS button
	if (mBindingAction->mType == InputAction::Axis)
		return;

	ICS::DetectingBindingListener::joystickButtonBindingDetected(pICS, control, deviceId, button, ICS::Control::INCREASE);
	notifyInputActionBound(true);
}

void App::joystickPOVBindingDetected(ICS::InputControlSystem* pICS, ICS::Control* control,
	int deviceId, int pov,ICS:: InputControlSystem::POVAxis axis, ICS::Control::ControlChangingDirection direction)
{
	return;
}
