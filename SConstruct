##
# The main build script
#
##

import os

# List of targets that only support the IoTivity C SDK.
targets_csdk_only = ['arduino','darwin','ios']

# Load common build config
SConscript('build_common/SConscript', exports = 'targets_csdk_only')

Import('env')

target_os = env.get('TARGET_OS')

if target_os not in targets_csdk_only:
	# Prepare libraries
	env.PrepareLib('cereal')
	env.PrepareLib('expat')
	env.PrepareLib('boost', 'boost_thread', os.path.join(env.get('SRC_DIR'), 'extlibs', 'boost'))
	env.PrepareLib('boost', 'boost_system', os.path.join(env.get('SRC_DIR'), 'extlibs', 'boost'))
else:
	if target_os == 'arduino':
		SConscript('arduino.scons')

# By default, src_dir is current dir, the build_dir is:
#     ./out/<target_os>/<target_arch>/<release or debug>/
#
# The build_dir is a variant directory of the source directory(You can
# consider build_dir as a soft link to src_dir, for detail please refer to:
#     http://www.scons.org/doc/production/HTML/scons-user.html#f-VariantDir
#
# Any way, to make the output is in build_dir, when load scripts, the path should
# be relevant to build_dir.
build_dir = env.get('BUILD_DIR')

# Build 'resource' sub-project
SConscript(os.path.join(build_dir, 'resource', 'SConscript'),
           exports = 'targets_csdk_only')

# Build 'service' sub-project
SConscript(os.path.join(build_dir, 'service', 'SConscript'),
           exports = 'targets_csdk_only')

# Append targets information to the help information, to see help info, execute command line:
#     $ scon [options] -h
env.PrintTargets()

# Print bin upload command line (arduino only)
if target_os == 'arduino':
	env.UploadHelp()
