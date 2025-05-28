SHELL := /bin/bash

# List of subdirectories (questions)
SUBDIRS := Q_1 Q_2 Q_3 Q_4 Q_5 Q_6

.PHONY: all clean

all:
	@for dir in $(SUBDIRS); do \
	  echo "📦 Building in $$dir..."; \
	  $(MAKE) -C $$dir || exit 1; \
	done

clean:
	@for dir in $(SUBDIRS); do \
	  echo "🧹 Cleaning in $$dir..."; \
	  $(MAKE) -C $$dir clean || true; \
	done
