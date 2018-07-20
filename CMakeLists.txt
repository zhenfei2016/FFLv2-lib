cmake_minimum_required(VERSION 3.4.1)

PROJECT(FFL)

FUNCTION(printf arg)
   message("message:" ${ARGV0})
ENDFUNCTION()


SET(LOCAL_SRC_FILES "")
FUNCTION(ADD_SRC_FILE arg)    
    SET(LOCAL_SRC_FILES "${LOCAL_SRC_FILES}${LOCAL_SRC_DIR}${ARGV0};" PARENT_SCOPE)
ENDFUNCTION()


INCLUDE_DIRECTORIES(include)


SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
#ADD_COMPILE_OPTIONS(-std=c++11)
SET(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

SET(LOCAL_SRC_DIR ./include/)
ADD_SRC_FILE(FFL_Config.h)
ADD_SRC_FILE(FFL_Platform.h)
ADD_SRC_FILE(FFL_Core.h)
ADD_SRC_FILE(FFL_Assert.h)
ADD_SRC_FILE(FFL_Atomic.h)
ADD_SRC_FILE(FFL_Error.h)
ADD_SRC_FILE(FFL_Version.h)
ADD_SRC_FILE(FFL_Log.h)
ADD_SRC_FILE(FFL_Memory.h)
ADD_SRC_FILE(FFL_Time.h)
ADD_SRC_FILE(FFL_Utility.h)
ADD_SRC_FILE(FFL_Loadmodule.h)
ADD_SRC_FILE(FFL_Initialize.h)
ADD_SRC_FILE(FFL_Commandline.h)
ADD_SRC_FILE(FFL_CommandHelper.h)



SET(LOCAL_SRC_DIR ./source/)
ADD_SRC_FILE(FFL_Error.c)
ADD_SRC_FILE(FFL_Version.c)
ADD_SRC_FILE(FFL_Log.c)
ADD_SRC_FILE(FFL_Memory.c)
ADD_SRC_FILE(FFL_Time.c)
ADD_SRC_FILE(FFL_Utility.c)
ADD_SRC_FILE(FFL_Loadmodule.c)
ADD_SRC_FILE(FFL_Initialize.c)
ADD_SRC_FILE(FFL_Commandline.c)
ADD_SRC_FILE(FFL_CommandHelper.c)

SET(LOCAL_SRC_DIR ./source/getopt/)
ADD_SRC_FILE(getopt.c)
ADD_SRC_FILE(getopt.h)
ADD_SRC_FILE(getopt_int.c)
ADD_SRC_FILE(getopt.h)

SET(LOCAL_SRC_DIR ./source/atomic/)
ADD_SRC_FILE(atomic.cpp)
ADD_SRC_FILE(atomic.h)
ADD_SRC_FILE(FFL_Atomic.c)


SET(LOCAL_SRC_DIR ./source/thread/)
ADD_SRC_FILE(FFL_thread.c)
ADD_SRC_FILE(FFL_threadpool.c)

#SET(LOCAL_SRC_DIR ./source/thread/threadpool/)
#ADD_SRC_FILE(threadpool.c)
#ADD_SRC_FILE(threadpool.h)
#ADD_SRC_FILE(threadpool_job.c)
#ADD_SRC_FILE(threadpool_job.h)


SET(LOCAL_SRC_DIR ./include/thread/)
ADD_SRC_FILE(FFL_Mutex.hpp)
ADD_SRC_FILE(FFL_Thread.hpp)
ADD_SRC_FILE(FFL_Coroutine.hpp)


SET(LOCAL_SRC_DIR ./source/thread/)
ADD_SRC_FILE(FFL_Mutex.cpp)
ADD_SRC_FILE(FFL_Thread.cpp)
ADD_SRC_FILE(FFL_Coroutine.cpp)






IF (WIN32)

SET(LOCAL_SRC_DIR ./source/thread/stdcpp/)
#ADD_SRC_FILE(syscond.cpp)
#ADD_SRC_FILE(sysmutex.cpp)
#ADD_SRC_FILE(systhread.cpp)
#ADD_SRC_FILE(syssem.c)


SET(LOCAL_SRC_DIR ./source/thread/windows/)
ADD_SRC_FILE(sysmutex.c)
ADD_SRC_FILE(systhread.c)
ADD_SRC_FILE(syssem.c)
ADD_SRC_FILE(syscond.c)

ELSE()

SET(LOCAL_SRC_DIR ./source/thread/pthread/)
ADD_SRC_FILE(syscond.c)
ADD_SRC_FILE(sysmutex.c)
ADD_SRC_FILE(syssem.c)
ADD_SRC_FILE(systhread.c)
ADD_SRC_FILE(systls.c)


ENDIF()


SET(LOCAL_SRC_DIR ./include/ref/)
ADD_SRC_FILE(FFL_Ref.hpp)
ADD_SRC_FILE(FFL_RefAtomic.hpp)
ADD_SRC_FILE(FFL_RefBase.hpp)
ADD_SRC_FILE(FFL_RefCount.hpp)
ADD_SRC_FILE(FFL_RefLog.hpp)
ADD_SRC_FILE(FFL_RefSp.hpp)
ADD_SRC_FILE(FFL_RefWeakimpl.hpp)
ADD_SRC_FILE(FFL_RefWp.hpp)

SET(LOCAL_SRC_DIR ./source/ref/)
ADD_SRC_FILE(FFL_RefBase.cpp)
ADD_SRC_FILE(FFL_RefWakeimpl.cpp)

SET(LOCAL_SRC_DIR ./include/utils/)
ADD_SRC_FILE(FFL_MemoryWatch.hpp)
ADD_SRC_FILE(FFL_Flags.hpp)

ADD_SRC_FILE(FFL_ByteReader.hpp)
ADD_SRC_FILE(FFL_ByteWriter.hpp)
ADD_SRC_FILE(FFL_ByteStream.hpp)
ADD_SRC_FILE(FFL_ByteBuffer.hpp)
ADD_SRC_FILE(FFL_Serializable.hpp)
ADD_SRC_FILE(FFL_MessageQueue.hpp)
ADD_SRC_FILE(FFL_Looper.hpp)
ADD_SRC_FILE(FFL_Handler.hpp)
ADD_SRC_FILE(FFL_Clock.hpp)
ADD_SRC_FILE(FFL_HandlerManager.hpp)
ADD_SRC_FILE(FFL_List.hpp)
ADD_SRC_FILE(FFL_String.hpp)
ADD_SRC_FILE(FFL_Vector.hpp)
ADD_SRC_FILE(FFL_Io.hpp)
ADD_SRC_FILE(FFL_File.hpp)
ADD_SRC_FILE(FFL_Singleton.hpp)
ADD_SRC_FILE(FFL_Dictionary.hpp)

SET(LOCAL_SRC_DIR ./source/utils/)
ADD_SRC_FILE(FFL_MemoryWatch.cpp)
ADD_SRC_FILE(FFL_Flags.cpp)
ADD_SRC_FILE(FFL_ByteStream.cpp)
ADD_SRC_FILE(FFL_ByteBuffer.cpp)
ADD_SRC_FILE(FFL_Serializable.cpp)
ADD_SRC_FILE(FFL_String.cpp)
ADD_SRC_FILE(FFL_MessageQueue.cpp)
ADD_SRC_FILE(FFL_Looper.cpp)
ADD_SRC_FILE(FFL_Handler.cpp)
ADD_SRC_FILE(FFL_HandlerManager.cpp)
ADD_SRC_FILE(FFL_Message.cpp)
ADD_SRC_FILE(FFL_Clock.cpp)
ADD_SRC_FILE(FFL_File.cpp)
ADD_SRC_FILE(FFL_Singleton.cpp)
ADD_SRC_FILE(FFL_Dictionary.cpp)


SET(LOCAL_SRC_DIR ./include/pipeline/)
ADD_SRC_FILE(FFL_Pipeline.hpp)
ADD_SRC_FILE(FFL_PipelineConnector.hpp)
ADD_SRC_FILE(FFL_PipelineEvent.hpp)
ADD_SRC_FILE(FFL_PipelineEventCallback.hpp)
ADD_SRC_FILE(FFL_PipelineEventFilter.hpp)
ADD_SRC_FILE(FFL_PipelineInput.hpp)
ADD_SRC_FILE(FFL_PipelineMessage.hpp)
ADD_SRC_FILE(FFL_PipelineMessageCache.hpp)
ADD_SRC_FILE(FFL_PipelineNode.hpp)
ADD_SRC_FILE(FFL_PipelineNodeParser.hpp)
ADD_SRC_FILE(FFL_PipelineOutput.hpp)
ADD_SRC_FILE(FFL_PipelineInputHandler.hpp)
ADD_SRC_FILE(FFL_PipelineNodeHandler.hpp)
ADD_SRC_FILE(FFL_PipelineAsyncConnector.hpp)
ADD_SRC_FILE(FFL_PipelineAsyncConnectorFixedSize.hpp)
ADD_SRC_FILE(FFL_PipelineSourceConnector.hpp)
ADD_SRC_FILE(FFL_PipelineHandler.hpp)

SET(LOCAL_SRC_DIR ./source/pipeline/)
ADD_SRC_FILE(FFL_Pipeline.cpp)
ADD_SRC_FILE(FFL_PipelineConnector.cpp)
ADD_SRC_FILE(FFL_PipelineEvent.cpp)
ADD_SRC_FILE(FFL_PipelineInput.cpp)
ADD_SRC_FILE(PipelineInputList.hpp)
ADD_SRC_FILE(PipelineInputList.cpp)
ADD_SRC_FILE(FFL_PipelineMessage.cpp)
ADD_SRC_FILE(FFL_PipelineMessageCache.cpp)
ADD_SRC_FILE(FFL_PipelineNode.cpp)
ADD_SRC_FILE(FFL_PipelineNodeManager.hpp)
ADD_SRC_FILE(FFL_PipelineNodeManager.cpp)
ADD_SRC_FILE(FFL_PipelineNodeParser.cpp)
ADD_SRC_FILE(FFL_PipelineOutput.cpp)
ADD_SRC_FILE(PipelineOutputList.cpp)
ADD_SRC_FILE(PipelineOutputList.hpp)
ADD_SRC_FILE(PipelineLooper.hpp)
ADD_SRC_FILE(PipelineLooper.cpp)
ADD_SRC_FILE(FFL_PipelineNodeHandler.cpp)
ADD_SRC_FILE(FFL_PipelineAsyncConnector.cpp)
ADD_SRC_FILE(FFL_PipelineAsyncConnectorFixedSize.cpp)
ADD_SRC_FILE(FFL_PipelineSourceConnector.cpp)
ADD_SRC_FILE(FFL_PipelineHandler.cpp)
ADD_SRC_FILE(PipelineMessageTrackFile.hpp)
ADD_SRC_FILE(PipelineMessageTrackFile.cpp)


#SET(LOCAL_SRC_DIR ./include/net/base/)
#ADD_SRC_FILE(FFL_Net.h)
#ADD_SRC_FILE(FFL_NetConst.h)

AUX_SOURCE_DIRECTORY(./source/net/ DIR_SRCS)
SET(LOCAL_SRC_FILES ${DIR_SRCS} ${LOCAL_SRC_FILES})

AUX_SOURCE_DIRECTORY(./source/net/base/ DIR_SRCS)
SET(LOCAL_SRC_FILES ${DIR_SRCS} ${LOCAL_SRC_FILES})

AUX_SOURCE_DIRECTORY(./source/net/http/ DIR_SRCS)
SET(LOCAL_SRC_FILES ${DIR_SRCS} ${LOCAL_SRC_FILES})

AUX_SOURCE_DIRECTORY(./source/net/http/http-parser-2.1/ DIR_SRCS)
SET(LOCAL_SRC_FILES ${DIR_SRCS} ${LOCAL_SRC_FILES})



if (MSVC)
    add_definitions("/wd4819")
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif(MSVC)

add_library( FFL STATIC ${LOCAL_SRC_FILES})

IF(WIN32)
INSTALL(TARGETS FFL RUNTIME DESTINATION  ${PROJECT_SOURCE_DIR}/../install/FFLv2-lib/bin/
        LIBRARY DESTINATION  ${PROJECT_SOURCE_DIR}/../install/FFLv2-lib/libs
        ARCHIVE DESTINATION  ${PROJECT_SOURCE_DIR}/../install/FFLv2-lib/libs)
ENDIF()


ADD_SUBDIRECTORY(example)

