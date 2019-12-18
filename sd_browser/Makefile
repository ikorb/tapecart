all: browser

browser:
	cl65 -t c64 -Oi screen.c browser.c dir.c base.c tapecartif.c minidelay.s getbyte_fast.s loader.s -o browser.prg

clean:
	rm -rf *.o browser.prg
