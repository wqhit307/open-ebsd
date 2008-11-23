import sys
sys.path.append('..')

#from ebsd import *
#from ebsd_ui import * 

#dataset = ebsd()	

# Enthought library imports.
from enthought.envisage.api import Application
from enthought.pyface.api import GUI
#from enthought.util.wx.do_later import do_later

# Local imports.
from plugin_definitions import PLUGIN_DEFINITIONS
from ebsd_plugin import ebsd_interface

def main():
    gui = GUI()
    
    # Create an Envisage application.
    application = Application(
        argv               = sys.argv,
        id                 = "ebsd.app",
        gui                = gui,
        plugin_definitions = PLUGIN_DEFINITIONS,
    )

    # Start it!
    application.start()
    gui.start_event_loop()    
    application.stop()    

# Application entry point.
if __name__ == '__main__':
    main()


#main_win = main_window(dataset)
#main_handler().configure_traits(context={"object":main_win})

print "..."