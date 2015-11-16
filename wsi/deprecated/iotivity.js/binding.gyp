{
    "targets": [
        {
            "target_name": "NativeExtension",
            "sources": [
                 "src/NativeExtension.cpp",
                 "src/FindResource.cpp",
                 "src/GetData.cpp",
                 "src/PostData.cpp",
                 "src/PutData.cpp",
                 "src/DeleteData.cpp",
                 "src/ObserveResource.cpp",
                 "src/CancelObserve.cpp",
                 "src/common.cpp"
            ],
            "include_dirs" : [
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

