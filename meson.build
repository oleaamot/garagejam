project('garagejam', 'c',
  version : '0.0.1',
  license : 'GPL')
add_project_arguments('-DDEBUG=0', language : 'c')
name = meson.project_name()
version = meson.project_version()

i18n = import ('i18n')
glib_dep = dependency('glib-2.0')
gobject_dep = dependency('gobject-2.0')
gtk3_dep = dependency('gtk+-3.0')
gst_dep = dependency('gstreamer-1.0')
gst_player_dep = dependency('gstreamer-player-1.0')
gst_plugins_bad_dep = dependency('gstreamer-plugins-bad-1.0')
gst_plugins_base_dep = dependency('gstreamer-plugins-base-1.0')
gst_tag_dep = dependency('gstreamer-tag-1.0')
gst_video_dep = dependency('gstreamer-tag-1.0')
champlain_gtk_dep = dependency('champlain-gtk-0.12')
champlain_dep = dependency('champlain-0.12')
geocode_dep = dependency('geocode-glib-1.0')
libxml_dep = dependency('libxml-2.0')
pangoft2_dep = dependency('pangoft2')
shout_dep = dependency('shout')
libgeoclue_dep = dependency('libgeoclue-2.0')
geoclue_dep = dependency('geoclue-2.0')
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
inc = include_directories('src')
msgfmt = find_program('msgfmt')
po_dir = join_paths(meson.source_root(), 'po')
subdir('data')
subdir('po')
subdir('src')
pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : 'garagejam',
                 version : '0.0.1',
                 name : 'libgaragejam',
                 filebase : 'garagejam',
                 description : 'GarageJam for GNOME')
