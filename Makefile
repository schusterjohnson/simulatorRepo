all:	clean
	clang++ -std=c++11 store.cpp event.cpp main.cpp -o simulator.out
	./simulator.out
	./plotSimulator

clean:
	-rm simulator.out
	-rm bankData
	-rm marketData
	-rm difference
	-rm percentage