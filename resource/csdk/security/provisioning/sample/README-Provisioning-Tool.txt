LAST UPDATED 7/16/2015

To execute Provisioning Tool sample:

1) Build IoTivity with security enabled:
	$ cd <iotivity-base>
	$ scons resource SECURED=1

2) Verify Provisioning Tool functionality using secure sample apps:

    Run Resource Server Device which needs to be 'provisioned':
	$ ./sampleserver_justworks (Just Works)
	or
	$ ./sampleserver_randompin (Random Pin)

    Run Provisioning Tool Device:
	$ ./provisioningclient

    Follow the prompts on Provisioning Tool device and provisioning should be completed
    successfully.

    All security functionality operate using CBOR data (DAT files).
    JSON files are for reference only as they are human-readable.
    JSON files are not used by security-related functions.

    If you wish to test functionality with data file different from the 
    provided default DAT file, modify the JSON files 
    (oic_svr_db_server_justworks.JSON, oic_svr_db_server_randompin.JSON)
    and then use the JSON-to-CBOR conversion tool 
    (<iotivity-base>/out/<...>/release/resource/csdk/security/tool/json2cbor)
    to create a new DAT file.
