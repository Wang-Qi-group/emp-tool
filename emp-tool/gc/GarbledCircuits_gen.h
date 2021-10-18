#ifndef EMP_HALFGATE_GEN_H__
#define EMP_HALFGATE_GEN_H__
#include "emp-tool/utils/utils.h"
#include "emp-tool/utils/hash.h"
#include "emp-tool/execution/circuit_execution.h"
#include <iostream>
namespace emp {

inline std::pair<int,int> point_and_permute(block *ct, block* gct, block wa, block wb){
	bool point[sizeof(block)];
	int ii=-1;
	int jj=-1;
	block nandct[4];
	nandct[0] = ct[0] ^ wb;
	nandct[1] = ct[1] ^ wb;
	nandct[2] = ct[2] ^ wb; 
	nandct[3] = ct[3] ^ wa;

	for(int i=0;i<sizeof(block);i++){
		if(ct[0][i]+ct[1][i]+ct[2][i]+ct[3][i]==2){
			point[i] = true;
			ii = i;
		}else{
			point[i] = false;
		}
	}

	for(int j=0;j<ii;j++){
		if(point[j]==true){
			int one=0;
			for(int i=0;i<4;i++){
				if(ct[i][j]+ct[i][ii]==1){
					one += 1;
				}
			}
			if(one == 2){
				jj = j;
				break;
			}
		}
	}

	for(int i=0;i<4;i++){
		if(ct[i][ii] == 0 && ct[i][jj] == 0){
			gct[0]=nandct[i];
		}else if(ct[i][ii] == 0 && ct[i][jj] == 1){
			gct[1]=nandct[i];
		}else if(ct[i][ii] == 1 && ct[i][jj] == 0){
			gct[2]=nandct[i];
		}else if(ct[i][ii] == 1 && ct[i][jj] == 1){
			gct[3]=nandct[i];
		}
	}
	return std::make_pair(ii,jj);
}

template<typename T>
class GarbledCircuitsGen:public CircuitExecution {
public:
	T * io;
	block gateid;
	block gct[4];
	GarbledCircuitsGen(T * io,const block& id) :io(io) {
		this->gateid = id;
	}
	
	block nand_gate(const block& la, const block& lb, const block& ra, const block& rb, const block& wa, const block& wb){
		block table[4];
		Hash hash;
		block ct[4];
		block pt[4][3];

		block pt[0][0] = pt[2][0] = la;
		block pt[1][0] = pt[3][0] = lb;
		block pt[0][1] = pt[1][1] = ra;
		block pt[2][1] = pt[3][1] = rb;
		block pt[0][2] = pt[1][2] = pt[2][2] = pt[3][2] = this->gateid;
		for(int i=0;i<4;i++){
			hash.put_block(pt[i], 3);
			hash.digest(ct[i]);
		}
		std::pair<int,int> point = point_and_permute(ct, this->gct, wa, wb);

		io->send_block(gct, 4);
		io->send(point);
		return res;
	}
};
}
#endif// HALFGATE_GEN_H__
