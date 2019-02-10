#ifndef _SSDSIM_DEBUG_H
#define _SSDSIM_DEBUG_H

#include <stdio.h>

#define compression_relevant
#define CONFIG_TRACE_WRITE_PROCESS
#define CONFIG_TRACE_READ_PROCESS
#define CONFIG_BUFFER_MONITOR_ENTRY
#define CONFIG_BUFFER_MONITOR_TRANS
#define CONFIG_BUFFER_MONITOR_COMPRESSION_TRANS






#define CONFIG_ENABLE_MSG
#define CONFIG_WRONG_PROCESS_DEBUG_ENABLE_MSG
//first for normal hint print
#ifdef CONFIG_ENABLE_MSG
#define ssdsim_msg(fmt, ...) \
do{\
printf("ssdsim:" fmt "\n", ##__VA_ARGS__);\
}while (0);


#else
#define ssdsim_msg(fmt, ...)

#endif  //end of ifdef CONFIG_ENABLE_MSG


//second: only for the wrong of process, have to debug and trace the process

#ifdef CONFIG_WRONG_PROCESS_DEBUG_ENABLE_MSG
#define traceError_msg(fmt, ...) \
do{\
printf("simulator trace:" fmt "\n", ##__VA_ARGS__);\
}while (0);


#else
#define traceError_msg(fmt, ...)

#endif  //end of ifdef CONFIG_WRONG_PROCESS_DEBUG_ENABLE_MSG


#ifdef CONFIG_TRACE_READ_PROCESS
#define traceRead_msg(fmt, ...) \
do{\
printf("simulator trace read:" fmt "\n", ##__VA_ARGS__);\
}while (0);


#else
#define traceRead_msg(fmt, ...)

#endif  //end of ifdef CONFIG_TRACE_READ_PROCESS

#ifdef CONFIG_TRACE_WRITE_PROCESS
#define traceWrite_msg(fmt, ...) \
do{\
printf("simulator trace write:" fmt "\n", ##__VA_ARGS__);\
}while (0);


#else
#define traceWrite_msg(fmt, ...)

#endif  //end of ifdef CONFIG_TRACE_READ_PROCESS


#ifdef CONFIG_BUFFER_MONITOR_ENTRY
#define traceEntrybuffer_msg(fmt, ...) \
do{\
printf("simulator trace buffer entry:" fmt "\n", ##__VA_ARGS__);\
}while (0);


#else
#define traceEntrybuffer_msg(fmt, ...)

#endif  //end of 





#ifdef CONFIG_BUFFER_MONITOR_TRANS
#define traceTranslationbuffer_msg(fmt, ...) \
do{\
printf("simulator trace buffer translation pages:" fmt "\n", ##__VA_ARGS__);\
}while (0);


#else
#define traceTranslationbuffer_msg(fmt, ...)

#endif  //end of ifdef 





#ifdef CONFIG_BUFFER_MONITOR_COMPRESSION_TRANS
#define traceCompTranslationbuffer_msg(fmt, ...) \
do{\
printf("simulator trace buffer compression translation pages:" fmt "\n", ##__VA_ARGS__);\
}while (0);


#else
#define traceCompTranslationbuffer_msg(fmt, ...)

#endif  //end of ifdef




#endif //end of _SSDSIM_DEBUG_H
