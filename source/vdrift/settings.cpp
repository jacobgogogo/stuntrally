#include "pch.h"
#include "settings.h"
#include "../network/protocol.hpp"
#include <stdio.h>


void SETTINGS::Load(std::string sfile)
{
	CONFIGFILE c;  c.Load(sfile);
	Serialize(false, c);
}
void SETTINGS::Save(std::string sfile)
{
	if (net_local_plr > 0)  // save only for host for many local games
		return;
	CONFIGFILE c;  c.Load(sfile);  version = SET_VER;
	Serialize(true, c);  c.Write();
}

void SETTINGS::Serialize(bool w, CONFIGFILE & c)
{
	c.bFltFull = false;
	for (int i=0; i < 6; ++i)
	{
		char ss[64];  sprintf(ss, "car%d.", i+1);   std::string s = ss;
		if (i < 4)
		{	Param(c,w, s+"car", gui.car[i]);			Param(c,w, s+"camera", cam_view[i]);
		}
		Param(c,w, s+"clr_hue", gui.car_hue[i]);
		Param(c,w, s+"clr_sat", gui.car_sat[i]);		Param(c,w, s+"clr_val", gui.car_val[i]);
		Param(c,w, s+"clr_gloss", gui.car_gloss[i]);	Param(c,w, s+"clr_refl", gui.car_refl[i]);
	}
	//todo: this for all 4 cars..
	Param(c,w, "car1.autotrans", autoshift);
	Param(c,w, "car1.autorear", autorear);		Param(c,w, "car1.autorear_inv", rear_inv);
	for (int i=0; i <= 1; ++i)
	{	std::string s = i==1 ? "A":"";
		Param(c,w, "car1.abs"+s, abs[i]);		Param(c,w, "car1.tcs"+s, tcs[i]);
		Param(c,w, "car1.sss_effect"+s, sss_effect[i]);
		Param(c,w, "car1.sss_velfactor"+s, sss_velfactor[i]);
		Param(c,w, "car1.steer_range"+s, steer_range[i]);
	}
	Param(c,w, "car1.steer_sim_easy", steer_sim_easy);
	Param(c,w, "car1.steer_sim_normal", steer_sim_normal);

	//  game
	Param(c,w, "game.start_in_main", startInMain);
	Param(c,w, "game.in_menu", inMenu);				Param(c,w, "game.in_main", isMain);
	Param(c,w, "game.pre_time", gui.pre_time);			Param(c,w, "game.chall_num", gui.chall_num);  //rem-
	Param(c,w, "game.champ_num", gui.champ_num);		Param(c,w, "game.champ_rev", gui.champ_rev);
	Param(c,w, "game.boost_type", gui.boost_type);		Param(c,w, "game.flip_type", gui.flip_type);
	Param(c,w, "game.boost_power", gui.boost_power);	Param(c,w, "game.damage_type", gui.damage_type);
	Param(c,w, "game.rewind_type", gui.rewind_type);
	Param(c,w, "game.collis_cars", gui.collis_cars);	Param(c,w, "game.collis_veget", gui.collis_veget);
	Param(c,w, "game.collis_roadw", gui.collis_roadw);	Param(c,w, "game.dyn_objects", gui.dyn_objects);
	Param(c,w, "game.track", gui.track);				Param(c,w, "game.track_user", gui.track_user);
	Param(c,w, "game.trk_reverse", gui.trackreverse);	Param(c,w, "game.sim_mode", gui.sim_mode);
	Param(c,w, "game.local_players", gui.local_players); Param(c,w, "game.num_laps", gui.num_laps);
	Param(c,w, "game.start_order", gui.start_order);	Param(c,w, "game.split_vertically", split_vertically);
	
	//  joystick
	Param(c,w, "joystick.ff_device", ff_device);		Param(c,w, "joystick.ff_gain", ff_gain);
	Param(c,w, "joystick.ff_invert", ff_invert);

	//  hud
	Param(c,w, "hud_show.fps", show_fps);				Param(c,w, "hud_show.mph", show_mph);
	Param(c,w, "hud_show.gauges", show_gauges);			Param(c,w, "hud_show.show_digits", show_digits);
	Param(c,w, "hud_show.trackmap", trackmap);			Param(c,w, "hud_show.times", show_times);
	Param(c,w, "hud_show.caminfo", show_cam);			Param(c,w, "hud_show.cam_tilt", cam_tilt);
	Param(c,w, "hud_show.car_dbgtxt", car_dbgtxt);		Param(c,w, "hud_show.show_cardbg", car_dbgbars);
	Param(c,w, "hud_show.car_dbgsurf", car_dbgsurf);
	Param(c,w, "hud_show.car_dbgtxtclr", car_dbgtxtclr); Param(c,w, "hud_show.car_dbgtxtcnt", car_dbgtxtcnt);
	Param(c,w, "hud_show.check_arrow", check_arrow);	Param(c,w, "hud_show.check_beam", check_beam);
	Param(c,w, "hud_show.opponents", show_opponents);	Param(c,w, "hud_show.opplist_sort", opplist_sort);
	Param(c,w, "hud_show.graphs", show_graphs);			Param(c,w, "hud_show.graphs_type", (int&)graphs_type);

	Param(c,w, "gui.tracks_view", tracks_view);
	Param(c,w, "gui.tracks_sort", tracks_sort);		Param(c,w, "gui.tracks_sortup", tracks_sortup);
	Param(c,w, "gui.cars_sort", cars_sort);			Param(c,w, "gui.car_ed_tab", car_ed_tab);
	Param(c,w, "gui.champ_tab", champ_type);		Param(c,w, "gui.tut_tab", tut_type);
	Param(c,w, "gui.champ_info", champ_info);

	Param(c,w, "hud_size.gauges", size_gauges);			Param(c,w, "hud_size.minimap", size_minimap);
	Param(c,w, "hud_size.mini_zoom", zoom_minimap);		Param(c,w, "hud_size.mini_zoomed", mini_zoomed);
	Param(c,w, "hud_size.mini_rotated", mini_rotated);	Param(c,w, "hud_size.mini_terrain", mini_terrain);
	Param(c,w, "hud_size.mini_border", mini_border);
	Param(c,w, "hud_size.arrow", size_arrow);			Param(c,w, "hud_size.gauges_type", gauges_type);

	//  graphics
	Param(c,w, "graph_detail.anisotropy", anisotropy);		Param(c,w, "graph_detail.view_dist", view_distance);
	Param(c,w, "graph_detail.ter_detail", terdetail);		Param(c,w, "graph_detail.ter_dist", terdist);
	Param(c,w, "graph_detail.road_dist", road_dist);		Param(c,w, "graph_detail.tex_size", tex_size);
	Param(c,w, "graph_detail.ter_mtr", ter_mtr);			Param(c,w, "graph_detail.ter_tripl", ter_tripl);

	Param(c,w, "graph_par.particles", particles);			Param(c,w, "graph_par.trails", trails);
	Param(c,w, "graph_par.particles_len", particles_len);	Param(c,w, "graph_par.trail_len", trails_len);

	Param(c,w, "graph_reflect.skip_frames", refl_skip);		Param(c,w, "graph_reflect.faces_once", refl_faces);
	Param(c,w, "graph_reflect.map_size", refl_size);		Param(c,w, "graph_reflect.dist", refl_dist);
	Param(c,w, "graph_reflect.mode", refl_mode);
	Param(c,w, "graph_reflect.water_reflect", water_reflect); Param(c,w, "graph_reflect.water_refract", water_refract);
	Param(c,w, "graph_reflect.water_rttsize", water_rttsize);
	
	Param(c,w, "graph_shadow.dist", shadow_dist);			Param(c,w, "graph_shadow.size", shadow_size);
	Param(c,w, "graph_shadow.count",shadow_count);			Param(c,w, "graph_shadow.type", (int&)shadow_type);
	Param(c,w, "graph_shadow.shaders", shaders);			Param(c,w, "graph_shadow.lightmap_size", lightmap_size);
	Param(c,w, "graph_shadow.filter", shadow_filter);
	Param(c,w, "graph_shadow.shader_mode", shader_mode);

	Param(c,w, "graph_veget.trees", gui.trees);				Param(c,w, "graph_veget.grass", grass);
	Param(c,w, "graph_veget.trees_dist", trees_dist);		Param(c,w, "graph_veget.grass_dist", grass_dist);
	Param(c,w, "graph_veget.use_imposters", use_imposters); Param(c,w, "graph_veget.imposters_only", imposters_only);

	//  misc
	Param(c,w, "misc.autostartgame", autostart);
	Param(c,w, "misc.ogredialog", ogre_dialog);		Param(c,w, "misc.escquit", escquit);
	Param(c,w, "misc.bulletDebug", bltDebug);		Param(c,w, "misc.bulletLines", bltLines);
	Param(c,w, "misc.profilerTxt", profilerTxt);	Param(c,w, "misc.bulletProfilerTxt", bltProfilerTxt);
	Param(c,w, "misc.language", language);			Param(c,w, "misc.loadingback", loadingbackground);
	Param(c,w, "misc.version", version);			Param(c,w, "misc.dev_keys", dev_keys);
	Param(c,w, "misc.x11_hwmouse", x11_hwmouse);	Param(c,w, "misc.capture_mouse", capture_mouse);

	Param(c,w, "network.nickname", nickname);		Param(c,w, "network.master_server_address", master_server_address);
	Param(c,w, "network.local_port", local_port);	Param(c,w, "network.master_server_port", master_server_port);
	Param(c,w, "network.game_name", netGameName);

	Param(c,w, "replay.rec", rpl_rec);				Param(c,w, "replay.ghost", rpl_ghost);
	Param(c,w, "replay.bestonly", rpl_bestonly);	Param(c,w, "replay.trackghost", rpl_trackghost);
	Param(c,w, "replay.listview", rpl_listview);	Param(c,w, "replay.listghosts", rpl_listghosts);
	Param(c,w, "replay.ghostpar", rpl_ghostpar);	Param(c,w, "replay.ghostother", rpl_ghostother);
	Param(c,w, "replay.num_views", rpl_numViews);	Param(c,w, "replay.ghostrewind", rpl_ghostrewind);
	
	Param(c,w, "sim.game_freq", game_fq);			Param(c,w, "sim.multi_thr", multi_thr);
	Param(c,w, "sim.bullet_freq", blt_fq);			Param(c,w, "sim.bullet_iter", blt_iter);
	Param(c,w, "sim.dynamics_iter", dyn_iter);		Param(c,w, "sim.thread_sleep", thread_sleep);
	Param(c,w, "sim.perf_speed", perf_speed);
	
	Param(c,w, "sound.volume", vol_master);			Param(c,w, "sound.vol_engine", vol_engine);
	Param(c,w, "sound.vol_tires", vol_tires);		Param(c,w, "sound.vol_env", vol_env);
	Param(c,w, "sound.vol_susp", vol_susp);
	Param(c,w, "sound.vol_fl_splash", vol_fl_splash);	Param(c,w, "sound.vol_fl_cont", vol_fl_cont);
	Param(c,w, "sound.vol_car_crash", vol_car_crash);	Param(c,w, "sound.vol_car_scrap", vol_car_scrap);

	//  effects
	Param(c,w, "video_eff.all_effects", all_effects);
	Param(c,w, "video_eff.bloom", bloom);				Param(c,w, "video_eff.bloomintensity", bloomintensity);
	Param(c,w, "video_eff.bloomorig", bloomorig);		
	Param(c,w, "video_eff.motionblur", motionblur);		Param(c,w, "video_eff.motionblurintensity", motionblurintensity);
	Param(c,w, "video_eff.ssao", ssao);					Param(c,w, "video_eff.softparticles", softparticles);
	Param(c,w, "video_eff.godrays", godrays);
	Param(c,w, "video_eff.dof", dof);
	Param(c,w, "video_eff.dof_focus", depthOfFieldFocus);	Param(c,w, "video_eff.dof_far", depthOfFieldFar);
	Param(c,w, "video_eff.boost_fov", boost_fov);
	//  effects hdr
	Param(c,w, "video_eff.hdr", hdr);					Param(c,w, "video_eff.hdr_p1", hdrParam1);
	Param(c,w, "video_eff.hdr_p2", hdrParam2);			Param(c,w, "video_eff.hdr_p3", hdrParam3);
	Param(c,w, "video_eff.hdr_bloomint", hdrbloomint);	Param(c,w, "video_eff.hdr_bloomorig", hdrbloomorig);
	Param(c,w, "video_eff.hdr_adaptationScale", hdrAdaptationScale);
	Param(c,w, "video_eff.hdr_vignettingRadius", vignettingRadius);  Param(c,w, "video_eff.hdr_vignettingDarkness", vignettingDarkness);
	
	//  video
	Param(c,w, "video.windowx", windowx);			Param(c,w, "video.windowy", windowy);
	Param(c,w, "video.fullscreen", fullscreen);		Param(c,w, "video.vsync", vsync);
	Param(c,w, "video.fsaa", fsaa);
	Param(c,w, "video.buffer", buffer);				Param(c,w, "video.rendersystem", rendersystem);
	Param(c,w, "video.render_not_active", renderNotActive);
	
	// not in gui-
	Param(c,w, "misc.boostFromExhaust", boostFromExhaust);
}

SETTINGS::SETTINGS()   ///  Defaults
	:version(100)  // old
	//  hud
	,show_fps(1), show_gauges(1), trackmap(1)
	,show_cam(1), show_times(0), show_digits(1)
	,show_opponents(1), opplist_sort(true), cam_tilt(1)
	,car_dbgtxt(0), car_dbgbars(0), car_dbgsurf(0), show_graphs(0)
	,car_dbgtxtclr(0), car_dbgtxtcnt(0)
	,size_gauges(0.18), size_minimap(0.2), zoom_minimap(1.0)
	,mini_zoomed(0), mini_rotated(1), mini_terrain(0), mini_border(1)
	,check_arrow(0),size_arrow(0.2), check_beam(1),  gauges_type(1),graphs_type(Gh_Fps)
	,tracks_view(0), tracks_sort(0), tracks_sortup(1), cars_sort(1), cars_sortup(1)
	,champ_type(0),tut_type(0), car_ed_tab(0), champ_info(1)
	//  graphics
	,anisotropy(4),	view_distance(2000), bFog(0)
	,terdetail(2), terdist(100), road_dist(1.0), tex_size(1), ter_mtr(2), ter_tripl(0), shaders(0.5)
	,refl_skip(200), refl_faces(1), refl_size(0), refl_dist(500.f), refl_mode(1)
	,water_reflect(0), water_refract(0), water_rttsize(0)
	,shadow_type(Sh_Depth), shadow_size(2), shadow_count(3), shadow_dist(3000)
	,shadow_filter(1), lightmap_size(0)
	,grass(1.f), trees_dist(1.f), grass_dist(1.f), use_imposters(true), imposters_only(false)
	,particles(true), trails(true), particles_len(1.f), trails_len(1.f), boost_fov(true)
	//  car
	,autoshift(1), autorear(1), rear_inv(1), show_mph(0)
	//  misc
	,isMain(1), startInMain(1), inMenu(0), rpl_rec(0), dev_keys(0)
	,split_vertically(true), language("") // "" = autodetect lang
	//  joystick
	,ff_device("/dev/input/event0"), ff_gain(1.0), ff_invert(false)
	//  misc
	,autostart(0), ogre_dialog(0), escquit(0)
	,bltDebug(0), bltLines(1),  bltProfilerTxt(0), profilerTxt(0)
	,loadingbackground(true)
	,capture_mouse(false), x11_hwmouse(false)
	//  network
	,nickname("Player"), netGameName("Default Game")
	,master_server_address("localhost")
	,master_server_port(protocol::DEFAULT_PORT)
	,local_port(protocol::DEFAULT_PORT)
	//  replay
	,rpl_ghost(1), rpl_bestonly(1), rpl_trackghost(1)
	,rpl_ghostpar(0), rpl_ghostrewind(1), rpl_ghostother(1)
	,rpl_listview(0), rpl_listghosts(0), rpl_numViews(4)
	//  sim
	,game_fq(82.f), blt_fq(160.f), blt_iter(24), dyn_iter(30)
	,multi_thr(0), thread_sleep(5), perf_speed(100000)
	//  sound
	,vol_master(1.f), vol_engine(0.6f), vol_tires(1.f), vol_env(1.f), vol_susp(1.f)
	,vol_fl_splash(1.f),vol_fl_cont(1.f), vol_car_crash(1.f),vol_car_scrap(1.f)
	//  video
	,windowx(800), windowy(600)
	,fullscreen(false), vsync(false)
	,rendersystem("OpenGL Rendering Subsystem")
	,buffer("FBO"), fsaa(0)
	//  video eff
	,all_effects(false), godrays(false), filmgrain(false)
	,bloom(false), bloomintensity(0.13), bloomorig(0.9), hdr(false)
	,motionblur(false), motionblurintensity(0.1)
	,depthOfFieldFocus(100), depthOfFieldFar(1000)
	,ssao(false), softparticles(false)
	//  hdr
	,hdrParam1(0.62), hdrParam2(0.10), hdrParam3(0.79)
	,hdrbloomint(0.81), hdrbloomorig(0.34), hdrAdaptationScale(0.51)
	,vignettingRadius(2.85), vignettingDarkness(0.34)
	//  not in gui
	,boostFromExhaust(0), net_local_plr(-1)
	,renderNotActive(false)
	,shader_mode("")
{
	//  track
	gui.track = "J1-T";  gui.track_user = false;  gui.trackreverse = false;
	gui.sim_mode = "easy";

	cam_view.resize(4);
	//  cars
	for (int i=0; i < 6; ++i)
	{	if (i < 4)  {  gui.car[i] = "ES";  cam_view[i] = 9;  }
		gui.car_hue[i] = 0.4f+0.2f*i;  gui.car_sat[i] = 1.f;  gui.car_val[i] = 1.f;
		gui.car_gloss[i] = 0.5f;  gui.car_refl[i] = 1.f;  }

	//  game
	gui.local_players = 1;  gui.num_laps = 2;
	gui.collis_veget = true;  gui.collis_cars = false;
	gui.collis_roadw = false;  gui.dyn_objects = true;
	gui.boost_type = 2;  gui.flip_type = 1;
	gui.boost_power = 1.f;  gui.damage_type = 1;  gui.rewind_type = 1;
	gui.trees = 1.f;
	//
	gui.rpl_rec = 1;  gui.champ_rev = false;  gui.start_order = 0;
	gui.champ_num = -1;  gui.pre_time = 2.f;  gui.chall_num = -1;
	game = gui;

	//  car setup  (update in game-default.cfg)
	abs[0] = 0;  abs[1] = 0;
	tcs[0] = 0;  tcs[1] = 0;
	sss_effect[0] = 0.f;  sss_effect[1] = 0.85f;
	sss_velfactor[0] = 1.f;  sss_velfactor[1] = 1.f;
	steer_range[0] = 1.0;  steer_range[1] = 0.7;
	steer_sim_easy = 0.51;  steer_sim_normal = 0.81;
}

SETTINGS::GameSet::GameSet()
{
	car_hue.resize(6);  car_sat.resize(6);  car_val.resize(6);
	car_gloss.resize(6);  car_refl.resize(6);
	car.resize(4);
}
