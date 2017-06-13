# Teamcenter®  ITK memory management c++ support

The repository contains code for management of ITK reserved memory by scope objects.

The allocated memory is freed automaticaly, regardless if the scope is leaved
by return, jump or exception.

## Functionality
- encapsulate ITK allocations in c++ objects
- ensure automatic memory release on scope end
- support modification of ITK arrays
- support modification of ITK strings

## Usage
Use reference to c++ object instead of memory pointer

```C++
{
	XFYITKString cisName;
	ITEM_ask_name(tItem, &name);
	printf ( "Name: %s\n", cisName );
} // the memory from ITEM_ask_name is released here - end of scope
```
