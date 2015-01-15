TCT Unit tests for Tizen:
==========================
1. TCT tool should be setup in linux to execute these test cases. Please refer to "CoreAPI Test Manual.pptx" present in this folder.
2. OIC-CA package should be built first.
3. Copy liboic to <TCT Directory>/core-api/src.
4. Copy packaging/core-liboic-tests.spec to <TCT Directory>/packaging.
5. Build oic test case project with "sudo ./tctbuild build liboic".
6. Install test framework to device with "sudo ./tctbuild install liboic".
7. Run tct tool with "tct-mgr".