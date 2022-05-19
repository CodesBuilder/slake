#ifndef __HAKEDEF_H__
#define __HAKEDEF_H__

typedef enum _HkValueType
{
	VALUE_TYPE_STR = 0,
	VALUE_TYPE_INT,
	VALUE_TYPE_LONG,
	VALUE_TYPE_UINT,
	VALUE_TYPE_ULONG,
	VALUE_TYPE_NULL
} HkValueType;

typedef struct _HkValue
{
	union
	{
		int i32;
		unsigned int u32;
		long long i64;
		unsigned long long u64;
		char *str;
	} data;
	HkValueType type;
} HkValue;

#endif
