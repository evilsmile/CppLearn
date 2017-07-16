CXXFLAGS+=-g -Werror -Wall
LDFLAGS+=-lpthread -laio

BINS=wait shm eventfd_epoll epoll_eventfd_aio
all:${BINS}

clean:
	rm -rf ${BINS}
