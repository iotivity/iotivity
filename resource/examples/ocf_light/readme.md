# Template: IOTivity C++ server

The generated code acts as an simulator:
- it creates values at start update
- handles incoming requests, 
    - stores the values on POST
    - respond on GET by giving out the stored values

## what is generated:
        
- server.cpp implementation code
    - per resource an class is generated.
        - constructor
            - creates the resource
        - has entity handler
            - get function to use the variables to create the return payload
            - post function to assign the variables from the request payload
                - single type: integer, number and strings 
                - arrays 
                    - array members are set for an GET
                    - array members are retrieved from an POST
                    - only arrays of a single type are handled.
                        e.g. array of int, array of strings, array of number
                        NOT handled: array of objects.
                        - TODO: add complex objecs, see garage example of how to next objects (see client and server side)
                - checks on minimum, maximum and readOnly, no update of the value if this occurs 
                - check if the correct interface is used (oic.if.a or oic.if.rw)
    - main 
        - creates all classes
        - creates the device
        - installs the reader for
            - introspection device data file (IDD)
            - security file (SVR) contents
            these files needs to be installed/copied where the the executable is.
           
- svr_server.json  - not used !!!
    default json definition of the secure virtual resources (svr)
    - just works
    - not onboarded
    - not tested, not used yet.
- oic_svr_db_server_mvjustworks.dat
    - copy from iotivity-1.3.1\resource\csdk\security\provisioning
    - security: just works
    - used in CTT testing (e.g. it can onboard the device!), and CTT can talk to the device.
- PICS.json
    - pics for CTT testing
    - security: just works.
 
          
            
## what is missing/incorrect:
- handling query params (none interfaces)
- handling observe
- manual update of resource data, e.g. out of bounds so that one can trigger this to pass CTT.
- creation/deletion of resources (PUT/DELETE functions)
- no correct makefile/scons file, so we do not yet know how to insert this in the IOTivity tree and then compile
    - see for manual changes below

# notes:
- only tested on windows
- readOnly params like: "precision", "maximumsaturation" from oic.r.colour.chroma is crashing the device when running the CTT
    - to avoid this: remove these properties from the generated device.


## SCONS adaption in resource/examples

old :
```
######################################################################
# Source files and Targets
######################################################################
example_names = [
    'simpleserver', 'simpleclient',
    'simpleclientserver',
    'directpairingclient',
    'devicediscoveryserver', 'devicediscoveryclient',
    'simpleserverHQ', 'simpleclientHQ',
    ]

if target_os not in ['windows', 'msys_nt']:
    example_names += [
        'fridgeserver', 'fridgeclient',
        'presenceserver', 'presenceclient',
        'roomserver', 'roomclient',
        'garageserver',
        'garageclient',
        'groupserver',
        'groupclient',
        'lightserver',
        'threadingsample',
        'server',
        'observer',
        ]
    if 'CLIENT' in examples_env.get('RD_MODE'):
        examples_env.AppendUnique(CPPPATH = ['../csdk/resource-directory/include'])
        examples_env.AppendUnique(LIBS = ['resource_directory'])
        example_names += ['rdclient']

examples = map(make_single_file_cpp_program, example_names)

```
new :
```
######################################################################
# Source files and Targets
######################################################################
example_names = [
    'server', 
    ]

if target_os not in ['windows', 'msys_nt']:
    example_names += [
        'fridgeserver', 'fridgeclient',
        'presenceserver', 'presenceclient',
        'roomserver', 'roomclient',
        'garageserver',
        'garageclient',
        'groupserver',
        'groupclient',
        'lightserver',
        'threadingsample',
        'server',
        'observer',
        ]
    if 'CLIENT' in examples_env.get('RD_MODE'):
        examples_env.AppendUnique(CPPPATH = ['../csdk/resource-directory/include'])
        examples_env.AppendUnique(LIBS = ['resource_directory'])
        example_names += ['rdclient']

examples = map(make_single_file_cpp_program, example_names)
```

## WINDOWS run.bat changes:
```
REM *** BUILD OPTIONS ***

if "!RUN_ARG!"=="server" (
  cd %BUILD_DIR%\resource\examples
  REM %DEBUG% simpleserver.exe
  %DEBUG% server.exe
) else if "!RUN_ARG!"=="client" (
```

# build on windows
in top directory:
```
run.bat build server -noTest
```

# run on windows
in top directory:
```
run.bat server
```


## CTT info

When CTT pops up:
"reset to onboarding state" means one needs to: 
1. Stop your device
2. Reset/replace security databases with a new/unowned one.
    e.g. copy the ORIGINAL security file to the executable directory.
3. Start your device.

Note if the IUT crashes during testing one has to reset the security state to not onboarded.
this is mentioned in the test case log of CTT when the CTT can't reset the device state properly.


# CTT PICS information

- IOTivity implements the next optional virtual security resources
    currently there is no mechanism available to remove those from the implementatino.
    hence they must be listed in the PICS:
    - oic.r.crl, oic.r.csr, oic.r.roles 
- oic/res
    This resource must be listed as none observable.
    
    


