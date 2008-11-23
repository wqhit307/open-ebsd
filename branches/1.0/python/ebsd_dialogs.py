from enthought.traits.api import Delegate, HasTraits, Int, Str, Float, Instance, \
								 Array, Bool, Callable, Enum, Trait, Range, Property, File
from types import StringType 

from enthought.traits.ui.api import View, Item, Group, VGrid
from enthought.traits.ui.menu import MenuBar, Menu, Action, OKButton, CancelButton
from enthought.traits.ui.handler import Handler
import wx 

#-------------------------------#
# Progress Dialog Box			#
#-------------------------------#
			
class progress_dialog:	

	f = wx.Frame(None,wx.ID_ANY,"Progress",wx.DefaultPosition,(300,95))
	t = wx.StaticText(f,wx.ID_ANY,"",(20,12))
	g = wx.Gauge(f,wx.ID_ANY,100,(20,40),(260,40),wx.GA_HORIZONTAL)
	_open = False
	callback = None
	
	def open(self,label,callback = None):
		self.t.SetLabel(label)
		self._label = label
		self.f.Centre()
		self.f.Show()
		self.f.MakeModal(True)
		self._finished = False
		self._open = True
	
		if callback != None:
			self.callback = callback
	
	def close(self):
		if self._open:
			self.g.SetValue(0)
			self.f.MakeModal(False)
			self.f.Hide()
			self._open = False
			self._finished = False
			self.callback()
			
	def check(self):
		if self._finished and self.f.IsShown():
			self.g.SetValue(0)
			self.f.MakeModal(False)
			self.f.Hide()
			self._open = False
			self._finished = False
			
			return True
		return False
	
	def update_label(self, text):
		self.t.SetLabel(text)
		
	def update(self, p, finished = False, text = ''):
		if finished:
			wx.CallAfter(self.close)
		else:
			if not self._open:
				self.open(text,)
			if text != '' and text != self._label:
				wx.CallAfter(self.update_label,text)
			if (p>=0 and p<=1):
				wx.CallAfter(self.g.SetValue,int(p*100))
		

class dlg_handler(Handler):
	def close(self,info,is_ok):
		info.object.is_ok = is_ok
		return True


#-------------------------------#
# Segmentation Dialog Box		#
#-------------------------------#
		
class segment_dialog(HasTraits):
	min_angle = Float(5)
   	is_ok = Bool(False)
   	
   	traits_view = View(
					Item(name='min_angle', label='Minimum Angle'),
					kind="modal",
					title="Segment Data",
				   	handler = dlg_handler(),
				   	buttons = [OKButton, CancelButton])


#-------------------------------#
# Filter Dialog Box				#
#-------------------------------#

class filter_dialog(HasTraits):		
	volume = Int(100)
 	is_ok = Bool(False)

   	traits_view = View(
					Item(name='volume', label='Minimum Grain Volume'),
					kind="modal",
					title="Filter Grains",
					handler = dlg_handler(),
				   	buttons = [OKButton, CancelButton])
