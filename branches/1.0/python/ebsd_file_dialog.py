from enthought.traits.api import Delegate, HasTraits, Int, Str, Float, Instance, \
								 Array, Bool, Callable, Enum, Trait, Range, Property, File
from types import StringType 

from enthought.traits.ui.api import View, Item, Group, VGrid
from enthought.traits.ui.menu import MenuBar, Menu, Action, OKButton, CancelButton
from enthought.traits.ui.handler import Handler

from enthought.pyface.file_dialog import FileDialog

import wx 

#-------------------------------#
# Handler						#
#-------------------------------#

class dlg_handler(Handler):
	def close(self,info,is_ok):
		info.object.is_ok = is_ok
		return True

#-------------------------------#
# Validator Functions			#
#-------------------------------#
 
def positive_definite(object, name, value): 
	if value <= 0: 
		return 1
	return value
		
#-------------------------------#
# Raw Dialog Box				#
#-------------------------------#

class raw_loader(HasTraits):
	is_ok = Bool(False)
	f = File("/Users/sean/ebsd/Mac/processed")
   	traits_view = View(
					Item(name='f', label='Select File'),
					width=600,
					kind="modal",
					title="Read Raw File",
					handler = dlg_handler(),
				   	buttons = [OKButton, CancelButton]
				  )

#-------------------------------#
# File Dialog Box				#
#-------------------------------#

class file_loader(HasTraits):
	is_ok = Bool(False)
	f = File("/Users/sean/ebsd/new.txt")
	nx = Trait(200,Int,positive_definite)
	ny = Trait(200,Int,positive_definite)
	nz = Trait(11,Int,positive_definite)
	stepx = Trait(1.0,Float,positive_definite)
   	stepy = Trait(1.0,Float,positive_definite)
   	stepz = Trait(1.4,Float,positive_definite)

   	traits_view = View(
					Item(name='f', label='Select File'),
					Group(
						VGrid(
							Item(name='nx', label='X'),
							Item(name='ny', label='Y'),
							Item(name='nz', label='Z'), 
							columns=3),
						label="Dimensions",
						show_border=True),
					Group(
						VGrid(
							Item(name='stepx', label='X'),
							Item(name='stepy', label='Y'),
							Item(name='stepz', label='Z'), 
							columns=3),
						label="Step Size",
						show_border=True),
				   	width=600,
				   	kind="modal",
					title="Read Channel 5 CSV File",
					handler = dlg_handler(),
				  	buttons = [OKButton, CancelButton]
				  )