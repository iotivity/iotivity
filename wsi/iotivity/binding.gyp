{
    "targets": [
        {
            "target_name": "NativeExtension",
            "sources": [ "src/NativeExtension.cpp", "src/functions.cpp" ],
            "include_dirs" : [
                 "<!(node -e \"require('nan')\")",
                 "../../resource/include",
                 "../../resource/oc_logger/include",
                 "../../resource/csdk/stack/include",
                 "../../resource/c_common/"
             ],
             "cflags": [
                  "-std=c++11", "-fpermissive", "-Wall", "-fPIC", "-g", "-fexceptions"
             ],
             "cflags_cc": [
                  "-fexceptions","-frtti"
             ],         	
             "libraries": [
                  "-L../../../out/linux/x86_64/release", 
                  "-loc", 
                  "-loc_logger"            
             ]
         }
    ],
}
