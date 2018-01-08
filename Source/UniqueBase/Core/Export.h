#pragma once

#define DEFINE_FUNC_1(clss)

#define DEFINE_FUNC_2(clss, typeRet)

#define DEFINE_FUNC_3(clss, typeRet, func)\
UNIQUE_C_API typeRet clss##_##func(clss* self)\
{\
	return (typeRet)self->func();\
}

#define DEFINE_FUNC_4(clss, typeRet, func, type1)\
UNIQUE_C_API typeRet clss##_##func(clss* self, type1 param1)\
{\
	return (typeRet)self->func(param1);\
}

#define DEFINE_FUNC_5(clss, typeRet, func, type1, param1)\
UNIQUE_C_API typeRet clss##_##func(clss* self, type1 param1)\
{\
	return (typeRet)self->func(param1);\
}

#define DEFINE_FUNC_6(clss, typeRet, func, type1, param1, type2)\
UNIQUE_C_API typeRet clss##_##func(clss* self, type1 param1, type2 param2)\
{\
	return (typeRet)self->func((type1)param1, (type2)param2);\
}

#define DEFINE_FUNC_7(clss, typeRet, func, type1, param1, type2, param2)\
UNIQUE_C_API typeRet clss##_##func(clss* self, type1 param1, type2 param2)\
{\
	return (typeRet)self->func((type1)param1, (type2)param2);\
}

#define DEFINE_FUNC_8(clss, typeRet, func, type1, param1, type2, param2, type3)\
UNIQUE_C_API typeRet clss##_##func(clss* self, type1 param1, type2 param2, type3 param3)\
{\
	return (typeRet)self->func((type1)param1, (type2)param2, (type3)param3);\
}

#define DEFINE_FUNC_9(clss, typeRet, func, type1, param1, type2, param2, type3, param3)\
UNIQUE_C_API typeRet clss##_##func(clss* self, type1 param1, type2 param2, type3 param3)\
{\
	return (typeRet)self->func((type1)param1, (type2)param2, (type3)param3);\
}

#define DEFINE_FUNC_10(clss, typeRet, func, type1, param1, type2, param2, type3, param3, type4)\
UNIQUE_C_API typeRet clss##_##func(clss* self, type1 param1, type2 param2, type3 param3, type4 param4)\
{\
	return (typeRet)self->func((type1)param1, (type2)param2, (type3)param3, (type4)param4);\
}

#define DEFINE_FUNC_11(clss, typeRet, func, type1, param1, type2, param2, type3, param3, type4, param4)\
UNIQUE_C_API typeRet clss##_##func(clss* self, type1 param1, type2 param2, type3 param3, type4 param4)\
{\
	return (typeRet)self->func((type1)param1, (type2)param2, (type3)param3, (type4)param4);\
}

#define DEFINE_STATIC_FUNC_1(clss)

#define DEFINE_STATIC_FUNC_2(clss, typeRet)


#define DEFINE_STATIC_FUNC_3(clss, typeRet, func)\
UNIQUE_C_API typeRet clss##_##func()\
{\
	return (typeRet)clss::func();\
}

#define DEFINE_STATIC_FUNC_4(clss, typeRet, func, type1)\
UNIQUE_C_API typeRet clss##_##func(type1 param1)\
{\
	return (typeRet)clss::func(param1);\
}

#define DEFINE_STATIC_FUNC_5(clss, typeRet, func, type1, param1)\
UNIQUE_C_API typeRet clss##_##func(type1 param1)\
{\
	return (typeRet)clss::func((type1)param1);\
}

#define DEFINE_STATIC_FUNC_6(clss, typeRet, func, type1, param1, type2)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2);\
}

#define DEFINE_STATIC_FUNC_7(clss, typeRet, func, type1, param1, type2, param2)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2);\
}

#define DEFINE_STATIC_FUNC_8(clss, typeRet, func, type1, param1, type2, param2, type3)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2, type3 param3)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2, (type3)param3);\
}

#define DEFINE_STATIC_FUNC_9(clss, typeRet, func, type1, param1, type2, param2, type3, param3)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2, type3 param3)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2, (type3)param3);\
}

#define DEFINE_STATIC_FUNC_10(clss, typeRet, func, type1, param1, type2, param2, type3, param3, type4)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2, type3 param3, type4 param4)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2, (type3)param3, (type4)param4);\
}

#define DEFINE_STATIC_FUNC_11(clss, typeRet, func, type1, param1, type2, param2, type3, param3, type4, param4)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2, type3 param3, type4 param4)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2, (type3)param3, (type4)param4);\
}

#define DEFINE_STATIC_FUNC_12(clss, typeRet, func, type1, param1, type2, param2, type3, param3, type4, param4, type5)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2, (type3)param3, (type4)param4, (type5)param5);\
}

#define DEFINE_STATIC_FUNC_13(clss, typeRet, func, type1, param1, type2, param2, type3, param3, type4, param4, type5, param5)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2, (type3)param3, (type4)param4, (type5)param5);\
}

#define DEFINE_STATIC_FUNC_14(clss, typeRet, func, type1, param1, type2, param2, type3, param3, type4, param4, type5, param5, type6)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5, type6 param6)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2, (type3)param3, (type4)param4, (type5)param5, (type6)param6);\
}

#define DEFINE_STATIC_FUNC_15(clss, typeRet, func, type1, param1, type2, param2, type3, param3, type4, param4, type5, param5, type6, param6)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5, type6 param6)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2, (type3)param3, (type4)param4, (type5)param5, (type6)param6);\
}

/*
#define DEFINE_STATIC_FUNC_10(clss, typeRet, func, type1, type2, type3, type4, type5, type6, type7)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5, type6 param6, type7 param7)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2, (type3)param3, (type4)param4, (type5)param5, (type6)param6, (type7)param7);\
}

#define DEFINE_STATIC_FUNC_11(clss, typeRet, func, type1, type2, type3, type4, type5, type6, type7, type8)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5, type6 param6, type7 param7, type8 param8)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2, (type3)param3, (type4)param4, (type5)param5, (type6)param6, (type7)param7, (type8)param8);\
}

#define DEFINE_STATIC_FUNC_12(clss, typeRet, func, type1, type2, type3, type4, type5, type6, type7, type8, type9)\
UNIQUE_C_API typeRet clss##_##func(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5, type6 param6, type7 param7, type8 param8, type9 param9)\
{\
	return (typeRet)clss::func((type1)param1, (type2)param2, (type3)param3, (type4)param4, (type5)param5, (type6)param6, (type7)param7, (type8)param8, (type9)param9);\
}
*/

#define uExportNew(clss)\
UNIQUE_C_API clss* clss##_##new()\
{\
	return new clss();\
}

#define uExportDelete(clss)\
UNIQUE_C_API void clss##_##delete(clss* c)\
{\
	delete c;\
}

#if UNIQUE_COMPILER_MSVC
#	define uExport(...) UNIQUE_MACRO_DISPATCHER(DEFINE_FUNC_, __VA_ARGS__) UNIQUE_VA_ARGS_PASS(__VA_ARGS__)
#else
#	define uExport(...) UNIQUE_MACRO_DISPATCHER(DEFINE_FUNC_, __VA_ARGS__)(__VA_ARGS__)
#endif // UNIQUE_COMPILER_MSVC


#if UNIQUE_COMPILER_MSVC
#	define uExportStatic(...) UNIQUE_MACRO_DISPATCHER(DEFINE_STATIC_FUNC_, __VA_ARGS__) UNIQUE_VA_ARGS_PASS(__VA_ARGS__)
#else
#	define uExportStatic(...) UNIQUE_MACRO_DISPATCHER(DEFINE_STATIC_FUNC_, __VA_ARGS__)(__VA_ARGS__)
#endif // UNIQUE_COMPILER_MSVC
