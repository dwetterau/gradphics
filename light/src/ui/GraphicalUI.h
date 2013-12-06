//
// GraphicalUI.h
//
// The header file for the graphical UI
//

#ifndef __GraphicalUI_h__
#define __GraphicalUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/Fl_File_Chooser.H>		// FLTK file chooser

#include "TraceUI.h"
#include "TraceGLWindow.h"
#include "debuggingWindow.h"

class ModelerView;

class GraphicalUI : public TraceUI {
public:
	GraphicalUI();

	int run();

	void		alert( const string& msg );

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_aaSlider;
	Fl_Slider*			m_depthLimitSlider;
	Fl_Slider*			m_objectLimitSlider;
	Fl_Slider*			m_cutoffSlider;
	Fl_Slider*			m_cameraUSlider;
	Fl_Slider*			m_cameraVSlider;
	Fl_Slider*			m_lfnSlider;

	Fl_Check_Button*	m_debuggingDisplayCheckButton;
	Fl_Check_Button*	m_acceleratedButton;
	Fl_Check_Button*	m_stocButton;
	Fl_Check_Button*	m_cmButton;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	Fl_Button*			m_loadXPButton;
	Fl_Button*			m_loadXNButton;
	Fl_Button*			m_loadYPButton;
	Fl_Button*			m_loadYNButton;
	Fl_Button*			m_loadZPButton;
	Fl_Button*			m_loadZNButton;
	Fl_Button*			m_reloadButton;
	Fl_Button*			m_generateLightfield;
	Fl_Button*			m_renderLightfield;

	TraceGLWindow*		m_traceGlWindow;

	DebuggingWindow*	m_debuggingWindow;
	// member functions
	void		setRayTracer(RayTracer *tracer);

	static void stopTracing();
private:

// static class members
	static Fl_Menu_Item menuitems[];

	static GraphicalUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_aaSlides(Fl_Widget* o, void* v);
	static void cb_depthLimitSlides(Fl_Widget* o, void* v);
	static void cb_objectLimitSlides(Fl_Widget* o, void* v);
	static void cb_cutoffSlides(Fl_Widget* o, void* v);
	
  static void cb_cameraUSlides(Fl_Widget* o, void* v);
	static void cb_cameraVSlides(Fl_Widget* o, void* v);
	static void cb_lfnSlides(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_reload(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
	static void cb_debuggingDisplayCheckButton(Fl_Widget* o, void* v);
	static void cb_acceleratedButton(Fl_Widget* o, void* v);
	static void cb_stocButton(Fl_Widget* o, void* v);
	static void cb_cmButton(Fl_Widget* o, void* v);

	static void cb_xpName(Fl_Widget* o, void* v);
	static void cb_xnName(Fl_Widget* o, void* v);
	static void cb_ypName(Fl_Widget* o, void* v);
	static void cb_ynName(Fl_Widget* o, void* v);
	static void cb_zpName(Fl_Widget* o, void* v);
	static void cb_znName(Fl_Widget* o, void* v);

  // Lightfield Button callbacks
	static void cb_renderLightfield(Fl_Widget* o, void* v);
	static void cb_generateLightfield(Fl_Widget* o, void* v);
	
  static bool doneTrace;		// Flag that gets set when the trace is done
	static bool stopTrace;		// Flag that gets set when the trace should be stopped
};

#endif
