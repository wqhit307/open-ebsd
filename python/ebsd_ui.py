from enthought.traits.api import Delegate, HasTraits, Int, Str, Float, Instance, \
								 Array, Bool, Callable, Enum, Trait, Range, Property, File
from types import StringType 

from enthought.traits.ui.api import View, Item, Group, VGrid
from enthought.traits.ui.menu import MenuBar, Menu, Action, NoButtons
from enthought.traits.ui.handler import Handler

import wx

import time
from math import floor

from ebsd_file_dialog import *
from ebsd_dialogs import *

from threading import Thread

#from numpy import arange, array, hstack
#from enthought.chaco2.chaco2_plot_editor import Chaco2PlotItem
				   
#-------------------------------#
# Main Window					#
#-------------------------------#	

#--Menu Items-------------------#
open_file = Action(
				name = "Open Channel 5 CSV...",
				accelerator = "Ctrl-O",
			  	action = "read_file")

open_raw = Action(
				name = "Open Raw File...",
				accelerator = "Ctrl-Shift-O",
			  	action = "read_raw")
			  	
segment = Action(
				name = "Segment...",
				accelerator = "Ctrl-S",
			  	action = "segment")			  	

dilate_grains = Action(
				name = "Dilate Grains",
				accelerator = "Ctrl-D",
			  	action = "dilate_grains")			  	

filter_grains = Action(
				name = "Filter Grains...",
				accelerator = "Ctrl-F",
			  	action = "filter_grains")	
			  	
align = Action(
				name = "Align Slices",
				accelerator = "Ctrl-A",
			  	action = "align")				  	

	
#--View-------------------------#
main_view = View(
				Group(
					Item(name='f', label="La la la"),			
					Item(name='g', label="La la la"),			
					
					show_left=True,
					orientation='horizontal',
					show_border=True
				),
				width=800,
				resizable=True,
				kind="live",
				title="3D EBSD",
				menubar = MenuBar( 
							Menu( 
								open_file, 
								open_raw,
								name = 'File'),
							 Menu(
								align,
								segment,
								dilate_grains,
								filter_grains,
								name = 'Processing'))
				  )
	
#--Handler----------------------#
#class TC_Handler(Handler): 
		
#	def object__updated_changed(self, info): 
#		if info.initialized: 
#			info.object.b1 = True
#			info.ui.title += "*" 
#	
#	traits_view = View('b1', 'b2', 'b3', 
#					   title="Alter Title on Update") 
#

class main_handler(Handler):

	def read_file(self,info):
		file = file_loader()
		file.configure_traits()	
		
		if file.f != "":
			progress.open('Reading Channel 5 CSV')
			process_thread = Thread(None,info.object.dataset.read_file,"read_file_thread",
								  	 (file.f, file.nx, file.ny, file.nz))						
			process_thread.start()
			
	def read_raw(self,info):
		file = raw_loader()
		file.configure_traits()	
		
		if file.f != "":
			if info.object.dataset.read_raw(file.f)==1:
				print "Success opening file"
			else:
				print "Error opening file"

	def align(self,info):	
		progress.open('Aligning slices')
		process_thread = Thread(None,info.object.dataset.align_slices,"align_thread",())
		process_thread.start()

				
	def segment(self,info):
		seg = segment_dialog() 
		seg.configure_traits()
		
		if seg.is_ok:
			progress.open('Segment')
			process_thread = Thread(None,info.object.dataset.segment,"segment_thread",(seg.min_angle,))
			process_thread.start()
		
	def dilate_grains(self,info):
		progress.open('Dilating grains')
		process_thread = Thread(None,info.object.dataset.dilate_grains,"dilate_thread",())
		process_thread.start()

	def filter_grains(self,info):
		filter = filter_dialog()
		filter.configure_traits()

		if filter.is_ok:
			progress.open('Filter grains')
			process_thread = Thread(None,info.object.dataset.filter_grains,"filter_thread",(filter.volume,))
			process_thread.start()

	traits_view = main_view	
	
class progress_timer(wx.Timer):
	def Notify(self):
		if progress.check():
			self.Stop()
		
timer = progress_timer()
		
class progress_dialog:	

	f = wx.Frame(None,wx.ID_ANY,"Progress",wx.DefaultPosition,(300,95))
	t = wx.StaticText(f,wx.ID_ANY,"",(20,12))
	g = wx.Gauge(f,wx.ID_ANY,100,(20,40),(260,40),wx.GA_HORIZONTAL)
	_finished = False
	
	def open(self,label):
		self.t.SetLabel(label)
		self.f.Centre()
		self.f.Show()
		self.f.MakeModal(True)
		self._finished = False
				
		timer.Start(500)
		
	def check(self):
		if self._finished and self.f.IsShown():
			self.g.SetValue(0)
			self.f.MakeModal(False)
			self.f.Hide()
			return True
		return False
	
	def update(self,p):
		if not self._finished:
			if (p>=0 and p<=1):
				self.g.SetValue(int(p*100))
			if p==1:
				self._finished = True
	


	
progress = progress_dialog()
process_thread = None

#--Main Class-------------------#		
class main_window(HasTraits):
	p = progress_dialog()
	def __init__(self, d):
		self.dataset = d
		self.dataset.set_progress_callback(progress.update)

	f = Int
	g = Int
	

	
	
	