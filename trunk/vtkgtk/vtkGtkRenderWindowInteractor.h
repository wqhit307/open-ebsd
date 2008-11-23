/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkGtkRenderWindowInteractor.cxx,v $
  Language:  C++
  Date:      $Date: 2002/11/22 16:25:58 $
  Version:   $Revision: 0.1 $

  Based on wvXRenderWindowInteractor and wxVTKRenderWindowInteractor.

  Copyright (c) 2004 Dov Grobgeld <dov.grobgeld@weizmann.ac.il>

  Based on code copyright by: Ken Martin, Will Schroeder, Bill Lorensen   

  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __vtkGtkRenderWindowInteractor_h
#define __vtkGtkRenderWindowInteractor_h

//===========================================================
// now we define the C++ class

#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include <gtk/gtk.h>

class VTK_RENDERING_EXPORT vtkGtkRenderWindowInteractor : public vtkRenderWindowInteractor
{
public:
  // Constructor
  vtkGtkRenderWindowInteractor();

  // Destructor
  ~vtkGtkRenderWindowInteractor();

  // Object factory. TBD.
  static vtkGtkRenderWindowInteractor *New();

  // vtkRenderWindowInteractor overrides
  void Initialize();
  void Enable();
  void Disable();
  void Start();
  void UpdateSize(int x, int y);
  int CreateTimer(int timertype);
  int DestroyTimer();
  void TerminateApp() {};
  void Render();

  // accessor function for drawing area used for gl interaction
  GtkWidget *get_drawing_area() { return drawing_area; }

  // Make all callback friends
  friend void     cb_realize   (GtkWidget*, gpointer user_data);
  friend gboolean cb_configure (GtkWidget *widget,
				GdkEventConfigure *event,
				gpointer user_data);
  friend gboolean cb_expose    (GtkWidget *widget,
				GdkEventExpose *event,
				gpointer user_data);
  friend gboolean cb_key_press_event(GtkWidget *widget,
			        GdkEventKey *event,
				gpointer user_data);
  friend gboolean cb_key_release_event(GtkWidget *widget,
			        GdkEventKey *event,
				gpointer user_data);
  friend gboolean cb_button_press_event(GtkWidget *widget,
		                GdkEventButton *event,
				gpointer user_data);
  friend gboolean cb_button_release_event(GtkWidget *widget,
			        GdkEventButton *event,
				gpointer user_data);
  friend gboolean cb_motion_notify_event(GtkWidget *widget,
		                GdkEventMotion *event,
				gpointer user_data);
  friend gboolean cb_scroll_event(GtkWidget *widget,
		                GdkEventScroll *event,
				gpointer user_data);

  // Dov - timeout callback - will have to cast the user_data...
  friend gboolean vtkGtkRenderWindowInteractorTimer(gpointer user_data);

 protected:
  int ActiveButton;
  int RenderAllowed;
  long GetHandle();
  int Stereo;

 private:
  long Handle;
  bool Created;
  int RenderWhenDisabled;
  int UseCaptureMouse;
  GtkWidget *drawing_area;
  gint last_xpos, last_ypos, last_key_state;
  // Flag used to break timer loop
  bool do_break_timer;

  void update_mouse_pos();
};

#endif



