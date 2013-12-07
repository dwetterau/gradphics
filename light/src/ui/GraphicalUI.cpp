//
// GraphicalUI.cpp
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#ifndef COMMAND_LINE_ONLY

#include <FL/fl_ask.H>
#include "debuggingView.h"

#include "GraphicalUI.h"
#include "../RayTracer.h"
#include "../fileio/bitmap.h"

bool GraphicalUI::stopTrace = false;
bool GraphicalUI::doneTrace = true;
Fl_Check_Button*	GraphicalUI::m_stButton;

using namespace std;

//------------------------------------- Help Functions --------------------------------------------
GraphicalUI* GraphicalUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (GraphicalUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void GraphicalUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	GraphicalUI* pUI=whoami(o);
	
	static char* lastFile = 0;
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			stopTracing();	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
		pUI->m_debuggingWindow->m_debuggingView->setDirty();

		if( lastFile != 0 && strcmp(newfile, lastFile) != 0 )
			pUI->m_debuggingWindow->m_debuggingView->resetCamera();

		pUI->m_debuggingWindow->redraw();
	}
}

void GraphicalUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	GraphicalUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void GraphicalUI::cb_exit(Fl_Menu_* o, void* v)
{
	GraphicalUI* pUI=whoami(o);

	// terminate the rendering
	stopTracing();

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
	pUI->m_debuggingWindow->hide();
}

void GraphicalUI::cb_exit2(Fl_Widget* o, void* v) 
{
	GraphicalUI* pUI=(GraphicalUI *)(o->user_data());
	
	// terminate the rendering
	stopTracing();

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
	pUI->m_debuggingWindow->hide();
}

void GraphicalUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS384g Fall 2013.");
}

void GraphicalUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());

	// terminate the rendering so we don't get crashes
	stopTracing();

	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
	// Need to call traceSetup before trying to render
	pUI->raytracer->setReady(false);
}

void GraphicalUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_aaSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_nAa=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_cutoffSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->cutoff=double( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_depthLimitSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->depthLimit=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_objectLimitSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->objectLimit=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_cameraUSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_cameraU = double( ((Fl_Slider *)o)->value() ) ;
}
void GraphicalUI::cb_cameraVSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_cameraV = double( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_lfnSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_lf_n = int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_focalSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_focal = double( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_debuggingDisplayCheckButton(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	pUI->m_displayDebuggingInfo = (((Fl_Check_Button*)o)->value() == 1);
	if( pUI->m_displayDebuggingInfo )
		pUI->m_debuggingWindow->show();
	else
		pUI->m_debuggingWindow->hide();
}

void GraphicalUI::cb_acceleratedButton(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
  pUI->setAccelerated(((Fl_Check_Button*)o)->value() == 1);
}

void GraphicalUI::cb_stocButton(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
  pUI->setStoc(((Fl_Check_Button*)o)->value() == 1);
}

void GraphicalUI::cb_uvButton(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
  pUI->setUVInterp(((Fl_Check_Button*)o)->value() == 1);
  if (((Fl_Check_Button*)o)->value() == 1) {
    m_stButton->activate();
  } else {
    pUI->setSTInterp(false);
    m_stButton->clear();
    m_stButton->deactivate();
  }
}

void GraphicalUI::cb_stButton(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
  pUI->setSTInterp(((Fl_Check_Button*)o)->value() == 1);
}

void GraphicalUI::cb_useDOFButton(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
  pUI->setUseDOF(((Fl_Check_Button*)o)->value() == 1);
}

void GraphicalUI::cb_cmButton(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
  pUI->setCubeMap(((Fl_Check_Button*)o)->value() == 1);
}

void GraphicalUI::cb_xpName(Fl_Widget* o, void* v) {
  GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	char* newfile = fl_file_chooser("Choose +X file", "*.{png,bmp}", NULL );
	if (newfile != NULL) {
    pUI->setXPName(newfile);
  }
}

void GraphicalUI::cb_xnName(Fl_Widget* o, void* v) {
  GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	char* newfile = fl_file_chooser("Choose -X file", "*.{png,bmp}", NULL );
	if (newfile != NULL) {
    pUI->setXNName(newfile);
  }
}

void GraphicalUI::cb_ypName(Fl_Widget* o, void* v) {
  GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	char* newfile = fl_file_chooser("Choose +Y file", "*.{png,bmp}", NULL );
	if (newfile != NULL) {
    pUI->setYPName(newfile);
  }
}

void GraphicalUI::cb_ynName(Fl_Widget* o, void* v) {
  GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	char* newfile = fl_file_chooser("Choose -Y file", "*.{png,bmp}", NULL );
	if (newfile != NULL) {
    pUI->setYNName(newfile);
  }
}

void GraphicalUI::cb_zpName(Fl_Widget* o, void* v) {
  GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	char* newfile = fl_file_chooser("Choose +Z file", "*.{png,bmp}", NULL );
	if (newfile != NULL) {
    pUI->setZPName(newfile);
  }
}

void GraphicalUI::cb_znName(Fl_Widget* o, void* v) {
  GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	char* newfile = fl_file_chooser("Choose -Z file", "*.{png,bmp}", NULL );
	if (newfile != NULL) {
    pUI->setZNName(newfile);
  }
}

void GraphicalUI::cb_renderLightfield(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
    char* newfile = fl_file_chooser("Open Lightfield?", "*.lf", NULL );

	if (newfile != NULL) {
        LIGHTFIELD_HEADER header;
        unsigned char* bigbuf = readLightfield(newfile, &header);
        int w = header.width;
        int h = header.height;
        pUI->m_lfWindow->resizeWindow( w, h );
        pUI->m_lfWindow->setHeader(&header);
        pUI->m_lfWindow->setBuffer(bigbuf);
        pUI->m_lfWindow->init();
        pUI->m_lfWindow->show();
    }

}

void GraphicalUI::cb_reload(Fl_Widget* o, void* v) {
	GraphicalUI* pUI=((GraphicalUI*)(o->user_data()));
  if (pUI->raytracer->sceneLoaded()) {
    pUI->raytracer->reloadScene();
  }
}

void GraphicalUI::cb_generateLightfield(Fl_Widget* o, void* v) {
	
  GraphicalUI* pUI=((GraphicalUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
	  int lf_n = pUI->getLFN();

    cout << "Generating lightfield with n=" << lf_n << endl;
    

    // Allocate massive buffer for light field
    int bufsize = width * height * lf_n * lf_n * 3;
    unsigned char * big_buffs = new unsigned char [bufsize];
    memset(big_buffs, 0, bufsize);
    cout << "Allocated big buffer of size: " << bufsize << endl;

    for (int r = 0; r < lf_n; r++) {
      for (int c = 0; c < lf_n; c++) {
        pUI->raytracer->traceSetup(width, height);
        // r and c go from 0 to n, we want them to go from -.5 to .5
        double u = (double(c) / lf_n) - .5;
        double v = (double(r) / lf_n) - .5;
        pUI->raytracer->setEyePos(u, v); 

		    doneTrace = false;
		    stopTrace = false;
       	for (int y=0; y<height; y++) {
       		for (int x=0; x<width; x++) {
       			if (stopTrace) break;
       			pUI->raytracer->tracePixel( x, y );
       		}

       		if (stopTrace) {
            cout << "trace aborted..." << endl;
          }
             //cout << "(%d%%) %s", (int)((double)y / (double)height * 100.0) << endl;
       	}
        cout << "finished rendering image: " << (r * lf_n) + c << " of: " << (lf_n * lf_n) << endl;
       	doneTrace=true;
       	stopTrace=false;
        int image_size = width * height * 3;
        unsigned char * tempPointer = &big_buffs[(r * (image_size * lf_n)) + (c * image_size)];
        // copy the raytracer's buffer back to our buffer
        pUI->raytracer->copyBufferTo(tempPointer, image_size);
      }
    }
    // write the buffer to disk
    char* savefile = fl_file_chooser("Save Lightfield data?", "*.lf", "scene.lf" );
	  if (savefile != NULL) {
      // write out the bmp
      LIGHTFIELD_HEADER header = pUI->raytracer->getLightfieldHeader();
      header.num_pictures = lf_n;
      header.width = width;
      header.height = height;
      writeLightfield(savefile, &header, big_buffs);
      cout << "finished writing lf " << header.v1 << endl;
    }
	}
}

void GraphicalUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	GraphicalUI* pUI=((GraphicalUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

    // LIGHTFIELD ----------------------------------
    double u = pUI->getCameraU();
    double v = pUI->getCameraV();
    std::cout << "about to set the eye position" << std::endl;
    pUI->raytracer->setEyePos(u, v); 


    // LIGHTFIELD ----------------------------------

		doneTrace = false;
		stopTrace = false;
		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (stopTrace) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					// refresh
					pUI->m_traceGlWindow->refresh();
					Fl::check();

					if (Fl::damage()) {
						Fl::flush();
					}
				}
				pUI->raytracer->tracePixel( x, y );
				pUI->m_debuggingWindow->m_debuggingView->setDirty();
			}
			if (stopTrace) break;

			// refresh at end of row
			pUI->m_traceGlWindow->refresh();
			Fl::check();

			if (Fl::damage()) {
				Fl::flush();
			}

			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		doneTrace=true;
		stopTrace=false;

		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void GraphicalUI::cb_stop(Fl_Widget* o, void* v)
{
	stopTrace = true;
}

int GraphicalUI::run()
{
	Fl::visual(FL_DOUBLE|FL_INDEX);

	m_mainWindow->show();

	return Fl::run();
}

void GraphicalUI::alert( const string& msg )
{
	fl_alert( "%s", msg.c_str() );
}

void GraphicalUI::setRayTracer(RayTracer *tracer)
{
	TraceUI::setRayTracer( tracer );
	m_traceGlWindow->setRayTracer(tracer);
	m_debuggingWindow->m_debuggingView->setRayTracer(tracer);
}

// menu definition
Fl_Menu_Item GraphicalUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)GraphicalUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)GraphicalUI::cb_save_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)GraphicalUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)GraphicalUI::cb_about },
		{ 0 },

	{ 0 }
};

void GraphicalUI::stopTracing()
{
	if( stopTrace ) return;			// Only one person can be waiting at a time

	stopTrace = true;

	// Wait for the trace to finish (simple synchronization)
	while( !doneTrace )	Fl::wait();
}

GraphicalUI::GraphicalUI() {
	// init.

	m_mainWindow = new Fl_Window(100, 40, 350, 620, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 320, 25);
		m_menubar->menu(menuitems);

		// install depth slider
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install size slider
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

   	m_aaSlider = new Fl_Value_Slider(10, 80, 180, 20, "AA (n x n)");
		m_aaSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_aaSlider->type(FL_HOR_NICE_SLIDER);
        m_aaSlider->labelfont(FL_COURIER);
        m_aaSlider->labelsize(12);
		m_aaSlider->minimum(1);
		m_aaSlider->maximum(10);
		m_aaSlider->step(1);
		m_aaSlider->value(m_nAa);
		m_aaSlider->align(FL_ALIGN_RIGHT);
		m_aaSlider->callback(cb_aaSlides);

 

		// set up debugging display checkbox
        m_debuggingDisplayCheckButton = new Fl_Check_Button(5, 410, 180, 20, "Debugging display");
		m_debuggingDisplayCheckButton->user_data((void*)(this));
		m_debuggingDisplayCheckButton->callback(cb_debuggingDisplayCheckButton);
		m_debuggingDisplayCheckButton->value(m_displayDebuggingInfo);

    m_stocButton = new Fl_Check_Button(5, 105, 180, 20, "Stochastic AA (uses same n x n total)");
		m_stocButton->user_data((void*)(this));
		m_stocButton->callback(cb_stocButton);
		m_stocButton->value(m_stoc);
    
    // set up kd tree checkbox
    m_acceleratedButton = new Fl_Check_Button(5, 130, 180, 20, "Use KD Tree");
		m_acceleratedButton->user_data((void*)(this));
		m_acceleratedButton->callback(cb_acceleratedButton);
		m_acceleratedButton->value(m_accelerated);

   	m_depthLimitSlider = new Fl_Value_Slider(10, 155, 180, 20, "Tree depth limit");
		m_depthLimitSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthLimitSlider->type(FL_HOR_NICE_SLIDER);
        m_depthLimitSlider->labelfont(FL_COURIER);
        m_depthLimitSlider->labelsize(12);
		m_depthLimitSlider->minimum(1);
		m_depthLimitSlider->maximum(50);
		m_depthLimitSlider->step(1);
		m_depthLimitSlider->value(depthLimit);
		m_depthLimitSlider->align(FL_ALIGN_RIGHT);
		m_depthLimitSlider->callback(cb_depthLimitSlides);

   	m_objectLimitSlider = new Fl_Value_Slider(10, 180, 180, 20, "Tree leaf size limit");
		m_objectLimitSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_objectLimitSlider->type(FL_HOR_NICE_SLIDER);
        m_objectLimitSlider->labelfont(FL_COURIER);
        m_objectLimitSlider->labelsize(12);
		m_objectLimitSlider->minimum(1);
		m_objectLimitSlider->maximum(50);
		m_objectLimitSlider->step(1);
		m_objectLimitSlider->value(objectLimit);
		m_objectLimitSlider->align(FL_ALIGN_RIGHT);
		m_objectLimitSlider->callback(cb_objectLimitSlides);

    m_cmButton = new Fl_Check_Button(5, 210, 180, 20, "Render with cube map");
		m_cmButton->user_data((void*)(this));
		m_cmButton->callback(cb_cmButton);
		m_cmButton->value(m_cm);
   
    // set up "XP file" button
		m_loadXPButton = new Fl_Button(5, 235, 180, 25, "&select +x texture file");
		m_loadXPButton->user_data((void*)(this));
		m_loadXPButton->callback(cb_xpName);
    // set up "XN file" button
		m_loadXNButton = new Fl_Button(5, 260, 180, 25, "&select -x texture file");
		m_loadXNButton->user_data((void*)(this));
		m_loadXNButton->callback(cb_xnName);
    // set up "YP file" button
		m_loadYPButton = new Fl_Button(5, 285, 180, 25, "&select +y texture file");
		m_loadYPButton->user_data((void*)(this));
		m_loadYPButton->callback(cb_ypName);
    // set up "YN file" button
		m_loadYNButton = new Fl_Button(5, 310, 180, 25, "&select -y texture file");
		m_loadYNButton->user_data((void*)(this));
		m_loadYNButton->callback(cb_ynName);
    // set up "ZP file" button
		m_loadZPButton = new Fl_Button(5, 335, 180, 25, "&select +z texture file");
		m_loadZPButton->user_data((void*)(this));
		m_loadZPButton->callback(cb_zpName);
	  // set up "ZN file" button
		m_loadZNButton = new Fl_Button(5, 360, 180, 25, "&select -z texture file");
		m_loadZNButton->user_data((void*)(this));
		m_loadZNButton->callback(cb_znName);	
    
    // set up "render" button
		m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

    // set up "reload" button
		m_reloadButton = new Fl_Button(200, 360, 130, 25, "&Update Scene");
		m_reloadButton->user_data((void*)(this));
		m_reloadButton->callback(cb_reload);

    // set up "render Lightfield" button
		m_generateLightfield = new Fl_Button(175, 515, 150, 25, "Render Lightfield");
		m_generateLightfield->user_data((void*)(this));
		m_generateLightfield->callback(cb_renderLightfield);

    // set up "generate Lightfield" button
		m_generateLightfield = new Fl_Button(5, 515, 150, 25, "Generate Lightfield");
		m_generateLightfield->user_data((void*)(this));
		m_generateLightfield->callback(cb_generateLightfield);

   	m_cutoffSlider = new Fl_Value_Slider(10, 385, 180, 20, "Adaptive Threshold");
		m_cutoffSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_cutoffSlider->type(FL_HOR_NICE_SLIDER);
        m_cutoffSlider->labelfont(FL_COURIER);
        m_cutoffSlider->labelsize(12);
		m_cutoffSlider->minimum(0.0);
		m_cutoffSlider->maximum(1.0);
		m_cutoffSlider->step(.001);
		m_cutoffSlider->value(cutoff);
		m_cutoffSlider->align(FL_ALIGN_RIGHT);
		m_cutoffSlider->callback(cb_cutoffSlides);

		// set up "stop" button
		m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

    m_cameraUSlider = new Fl_Value_Slider(10, 450, 180, 20, "Camera U skew");
		m_cameraUSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_cameraUSlider->type(FL_HOR_NICE_SLIDER);
        m_cameraUSlider->labelfont(FL_COURIER);
        m_cameraUSlider->labelsize(12);
		m_cameraUSlider->minimum(-0.5);
		m_cameraUSlider->maximum(0.5);
		m_cameraUSlider->step(.01);
		m_cameraUSlider->value(0.0);
		m_cameraUSlider->align(FL_ALIGN_RIGHT);
		m_cameraUSlider->callback(cb_cameraUSlides);


    m_cameraVSlider = new Fl_Value_Slider(10, 470, 180, 20, "Camera V skew");
		m_cameraVSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_cameraVSlider->type(FL_HOR_NICE_SLIDER);
        m_cameraVSlider->labelfont(FL_COURIER);
        m_cameraVSlider->labelsize(12);
		m_cameraVSlider->minimum(-.5);
		m_cameraVSlider->maximum(.5);
		m_cameraVSlider->step(.01);
		m_cameraVSlider->value(0.0);
		m_cameraVSlider->align(FL_ALIGN_RIGHT);
		m_cameraVSlider->callback(cb_cameraVSlides);

    m_lfnSlider = new Fl_Value_Slider(10, 490, 180, 20, "Lightfield n");
		m_lfnSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_lfnSlider->type(FL_HOR_NICE_SLIDER);
        m_lfnSlider->labelfont(FL_COURIER);
        m_lfnSlider->labelsize(12);
		m_lfnSlider->minimum(1);
		m_lfnSlider->maximum(100);
		m_lfnSlider->step(1);
		m_lfnSlider->value(1);
		m_lfnSlider->align(FL_ALIGN_RIGHT);
		m_lfnSlider->callback(cb_lfnSlides);

    m_uvButton = new Fl_Check_Button(5, 545, 180, 20, "UV Interpolation");
		m_uvButton->user_data((void*)(this));
		m_uvButton->callback(cb_uvButton);
		m_uvButton->value(m_uv);

    m_stButton = new Fl_Check_Button(185, 545, 180, 20, "ST Interpolation");
		m_stButton->user_data((void*)(this));
		m_stButton->callback(cb_stButton);
		m_stButton->value(m_st);
    m_stButton->deactivate();

    m_useDOFButton = new Fl_Check_Button(5, 570, 180, 20, "Use Depth of Field");
		m_useDOFButton->user_data((void*)(this));
		m_useDOFButton->callback(cb_useDOFButton);
		m_useDOFButton->value(m_usedof);

    m_focalSlider = new Fl_Value_Slider(10, 595, 180, 20, "Focal Length");
		m_focalSlider->user_data((void*)(this));
		m_focalSlider->type(FL_HOR_NICE_SLIDER);
        m_focalSlider->labelfont(FL_COURIER);
        m_focalSlider->labelsize(12);
		m_focalSlider->minimum(0.01);
		m_focalSlider->maximum(10.0);
		m_focalSlider->step(0.01);
		m_focalSlider->value(1.0);
		m_focalSlider->align(FL_ALIGN_RIGHT);
		m_focalSlider->callback(cb_focalSlides);

    m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);

    m_lfWindow = new LFWindow(100, 150, m_nSize, m_nSize, "Lightfield Renderer");
    m_lfWindow->end();
    m_lfWindow->resizable(m_lfWindow);

	// debugging view
	m_debuggingWindow = new DebuggingWindow();
}

#endif

