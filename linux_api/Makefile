CXXFLAGS+=-g -Werror -Wall -rdynamic
LDFLAGS+=-lpthread -laio -ldl

BINS=wait shm eventfd_epoll epoll_eventfd_aio sigsuspend_test settimer_test posix_memalign_test socketpair_test epoll_for_file_test pthread_test mlock_test new_test dlopen
all:${BINS}


lib:test_math.o
	$(CC) -fPIC -shared -o libtest_math.so $<


clean:
	rm -rf ${BINS}
