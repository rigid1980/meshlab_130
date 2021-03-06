#
# meshlab_mini.pro
#
# This is a minimal project file for compiling as less as possible and having a minimal meshlab setup.
# Compiling this minimal subset does not require any additional library (except obviously qt and vcg).
#

TEMPLATE      = subdirs
CONFIG       += ordered


                           # the common framework, used by all the plugins,
SUBDIRS       = common \ 
				meshlab \                         # the GUI framework
               meshlabplugins/io_base\           # a few basic file formats (ply, obj, off), without this you cannot open anything
#			   meshlabplugins/filter_meshing \
teichplugins/dual_widget     \
teichplugins/mesh_grid       \
teichplugins/landmarkcapture        \
teichplugins/landmark_serialize     \
#teichplugins/algo_demo              \
teichplugins/algo_teich              \
teichplugins/map_cons              \

external/map_cons              \

#teichplugins/switch_mesh     \
sampleplugins/sampleedit    \

            meshlabplugins/io_base \
#	    meshlabplugins/io_collada \
#meshlabplugins/io_3ds \
#meshlabplugins/io_u3d \
#meshlabplugins/filter_poisson \
#
# Next some other useful, but still easy to be compiled, plugins
# Uncomment them if you succeed in compiling the above ones.
#                meshlabplugins/edit_select \
#                 meshlabplugins/filter_create\
#                 meshlabplugins/filter_mutualinfoxml \
#                 meshlabplugins/decorate_background \
#                meshlabplugins/filter_select \
#                meshlabplugins/decorate_base \
#                meshlabplugins/filter_colorize\
#                meshlabplugins/filter_measure\
#                meshlabplugins/filter_sampling\
#                meshlabplugins/filter_layer\
#                meshlabplugins/filter_bnpts \
#                meshlabplugins/edit_manipulators \
#                meshlabserver \
#

