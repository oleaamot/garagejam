as_version = meson.project_version() # set in project() below
ver_arr = as_version.split('.')
as_major_version = ver_arr[0]
as_minor_version = ver_arr[1]
as_micro_version = ver_arr[2]
conf = configuration_data()
conf.set_quoted('PACKAGE_STRING', as_version)
conf.set_quoted('VERSION', as_version)
conf.set_quoted('GARAGEJAM_DATADIR', join_paths(join_paths(get_option('prefix'), get_option('datadir')), meson.project_name()))
configure_file(output : 'config.h', configuration : conf)
garagejam_sources = ['garagejam-gingerblue-app.c', 'garagejam-gingerblue-config.c', 'garagejam-gingerblue-file.c', 'garagejam.c', 'garagejam.h', 'garagejam-gingerblue-knob.c', 'garagejam-gingerblue-line.c', 'garagejam-gingerblue-main.c', 'garagejam-gingerblue-record.c','garagejam-gingerblue-main-loop.c','garagejam-gingerblue-studio-config.c','garagejam-gingerblue-studio-player.c','garagejam-gingerblue-studio-stream.c']
garagejamlibrary = shared_library('garagejam', garagejam_sources, dependencies : [geoclue_dep, glib_dep, gtk3_dep, gst_dep, gobject_dep, gst_player_dep, gst_plugins_bad_dep, gst_plugins_base_dep, champlain_gtk_dep, champlain_dep, geocode_dep, gst_tag_dep, gst_video_dep, libgeoclue_dep, libxml_dep, m_dep, pangoft2_dep, shout_dep], install : true)
install_data(
    meson.project_name() + '.xml.in',
    install_dir: join_paths(get_option('datadir'), 'garagejam')
)
gnome = import('gnome')
garagejam_schemas = gnome.compile_schemas()
garagejam = executable('garagejam', garagejam_sources, dependencies: [geoclue_dep, glib_dep, gtk3_dep, gst_dep, champlain_gtk_dep, champlain_dep, geocode_dep, gobject_dep, gst_plugins_bad_dep, gst_plugins_base_dep, gst_player_dep, gst_tag_dep, gst_video_dep, libgeoclue_dep, libxml_dep, m_dep, pangoft2_dep, shout_dep], link_with: garagejamlibrary, install: true)
