all: readTRC

clean:
	rm -f readLecroy trc2root

readTRC: trc2root.cc LECROYTRCReader.cxx
	g++ -fPIC -g -O2 -m64 -o trc2root trc2root.cc LECROYTRCReader.cxx -Isrc -L/usr/lib/ -lm -lpthread `root-config --cflags --glibs`
