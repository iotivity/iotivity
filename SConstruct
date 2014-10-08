##
# The main build script
#
##
import os

SRC_TOP_DIR = os.path.abspath('.') + '/' #oic-resource project top directory

# Common build options
SConscript('build_common/SConscript')
Import('RELEASE_BUILD', 'BUILD_TARGET', 'TARGET_CPU_ARCH')

# Set build directory
if RELEASE_BUILD:
	BUILD_DIR = SRC_TOP_DIR + '/out/' + BUILD_TARGET + '/' + TARGET_CPU_ARCH + '/release/'
else:
	BUILD_DIR = SRC_TOP_DIR + '/out/' + BUILD_TARGET + '/' + TARGET_CPU_ARCH + '/debug/'
VariantDir(BUILD_DIR, SRC_TOP_DIR, duplicate=0)
Export('SRC_TOP_DIR', 'BUILD_DIR')

if BUILD_TARGET == 'arduino':
	SConscript('arduino.scons')

# Build libcoap
SConscript(BUILD_DIR + 'csdk/libcoap/SConscript')

# Build liboctbstack
SConscript(BUILD_DIR + 'csdk/SConscript')

if BUILD_TARGET != 'arduino':
	# Build liboc_logger
	SConscript(BUILD_DIR + 'oc_logger/SConscript')

	# Build liboc
	SConscript(BUILD_DIR + 'src/SConscript')

	# Build examples
	SConscript(BUILD_DIR + 'examples/SConscript')

# Print targets
Import('env')

Help('''
===============================================================================
Targets:\n    ''')
for t in env.get('TS'):
	Help(t + ' ')
Help('''
\nDefault all targets will be built. You can specify the target to build:

    #scons [options] [target]
===============================================================================
''')