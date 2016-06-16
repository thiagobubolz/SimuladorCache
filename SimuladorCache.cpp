#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

using namespace std;

/*
Estrutura usada na cache para guardar as informações
*/
struct Dado{
	int validade;
	int tag;
	int informacao;
};


/*
Classe do simulador, com seus atributos e metodos
*/
class SimuladorCache{
 public:
 	Dado **dados;
 	int tamanhoCache;
 	int nsets, bsize, assoc;
 	int nbits_tag, nbits_indice, nbits_offset;
 	int miss, hit, misscomp;

 	/*
	Contrutor da classe, recebe por parametro o numero de conjuntos da cache, tamanho de blocos e a associatividade
	inicializa os atributos com base nos parametros, calcula o tamanho da cache, o numero de bits para offset, 
	indice e tag, aloca memoria necessária para a estrutura Dados, de acordo com o numero de conjuntos e a associatividade
	seta 0 para todos os campos de validade
 	*/
 	SimuladorCache(int numsets, int blocksize, int associativity){
 		this->nsets = numsets;
 		this->bsize = blocksize;
 		this->assoc = associativity;
 		this->tamanhoCache = nsets * bsize * assoc;
 		this->nbits_offset = log2(bsize/assoc);
 		this->nbits_indice = log2((tamanhoCache/bsize)/assoc);
 		this->nbits_tag = 32 - nbits_indice - nbits_offset;
 		dados = new Dado*[numsets];
 		for(int i=0; i < nsets; i++){
 			dados[i] = new Dado[associativity];
 		}
 		for(int i=0; i < nsets; i++){
 			for (int j=0; j < assoc; j++){
 				dados[i][j].validade = 0;
 			}
 		}
 		miss = 0;
 		hit = 0;
 		misscomp = 0;
 	}

 	~SimuladorCache();

	/*
	Método usado para debug dos valores da validade da cache, printa a cache por linha e seus valores
 	*/
 	void printaValidadeCache(){
 		for(int i=0; i < nsets; i++){
 			for (int j=0; j < assoc; j++){
 				cout << dados[i][j].validade << " ";
 			}
 			cout << endl;
 		}
 	}

 	/*
	Método usado para debug, printa todos os dados armazenados na cache por linha
 	*/
 	void printaDadosCache(){
 		for(int i=0; i < nsets; i++){
 			for (int j=0; j < assoc; j++){
 				cout << dados[i][j].informacao << " ";
 			}
 			cout << endl;
 		}
 	}

 	/*
	Método que verifica se o valor está na cache, recebe por parametro um indice e uma tag, e compara a tag 
	do parametro com a da cache no indice indicado, e se a validade naquela posição é 1 se for conta um hit
	e retorna verdadeiro, caso contrário conta um miss e retorna falso
 	*/
 	bool estaNaCache(int ind, int dadoTag){
 		for(int j=0; j < assoc; j++){
 			if(dados[ind][j].validade == 1 && dados[ind][j].tag == dadoTag){
 				hit++;
 				return true;
 			}
 		}
 		miss++;
 		return false;
 	}

 	/*
	Método que insere na cache, recebe como parametro um indice, a informação a ser inserida e uma tag
	calcula um valor randomico de acordo com a associatividade para a posição da inserção
	testa se na posição a ser inserida a validade e igual a 0 insere na posição, troca a validade, e conta um miss compulsório
	caso contrario apenas adiciona o dado novo.
 	*/
 	void insereCache(int ind, int info, int dadoTag){
 		int random = rand() % assoc;
		if(dados[ind][random].validade == 0){
			dados[ind][random].validade = 1;
			dados[ind][random].tag = dadoTag;
			dados[ind][random].informacao = info;
			misscomp++;
		}else{
			dados[ind][random].tag = dadoTag;
			dados[ind][random].informacao = info;
		}
 	}

 }; 


int main(int argc,char *argv[]){
	int dado, leiesc, offset, indice, tag, l1doffset, l1dindice, l1dtag,  l2offset, l2indice, l2tag;
	string nomearquivo;

	SimuladorCache* CacheL1i;
	SimuladorCache* CacheL1d;
	SimuladorCache* CacheL2;

	CacheL1i = new SimuladorCache(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
	CacheL1d = new SimuladorCache(atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
	CacheL2 = new SimuladorCache(atoi(argv[7]), atoi(argv[8]), atoi(argv[9]));

	fstream file;

	file.open(argv[10]);

	
	
	if(!file.is_open()){
		cout << "Arquivo não foi aberto" << endl;
	}else{
		while(file >> dado){
			file >> leiesc;

		    indice = (dado >> CacheL1i->nbits_offset)%(int)(pow(2,CacheL1i->nbits_indice));
		    tag = (dado >> (CacheL1i->nbits_offset + CacheL1i->nbits_indice));

		    l1dindice = (dado >> CacheL1d->nbits_offset)%(int)(pow(2,CacheL1d->nbits_indice));
		    l1dtag = (dado >> (CacheL1d->nbits_offset + CacheL1d->nbits_indice));

		    l2indice = (dado >> CacheL2->nbits_offset)%(int)(pow(2,CacheL2->nbits_indice));
		    l2tag = (dado >> (CacheL2->nbits_offset + CacheL2->nbits_indice));
			if(leiesc == 0){
				if(dado < 500){
					if(!CacheL1i->estaNaCache(indice, tag)){
						CacheL1i->insereCache(indice, dado, tag);
						if(!CacheL2->estaNaCache(l2indice, l2tag)){
							CacheL2->insereCache(l2indice, dado, l2tag);
						}
					}
				}else{
					if(!CacheL1d->estaNaCache(l1dindice, l1dtag)){
						CacheL1d->insereCache(l1dindice, dado, l1dtag);
						if(!CacheL2->estaNaCache(l2indice, l2tag)){
							CacheL2->insereCache(l2indice, dado, l2tag);
						}
					}
				}
			}else if(leiesc == 1){
				if(dado < 500){
					if(!CacheL1i->estaNaCache(indice, tag)){
						CacheL1i->insereCache(indice, dado, tag);
					}
					if(!CacheL2->estaNaCache(l2indice, l2tag)){
						CacheL2->insereCache(l2indice, dado, l2tag);
					}
				}else{
					if(!CacheL1d->estaNaCache(l1dindice, l1dtag)){
						CacheL1d->insereCache(l1dindice, dado, l1dtag);
					}
					if(!CacheL2->estaNaCache(l2indice, l2tag)){
						CacheL2->insereCache(l2indice, dado, l2tag);
					}
				}
			}
		}


		cout << "-------ESTATISTICAS:--------" << endl << endl;
	        
	    cout << "CACHE L1 - INSTRUCOES" << endl;
	    cout << "ACESSOS: " << CacheL1i->miss+CacheL1i->hit << "  HIT: " << CacheL1i->hit << "  MISS: " << CacheL1i->miss << endl;   		
	    cout << "Hit-ratio: " <<  (float)CacheL1i->hit/(CacheL1i->miss+CacheL1i->hit) << "  Miss-ratio: " << (float)CacheL1i->miss/(CacheL1i->miss+CacheL1i->hit) << endl;
	    cout << "Miss Compulsorio: " << CacheL1i->misscomp <<  "  Miss Conflito: " <<  CacheL1i->miss - CacheL1i->misscomp << endl << endl << endl;
	        
	    cout << "CACHE L1 - Dados" << endl;
	    cout << "ACESSOS: " << CacheL1d->miss+CacheL1d->hit << "  HIT: " << CacheL1d->hit << "  MISS: " << CacheL1d->miss << endl;   		
	    cout << "Hit-ratio: " <<  (float)CacheL1d->hit/(CacheL1d->miss+CacheL1d->hit) << "  Miss-ratio: " << (float)CacheL1d->miss/(CacheL1d->miss+CacheL1d->hit) << endl;
	    cout << "Miss Compulsorio: " << CacheL1d->misscomp <<  "  Miss Conflito: " <<  CacheL1d->miss - CacheL1d->misscomp << endl << endl << endl;
	        
	    cout << "CACHE L2" << endl;
	    cout << "ACESSOS: " << CacheL2->miss+CacheL2->hit << "  HIT: " << CacheL2->hit << "  MISS: " << CacheL2->miss << endl;   		
	    cout << "Hit-ratio: " <<  (float)CacheL2->hit/(CacheL2->miss+CacheL1i->hit) << "  Miss-ratio: " << (float)CacheL2->miss/(CacheL2->miss+CacheL2->hit) << endl;
	    cout << "Miss Compulsorio: " << CacheL2->misscomp <<  "  Miss Conflito: " <<  CacheL2->miss - CacheL2->misscomp << endl << endl << endl;
	}
	
}