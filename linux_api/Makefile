CXXFLAGS+=-g -Werror -Wall
LDFLAGS+=-lpthread -laio

BINS=wait shm eventfd_epoll epoll_eventfd_aio sigsuspend_test settimer_test
all:${BINS}

clean:
	rm -rf ${BINS}
