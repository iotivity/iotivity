Convenience Methods Added to 'env' by IoTivity Project
======================================================

Methods
---------

`AddPthreadIfNeeded(ienv)`
:   Set a linker flag to enable POSIX threads, if needed.
    
    Args:
        ienv: construction environment of caller

`AppendTarget(ienv, name, targets=None, default=True)`
:   Add targets to target list.
    
    The target 'name' is added to either the list of default targets
    or non-default targets, which are later handed to the Help function
    to help display options to the user.  If this is a default target,
    it is also added to the DefaultTargets alias.
    
    Args:
        ienv: the construction environment of the caller.
        name: target name to add.
        targets: aliases the 'name' target will expand to.
        default: whether this is a default target or not.
    
    See Also:
        AppendTarget

`InstallTarget(ienv, targets, name=None, default=True)`
:   Copy a built target to the common location.
    
    The iotivity build puts the libraries and executables in a common
    location to make it easy to run the executables in place without
    worrying about the rpath (for systems that have it), or to account
    for no rpath option being available (Windows systems).  This not
    the same as "installing", see the UserInstall* methods for that.
    
    As side effects, the target is added to either the list of default
    targets or non-default targets, which are later handed to the Help
    function to help display options to the user.  If this is a default
    target, it is also added to the DefaultTargets alias.
    
    Args:
        ienv: the construction environment of the caller.
        targets: list of targets to copy.
        name: if set, the name to copy to; else use the target name.
        default: whether this is a default target or not.
    
    Notes:
        If used for copying a library, a race condition may occur on
        Windows systems if parallel building is enabled.  The separate
        operations from the SharedLibrary builder and the Command builder
        calling Copy may see Python release the Global Interpreter Lock
        in between steps, and another task may conclude the library
        is ready before the copy has taken place, and then report a
        missing dependency. A workaround is to generate the library
        directly into the common location so it does not need to be
        copied after building.
    
    See Also:
        AppendTarget

`PrintTargets(env)`
:   Add message about IoTivity build targets to help message.
    
    Args:
        env: construction envrionment

`ScanJSON(env, directory=None)`
:   scanner

`SetDir(env, dir)`
:   Set up the target directory for building.
    
    Sets up the scons Variant directory, which is where objects
    built under scons control will go, rather than in the source tree.
    Also shortcuts some of the available scons mechansims by saving the
    paths in the construction environment. Since this sets the
    variant dir, this should be called only once, using the main
    main construction environment. These are retrieved by:
        env.get('SRC_DIR')
        env.get('BUILD_DIR')
    
    SRC_DIR will be the passed directory, while BUILD_DIR is
    constructed based on target system/arch and release mode.
    On Windows:
        'dir'/out/windows/<win32 or uwp>/<target_arch>/<release or debug>/
    
    On all others:
        'dir'/out/<target_os>/<target_arch>/<release or debug>/
    
    Args:
        env: construction environment
        dir: the source directory

`SrcToObj(env, src, home='')`
:   Build an object in BUILD_DIR.
    
    Uses BUILD_DIR, SetDir should have been called previously.
    Note this is used only in the android compat script. 
    
    Args:
        env: construction environment
        src: source directory
        home: the part of the path to chop off

`UserInstallTargetBin(ienv, targets, name=None)`
:   Install built binaries to a "system" location.
    
    Install files to system location, when running "scons install".
    If PREFIX is set it is used with a 'bin' suffix, else a 'deploy'
    subdirectory of the common build location is used.
    
    Args:
        ienv: construction environment of caller
        targets: libraries to install
        name: currently unused (future: installation name)

`UserInstallTargetExtra(ienv, targets, subdir=None)`
:   Install files to system location.
    
    Install "extra" files not covered by the other system-install methods,
    such as tests, xml files, security data files, etc., when doing
    "scons install". If PREFIX is set it is used with a 'lib/iotivity' suffix,
    unless LIB_INSTALL_DIR is also set in which case it is used with an
    'iotivity' suffix. If neither, a 'deploy/extra' subdirectory of the
    common build location is used.  If subdir is set, it is added to
    the end of whichever install path is selected. If not set, the
    tail of the path is computed 
    
    Args:
        ienv: construction environment of caller
        targets: files to install
        subdir: if set, use as the tail of the installation path

`UserInstallTargetHeader(ienv, targets, dir='', name=None)`
:   Install header files to a "system" location.
    
    Install files to system location, when running "scons install".
    If PREFIX is set it is used with an 'include/iotivity' suffix, else a
    'deploy/include' subdirectory of the common build location is used.
    If dir is present, it as appended to the installation path.
    
    Args:
        ienv: construction environment of caller
        targets: headers to install
        dir: additional subdirectory to use in install location
        name: currently unused (future: installation name)

`UserInstallTargetLib(ienv, targets, name=None)`
:   Install built libraries to a "system" location.
    
    Install files to system location, when running "scons install".
    If PREFIX is set it is used with a 'lib' suffix, unless
    LIB_INSTALL_DIR is also set in which case it is used unchanged.
    If neither, a 'deploy' subdirectory of the common build location
    is used. Unless deploy was used, the rpath is stripped.
    
    Args:
        ienv: construction environment of caller
        targets: libraries to install
        name: currently unused (future: installation name)

`UserInstallTargetPCFile(ienv, targets, name=None)`
:   Install pkg-config files to a "system" location.
    
    Install files to system location, when running "scons install".
    If PREFIX is set it is used with a 'lib/pkgconfig' suffix, unless
    LIB_INSTALL_DIR is also set in which case it is used with a 'pkgconfig'
    suffix.  If neither, a 'deploy/pkgconfig' subdirectory of the common
    build location is used.
    
    Args:
        ienv: construction environment of caller
        targets: files to install
        name: currently unused (future: installation name)

