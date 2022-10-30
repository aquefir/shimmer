# Shimmer

---

Shimmer is a Linux C library for debugging and diagnostics through shared memory. There are several advantages to this:

* Shared memory is extremely cheap and fast
* Shared memory can persist after a program crashes
* Both read and write are supported for supervisor programs

Shimmer is currently optimized for a small number of shared memory segments; generally meant to be arrays, memory arenas, etc. It currently does not support deleting segments, and only a finite number of instances can be allocated (set during initialization, user configurable). It also is not safe for both programs to be adding new shared memory objects at the same time, and any synchronization is a problem for the user. Some of these details may change in future versions.

For now the main features are shared buffers and shared locks.
