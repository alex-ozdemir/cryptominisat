proofs: ./drat-from-api ./drat-from-bin

check-api: ./drat-from-api
	drat-trim ./cnf ./drat-from-api

check-bin: ./drat-from-bin
	drat-trim ./cnf ./drat-from-bin

cms: cms.cpp ../build/include/cryptominisat5/cryptominisat.h ../build/lib/libcryptominisat5.so
	clang++ -std=c++17 -O2 -I../build/include/ -lcryptominisat5 -L../build/lib cms.cpp -o cms

drat-from-api: cnf cms
	./cms cnf $@

drat-from-bin: cnf ../build/cryptominisat5
	../build/cryptominisat5 cnf $@ || true

clean:
	rm -rf ./drat-from-api ./drat-from-bin ./cms
