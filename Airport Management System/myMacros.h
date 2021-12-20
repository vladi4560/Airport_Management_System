#include <stdio.h>
#include <stdlib.h>
#define CHECK_RETURN_0(object){\
if (!object)\
	return 0;\
}
#define CHECK_RETURN_NULL(object){\
if (!object)\
	return NULL ;\
}
#define CHECK_MSG_RETURN_0(object,msg){\
if (!object)\
{\
	printf(#msg "\n") ;\
return 0;\
}\
}
#define CHECK_0_MSG_CLOSE_FILE(num,file,msg){\
if (num == 0)\
{\
printf(#msg"\n");\
fclose(file);\
return 0;\
}\
}
#define CHECK_NULL__MSG_CLOSE_FILE(object,file,msg){\
if (!object)\
{\
printf(#msg"\n"); \
fclose(file); \
return 0; \
}\
}
#define MSG_CLOSE_RETURN_0(file,msg){\
printf(#msg"\n"); \
fclose(file);\
return 0;\
}
