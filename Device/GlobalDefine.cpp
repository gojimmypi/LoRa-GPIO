// 
// 
// 

#include "GlobalDefine.h"

String HEAP_DEBUG_MSG;

void setHeapMsg(String str) {
	HEAP_DEBUG_MSG = str;
}

String getHeapMsg() {
	return HEAP_DEBUG_MSG;
}

String DefaultDebugMessage() {
	return "";
}
 