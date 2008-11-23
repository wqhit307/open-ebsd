/**
	\file ebsd.cpp
	\brief	Example data processing session 
*/

#include "ebsd.h"

#include <gtk/gtk.h>
#include <glade/glade.h>

#include "vtkSphereSource.h"
#include "vtkCubeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkGtkRenderWindowInteractor.h"
#include "vtkInteractorStyleFlight.h"
#include "gtk/gtkgl.h"


// Creates a scene consisting of a sphere and a 3D block
vtkRenderer *create_scene()
{
  // create sphere geometry
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(1.0);
  sphere->SetThetaResolution(50);
  sphere->SetPhiResolution(50);

  // map to graphics library
  vtkPolyDataMapper *sphere_map = vtkPolyDataMapper::New();
  sphere_map->SetInput(sphere->GetOutput());

  // actor coordinates geometry, properties, transformation
  vtkActor *sphere_actor = vtkActor::New();
  sphere_actor->SetMapper(sphere_map);
  sphere_actor->GetProperty()->SetColor(0,0,1); // sphere color blue

  // cube
  vtkCubeSource *cube = vtkCubeSource::New();
  cube->SetXLength(1.5);
  cube->SetYLength(1.0);
  cube->SetZLength(0.5);

  // map to graphics library
  vtkPolyDataMapper *cube_map = vtkPolyDataMapper::New();
  cube_map->SetInput(cube->GetOutput());

  // actor coordinates geometry, properties, transformation
  vtkActor *cube_actor = vtkActor::New();
  cube_actor->SetMapper(cube_map);
  cube_actor->SetPosition(2.5,0,0);
  cube_actor->GetProperty()->SetColor(1,0,0); // cube color is red

  // a renderer and and place the objects in it render window
  vtkRenderer *ren1 = vtkRenderer::New();
  ren1->AddActor(sphere_actor);
  ren1->AddActor(cube_actor);
  ren1->SetBackground(1,1,1); // Background color white

  return ren1;
}


static gint delete_event_cb(GtkWidget* w, GdkEventAny* e, gpointer data)
{   
         /* callback for "delete" signal */
         g_print("main.c:delete_event_cb()\n");
         return 0;
 }

 static gint destroy_cb(GtkWidget* w, GdkEventAny* e, gpointer data)
 {
        /* callback for "destroy" signal */
        g_print("main.c:destroy_event_cb()\n");  
        
        /* quit application */
        gtk_main_quit();
        return 0;
}

void build_menu()
{

	GtkWidget* open_item, save_item, quit_item;

	GtkWidget* file_menu = gtk_menu_new ();    /* Don't need to show menus */

    /* Create the menu items */
    open_item = gtk_menu_item_new_with_label ("Open");
    save_item = gtk_menu_item_new_with_label ("Save");
    quit_item = gtk_menu_item_new_with_label ("Quit");

    /* Add them to the menu */
    gtk_menu_append (GTK_MENU (file_menu), open_item);
    gtk_menu_append (GTK_MENU (file_menu), save_item);
    gtk_menu_append (GTK_MENU (file_menu), quit_item);

    /* Attach the callback functions to the activate signal */
    gtk_signal_connect_object (GTK_OBJECT (open_items), "activate",
                               GTK_SIGNAL_FUNC (menuitem_response),
                               (gpointer) "file.open");
    gtk_signal_connect_object (GTK_OBJECT (save_items), "activate",
                               GTK_SIGNAL_FUNC (menuitem_response),
                               (gpointer) "file.save");

    /* We can attach the Quit menu item to our exit function */
    gtk_signal_connect_object (GTK_OBJECT (quit_items), "activate",
                               GTK_SIGNAL_FUNC (destroy),
                               (gpointer) "file.quit");

    /* We do need to show menu items */
    gtk_widget_show (open_item);
    gtk_widget_show (save_item);
    gtk_widget_show (quit_item);


    GtkWidget* menu_bar = gtk_menu_bar_new ();
    gtk_container_add (GTK_CONTAINER (window), menu_bar);
    gtk_widget_show (menu_bar);

    GtkWidget* file_item = gtk_menu_item_new_with_label ("File");
    gtk_widget_show (file_item);

    gtk_menu_item_set_submenu (GTK_MENU_ITEM (file_item), file_menu);
    gtk_menu_bar_append (GTK_MENU_BAR (menu_bar), file_item);

}

void build_gui(GtkWidget *vtk_area)
{
  // Build gui
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  GtkWidget *vbox = gtk_vbox_new(0,0);

  gtk_window_set_policy(GTK_WINDOW(window), TRUE, TRUE, TRUE);

  gtk_container_add (GTK_CONTAINER (window),
		     vbox);
  gtk_box_pack_start (GTK_BOX (vbox),
		      vtk_area, 1,1,0);
  gtk_widget_set_size_request(vtk_area, 400,400);
  GtkWidget *button = gtk_button_new_with_label("Quit");
  g_signal_connect(G_OBJECT(button), "clicked",
		   G_CALLBACK(gtk_main_quit), NULL);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE,FALSE,0);
 
  gtk_widget_show_all (window);

}

int main(int argc,char *argv[])
{	
	GtkWidget *app_window;          /* main application window widget */
	GladeXML *gxml;                 /* glade xml file */
	
	GtkWidget *widget, *vtk_widget, *vtk_placeholder;

	gtk_init(&argc, &argv);
	gxml = glade_xml_new("interface.glade", NULL, NULL);

 	/* get a widget (useful if you want to change something) */
 	widget = glade_xml_get_widget(gxml, "widgetname");

  	/* connect signal handlers */
  	glade_xml_signal_autoconnect(gxml);

	app_window = glade_xml_get_widget (gxml, "main_window");
	vtk_placeholder = glade_xml_get_widget (gxml, "placeholder");

	g_signal_connect(G_OBJECT(app_window), "delete_event",
					 G_CALLBACK(delete_event_cb), NULL);

	g_signal_connect(G_OBJECT(app_window), "destroy",
					 G_CALLBACK(destroy_cb), NULL);

	// Create the gtk vtk interactor. This also creates the gtk widget.
	vtkGtkRenderWindowInteractor *iren = vtkGtkRenderWindowInteractor::New();

	// Connect the renderer to the gtk vtk rendering window
  	vtkRenderer *scene = create_scene();
  	iren->GetRenderWindow()->AddRenderer(scene);
	vtk_placeholder = iren->get_drawing_area();
	build_gui(vtk_placeholder);
	
	gtk_container_add(GTK_CONTAINER(app_window),vtk_placeholder);

	//gtk_widget_show (app_window);
  	gtk_main();

  	return 0;


	/*

	//Set Data Dimensions
	int dims[3] = {200,200,11};
	float steps[3] = {0.07,0.07,0.101};	

	//Open data file
	Dataset ds = Dataset("../new.txt",dims,steps);
	
	// Calculate offsets
	ds.calculate_offsets();
	
	// Mark slices to replace
	ds.marked[80] = true;
	ds.marked[42] = true;
	
	// Recalculate offsets with replaced slices and realign slices accordingly
	ds.calculate_offsets();	
	ds.realign_slices();

	//Segment, and clean data
	ds.segment(5);
	ds.calculate_grain_info();
	
	ds.dilate_grains();
	ds.calculate_grain_info();

	ds.merge_grains(2);
	ds.calculate_grain_info();

	ds.filter_grains(100);
	ds.calculate_grain_info();
	
	// Calculate ellipsoids, surfaces
	ds.calculate_extra_stats();

	// Write raw file for later processing
	ds.write_raw("raw.ebsd");

	*/
	
}
