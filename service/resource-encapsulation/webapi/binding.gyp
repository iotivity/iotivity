{
  "targets": [
    {
      "target_name": "JSIoTivityRE",
      "sources": [
                   "src/JSREClient.cc",  
		     "src/AsyncWorkerThread.cc",
		     "src/CommonUtil.cc",
		     "src/RECommon.cc",
		      ],
      
	"include_dirs": [
	'<!@(echo "$(pwd)")',
	'<!@(echo "$(pwd)")/src/inc',
       '<!@(echo "$(pwd)")/../include',
       '<!@(echo "$(pwd)")/../src/common/primitiveResource/include',
       '<!@(echo "$(pwd)")/../src/common/expiryTimer/include',
       '<!@(echo "$(pwd)")/../src/common/utils/include',
       '<!@(echo "$(pwd)")/../resourceBroker/include',
       '<!@(echo "$(pwd)")/../resourceCache/include',
       '<!@(echo "$(pwd)")/../../../resource/csdk/stack/include',
       '<!@(echo "$(pwd)")/../../../resource/c_common',
       '<!@(echo "$(pwd)")/../../../resource/include',
       '<!@(echo "$(pwd)")/../../../resource/oc_logger/include',
       ],
      
	"cflags_cc": [ '-std=c++11','-frtti', '-fexceptions' ],
      
	"cflags": [ '-std=c++11' ],
      
	"libraries":["-loc", 
		"-lrcs_common", 
		"-loctbstack", 
		"-loc_logger",
		"-lrcs_client", 
		'<!@(echo "-L$(pwd)")/../../../out/linux/x86/release', '<!@(echo "-Wl,-rpath $(pwd)/../../../out/linux/x86/release")']
    }
  ]
}
