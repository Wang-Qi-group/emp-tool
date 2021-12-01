#include <iostream>

#include "emp-tool/emp-tool.h"
using namespace std;
using namespace emp;

class AbandonIO: public IOChannel<AbandonIO> { public:
	void send_data_internal(const void * data, int len) {
	}

	void recv_data_internal(void  * data, int len) {
	}
};

int port, party;
template <typename T>
void test(T* netio) {
	block* ingoing = new block[256];
	block* outgoing = new block[128];

	PRG prg;
	prg.random_block(ingoing, 256);
	prg.random_block(outgoing, 128);
	// int gate_arr[12]={1,2,5,3,3,4,6,3,5,6,7,3};
	// BristolFormat cf(3,7,4,0,1,gate_arr);

	string file = "./emp-tool/circuits/files/bristol_format/AES-non-expanded.txt";
	BristolFormat cf(file.c_str());

	if (party == BOB) {
		HalfGateEva<T>::circ_exec = new HalfGateEva<T>(netio);
		for (int i = 0; i < 100; ++i)
			cf.compute(ingoing[4*i], outgoing[4*i+2]);
		delete HalfGateEva<T>::circ_exec;
	} else {
		// AbandonIO* aio = new AbandonIO();
		// HalfGateGen<AbandonIO>::circ_exec = new HalfGateGen<AbandonIO>(aio);

		// auto start = clock_start();
		// for (int i = 0; i < 100; ++i) {
		// 	cf.compute(c, a, b);
		// }
		// double interval = time_from(start);
		// cout << "Pure AES garbling speed : " << 100 * 6800 / interval << " million gate per second\n";
		// delete aio;
		// delete HalfGateGen<AbandonIO>::circ_exec;

		// MemIO* mio = new MemIO();
		// HalfGateGen<MemIO>::circ_exec = new HalfGateGen<MemIO>(mio);

		// start = clock_start();
		// for (int i = 0; i < 20; ++i) {
		// 	mio->clear();
		// 	for (int j = 0; j < 5; ++j)
		// 		cf.compute(c, a, b);
		// }
		// interval = time_from(start);
		// cout << "AES garbling + Writing to Memory : " << 100 * 6800 / interval << " million gate per second\n";
		// delete mio;
		// delete HalfGateGen<MemIO>::circ_exec;

		HalfGateGen<T>::circ_exec = new HalfGateGen<T>(netio);

		start = clock_start();
		for (int i = 0; i < 100; ++i) {
			cf.initialization(ingoing, outgoing);
		}
		interval = time_from(start);
		cout << "AES garbling + Loopback Network : " << 100 * 6800 / interval << " million gate per second\n";

		delete HalfGateGen<T>::circ_exec;
	}

	delete[] a;
	delete[] b;
	delete[] c;
}

int main(int argc, char** argv) {
	parse_party_and_port(argv, &party, &port);
	cout << "Using NetIO\n";
	NetIO* netio = new NetIO(party == ALICE ? nullptr : "127.0.0.1", port);
	test<NetIO>(netio);
	delete netio;

	cout << "Using HighSpeedNetIO\n";
	HighSpeedNetIO* hsnetio = new HighSpeedNetIO(party == ALICE ? nullptr : "127.0.0.1", port, port+1);
	test<HighSpeedNetIO>(hsnetio);
	delete hsnetio;
}