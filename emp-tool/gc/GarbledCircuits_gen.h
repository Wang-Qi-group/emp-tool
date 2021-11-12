#ifndef EMP_GC_GEN_H__
#define EMP_GC_GEN_H__
#include "emp-tool/utils/utils.h"
#include "emp-tool/utils/hash.h"
#include "emp-tool/execution/circuit_execution.h"
#include <iostream>
namespace emp {

inline int* point_and_permute(block *ct, block* gct, block wa, block wb){
	bool point[sizeof(block)];
	int ii=-1;
	int jj=-1;
	block nandct[4];
	nandct[0] = ct[0] ^ wb;
	nandct[1] = ct[1] ^ wb;
	nandct[2] = ct[2] ^ wb; 
	nandct[3] = ct[3] ^ wa;

	//use input a and input b to encrypt input c := hash(input_a, input_b, gateid) xor output_c 

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
	int* po;
	po[0]=ii;
	po[1]=jj;

	/*point and permute

	Use a,b,c,d to donate the four rows of the truth table of the circuits without garbling. Find a pair(i,j) makes the set {(a[i],a[j]),(b[i],b[j]),(c[i],c[j]),(d[i],d[j])} = {(0,0),(0,1),(1,0),(1,1)}.
	Additionally, use the pair(i,j) to garble the circuits. 

	Use e,f,g,h to donate the four rows of the truth table of the garbled circuits.
	(e[i],e[j])=(0,0),(f[i],f[j])=(0,1),(g[i],g[j])=(1,0),(h[i],h[j])=(1,1)

	*/

	return po;
}

template<typename T>
class GarbledCircuitsGen:public CircuitExecution {
public:
	T * io;
	block gct[4];
	GarbledCircuitsGen(T * io) :io(io) {
	}
	block public_label(bool b) override {
		return zero_block;
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
	block nand_gate(int gateid, block *in, block *out) override {

		//in := in_a_0, in_a_1, in_b_0, in_b_1 
		//out := out_c_0, out_c_1

		block table[4];
		Hash hash;
		block ct[4];
		block pt[4][3];
		char tmp[16];

		pt[0][0] = pt[2][0] = in[0];
		pt[1][0] = pt[3][0] = in[1];
		pt[0][1] = pt[1][1] = in[2];
		pt[2][1] = pt[3][1] = in[3];
		pt[0][2] = pt[1][2] = pt[2][2] = pt[3][2] = _mm_set1_epi32(gateid);
		for(int i=0;i<4;i++){
			hash.put_block(pt[i], 3);
			hash.digest(tmp);
			ct[i] = _mm_setr_epi8(tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6],tmp[7],tmp[8],tmp[9],tmp[10],tmp[11],tmp[12],tmp[13],tmp[14],tmp[15]);
		}

		//ct := hash(in_a,in_b,gate_id)

		int* point = point_and_permute(ct, this->gct, out[0], out[1]);

		//gct := garbled(ct) 

		io->send_block(gct, 4);
		io->send_data(point,2);
		return gct[0];
	}
};
}
#endif// HALFGATE_GEN_H__
