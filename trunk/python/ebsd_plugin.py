import sys
import string
sys.path.append('..')
sys.path.append('.')

from scipy import array, arange, c_, zeros
from scipy.integrate import odeint

# Enthought libraries
from enthought.traits.api import *
from enthought.traits.ui.api import View, Item, HGroup, VGroup
from enthought.traits.ui.editors import RangeEditor, TableEditor, EnumEditor
from enthought.chaco2.chaco2_plot_editor import Chaco2PlotItem
from enthought.traits.ui.menu import MenuBar, Menu, Action, OKButton, CancelButton

from enthought.traits.ui.table_column import ObjectColumn, NumericColumn
from enthought.traits.ui.extras.checkbox_column import CheckboxColumn

# Mayavi related
from enthought.tvtk.api import tvtk
from enthought.mayavi.services import IMAYAVI
from enthought.mayavi.sources.vtk_data_source import VTKDataSource
from enthought.mayavi.modules.surface import Surface
from enthought.mayavi.modules.volume import Volume
from enthought.mayavi.modules.outline import Outline
from enthought.mayavi.modules.image_actor import ImageActor 
from enthought.mayavi.modules.grid_plane import GridPlane 
from enthought.mayavi.modules.scalar_cut_plane import ScalarCutPlane 
from enthought.pyface.tvtk.picker import Picker, PickHandler, PickedData, get_last_input

from ebsd import *

from ebsd_file_dialog import *
from ebsd_dialogs import *

from numpy import *

from threading import Thread

MODE_EULER = 0
MODE_RF = 1
MODE_MAD = 2
MODE_GRAIN = 3
MODE_AVG_EULER = 4
MODE_AVG_RF = 5

class suspicious_slice(HasTraits):
	slice = Int
	probability = String
	mark = Bool(False)


class ebsd_interface(HasTraits):
	# Set by envisage when envisage when this class is instantiated 
	# using the ApplicationObject
	application = Instance('enthought.envisage.core.application.Application')

	dataset = ebsd()
	progress = progress_dialog()
	process_thread = None

	######################################################
	## Data control traits								##
	######################################################

	dims = Str("-")
	steps = Str("-")
	num_grains = Int(0)

	#Grain list
	grain_list = Array()

	suspicious_list = List( suspicious_slice )

	######################################################
	## Visualisation traits								##
	######################################################

	display_mode = Trait(MODE_EULER, MODE_AVG_EULER,
						 MODE_RF, MODE_AVG_RF)
								   
	gx_position = Float(0)
	gy_position = Float(0)
	gz_position = Float(0)
	
	gx_visibility = Bool(True)
	gy_visibility = Bool(True)
	gz_visibility = Bool(True)
	
	dimx = Int(0)
	dimy = Int(0)
	dimz = Int(0)

	gx = None
	gy = None
	gz = None
	
	grain_hover = Int(0)
	
	grain_surfaces = Dict()
	
	volume_scene = None
	key_pressed = ""

	rendered_grains = [-1,]


	######################################################
	## Views											##
	######################################################

	## Suspicious Slice View							
	######################################################
	view_suspicious = View(
						Item(name="suspicious_list",
							 show_label=False,
							 editor=TableEditor(
										sortable 	 = False,
										configurable = False,
										auto_size 	 = False,
										columns = [ 
													ObjectColumn( label="Slice", 
																  name="slice",
																  width=50,
																  editable=False),
													ObjectColumn( label="Likelihood", 
																  name="probability",
																  editable=False),			  
													CheckboxColumn( label="Replace?",
																	width=50,
																    name="mark")
 												  ]) ))

	## Data View							
	######################################################	
 	view_data = View(Item(name  = 'dims',
 						  style = 'readonly',
 						  label = 'Dimensions'),
 					 Item(name  = 'steps',
 						  style = 'readonly',
 						  label = 'Step Size'),	  
 					 Item(name  = 'num_grains',
 						  style = 'readonly',
 						  label = 'Grains'),
 					 
 					 Group(
 					 	Item(name='display_mode',
 					 		 editor=EnumEditor(values={ MODE_EULER : "Euler Angles",
 					 		 							MODE_AVG_EULER : "Grain Euler Angles",
 					 		 							MODE_RF : "RF Space",
 					 		 							MODE_AVG_RF: "Grain RF"} )),
					 
					 
 					 Group(
						 HGroup(
							 Item(name='gx_visibility', 
								  label='X'),
							 Item(name='gy_visibility', 
								  label='Y'),
							 Item(name='gz_visibility', 
								  label='Z'),
							 columns=3),
						 
						 Item(name = 'gx_position',
							  editor = RangeEditor(low=0, high_name='dimx', mode='auto', format='%i'),
							  label = 'X'),
						 Item(name = 'gy_position',
							  editor = RangeEditor(low=0, high_name='dimy', mode='auto', format='%i'),
							  label = 'Y'),
						 Item(name = 'gz_position',
							  editor = RangeEditor(low=0, high_name='dimz', mode='auto', format='%i'),
							  label = 'Z'),
	 					 label="Slices",
						 show_border=True),
 					 	
 					 Group(
 					 		Item(name="grain_hover",
							     label="Grain",
							     style="readonly"),
 					 		label="Selection",
 					 		show_border=True),

 
 					 ))
 					 
	######################################################
	## Setup Mayavi										##
	######################################################

	def get_mayavi(self):
		return self.application.get_service(IMAYAVI)

	def _application_changed(self):
		mayavi = self.get_mayavi()

	def __init__(self, **traits):
		super(ebsd_interface, self).__init__(**traits) 
		self.dataset.set_progress_callback(self.progress.update)



	######################################################
	## Update controls									##
	######################################################
	
	def update(self):
		print "update"
		
		dims = self.dataset.dims
		self.dims = '%(x)ix%(y)ix%(z)i' % {'x':dims[0], 'y':dims[1], "z":dims[2]} 
				
		steps = self.dataset.steps
		self.steps = '%(x).3fx%(y).3fx%(z).3f' % {'x':steps[0], 'y':steps[1], "z":steps[2]} 
		
		self.dimx = dims[0]
		self.dimy = dims[1]
		self.dimz = dims[2]
		
		self.num_grains = self.dataset.num_grains

		if self.num_grains>0:
			self.grain_info = self.dataset.grains()
			
		self.grain_map = self.dataset.grain_mapper()
		
		self.dataset_vis()
	
	######################################################
	## Plane visibility handlers						##
	######################################################

	def _gx_visibility_changed(self,name,new,old):
		if self.gx != None:
			self.gx.actor.actor.visibility = old

	def _gy_visibility_changed(self,name,new,old):
		if self.gy != None:
			self.gy.actor.actor.visibility = old

	def _gz_visibility_changed(self,name,new,old):
		if self.gz != None:
			self.gz.actor.actor.visibility = old


	######################################################
	## Plane index handlers								##
	######################################################
	
	def _gx_position_changed(self,name,new,old):
		if self.gx != None and int(new) > 0:
			self.gx.grid_plane.position = int(new)

	def _gy_position_changed(self,name,new,old):
		if self.gy != None and int(new) > 0:
			self.gy.grid_plane.position = int(new)

	def _gz_position_changed(self,name,new,old):
		if self.gz != None and int(new) > 0:
			self.gz.grid_plane.position = int(new)
	
	
	######################################################
	## Suspicious slice handler							##
	######################################################

	def suspicious_selected(self,event):
		print event

	def _display_mode_changed(self,w,event):
		self.update()


	######################################################
	## VTK click/key handlers							##
	######################################################

	def handle_click(self,object,event):
		#Get TVTK object
		o = tvtk.to_tvtk(object)
		
		#Find click position
		x = float(o.event_position[0])
		y = float(o.event_position[1])

		if self.volume_scene!=None and self.num_grains>0:
			picker = tvtk.PointPicker()
			if picker.pick((x,y,0), self.volume_scene.scene.renderer):
				pos = picker.pick_position
				grain = self.dataset.grain_at_position(pos)
				if grain > -1:	
					self.select_grain(grain)

					
	def handle_key(self,object,event):
		o = tvtk.to_tvtk(object)
		if self.volume_scene!=None and self.num_grains>0:
			if o.key_code=="g":
				self.render_grain(int(self.grain_hover))
			if o.key_code=="h":
				self.render_adj_grain(int(self.grain_hover))
			if o.key_code=="r":
				self.remove_grain(int(self.grain_hover))
		
	#Menu Handlers
	def dataset_vis(self):
		mayavi = self.get_mayavi()
		a_vtk = self.dataset.dataset_vis(self.display_mode)
		a = tvtk.to_tvtk(a_vtk)
		
		if self.volume_scene is None:
			first_view = True
			
			mayavi.new_scene()
			self.volume_scene = mayavi.engine.current_scene
			self.source = VTKDataSource()
			self.source.data = a
			self.source.name = "3D Plot"
			
			mayavi.add_source(self.source)
		
			self.gx = GridPlane()
			self.gy = GridPlane()
			self.gz = GridPlane()
		
			mayavi.add_module(self.gx)
			mayavi.add_module(self.gy)
			mayavi.add_module(self.gz)
				
#			self.gx.configure_traits()	
#			self.gy.configure_traits()
#			self.gz.configure_traits()
			
			o = Outline()
			mayavi.add_module(o)
		else:
			first_view = False
			self.source.data = a
		
		
		dims = self.dataset.dims
		steps = self.dataset.steps	
				
		self.gx_position = int(dims[0]*0.5)		
		self.gy_position = int(dims[1]*0.5)		
		self.gz_position = int(dims[2]*0.5)		
	
		self.gz.grid_plane.axis = 'z'
		self.gz.actor.mapper.scalar_visibility = True	
		self.gz.grid_plane.position = self.gz_position
		self.gz.actor.property.representation = 'surface'

		self.gy.grid_plane.axis = 'y'
		self.gy.actor.mapper.scalar_visibility = True
		self.gy.grid_plane.position = self.gy_position
		self.gy.actor.property.representation = 'surface'

		self.gx.grid_plane.axis = 'y'
		self.gx.grid_plane.axis = 'x'
		self.gx.grid_plane.position = self.gx_position
		self.gx.actor.mapper.scalar_visibility = True	
		self.gx.actor.property.representation = 'surface'
				
		mayavi.engine.current_scene.scene.interactor.add_observer("LeftButtonPressEvent",self.handle_click)
		mayavi.engine.current_scene.scene.interactor.add_observer("KeyPressEvent",self.handle_key)

		if first_view:
			self.volume_scene.scene.isometric_view()	
		
	def rf_vis(self):
		mayavi = self.get_mayavi()

		frame_vtk = self.dataset.rf_frame(self.display_mode)
		frame = tvtk.to_tvtk(frame_vtk)

		space_vtk = self.dataset.rf_space(self.display_mode)
		space = tvtk.to_tvtk(space_vtk)

		glut = self.dataset.grain_lut()
		lut = tvtk.to_tvtk(glut)
	
		mayavi.new_scene()

		s1 = VTKDataSource(data = frame)
		mayavi.add_source(s1)
		
		a1 = Surface()
		mayavi.add_module(a1)

		s2 = VTKDataSource(data = space)
		mayavi.add_source(s2)

		a2 = Surface()
		mayavi.add_module(a2)
			
		a2.actor.mapper.scalar_mode = 'use_cell_data'
		a2.actor.mapper.lookup_table = lut

		a2.configure_traits()

	
	def select_grain(self, grain):
		self.grain_hover = grain
		self.grain_volume = self.grain_info["volume"][grain]
	
	
	def render_grain(self, grain=None):
		#Check if we've already rendered this grain
		if grain==None:
			grain=int(self.grain_hover)
		
		rendered = False
		for g in self.rendered_grains:
			if g==grain:
				rendered = True
			
		if not rendered and grain>-1:	
			self.rendered_grains.append(grain)
			print grain
			gvtk = self.dataset.grain_surface(grain)
			g = tvtk.to_tvtk(gvtk)
			s = VTKDataSource(data = g)
			mayavi = self.get_mayavi()
			mayavi.add_source(s)
			
			#Set surface properties
			surf = Surface()
			mayavi.add_module(surf)
			
			c = self.dataset.grain_representation(self.grain_hover,self.display_mode)										
			surf.actor.property.color = c

			self.grain_surfaces[grain] = surf

	def remove_grain(self, grain=None):
		if grain==None:
			grain = int(self.grain_hover)
			print grain
		
		if self.grain_surfaces.has_key(grain):
			self.grain_surfaces[grain].stop()
			del self.grain_surfaces[grain]
			
			
	def render_adj_grain(self, grain=None):
		#Check if we've already rendered this grain
		if grain==None:
			grain=int(self.grain_hover)
		
		rendered = False
		for g in self.rendered_grains:
			if g==grain:
				rendered = True
			
		if not rendered and grain>-1:	
			self.rendered_grains.append(grain)
			mayavi = self.get_mayavi()
			gvtk = self.dataset.adj_grain_surface(grain)

			glut = self.dataset.grain_lut()
			lut = tvtk.to_tvtk(glut)

			g = tvtk.to_tvtk(gvtk)
			s = VTKDataSource(data = g)
			mayavi = self.get_mayavi()
			mayavi.add_source(s)
			
			#Set surface properties
			surf = Surface()

			mayavi.add_module(surf)

			surf.actor.mapper.lookup_table = lut
#			surf.actor.mapper.scalar_mode = 'use_cell_data'


	def render_boundary_network(self):

		mayavi = self.get_mayavi()
		gvtk = self.dataset.boundary_network()

		glut = self.dataset.boundary_lut()
		lut = tvtk.to_tvtk(glut)

		g = tvtk.to_tvtk(gvtk)
		s = VTKDataSource(data = g)
		mayavi = self.get_mayavi()
		mayavi.add_source(s)
		
		#Set surface properties
		surf = Surface()

		mayavi.add_module(surf)

#		surf.actor.mapper.scalar_mode = 'use_cell_data'
		surf.actor.mapper.lookup_table.set_range(0,63)	

		surf.configure_traits()

	######################################################
	## Menu Handlers									##
	######################################################

	## Read Channel 5 CSV									
	######################################################
	
	def read_file(self):

		file = file_loader()
		file.configure_traits()	
		if file.is_ok and file.f != "":
			self.progress.open('Reading Channel 5 CSV',self.update)
			self.process_thread = Thread(None,self.dataset.read_channel5,"read_file_thread",
								  	 (file.f, file.nx, file.ny, file.nz, 
								  	 		  file.stepx, file.stepy, file.stepz))						
			self.process_thread.start()


	## Read raw file									
	######################################################
			
	def read_raw(self):

		self.dataset.set_progress_callback(self.progress.update)


		file = FileDialog(action="open",)
		if file.open():
			self.progress.open('Reading Raw File',self.update)
			self.process_thread = Thread(None,self.dataset.read_raw,"read_file_thread",(file.path,))
			self.process_thread.start()


	## Write raw file									
	######################################################

	def write_raw(self):
		file = FileDialog(action="save as",)
		if file.open():
			if self.dataset.write_raw(file.path)==1:
				print "Success writing file"
			else:
				print "Error writing file"
	
	
	## Get suspicious slices									
	######################################################
	
	def get_suspicious(self):	
		suspicious = self.dataset.suspicious()
		self.suspicious_list = []
		for (s,p) in suspicious:
			self.suspicious_list.append( suspicious_slice(slice=s,probability=p) )
	
	
	## Calculate offsets									
	######################################################

	def calculate_offsets(self):	
		self.progress.open('Calculating offsets',self.get_suspicious)
		self.process_thread = Thread(None,self.dataset.calculate_offsets,"align_thread",())
		self.process_thread.start()


	## Align slices									
	######################################################
		
	def align_slices(self):
		marked = []
		
		for s in self.suspicious_list:
			if s.mark:
				marked.append(s.slice) 
		
		self.progress.open('Aligning slices',self.update)
		self.process_thread = Thread(None,self.dataset.align_slices,"align_thread",(marked,))
		self.process_thread.start()	


	## Reconstruct grains									
	######################################################
	
	def reconstruct(self):
		self.progress.open('Reconstructing',self.update)
		self.process_thread = Thread(None,self.dataset.reconstruct,"reconstruct_thread",())
		self.process_thread.start()
			
			
	## Segment grains									
	######################################################
			
	def segment(self):
		seg = segment_dialog() 
		seg.configure_traits()
		
		if seg.is_ok:
			self.progress.open('Segment',self.update)
			self.process_thread = Thread(None,self.dataset.segment,"segment_thread",(seg.min_angle,))
			self.process_thread.start()
		
		
	## Dilate grains									
	######################################################
			
	def dilate_grains(self):
		self.progress.open('Dilating grains',self.update)
		self.process_thread = Thread(None,self.dataset.dilate_grains,"dilate_thread",())
		self.process_thread.start()


	## Filter grains
	######################################################

	def filter_grains(self):
		filter = filter_dialog()
		filter.configure_traits()

		if filter.is_ok:
			self.progress.open('Filter grains',self.update)
			self.process_thread = Thread(None,self.dataset.filter_grains,"filter_thread",(filter.volume,))
			self.process_thread.start()

	
	## Calculate extra stats
	######################################################

	def calculate_extra_stats(self):
		self.progress.open('Calculating statistics',self.update)
		self.process_thread = Thread(None,self.dataset.calculate_extra_stats,"filter_thread",())
		self.process_thread.start()

