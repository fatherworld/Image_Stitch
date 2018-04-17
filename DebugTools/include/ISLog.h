#ifndef _ISLog_h_
#define _ISLog_h_

#define CHECK_ALLOC_P(p) if((p) == NULL) return -1;
#define CHECK_IS_NULL(p) if((p) == NULL) return NULL;
#define CHECK_IS_VOID(p) if((p) == NULL) exit(0);

#endif