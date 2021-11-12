#ifndef EMP_GC_EVA_H__
#define EMP_GC_EVA_H__
#include "emp-tool/utils/utils.h"
#include "emp-tool/utils/mitccrh.h"
#include "emp-tool/execution/circuit_execution.h"
#include <iostream>
namespace emp {

template<typename T>
class GarbledCircuitsEva:public CircuitExecution {
public:
	T * io;
	int* point;
	block gct[4];
	GarbledCircuitsEva(T * io) :io(io) {
	}
	block public_label(bool b) override {
		return gct[0];
	}
	block and_gate(const block& a, const block& b) override {
		return a&b;
	}
	block xor_gate(const block& a, const block& b) override {
		return a ^ b;
	}
	block not_gate(const block&a) override {
		return xor_gate(a, public_label(true));
	}
	uint64_t num_and() override {
		return (uint64_t)0;
	}
	block nand_gate(int gateid, block *a, block *b) override {

		//input a and input b 
		io->recv_block(&gct[0], 4);
		io->recv_data(point,2);

		
		Hash hash;
		block ct[3];
		block hct;
		char tmp[16];
		ct[0]=*a;
		ct[1]=*b;
		ct[2]=_mm_set1_epi32(gateid);	
		hash.put_block(ct, 3);
		hash.digest(tmp);

		//hash(input_a, input_b, gateid)

		hct = _mm_setr_epi8(tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6],tmp[7],tmp[8],tmp[9],tmp[10],tmp[11],tmp[12],tmp[13],tmp[14],tmp[15]);
		int place = (int) tmp[point[1]]*2+hct[point[2]];
		return hct^gct[place];
	}
};	
}
#endif// HALFGATE_EVA_H__