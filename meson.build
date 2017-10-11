project(
  'blasr',
  'cpp',
  version : '5.3.0',
  default_options : [
    'buildtype=release',
    'warning_level=3',
    'cpp_std=c++14'],
  license : 'BSD-3',
  meson_version : '>= 0.41.1')

############
# CXXFLAGS #
############

blasr_warning_flags = []
cpp = meson.get_compiler('cpp')
foreach cflag: [
  '-Wno-non-virtual-dtor',
  '-Wno-unused-but-set-variable',
  '-Wno-unused-variable']
    if cpp.has_argument(cflag)
      blasr_warning_flags += cflag
    endif
endforeach

################
# dependencies #
################

# HDF5
libhdf5_cflags = []
if get_option('HDF5_CFLAGS') != ''
  libhdf5_cflags = get_option('HDF5_CFLAGS').split(' ')
endif

libhdf5_libs = []
if get_option('HDF5_LIBS') != ''
  libhdf5_libs = get_option('HDF5_LIBS').split(' ')
endif

# threads
blasr_thread_dep = dependency('threads', required : true)

# pbbam
blasr_pbbam_dep = dependency('pbbam', fallback : ['pbbam', 'pbbam_dep'])

# libblasr
blasr_libblasr_dep = dependency('libblasr', fallback : ['blasr_libcpp', 'libblasr_dep'])

########################
# sources + executable #
########################

# replace version strings
blasr_ver_arr = meson.project_version().split('.')
blasr_major_version = blasr_ver_arr[0]
blasr_minor_version = blasr_ver_arr[1]
blasr_patch_version = blasr_ver_arr[2]

blasr_config = configuration_data()
blasr_config.set('BLASR_VERSION', meson.project_version())
blasr_config.set('BLASR_VERSION_MAJOR', blasr_major_version)
blasr_config.set('BLASR_VERSION_MINOR', blasr_minor_version)
blasr_config.set('BLASR_VERSION_PATCH', blasr_patch_version)

configure_file(
  input : 'BlasrVersion.h.in',
  output : 'BlasrVersion.h',
  configuration : blasr_config)

# replace git commit id
blasr_git_commit_id = vcs_tag(
  input : 'BlasrGitHash.h.in',
  output : 'BlasrGitHash.h',
  command : ['git', 'describe', '--always', '--dirty=*'],
  replace_string : '@BLASR_GIT_SHA1@')

blasr_sources = []
blasr_include_directories = [include_directories('.')]

subdir('extrautils')
subdir('utils')

blasr_static_impl = static_library(
  'blasr_impl',
  [blasr_sources, blasr_git_commit_id],
  install : false,
  dependencies : blasr_libblasr_dep,
  cpp_args : [blasr_warning_flags, '-DUSE_PBBAM=1'])

blasr_main = executable(
  'blasr',
  files('Blasr.cpp'),
  install : true,
  dependencies : [blasr_libblasr_dep, blasr_pbbam_dep, blasr_thread_dep],
  link_with : blasr_static_impl,
  cpp_args : [libhdf5_cflags, blasr_warning_flags, '-DUSE_PBBAM=1', '-DCMAKE_BUILD=1'],
  link_args : libhdf5_libs)

blasr_utils_sawriter = executable(
  'sawriter',
  files('utils/SAWriter.cpp'),
  install : true,
  dependencies : [blasr_libblasr_dep, blasr_pbbam_dep, blasr_thread_dep],
  link_with : blasr_static_impl,
  cpp_args : [libhdf5_cflags, blasr_warning_flags, '-DUSE_PBBAM=1', '-DCMAKE_BUILD=1'],
  link_args : libhdf5_libs)

blasr_utils_toAfg = executable(
  'toAfg',
  files('utils/ToAfg.cpp'),
  install : true,
  dependencies : [blasr_libblasr_dep, blasr_pbbam_dep, blasr_thread_dep],
  link_with : blasr_static_impl,
  cpp_args : [libhdf5_cflags, blasr_warning_flags, '-DUSE_PBBAM=1', '-DCMAKE_BUILD=1'],
  link_args : libhdf5_libs)
