default:
	cd makeprocs; make
	cd h2o_injection; make
	cd so4_injection; make
	cd h2o_split; make
	cd so4_split; make
	cd h2so4_produce; make

clean:
	cd makeprocs; make clean
	cd h2o_injection; make clean
	cd so4_injection; make clean
	cd h2o_split; make clean
	cd so4_split; make clean
	cd h2so4_produce; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3 3; ee469_fixterminal
