# Shimmer

Shimmer is an ANSI C library for streaming data to multiple programs using shared memory buffers.


Planned features:
* breakpoints
* circular buffers for streaming frequent and small objects
* double-buffers for infrequent large objects and bulk communication
* tags on buffers and breakpoints for user/tool-defined labeling and sorting
* shared enable flag to keep shared memory disabled when not being actively read by a parent program
