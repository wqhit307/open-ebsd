from enthought.envisage.api import PluginDefinition
from enthought.envisage.core.core_plugin_definition import ApplicationObject
from enthought.envisage.workbench.workbench_plugin_definition import \
     Workbench, View, Perspective
from enthought.envisage.workbench.action.action_plugin_definition import \
	WorkbenchActionSet, Action, ActionSet, Group, Location, Menu

	
# Plugin extensions
my_app_obj = ApplicationObject(class_name='ebsd_plugin.ebsd_interface', uol='service://ebsd_plugin.ebsd')

views = [View(name='Data',
              id='ebsd_plugin.Data',
              uol='service://ebsd_plugin.ebsd',
              traits_ui_view='view_data',
              ),
#         View(name='2D Plot',
#              id='ebsd_plugin.2DPlot',
#              uol='service://ebsd_plugin.ebsd',
#              traits_ui_view='view_plot2d',
#              ), 
         View(name='Suspicious Slices',
              id='ebsd_plugin.suspicious_list',
              uol='service://ebsd_plugin.ebsd',
              traits_ui_view='view_suspicious',
              )
         ]
perspectives = [
    Perspective(id='ebsd_plugin.default_perspective',
                name='EBSD View',
                contents =
                [ Perspective.Item(id='ebsd_plugin.ebsd_interface',
                                   position='right',
                                   width=0.05),
                  Perspective.Item(id='ebsd_plugin.Data',
                                   position='left'),
                ],
                enabled=True,
                )
    ]
workbench = Workbench(views=views, perspectives=perspectives,
                      default_perspective='ebsd_plugin.default_perspective')
               
APP_SERVICE = "service://ebsd_plugin.ebsd"               
               
actionset = WorkbenchActionSet(
				id   = "ebsd_plugin.action_set",
				name = "ebsd Menus",
				
				groups = [
					Group(
						id = "ProcessingMenuGroup",
						location = Location(path="MenuBar"),
					),
					Group(
						id = "VisualisationMenuGroup",
						location = Location(path="MenuBar"),
					),
					Group(
						id = "AlignmentMenuGroup",
						location = Location(path="MenuBar/ProcessingMenu"),
					),
					Group(
						id = "OpenGroup",
						location = Location(path="MenuBar/FileMenu"),
					),
					Group(
						id = "SaveGroup",
						location = Location(path="MenuBar/FileMenu"),
					),
				],
			
				menus = [
					Menu(
						id = "ProcessingMenu",
						name = "&Processing",
						location = Location(path="MenuBar/ProcessingMenuGroup"),
			
						groups = [
							Group(id="ProcessingGroup")
						]
					),
					
					Menu(
						id = "VisualisationMenu",
						name = "&Visualisation",
						location = Location(path="MenuBar/VisualisationMenuGroup"),
			
						groups = [
							Group(id="VisualisationGroup")
						]
					),
					
					Menu(
						id = "AlignmentMenu",
						name = "&Alignment",
						location = Location(path="MenuBar/ProcessingMenu/AlignmentMenuGroup"),
			
						groups = [
							Group(id="AlignmentGroup")
						]
					),
					
				],
			
				actions = [
					Action(
						id = "read_file",
						name = "Open Channel 5 CSV...",
						accelerator = "Ctrl-O",
						method_name = "read_file",
						object = APP_SERVICE,
						locations = [Location(path="MenuBar/FileMenu/OpenGroup")]),

					Action(
						id = "read_raw",
						name = "Open Raw File...",
						accelerator = "Ctrl-Shift-O",
						method_name = "read_raw",
						object = APP_SERVICE,
						locations = [Location(path="MenuBar/FileMenu/OpenGroup")]),
						
					Action(
						id = "write_raw",
						name = "Save Raw File...",
						accelerator = "Ctrl-S",
						method_name = "write_raw",
						object = APP_SERVICE,
						locations = [Location(path="MenuBar/FileMenu/SaveGroup")]),	
						
					Action(
						id = "segment",
						name = "Segment...",
						accelerator = "Ctrl-Shift-S",
						object = APP_SERVICE,
						method_name = "segment",
						locations = [Location(path="MenuBar/ProcessingMenu/ProcessingGroup")]),	  	
		
					Action(
						id = "reconstruct",
						name = "Reconstruct Grains",
						accelerator = "Ctrl-R",
						object = APP_SERVICE,
						method_name = "reconstruct",
						locations = [Location(path="MenuBar/ProcessingMenu/ProcessingGroup")]),		

					Action(
						id = "calculate_extra_stats",
						name = "Calculate Grain Surfaces",
						accelerator = "Ctrl-Shift-S",
						object = APP_SERVICE,
						method_name = "calculate_extra_stats",
						locations = [Location(path="MenuBar/ProcessingMenu/ProcessingGroup")]),		

		
					Action(
						id = "dilate_grains",
						name = "Dilate Grains",
						accelerator = "Ctrl-D",
						object = APP_SERVICE,
						method_name = "dilate_grains",
						locations = [Location(path="MenuBar/ProcessingMenu/ProcessingGroup")]),			  	
		
					Action(
						id = "filter_grains",
						name = "Filter Grains...",
						accelerator = "Ctrl-F",
						object = APP_SERVICE,
						method_name = "filter_grains",
						locations = [Location(path="MenuBar/ProcessingMenu/ProcessingGroup")]),	
		
					Action(
						id = "align_slices",
						name = "Align Slices",
						accelerator = "Ctrl-Shift-A",
						object = APP_SERVICE,
						method_name = "align_slices",
						locations = [Location(path="MenuBar/ProcessingMenu/AlignmentMenu/AlignmentGroup")]),		

					Action(
						id = "calculate_offsets",
						name = "Calculate Offsets",
						accelerator = "Ctrl-A",
						object = APP_SERVICE,
						method_name = "calculate_offsets",
						locations = [Location(path="MenuBar/ProcessingMenu/AlignmentMenu/AlignmentGroup")]),		

						
					Action(
						id = "grain_vis",
						name = "Visualise Selected Grain",
						accelerator = "Ctrl-G",
						method_name = "render_grain",
						object = APP_SERVICE,
						locations = [Location(path="MenuBar/VisualisationMenu/VisualisationGroup")]),

					Action(
						id = "boundary_vis",
						name = "Visualise Boundary Network",
						accelerator = "Ctrl-B",
						method_name = "render_boundary_network",
						object = APP_SERVICE,
						locations = [Location(path="MenuBar/VisualisationMenu/VisualisationGroup")]),

					Action(
						id = "rf_vis",
						name = "Visualise RF",
						accelerator = "Ctrl-Shift-R",
						method_name = "rf_vis",
						object = APP_SERVICE,
						locations = [Location(path="MenuBar/VisualisationMenu/VisualisationGroup")]),

					Action(
						id = "remove_grain_vis",
						name = "Remove Selected Grain",
						accelerator = "Ctrl-Shift-G",
						method_name = "remove_grain",
						object = APP_SERVICE,
						locations = [Location(path="MenuBar/VisualisationMenu/VisualisationGroup")]),


				]
			)                      
# Plugin Definition
PluginDefinition(
    
    # The plugin's globally unique identifier
    id = 'ebsd_plugin.ebsdPlugin',

    # General info about the plugin
    name = "ebsd Plugin",
    version = "1.0.0",
    provider_name = "Sean Warren",
    enabled = True,
    autostart = True,

    # The ids of plugins that this plugin requires
    requires = [ 'enthought.envisage.workbench', 'enthought.mayavi' ],

    # The extension points offered by this plugin
    extension_points = [WorkbenchActionSet],

    # The contributions that this plugin makes to extension points
    extensions = [ workbench, actionset, my_app_obj ],
)
