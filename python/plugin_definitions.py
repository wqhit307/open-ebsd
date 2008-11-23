""" The absolute filenames of the plugin definitions used in the application.

The only reason that we put this in a separate module is that it often entails
lots of path and filename manipulation code that tends to clutter up 'run.py'.

The important thing to realise is that all we are doing is defining a list of
strings which are the absolute filenames of the plugin definition used in the
application!

"""

# Enthought library imports
from enthought.envisage.api import join

# We use this package to find the absolute location of the plugin definition
# files
from enthought import envisage, mayavi, tvtk

# The plugin definitions required by the application.
PLUGIN_DEFINITIONS = [
    # Envisage plugins
    join(envisage, 'core/core_plugin_definition.py'),
    join(envisage, 'action/action_plugin_definition.py'),
    join(envisage, 'resource/resource_plugin_definition.py'),
    join(envisage, 'workbench/workbench_plugin_definition.py'),
    join(envisage, 'workbench/action/action_plugin_definition.py'),
    
    # Mayavi plugins
    join(tvtk, 'plugins/scene/scene_plugin_definition.py'),
    join(mayavi, 'mayavi_plugin_definition.py'),
    
    # Application plugins
    join('ebsd_plugin_definition.py'),
]

# The plugin definitions that we want to import from but don't want as part of
# the application
INCLUDE = []
