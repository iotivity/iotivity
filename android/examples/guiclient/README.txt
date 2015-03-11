README for guiclient

The guiclient is an example of a multi-threaded client app with separate GUI, IoTivity worker, Resource and Attribute objects.  

Currently supported resources for this app include:
1) core.light (e.g. from a simpleserver server app)
2) core.edison.resources (e.g. from edisonserver server app)
	* more info on building and running edisonserver on an edison board can be found on the IoTivity Edison wiki at:
		https://wiki.iotivity.org/running_sample_codes_in_iotivity_0.9_sdk_on_edison#build_and_execute_iotivity_sample_codes-oicsensorboard

USAGE:
1) Launch guiclient (debug or release) on an Android device.  The device must be connected to the same subnet as the server (e.g. the same subnet as the simpleserver app, or Edison board running edisonserver app).
2) Tap the "Refresh" icon in the title bar to discover resources
3) Expand a Resource using the dropdown arrow alongside it to view its Attributes
4) For Read-only Attributes, tap the Attribute item to refresh its value
5) For Read/Write objects, use the slider or switch to set the value
6) Tap the "Garbage" icon to clear resources and start over

DESIGN OVERVIEW:

The MainActivity launches and constructs and launches an "OcWorker" object.  

The OcWorker object is a "servant" object, with its own thread.  OcWorker initializes and then awaits further event messages.  OcWorker has two messages queues that it processes: one for commands from MainActivity, and one for results from the IoTivity layer.

MainActivity will send OcWorker messages (e.g. "Discover Resources").  OcWorker then handles interacting with the IoTivity API to perform the function requested by MainActivity, and later, notifies MainActivity of results via a callback to MainActivity's "OcWorkerListener" interface.  Typically these results are new (or updated) OcResourceInfo objects, each of which may have one or more OcAttributeInfo objects attached to it.

MainActivity renders the OcResourceInfo List created by OcWorker, using an ExpandableResourceListAdapter object.

See code and comments for more detail.

EXTENDING GUICLIENT:
Guiclient is written to be readily extended to support additional resource types.  See OcProtocolStrings to introduce a new supported resource, and then search the code for the string constants defined therein to follow the pattern for supporting a new type throughout the code.