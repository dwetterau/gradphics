//
// impressionistUI.cpp
//
// The user interface part for the program.
//


#include <FL/fl_ask.H>

#include <algorithm>

#include <math.h>
#include <string>

#include "impressionistUI.h"
#include "impressionistDoc.h"

/*
//------------------------------ Widget Examples -------------------------------------------------
Here is some example code for all of the widgets that you may need to add to the 
project.  You can copy and paste these into your code and then change them to 
make them look how you want.  Descriptions for all of the widgets here can be found 
in links on the fltk help session page.

//---------Window/Dialog and Menubar-----------------------------------
	
	//----To install a window--------------------------
	Fl_Window* myWindow = new Fl_Window(600, 300, "MyWindow");
		myWindow->user_data((void*)(this));	// record self to be used by static callback functions
		
		// install menu bar
		myMenubar = new Fl_Menu_Bar(0, 0, 600, 25);
		Fl_Menu_Item ImpressionistUI::myMenuItems[] = {
			{ "&File",		0, 0, 0, FL_SUBMENU },
				{ "&Load...",	FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_load },
				{ "&Save...",	FL_ALT + 's', (Fl_Callback *)ImpressionistUI::cb_save }.
				{ "&Quit",			FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_exit },
				{ 0 },
			{ "&Edit",		0, 0, 0, FL_SUBMENU },
				{ "&Copy",FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_copy, (void *)COPY },
				{ "&Cut",	FL_ALT + 'x', (Fl_Callback *)ImpressionistUI::cb_cut, (void *)CUT },
				{ "&Paste",	FL_ALT + 'v', (Fl_Callback *)ImpressionistUI::cb_paste, (void *)PASTE },
				{ 0 },
			{ "&Help",		0, 0, 0, FL_SUBMENU },
				{ "&About",	FL_ALT + 'a', (Fl_Callback *)ImpressionistUI::cb_about },
				{ 0 },
			{ 0 }
		};
		myMenubar->menu(myMenuItems);
    myWindow->end();

	//----The window callback--------------------------
	// One of the callbacks
	void ImpressionistUI::cb_load(Fl_Menu_* o, void* v) 
	{	
		ImpressionistDoc *pDoc=whoami(o)->getDocument();

		char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
		if (newfile != NULL) {
			pDoc->loadImage(newfile);
		}
	}


//------------Slider---------------------------------------

	//----To install a slider--------------------------
	Fl_Value_Slider * mySlider = new Fl_Value_Slider(10, 80, 300, 20, "My Value");
	mySlider->user_data((void*)(this));	// record self to be used by static callback functions
	mySlider->type(FL_HOR_NICE_SLIDER);
    mySlider->labelfont(FL_COURIER);
    mySlider->labelsize(12);
	mySlider->minimum(1);
	mySlider->maximum(40);
	mySlider->step(1);
	mySlider->value(m_nMyValue);
	mySlider->align(FL_ALIGN_RIGHT);
	mySlider->callback(cb_MyValueSlides);

	//----The slider callback--------------------------
	void ImpressionistUI::cb_MyValueSlides(Fl_Widget* o, void* v)
	{
		((ImpressionistUI*)(o->user_data()))->m_nMyValue=int( ((Fl_Slider *)o)->value() ) ;
	}
	

//------------Choice---------------------------------------
	
	//----To install a choice--------------------------
	Fl_Choice * myChoice = new Fl_Choice(50,10,150,25,"&myChoiceLabel");
	myChoice->user_data((void*)(this));	 // record self to be used by static callback functions
	Fl_Menu_Item ImpressionistUI::myChoiceMenu[3+1] = {
	  {"one",FL_ALT+'p', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)ONE},
	  {"two",FL_ALT+'l', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)TWO},
	  {"three",FL_ALT+'c', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)THREE},
	  {0}
	};
	myChoice->menu(myChoiceMenu);
	myChoice->callback(cb_myChoice);
	
	//-----The choice callback-------------------------
	void ImpressionistUI::cb_myChoice(Fl_Widget* o, void* v)
	{
		ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
		ImpressionistDoc* pDoc=pUI->getDocument();

		int type=(int)v;

		pDoc->setMyType(type);
	}


//------------Button---------------------------------------

	//---To install a button---------------------------
	Fl_Button* myButton = new Fl_Button(330,220,50,20,"&myButtonLabel");
	myButton->user_data((void*)(this));   // record self to be used by static callback functions
	myButton->callback(cb_myButton);

	//---The button callback---------------------------
	void ImpressionistUI::cb_myButton(Fl_Widget* o, void* v)
	{
		ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
		ImpressionistDoc* pDoc = pUI->getDocument();
		pDoc->startPainting();
	}


//---------Light Button------------------------------------
	
	//---To install a light button---------------------
	Fl_Light_Button* myLightButton = new Fl_Light_Button(240,10,150,25,"&myLightButtonLabel");
	myLightButton->user_data((void*)(this));   // record self to be used by static callback functions
	myLightButton->callback(cb_myLightButton);

	//---The light button callback---------------------
	void ImpressionistUI::cb_myLightButton(Fl_Widget* o, void* v)
	{
		ImpressionistUI *pUI=((ImpressionistUI*)(o->user_data()));

		if (pUI->myBool==TRUE) pUI->myBool=FALSE;
		else pUI->myBool=TRUE;
	}

//----------Int Input--------------------------------------

    //---To install an int input-----------------------
	Fl_Int_Input* myInput = new Fl_Int_Input(200, 50, 5, 5, "&My Input");
	myInput->user_data((void*)(this));   // record self to be used by static callback functions
	myInput->callback(cb_myInput);

	//---The int input callback------------------------
	void ImpressionistUI::cb_myInput(Fl_Widget* o, void* v)
	{
		((ImpressionistUI*)(o->user_data()))->m_nMyInputValue=int( ((Fl_Int_Input *)o)->value() );
	}

//------------------------------------------------------------------------------------------------
*/

//------------------------------------- Help Functions --------------------------------------------

//------------------------------------------------------------
// This returns the UI, given the menu item.  It provides a
// link from the menu items to the UI
//------------------------------------------------------------
ImpressionistUI* ImpressionistUI::whoami(Fl_Menu_* o)	
{
	return ( (ImpressionistUI*)(o->parent()->user_data()) );
}


//--------------------------------- Callback Functions --------------------------------------------

//------------------------------------------------------------------
// Brings up a file chooser and then loads the chosen image
// This is called by the UI when the load image menu item is chosen
//------------------------------------------------------------------
void ImpressionistUI::cb_load_image(Fl_Menu_* o, void* v) 
{
	ImpressionistDoc *pDoc=whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
	if (newfile != NULL) {
		pDoc->loadImage(newfile);
	}
}


//------------------------------------------------------------------
// Brings up a file chooser and then saves the painted image
// This is called by the UI when the save image menu item is chosen
//------------------------------------------------------------------
void ImpressionistUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	ImpressionistDoc *pDoc=whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Save File?", "*.bmp", "save.bmp" );
	if (newfile != NULL) {
		pDoc->saveImage(newfile);
	}
}

//-------------------------------------------------------------
// Brings up the paint dialog
// This is called by the UI when the brushes menu item
// is chosen
//-------------------------------------------------------------
void ImpressionistUI::cb_brushes(Fl_Menu_* o, void* v) 
{
	whoami(o)->m_brushDialog->show();
}


//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas menu item is chosen
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas(Fl_Menu_* o, void* v)
{
	ImpressionistDoc* pDoc=whoami(o)->getDocument();

	pDoc->clearCanvas();
}

//------------------------------------------------------------
// Causes the Impressionist program to exit
// Called by the UI when the quit menu item is chosen
//------------------------------------------------------------
void ImpressionistUI::cb_exit(Fl_Menu_* o, void* v) 
{
	whoami(o)->m_mainWindow->hide();
	whoami(o)->m_brushDialog->hide();

}



//-----------------------------------------------------------
// Brings up an about dialog box
// Called by the UI when the about menu item is chosen
//-----------------------------------------------------------
void ImpressionistUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("Impressionist FLTK version 1.3.0 for CS 384G, Fall 2013");
}

//------- UI should keep track of the current for all the controls for answering the query from Doc ---------
//-------------------------------------------------------------
// Sets the type of brush to use to the one chosen in the brush 
// choice.  
// Called by the UI when a brush is chosen in the brush choice
//-------------------------------------------------------------
void ImpressionistUI::cb_brushChoice(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc* pDoc=pUI->getDocument();

	//	int type=(int)v;
	long long tmp = reinterpret_cast<long long>(v);
	int type = static_cast<int>(tmp);

	pDoc->setBrushType(type);
}

//-------------------------------------------------------------
// Sets the type of brush to use to the one chosen in the brush 
// choice.  
// Called by the UI when a brush is chosen in the brush choice
//-------------------------------------------------------------
void ImpressionistUI::cb_angleChoice(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc* pDoc=pUI->getDocument();

	//	int type=(int)v;
	long long tmp = reinterpret_cast<long long>(v);
	int type = static_cast<int>(tmp);

	pDoc->setAngleChoice(type);
}

//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas button is pushed
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas_button(Fl_Widget* o, void* v)
{
	ImpressionistDoc * pDoc = ((ImpressionistUI*)(o->user_data()))->getDocument();

	pDoc->clearCanvas();
}

void ImpressionistUI::cb_kernel_source(Fl_Widget* o, void* v)
{
	ImpressionistDoc * pDoc = ((ImpressionistUI*)(o->user_data()))->getDocument();
  ImpressionistUI * ui = ((ImpressionistUI*)(o->user_data())); 
  printf("Applying kernel to image:\n"); 
  ui->printKernel();
  int** kernel = ui->readKernel(); 
  pDoc->applyFilter(
    pDoc->m_ucBitmap, 
    pDoc->m_nWidth, 
    pDoc->m_nHeight,
    pDoc->m_ucPainting,
    kernel,
    ui->m_nKCol, ui->m_nKRow,
    ui->divisor, ui->offset);
  ui->m_paintView->refresh();
  printf("Finished!\n");
}

void ImpressionistUI::cb_kernel_paint(Fl_Widget* o, void* v)
{
	ImpressionistDoc * pDoc = ((ImpressionistUI*)(o->user_data()))->getDocument();
  ImpressionistUI * ui = ((ImpressionistUI*)(o->user_data())); 
  printf("Applying kernel to painting:\n"); 
  ui->printKernel();
  // Create new image buffer
  unsigned char * newPaint = new unsigned char [
          pDoc->m_nPaintWidth * pDoc->m_nPaintHeight * 3];
  memset(newPaint, 0, pDoc->m_nPaintWidth * pDoc->m_nPaintHeight * 3);
  int** kernel = ui->readKernel(); 
  pDoc->applyFilter(
    pDoc->m_ucPainting, 
    pDoc->m_nWidth, 
    pDoc->m_nHeight,
    newPaint,
    kernel,
    ui->m_nKCol, ui->m_nKRow,
    ui->divisor, ui->offset);
  delete [] pDoc->m_ucPainting;
  pDoc->m_ucPainting = newPaint;
  ui->m_paintView->refresh();
  printf("Finished!\n");
}

//-----------------------------------------------------------
// Updates the brush size to use from the value of the size
// slider
// Called by the UI when the size slider is moved
//-----------------------------------------------------------
void ImpressionistUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
}

void ImpressionistUI::cb_alphaSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nAlpha=float( ((Fl_Slider *)o)->value() ) ;
}

void ImpressionistUI::cb_lineThicknessSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nLineThickness=int( ((Fl_Slider *)o)->value() ) ;
}

void ImpressionistUI::cb_angleSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nAngle=int( ((Fl_Slider *)o)->value() ) ;
}

void ImpressionistUI::cb_KRowSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->setKRow(int(((Fl_Slider *)o)->value()));
}
void ImpressionistUI::cb_KColSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->setKCol(int(((Fl_Slider *)o)->value()));
}


//---------------------------------- per instance functions --------------------------------------

//------------------------------------------------
// Return the ImpressionistDoc used
//------------------------------------------------
ImpressionistDoc* ImpressionistUI::getDocument()
{
	return m_pDoc;
}

//------------------------------------------------
// Draw the main window
//------------------------------------------------
void ImpressionistUI::show() {
	m_mainWindow->show();
	m_paintView->show();
	m_origView->show();
}

//------------------------------------------------
// Change the paint and original window sizes to 
// w by h
//------------------------------------------------
void ImpressionistUI::resize_windows(int w, int h) {
	m_paintView->size(w,h);
	m_origView->size(w,h);
}

//------------------------------------------------ 
// Set the ImpressionistDoc used by the UI to 
// communicate with the brushes 
//------------------------------------------------
void ImpressionistUI::setDocument(ImpressionistDoc* doc)
{
	m_pDoc = doc;

	m_origView->m_pDoc = doc;
	m_paintView->m_pDoc = doc;
}

//------------------------------------------------
// Return the brush size
//------------------------------------------------
int ImpressionistUI::getSize()
{
	return m_nSize;
}

//-------------------------------------------------
// Set the brush size
//-------------------------------------------------
void ImpressionistUI::setSize( int size )
{
	m_nSize=size;

	if (size<=100) 
		m_BrushSizeSlider->value(m_nSize);
}

//------------------------------------------------
// Return the alpha
//------------------------------------------------
float ImpressionistUI::getAlpha()
{
	return m_nAlpha;
}

//-------------------------------------------------
// Set the alpha
//-------------------------------------------------
void ImpressionistUI::setAlpha(float alpha)
{
	m_nAlpha=alpha;

	if (alpha<=1) 
		m_AlphaSlider->value(m_nAlpha);
}

//------------------------------------------------
// Return the line thickness
//------------------------------------------------
int ImpressionistUI::getLineThickness()
{
	return m_nLineThickness;
}

//-------------------------------------------------
// Set the line thickness
//-------------------------------------------------
void ImpressionistUI::setLineThickness( int thickness )
{
	m_nLineThickness=thickness;

	if (thickness<=10) 
		m_LineThicknessSlider->value(m_nLineThickness);
}

//------------------------------------------------
// Return the line angle
//------------------------------------------------
int ImpressionistUI::getAngle()
{
	return m_nAngle;
}

//-------------------------------------------------
// Set the line angle
//-------------------------------------------------
void ImpressionistUI::setAngle( int angle )
{
	m_nAngle = angle;

	if (angle <= 360) 
		m_AngleSlider->value(m_nAngle);
}

void ImpressionistUI::setKRow( int num )
{
  if (num % 2 == 0) {
    num += 1;
  }
	bool diff = m_nKRow != num;
  m_nKRow = num;

	if (num <= 9) {
		m_KRowSlider->value(m_nKRow);
  }
  if (diff) {
    resetKernel();
  }
}
int ImpressionistUI::getKRow()
{
	return m_nKRow;
}
void ImpressionistUI::setKCol( int num )
{
	if (num % 2 == 0) {
    num += 1;
  }
	bool diff = m_nKCol != num;
  m_nKCol = num;
	if (num <= 9) {
		m_KColSlider->value(m_nKCol);
  }
  if (diff) {
    resetKernel();
  }
}
int ImpressionistUI::getKCol()
{
	return m_nKCol;
}

void ImpressionistUI::resetKernel() {
  m_brushDialog->redraw();
  int rows = m_nKRow;
  int cols = m_nKCol;
  for (int r = 0; r < 9; r++) {
    for (int c = 0; c < 9; c++) {
      kernel_inputs[r][c]->value("0");
      if (r >= rows || c >= cols) {
        kernel_inputs[r][c]->hide();
      } else {
        kernel_inputs[r][c]->show();
      }
    }
  }
}

int** ImpressionistUI::readKernel() {
  int rows = m_nKRow;
  int cols = m_nKCol;
  int** toReturn = new int* [rows];
  for (int r = 0; r < rows; r++) {
    toReturn[r] = new int [cols];
    for (int c = 0; c < cols; c++) {
      std::string s = kernel_inputs[r][c]->value();
      sscanf(s.c_str(), "%d", &toReturn[r][c]);
    }
  }
  std::string s1 = m_KDivisor->value();
  sscanf(s1.c_str(), "%lf", &divisor);
   std::string s2 = m_KOffset->value();
  sscanf(s2.c_str(), "%lf", &offset); 
  return toReturn;
}

void ImpressionistUI::printKernel() {
  int rows = m_nKRow;
  int cols = m_nKCol;
  int ** kernel = readKernel();
  printf("Kernel: %d x %d\n", rows, cols);
  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      printf("%d ", kernel[r][c]);
    }
    printf("\n");
  }
  printf("%lf divisor, %lf offset\n", divisor, offset);
}



// Main menu definition
Fl_Menu_Item ImpressionistUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Image...",	FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_load_image },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)ImpressionistUI::cb_save_image },
		{ "&Brushes and Kernels...",	FL_ALT + 'b', (Fl_Callback *)ImpressionistUI::cb_brushes }, 
		{ "&Clear Canvas", FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_clear_canvas, 0, FL_MENU_DIVIDER },
		
		{ "&Quit",			FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)ImpressionistUI::cb_about },
		{ 0 },

	{ 0 }
};

// Brush choice menu definition
Fl_Menu_Item ImpressionistUI::brushTypeMenu[NUM_BRUSH_TYPE+1] = {
  {"Points",			FL_ALT+'p', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_POINTS},
  {"Lines",				FL_ALT+'l', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_LINES},
  {"Circles",			FL_ALT+'c', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_CIRCLES},
  {"Scattered Points",	FL_ALT+'q', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_POINTS},
  {"Scattered Lines",	FL_ALT+'m', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_LINES},
  {"Scattered Circles",	FL_ALT+'d', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_CIRCLES},
  {0}
};

// Brush choice menu definition
Fl_Menu_Item ImpressionistUI::angleChoiceMenu[5] = {
  {"Slider",			FL_ALT+'p', (Fl_Callback *)ImpressionistUI::cb_angleChoice, (void *)0},
  {"Right click",				FL_ALT+'l', (Fl_Callback *)ImpressionistUI::cb_angleChoice, (void *)1},
  {"Paint direction",			FL_ALT+'c', (Fl_Callback *)ImpressionistUI::cb_angleChoice, (void *)2},
  {"Gradient direction",			FL_ALT+'g', (Fl_Callback *)ImpressionistUI::cb_angleChoice, (void *)3},
  {0}
};



//----------------------------------------------------
// Constructor.  Creates all of the widgets.
// Add new widgets here
//----------------------------------------------------
ImpressionistUI::ImpressionistUI() {
	// Create the main window
	m_mainWindow = new Fl_Window(600, 300, "Impressionist");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 600, 25);
		m_menubar->menu(menuitems);

		// Create a group that will hold two sub windows inside the main
		// window
		Fl_Group* group = new Fl_Group(0, 25, 600, 275);

			// install paint view window
			m_paintView = new PaintView(300, 25, 300, 275, "This is the paint view");//0jon
			m_paintView->box(FL_DOWN_FRAME);

			// install original view window
			m_origView = new OriginalView(0, 25, 300, 275, "This is the orig view");//300jon
			m_origView->box(FL_DOWN_FRAME);
			m_origView->deactivate();

		group->end();
		Fl_Group::current()->resizable(group);
    m_mainWindow->end();

	// init values
	m_nSize = 10;
  m_nAlpha = 1.0;
  m_nLineThickness = 3;
  m_nAngle = 0;

  m_nKRow = 3;
  m_nKCol = 3;

	// brush dialog definition
	m_brushDialog = new Fl_Window(500, 600, "Brush and Kernel Dialog");
		// Add a brush type choice to the dialog
		m_BrushTypeChoice = new Fl_Choice(50,10,150,25,"&Brush");
		m_BrushTypeChoice->user_data((void*)(this));	// record self to be used by static callback functions
		m_BrushTypeChoice->menu(brushTypeMenu);
		m_BrushTypeChoice->callback(cb_brushChoice);

    m_AngleChoiceDropdown = new Fl_Choice(150,200,150,25,"&Angle Choice");
    m_AngleChoiceDropdown->user_data((void*)(this));
    m_AngleChoiceDropdown->menu(angleChoiceMenu);
    m_AngleChoiceDropdown->callback(cb_angleChoice);

		m_ClearCanvasButton = new Fl_Button(240,10,150,25,"&Clear Canvas");
		m_ClearCanvasButton->user_data((void*)(this));
		m_ClearCanvasButton->callback(cb_clear_canvas_button);

		m_BrushSizeSlider = new Fl_Value_Slider(10, 80, 300, 20, "Size");
		m_BrushSizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_BrushSizeSlider->type(FL_HOR_NICE_SLIDER);
        m_BrushSizeSlider->labelfont(FL_COURIER);
        m_BrushSizeSlider->labelsize(12);
		m_BrushSizeSlider->minimum(1);
		m_BrushSizeSlider->maximum(100);
		m_BrushSizeSlider->step(1);
		m_BrushSizeSlider->value(m_nSize);
		m_BrushSizeSlider->align(FL_ALIGN_RIGHT);
		m_BrushSizeSlider->callback(cb_sizeSlides);

		m_AlphaSlider = new Fl_Value_Slider(10, 100, 300, 20, "Alpha");
		m_AlphaSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_AlphaSlider->type(FL_HOR_NICE_SLIDER);
        m_AlphaSlider->labelfont(FL_COURIER);
        m_AlphaSlider->labelsize(12);
		m_AlphaSlider->minimum(0);
		m_AlphaSlider->maximum(1);
		m_AlphaSlider->step(.01);
		m_AlphaSlider->value(m_nAlpha);
		m_AlphaSlider->align(FL_ALIGN_RIGHT);
		m_AlphaSlider->callback(cb_alphaSlides);


		m_LineThicknessSlider = new Fl_Value_Slider(10, 120, 300, 20, "Line Thickness");
		m_LineThicknessSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_LineThicknessSlider->type(FL_HOR_NICE_SLIDER);
        m_LineThicknessSlider->labelfont(FL_COURIER);
        m_LineThicknessSlider->labelsize(12);
		m_LineThicknessSlider->minimum(1);
		m_LineThicknessSlider->maximum(10);
		m_LineThicknessSlider->step(1);
		m_LineThicknessSlider->value(m_nLineThickness);
		m_LineThicknessSlider->align(FL_ALIGN_RIGHT);
		m_LineThicknessSlider->callback(cb_lineThicknessSlides);

		m_AngleSlider = new Fl_Value_Slider(10, 140, 300, 20, "Angle");
		m_AngleSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_AngleSlider->type(FL_HOR_NICE_SLIDER);
        m_AngleSlider->labelfont(FL_COURIER);
        m_AngleSlider->labelsize(12);
		m_AngleSlider->minimum(0);
		m_AngleSlider->maximum(360);
		m_AngleSlider->step(1);
		m_AngleSlider->value(m_nAngle);
		m_AngleSlider->align(FL_ALIGN_RIGHT);
		m_AngleSlider->callback(cb_angleSlides);

    m_KRowSlider = new Fl_Value_Slider(10, 250, 300, 20, "Kernel Rows");
		m_KRowSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_KRowSlider->type(FL_HOR_NICE_SLIDER);
        m_KRowSlider->labelfont(FL_COURIER);
        m_KRowSlider->labelsize(12);
		m_KRowSlider->minimum(1);
		m_KRowSlider->maximum(9);
		m_KRowSlider->step(1);
		m_KRowSlider->value(m_nKRow);
		m_KRowSlider->align(FL_ALIGN_RIGHT);
		m_KRowSlider->callback(cb_KRowSlides);

		m_KColSlider = new Fl_Value_Slider(10, 270, 300, 20, "Kernel Columns");
		m_KColSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_KColSlider->type(FL_HOR_NICE_SLIDER);
        m_KColSlider->labelfont(FL_COURIER);
        m_KColSlider->labelsize(12);
		m_KColSlider->minimum(1);
		m_KColSlider->maximum(9);
		m_KColSlider->step(1);
		m_KColSlider->value(m_nKCol);
		m_KColSlider->align(FL_ALIGN_RIGHT);
		m_KColSlider->callback(cb_KColSlides);
    
  
    kernel_inputs = new Fl_Int_Input** [9];
    int xBase = 20;
    int yBase = 300;
    for (int row = 0; row < 9; row ++) {
      kernel_inputs[row] = new Fl_Int_Input* [9];
      for (int col = 0; col < 9; col++) {
        kernel_inputs[row][col] = new Fl_Int_Input(xBase + (40 * col), yBase + (20 * row), 35, 20, "");
		    kernel_inputs[row][col]->user_data((void*)(this));	// record self to be used by static callback functions
        kernel_inputs[row][col]->value("0");
        if (row >= 3 || col >=3) {
          kernel_inputs[row][col]->hide();
        }
      }
    }
    
    m_KDivisor = new Fl_Input(100, 500, 100, 20, "Divisor");
    m_KDivisor->user_data((void*)(this));
    m_KDivisor->value("1.0");
    m_KOffset = new Fl_Input(100, 520, 100, 20, "Offset");
    m_KOffset->user_data((void*)(this));
    m_KOffset->value("0.0");


		m_KernelSource = new Fl_Button(20,550,150,25,"&Convolve Original");
		m_KernelSource->user_data((void*)(this));
		m_KernelSource->callback(cb_kernel_source);

		m_KernelPaint = new Fl_Button(240,550,150,25,"&Convolve Painting");
		m_KernelPaint->user_data((void*)(this));
		m_KernelPaint->callback(cb_kernel_paint);

    m_brushDialog->end();	
}

ImpressionistUI::~ImpressionistUI()
{
}
