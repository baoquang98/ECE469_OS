default:
	cd makeprocs; make
	cd s2_injection; make
	cd co_injection; make
	cd s2_split; make
	cd co_split; make
	cd so4_produce; make

clean:
	cd makeprocs; make clean
	cd s2_injection; make clean
	cd co_injection; make clean
	cd s2_split; make clean
	cd co_split; make clean
	cd so4_produce; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3 4; ee469_fixterminal
