/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkGtkRenderWindowInteractor.cxx,v $
  Language:  C++
  Date:      $Date: 2002/11/22 16:25:58 $
  Version:   $Revision: 0.1 $

  Based on wvXRenderWindowInteractor and wxVTKRenderWindowInteractor.

  Copyright (c) 2007 Dov Grobgeld <dov.grobgeld@gmail.com>

  Based on code copyright by: Ken Martin, Will Schroeder, Bill Lorensen   

  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkGtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#ifdef G_OS_WIN32
#include "gdk/gdkwin32.h"
#else
#include <gdk/gdkx.h>
#endif
#include <gdk/gdk.h>
#include <gdk/gdkprivate.h>

void     cb_realize          (GtkWidget*,
                              gpointer user_data);
gboolean cb_expose           (GtkWidget *widget,
                              GdkEventExpose *event,
                              gpointer user_data);
gboolean cb_key_press_event  (GtkWidget *widget,
                              GdkEventKey *event,
                              gpointer user_data);
gboolean cb_key_release_event(GtkWidget *widget,
                              GdkEventKey *event,
                              gpointer user_data);
gboolean cb_button_press_event(GtkWidget *widget,
                               GdkEventButton *event,
                               gpointer user_data);
gboolean cb_button_release_event(GtkWidget *widget,
                                 GdkEventButton *event,
                                 gpointer user_data);
gboolean cb_motion_notify_event(GtkWidget *widget,
                                GdkEventMotion *event,
                                gpointer user_data);
gboolean cb_scroll_event        (GtkWidget      *widget,
                                 GdkEventScroll *event,
                                 gpointer        user_data);
gboolean cb_configure(GtkWidget *widget,
                      GdkEventConfigure *event,
                      gpointer user_data);
gboolean vtkGtkRenderWindowInteractorTimer(gpointer client_data);


// Construct an instance so that the light follows the camera motion.
vtkGtkRenderWindowInteractor::vtkGtkRenderWindowInteractor()
  : vtkRenderWindowInteractor()
  , RenderAllowed(false)
  , RenderWhenDisabled(true)
  , Created(true)
  , UseCaptureMouse(false)
  , Handle(0)
  , Stereo(0)
  , ActiveButton(0)
  , last_xpos(0)
  , last_ypos(0)
  , last_key_state(0)
  , do_break_timer(false)
{
  GdkGLConfig *glconfig;

  drawing_area = gtk_drawing_area_new();
  GTK_WIDGET_SET_FLAGS (drawing_area, GTK_CAN_FOCUS);

  gtk_widget_set_events(GTK_WIDGET(drawing_area),
			GDK_EXPOSURE_MASK
			| GDK_STRUCTURE_MASK
			| GDK_PROPERTY_CHANGE_MASK
			| GDK_POINTER_MOTION_MASK 
			| GDK_POINTER_MOTION_HINT_MASK 
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_KEY_RELEASE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_ENTER_NOTIFY_MASK
			);
  
  glconfig = gdk_gl_config_new_by_mode (GdkGLConfigMode(GDK_GL_MODE_RGB    |
							GDK_GL_MODE_DEPTH  |
							GDK_GL_MODE_DOUBLE));

  /* Set OpenGL-capability to the widget. */
  gtk_widget_set_gl_capability (drawing_area,
                                glconfig,
                                NULL,
                                TRUE,
                                GDK_GL_RGBA_TYPE);

  gtk_widget_set_double_buffered(drawing_area,
                                 FALSE);
  
  g_signal_connect(drawing_area, "realize",
		   G_CALLBACK(&cb_realize),
		   (gpointer)this);
  g_signal_connect(drawing_area, "configure_event",
		   G_CALLBACK(&cb_configure),
		   (gpointer)this);
  g_signal_connect(drawing_area, "expose_event",
		   G_CALLBACK(&cb_expose),
		   (gpointer)this);
  g_signal_connect(drawing_area, "key_press_event",
		   G_CALLBACK(&cb_key_press_event),
		   (gpointer)this);
  g_signal_connect(drawing_area, "key_release_event",
		   G_CALLBACK(&cb_key_release_event),
		   (gpointer)this);
  g_signal_connect(drawing_area, "button_press_event",
		   G_CALLBACK(&cb_button_press_event),
		   (gpointer)this);
  g_signal_connect(drawing_area, "button_release_event",
		   G_CALLBACK(&cb_button_release_event),
		   (gpointer)this);
  g_signal_connect(drawing_area, "scroll_event",  
		   G_CALLBACK(&cb_scroll_event),
		   (gpointer)this);
  g_signal_connect(drawing_area, "motion_notify_event",
		   G_CALLBACK(&cb_motion_notify_event),
		   (gpointer)this);

  // Copied from wxVTKRenderWindowInteractor... What is it for?
  vtkRenderWindowInteractor::SetRenderWindow(vtkRenderWindow::New());
  RenderWindow->Delete();
}

vtkGtkRenderWindowInteractor * vtkGtkRenderWindowInteractor::New()
{
  // we are not registered in the objectfactor
  return new vtkGtkRenderWindowInteractor;
}

// Todo constructor with more options...

vtkGtkRenderWindowInteractor::~vtkGtkRenderWindowInteractor()
{
  gtk_widget_destroy(drawing_area);
}

// Initializes the event handlers using a parent widget that you have
// provided.  This assumes that you want to own the event loop.
void vtkGtkRenderWindowInteractor::Initialize()
{
  Enable();
  RenderWindow->Start();

  Initialized = 1;
}

void vtkGtkRenderWindowInteractor::Enable()
{
  if (Enabled)
    return;

  Enabled = true;
  Modified();
}

void vtkGtkRenderWindowInteractor::Disable()
{
  if (!Enabled)
    return;

  Enabled = false;
  Modified();
}

void vtkGtkRenderWindowInteractor::Start()
{
  fprintf(stderr, "Interactor cannot control event loop!\n");
}

void vtkGtkRenderWindowInteractor::UpdateSize(int x,int y)
{
  // if the size changed send this on to the RenderWindow
  if ((x != this->Size[0]) || (y != this->Size[1]))
  {
    this->Size[0] = x;
    this->Size[1] = y;
    this->RenderWindow->SetSize(x,y);
  }
}

int vtkGtkRenderWindowInteractor::CreateTimer(int timertype) 
{
  
  if (timertype == VTKI_TIMER_FIRST)
  {
    // We use a repeating timer? Do we need to keep an id for it?
    g_timeout_add(10, &vtkGtkRenderWindowInteractorTimer, (gpointer)(this));
    this->do_break_timer = false;
  }

  return 1;
}

int vtkGtkRenderWindowInteractor::DestroyTimer() 
{
  this->do_break_timer = true;
  return 1;
}

long vtkGtkRenderWindowInteractor::GetHandle()
{
#ifdef G_OS_WIN32
  return (long)GDK_WINDOW_HWND(drawing_area->window);
#else
  return (long)GDK_WINDOW_XWINDOW(drawing_area->window);
#endif
}

void vtkGtkRenderWindowInteractor::Render()
{
  RenderAllowed = 1;
  if (!RenderWhenDisabled)
  {
    // tbd - check if parent is enabled
  }

  if (RenderAllowed)
  {
    if (Handle && (Handle == GetHandle()) )
    {
      RenderWindow->Render();
    }

    // TBD for reparenting
  }
}

void vtkGtkRenderWindowInteractor::update_mouse_pos()
{
  GdkModifierType mask;
  gdk_window_get_pointer(drawing_area->window,
			 &last_xpos,
			 &last_ypos,
			 &mask);
  last_key_state = mask;
}

gboolean vtkGtkRenderWindowInteractorTimer(gpointer client_data)
{
  vtkGtkRenderWindowInteractor *rwi = (vtkGtkRenderWindowInteractor *)client_data;

  if (rwi->do_break_timer) {
    return FALSE;
  }
  
  if (rwi->Enabled) 
  {
    rwi->InvokeEvent(vtkCommand::TimerEvent,NULL);
  }

  return TRUE;
}

void cb_realize(GtkWidget*,
		gpointer user_data)
{
  vtkGtkRenderWindowInteractor *rwi = (vtkGtkRenderWindowInteractor *)user_data;
  
  if (!rwi->Handle)
  {
    rwi->Handle = rwi->GetHandle();
    rwi->RenderWindow->SetWindowId((void*)rwi->Handle);
  }

  // get vtk to render to the drawing area
  //  rwi->Render();
}

gboolean cb_configure(GtkWidget *widget,
		  GdkEventConfigure *event,
		  gpointer user_data)
{
  vtkGtkRenderWindowInteractor *rwi = (vtkGtkRenderWindowInteractor *)user_data;

  rwi->UpdateSize(event->width, event->height);

  if (!rwi->Enabled)
  {
    return TRUE;
  }

  rwi->InvokeEvent(vtkCommand::ConfigureEvent, NULL);
  
  // get vtk to render to the drawing area
  rwi->Render();

  return TRUE;
}

// I'm rendering everytime I'm getting an expose event. Perhaps this
// is a waist of time...
gboolean cb_expose(GtkWidget *widget,
		   GdkEventExpose *event,
		   gpointer user_data)
{
  vtkGtkRenderWindowInteractor *rwi = (vtkGtkRenderWindowInteractor *)user_data;
  rwi->Render();
  return TRUE;
}

gboolean
cb_key_press_event(GtkWidget *widget,
		   GdkEventKey *event,
		   gpointer user_data)
{
  vtkGtkRenderWindowInteractor *rwi = (vtkGtkRenderWindowInteractor *)user_data;
  gint keyval = event->keyval;
  char key = 0;

  if (keyval < 256)
  {
    // Todo: Unicode in non-Unicode mode??
    key = (char)keyval;
  }

  rwi->update_mouse_pos();

  rwi->SetEventInformationFlipY(rwi->last_xpos,
				rwi->last_ypos,
				event->state & GDK_CONTROL_MASK,
				event->state & GDK_SHIFT_MASK,
				key,
				0,
				NULL);

  rwi->last_key_state = event->state;
  
  rwi->InvokeEvent(vtkCommand::KeyPressEvent, NULL);
  rwi->InvokeEvent(vtkCommand::CharEvent, NULL);

  return true;
}

gboolean
cb_key_release_event(GtkWidget *widget,
		     GdkEventKey *event,
		     gpointer user_data)
{
  vtkGtkRenderWindowInteractor *rwi = (vtkGtkRenderWindowInteractor *)user_data;
  gint keyval = event->keyval;
  char key = 0;

  if (keyval < 256)
  {
    // Todo: Unicode in non-Unicode mode??
    key = (char)keyval;
  }

  rwi->update_mouse_pos();
  rwi->SetEventInformationFlipY(rwi->last_xpos,
				rwi->last_ypos,
				event->state & GDK_CONTROL_MASK,
				event->state & GDK_SHIFT_MASK,
				key,
				0,
				NULL);

  rwi->last_key_state = event->state;
  
  rwi->InvokeEvent(vtkCommand::KeyReleaseEvent, NULL);

  return TRUE;
}


gboolean
cb_button_press_event(GtkWidget *widget,
		      GdkEventButton *event,
		      gpointer user_data)
{
  vtkGtkRenderWindowInteractor *rwi = (vtkGtkRenderWindowInteractor *)user_data;
  gint button = event->button;
  
  if (!rwi->Enabled || rwi->ActiveButton != 0)
    return true;

  rwi->update_mouse_pos();
  rwi->SetEventInformationFlipY(rwi->last_xpos,
				rwi->last_ypos,
				rwi->last_key_state & GDK_CONTROL_MASK,
				rwi->last_key_state & GDK_SHIFT_MASK,
				'\0',
				0,
				NULL);
  
  rwi->ActiveButton = button;

  switch (button) {
  case 1:
    rwi->InvokeEvent(vtkCommand::LeftButtonPressEvent, NULL);
    break;
  case 2:
    rwi->InvokeEvent(vtkCommand::MiddleButtonPressEvent, NULL);
    break;
  case 3:
    rwi->InvokeEvent(vtkCommand::RightButtonPressEvent, NULL);
    break;
  default:
    // Ignore other buttons at the moment
    break;
  }

  // Do we need to worry about capturing the mouse
  return TRUE;
}

gboolean
cb_scroll_event(GtkWidget *widget,
                GdkEventScroll *event,
                gpointer user_data)
{
  vtkGtkRenderWindowInteractor *rwi = (vtkGtkRenderWindowInteractor *)user_data;
  gint direction = event->direction;
  double focalPt[3];
  double distance = 0;
  double zoom = 1.0;
  
  if (!rwi->Enabled || rwi->ActiveButton != 0)
    return true;

  if (direction) 
    zoom = 0.9;
  else 
    zoom = 1.1;

  vtkRenderer *ren = rwi->FindPokedRenderer(rwi->last_xpos,
                                            rwi->last_ypos);

  ren->GetActiveCamera()->Zoom(zoom);
  rwi->RenderWindow->Render();
    
  return TRUE;
}

gboolean
cb_button_release_event(GtkWidget *widget,
			GdkEventButton *event,
			gpointer user_data)
{
  vtkGtkRenderWindowInteractor *rwi = (vtkGtkRenderWindowInteractor *)user_data;
  gint button = event->button;

  // is this right?
  if (!rwi->Enabled || rwi->ActiveButton != button)
    return true;

  rwi->update_mouse_pos();
  rwi->SetEventInformationFlipY(rwi->last_xpos,
				rwi->last_ypos,
				rwi->last_key_state & GDK_CONTROL_MASK,
				rwi->last_key_state & GDK_SHIFT_MASK,
				'\0',
				0,
				NULL);
  switch (button) {
  case 1:
    rwi->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, NULL);
    break;
  case 2:
    rwi->InvokeEvent(vtkCommand::MiddleButtonReleaseEvent, NULL);
    break;
  case 3:
    rwi->InvokeEvent(vtkCommand::RightButtonReleaseEvent, NULL);
    break;
  default:
    // Ignore other buttons at the moment
    break;
  }

  rwi->ActiveButton = 0;
}

gboolean
cb_motion_notify_event(GtkWidget *widget,
		       GdkEventMotion *event,
		       gpointer user_data)
{
  vtkGtkRenderWindowInteractor *rwi = (vtkGtkRenderWindowInteractor *)user_data;

  rwi->update_mouse_pos();
  rwi->SetEventInformationFlipY(rwi->last_xpos,
				rwi->last_ypos,
				rwi->last_key_state & GDK_CONTROL_MASK,
				rwi->last_key_state & GDK_SHIFT_MASK,
				'\0',
				0,
				NULL);

  rwi->InvokeEvent(vtkCommand::MouseMoveEvent, NULL);

  return true;
}

