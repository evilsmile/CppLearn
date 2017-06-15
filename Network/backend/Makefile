DIRS=libs servers test

all:
	@for dir in ${DIRS}; do \
		make -C $$dir; \
	done

install:
	@for dir in ${DIRS}; do \
		make -C $$dir install; \
	done


clean:
	@for dir in ${DIRS}; do \
		make -C $$dir clean; \
	done


