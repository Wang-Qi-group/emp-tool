#ifndef EMP_HALFGATE_EVA_H__
#define EMP_HALFGATE_EVA_H__
#include "emp-tool/utils/utils.h"
#include "emp-tool/utils/mitccrh.h"
#include "emp-tool/execution/circuit_execution.h"
#include <iostream>
namespace emp {

template<typename T>
class GarbledCircuitsEva:public CircuitExecution {
public:
	T * io;
	block gateid;
	std::pair<int,int> point;
	block gct[4];
	GarbledCircuitsEva(T * io, const block& id) :io(io) {
		this->gateid = id;
		io->recv_block(&gct, 4);
		io->recv(point);
	}
	block nand_gate(const block& a, const block& b){
		Hash hash;
		block ct[3];
		block hct;
		char tmp[16];
		ct[0]=a;
		ct[1]=b;
		ct[2]=gateid;
		hash.put_block(ct, 3);
		hash.digest(tmp);
		hct = _mm_setr_epi8(tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6],tmp[7],tmp[8],tmp[9],tmp[10],tmp[11],tmp[12],tmp[13],tmp[14],tmp[15]);
		int place=hct[point.first()]*2+hct[point.second()]
		return hct^gct[place];
	}
};
}
#endif// HALFGATE_EVA_H__
