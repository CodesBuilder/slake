#ifndef __UTIL_OBJECT_H__
#define __UTIL_OBJECT_H__

typedef struct _UtilClass
{
	void (*constructor)();
	void (*destructor)();
}UtilClass;

#endif
