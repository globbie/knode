
set(SOURCE_FILES
        publisher.c
)

add_executable(kmq-ex-publisher ${SOURCE_FILES})
target_link_libraries(kmq-ex-publisher kmq ${LIBEVENT_LIBRARY})

set(SOURCE_FILES
        subscriber.c
)

add_executable(kmq-ex-subscriber ${SOURCE_FILES})
target_link_libraries(kmq-ex-subscriber kmq ${LIBEVENT_LIBRARY})
