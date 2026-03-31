PKGNAME := clock-controller
ARCH    := all

$(shell touch -d @$(shell find Makefile src -printf "%Ts\n" | sort -n | tail -n1) src)
$(shell mkdir -p tmp)


VERSION := $(shell git tag --points-at HEAD)
ifeq ($(VERSION),)
    VERSION := 0
endif

BRANCH := $(shell git branch | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/')
UNCOMMITTED := $(shell git status --short | wc --lines)
AHEAD :=  $(shell git status --short --branch | grep $(BRANCH) | grep ahead | wc --lines)
ifneq ($(UNCOMMITTED), 0)
    VERSION := $(VERSION)+dirty
else ifneq ($(AHEAD), 0)
    VERSION := $(VERSION)+dirty
else
    VERSION := $(VERSION)+$(shell git log --no-walk --pretty='%h' HEAD)
endif

BUILDDIR := build/$(PKGNAME)_$(VERSION)_$(ARCH)
DEBFILE := build/$(PKGNAME)_$(VERSION)_$(ARCH).deb

$(DEBFILE): src
	mkdir -p $(BUILDDIR)
	rsync -a --delete --delete-excluded --exclude '*~' --exclude '__pycache__' src/ tmp/ $(BUILDDIR)/
	sed -e 's/PKGNAME/$(PKGNAME)/g' \
	    -e 's/VERSION/$(VERSION)/g' \
	    -e 's/ARCH/$(ARCH)/g' \
	    -i $(BUILDDIR)/DEBIAN/control
	dpkg-deb --build --root-owner-group $(BUILDDIR)


clean:
	rm -rf build
distclean:
	rm -rf build tmp
