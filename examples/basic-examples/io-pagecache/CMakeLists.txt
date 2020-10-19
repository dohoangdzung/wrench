
set(SOURCE_FILES
        ConcurrentPipelineWMS.h
        ConcurrentPipelineWMS.cpp
        IOPageCacheSingle.cpp
        )

add_executable(wrench-example-io-pagecache-single ${SOURCE_FILES})

if (ENABLE_BATSCHED)
    find_library(ZMQ_LIBRARY NAMES zmq)
    target_link_libraries(wrench-example-io-pagecache-single wrench ${SimGrid_LIBRARY} ${PUGIXML_LIBRARY} ${ZMQ_LIBRARY})
else()
    target_link_libraries(wrench-example-io-pagecache-single wrench ${SimGrid_LIBRARY} ${PUGIXML_LIBRARY})
endif()

install(TARGETS wrench-example-io-pagecache-single DESTINATION bin)

#============================================================
set(SOURCE_FILES
        ConcurrentPipelineWMS.h
        ConcurrentPipelineWMS.cpp
        IOPageCacheMulti.cpp
        )

add_executable(wrench-example-io-pagecache-multi ${SOURCE_FILES})

if (ENABLE_BATSCHED)
    find_library(ZMQ_LIBRARY NAMES zmq)
    target_link_libraries(wrench-example-io-pagecache-multi wrench ${SimGrid_LIBRARY} ${PUGIXML_LIBRARY} ${ZMQ_LIBRARY})
else()
    target_link_libraries(wrench-example-io-pagecache-multi wrench ${SimGrid_LIBRARY} ${PUGIXML_LIBRARY})
endif()

install(TARGETS wrench-example-io-pagecache-multi DESTINATION bin)

#============================================================
set(SOURCE_FILES
        ConcurrentPipelineWMS.h
        ConcurrentPipelineWMS.cpp
        IOPageCacheRemote.cpp
        )

add_executable(wrench-example-io-pagecache-remote ${SOURCE_FILES})

if (ENABLE_BATSCHED)
    find_library(ZMQ_LIBRARY NAMES zmq)
    target_link_libraries(wrench-example-io-pagecache-remote wrench ${SimGrid_LIBRARY} ${PUGIXML_LIBRARY} ${ZMQ_LIBRARY})
else()
    target_link_libraries(wrench-example-io-pagecache-remote wrench ${SimGrid_LIBRARY} ${PUGIXML_LIBRARY})
endif()

install(TARGETS wrench-example-io-pagecache-remote DESTINATION bin)