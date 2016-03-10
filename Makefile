default: CannamCodecFixed.ttf CannamCodecProportional.ttf

clean:
	rm -f CannamCodecFixed.ttf CannamCodecProportional.ttf
	rm -f cannam-codec-fixed.sfd cannam-codec-proportional.sfd
	rm -f bdf2sfd.o
	rm -f bdf2sfd

CannamCodecFixed.ttf: cannam-codec-fixed.sfd
	fontforge -lang=ff -c 'Open("$^");RenameGlyphs("Adobe Glyph List");Generate("$@");'

CannamCodecProportional.ttf: cannam-codec-proportional.sfd
	fontforge -lang=ff -c 'Open("$^");RenameGlyphs("Adobe Glyph List");Generate("$@");'

cannam-codec-fixed.sfd: codec-mono.bdf bdf2sfd
	./bdf2sfd < $< > $@
cannam-codec-proportional.sfd: codec-prop.bdf bdf2sfd
	./bdf2sfd < $< > $@

bdf2sfd: bdf2sfd.o
	$(CC) $(CFLAGS) -o $@ $^

