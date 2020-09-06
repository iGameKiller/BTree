#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#define EMPTY 0

#define ORDER 32 //Ordem da Árvore
#define POINTERS	(ORDER*2) //6
#define KEYS		(POINTERS-1) //5

/*
#######################################
# Aluno: João Pedro Mendonça de Souza #
# Matrícula: 0035330				  #
# Curso: Ciência da Computação	   	  #
# Disciplina: Estrutura de Dados 2    #
#######################################
*/

typedef unsigned char bool;
int GetLeftMax(Node* node);
int GetRightMin(Node* node);

typedef struct node{
	int key_array[KEYS];///vetor de inteiros
	struct node* child_array[POINTERS];//vetor de ponteiros que apontam para o mesmo tipo de nó
	unsigned int key_index;//aponta quantas posições estão ocupadas no vetor de inteiros
	bool leaf;//indica se é o não um nó folha
}Node;

typedef struct result{
	Node* node_pointer;
	int key;
	bool found;
}Result;

typedef struct b_tree{
	Node *root;
	unsigned short order;
	int height;
}BTree;

Node* create_node(){

	int i;
	Node *new = (Node*)malloc(sizeof(Node));

	if(!new){
		printf("Sem memória disponível");
		exit(0);
	}

	for(i = 0; i < KEYS; i++){
		new->key_array[i] = 0;
	}

	for(i = 0;i < POINTERS; i++){
		new->child_array[i] = NULL;
	}

	new->key_index = EMPTY;
	new->leaf = TRUE;

	return new;
}

BTree* create_btree(){

	BTree* new_root = (BTree*)malloc(sizeof(BTree));

	if(!new_root){

		printf("Sem memória disponível");
		exit(0);

		return NULL;
	}

	Node* head = create_node();

	if(!head){
		return NULL;
	}

	new_root->order = ORDER;
	new_root->root = head;
	new_root->height = 1;
	new_root->diskaccess = 0;

	return new_root;
}

void split_child(Node* dad_node, int i, Node* son_node){

	int j;

	//Aloca um novo nó para guardar o nó do "array filho"
	Node* new = create_node();
	new->leaf = son_node->leaf;
	new->key_index = ORDER-1;

	//Move a metade da direita do array filho para o novo nó
	for (int j = 0; j < ORDER-1; j++){
		new->key_array[j] = son_node->key_array[ORDER+j];
		son_node->key_array[ORDER+j] = 0;
	}

	//se o array filho não for folha, então mova o child_array do son_node
	//para o child_array do novo nó
	if(!son_node->leaf){
		for (j = 0; j < ORDER; j++){
			new->child_array[j] = son_node->child_array[ORDER+j];
		}
	}
	son_node->key_index = ORDER;

	for(j = dad_node->key_index; j >= i; j--){
		dad_node->child_array[j+1] = dad_node->child_array[j];
	}

	dad_node->child_array[i] = new;

	for (j = dad_node->key_index; j >= i; j--){
		dad_node->key_array[j] = dad_node->key_array[j-1];
	}

	dad_node->key_array[i-1] = son_node->key_array[ORDER-1];

	dad_node->key_index++;
}

void insert_nonfull(Node* node, int key){

	int i = node->key_index;

	if(node->leaf){//se "leaf (0 ou 1)" for verdadeiro
		while(i >= 1 && key < node->key_array[i - 1]){//enquanto i for maior ou igual a 1 e a posição i
			node->key_array[i] = node->key_array[i - 1];//for menor que a posição i - 1, mova o número uma posição no vetor
			i--;
		}
		node->key_array[i] = key;
		node->key_index++;//aumente o contador do número de chaves ocupadas
	}
	else{
		while(i >= 1 && key < node->key_array[i - 1]){//enquanto i for maior ou igual a 1 e a posição i
			i--;
		}

		if(node->child_array[i]->key_index == KEYS){
			split_child(node, i+1, node->child_array[i]);
			if(key > node->key_array[i]){
				i++;
			}
		}
		insert_nonfull(node->child_array[i], key);
	}
}

Node* insert(int key, BTree* btree){

	Node* root = btree->root;

	if(root->key_index == KEYS){//Verifica se o vetor de chaves está cheio, se estiver.....?
		Node* new = create_node();
		btree->root = new;
		new->leaf = FALSE;
		new->key_index = 0;
		new->child_array[0] = root;
		split_child(new,1,root);
		insert_nonfull(new,key);
		btree->height++;
	}
	else
		insert_nonfull(btree->root,key);//Caso não esteja, basta inserir o nó desejado
	

	return btree->root;
}

Result* get_result(){

	Result *ret = (Result*)malloc(sizeof(Result));

	if(!ret){
		printf("Erro! Sem memória\n");
		exit(0);
	}

	ret->node_pointer = NULL;
	ret->key = 0;
	ret->found = FALSE;

	return ret;
}

Result* search(int key, Node* node){

	int i = 0;

	while((i < node->key_index)&&(key > node->key_array[i])){
		i++;
	}

	if((i <= node->key_index)&&(key == node->key_array[i])){
		Result* result = get_result();
		result->node_pointer = node;
		result->key = i;
		result->found = TRUE;
		return result;
	}
	else if(node->leaf){
		Result* result = get_result();
		result->node_pointer = node;
		result->found = FALSE;
		return result;
	}
	else{
		Result* result = get_result();
		return search(key,node->child_array[i]);
	}
}

int GetLeftMax(Node* node){

	if(0 == node->leaf)
		return GetLeftMax(node->child_array[node->key_index]);
	else
		return node->key_array[node->key_index-1];
}

int GetRightMin(Node* node){

	if(0 == node->leaf)
		return GetRightMin(node->child_array[0]);
	else
		return node->key_array[node->key_index-1];
}

void mergeChildren(Node* root, int index, Node* child1, Node* child2){

	child1->key_index = KEYS;
	int i;

	for (int i = ORDER; i < KEYS; i++)
		child1->key_array[i] = child2->key_array[i-ORDER];

	child1->key_array[ORDER-1] = root->key_array[index];

	if(0 == child2->leaf){

		for (i = ORDER; i < POINTERS; i++)
			child1->child_array[i] = child2->child_array[i-ORDER];
	}

	for (i = index + 1; i < root->key_index; i++){
		root->key_array[i-1] = root->key_array[i];
		root->child_array[i] = root->child_array[i+1];
	}
	root->key_index--;
	free(child2);
}

void BorrowFromLeft(Node* root, int index, Node* leftPtr, Node* curPtr){

	curPtr->key_index++;
	int i;

	for (i = curPtr->key_index; i > 0 ; i--)
		curPtr->key_array[i] = curPtr->key_array[i-1];

	curPtr->key_array[0] = root->key_array[index];
	root->key_array[index] = leftPtr->key_array[leftPtr->key_index-1];

	if(0 == leftPtr->leaf)
		for (i = curPtr->key_index; i > 0; i--)
			curPtr->child_array[i] = curPtr->child_array[i-1];

	curPtr->child_array[0] = leftPtr->child_array[leftPtr->key_index];
	leftPtr->key_index--;
}

void BorrowFromRight(Node* root, int index, Node* rightPtr, Node* curPtr){

	curPtr->key_index++;
	curPtr->key_array[curPtr->key_index-1] = root->key_array[index];
	root->key_array[index] = rightPtr->key_array[0];
	int i;

	for (i = 0; i < rightPtr->key_index-1; i++)
		rightPtr->key_array[i] = rightPtr->key_array[i+1];

	if(0 == rightPtr->leaf){
		curPtr->child_array[curPtr->key_index] = rightPtr->child_array[0];

		for (i = 0; i < rightPtr->key_index; i++)
			rightPtr->child_array[i] = rightPtr->child_array[i+1];
	}

	rightPtr->key_index--;
}

void DeleteNoNone(int key, Node* root){

	int i;

	if(1 == root->leaf){
		i = 0;
		while((i < root->key_index)&&(key > root->key_array[i]))
			i++;

		if(key == root->key_array[i]){
			for (i = 0; i < root->key_index-1; i++)
				root->key_array[i] = root->key_array[i+1];

			root->key_index--;
		}
		else{
			printf("Número não encontrado\n");
		}
	}
	else{
		i = 0;

		Node* previous = NULL;
		Node* next = NULL;

		while((i < root->key_index) && (key > root->key_array[i]))
			i++;

		if((i < root->key_index) && (key == root->key_array[i])){
			previous = root->child_array[i];
			next = root->child_array[i+1];

			if(previous->key_index > ORDER-1){//se um dois nós tiver menos que o mínimo de chaves, merge!
				int aPrecursor = GetLeftMax(previous);
				root->key_array[i] = aPrecursor;
				DeleteNoNone(aPrecursor,previous);
			}
			else if(next->key_index > ORDER-1){

				int aSuccessor = GetRightMin(next);
				root->key_array[i] = aSuccessor;
				DeleteNoNone(aSuccessor,next);
			}
			else{
				mergeChildren(root,i,previous,next);
				DeleteNoNone(key,previous);
			}
		}
		else{
			previous = root->child_array[i];
			Node* leftBro = NULL;

			if(i < root->key_index)
				next = root->child_array[i+1];
			if(i > 0)
				leftBro = root->child_array[i-1];

			if(ORDER-1 == previous->key_index){

				if((leftBro != NULL) && (leftBro->key_index > ORDER-1))
					BorrowFromLeft(root, i-1, leftBro, previous);

				else if((next != NULL)&&(next->key_index > ORDER-1))
					BorrowFromRight(root,i,next,previous);

				else if(leftBro != NULL){
					mergeChildren(root,i-1,leftBro,previous);
					previous = leftBro;
				}
				else
					mergeChildren(root,i,previous,next);
				
			}
			DeleteNoNone(key,previous);
		}
	}
}

Node* delete(int key, BTree* btree){

	if(btree->root->key_index == 1){
		Node* child1 = btree->root->child_array[0];
		Node* child2 = btree->root->child_array[1];

		if((!child1)&&(!child2)){

			if((ORDER-1 == child1->key_index) && (ORDER-1 == child2->key_index)){
				mergeChildren(btree->root, 0, child1, child2);
				free(btree->root);
				DeleteNoNone(key,child1);
				return child1;
			}
		}
	}
	DeleteNoNone(key,btree->root);

	return btree->root;
}

void print(Node* node){

	printf("Índice de chaves: ");
	for (int i = 0; i < KEYS; i++){
		printf("%d ",node->key_array[i]);
	}
	printf("\n");
	printf("\n");

	printf("Número de chaves preenchidas: %d",node->key_index);
	printf("\n");

	if(node->leaf){
		printf("Esse nó é folha!\n");
	}
	else
		printf("Esse nó não é folha\n");

	printf("______________________________________\n");
}

void run_test(BTree* btree, int keys){

	Result* rs;
	int op, number;
	float numpages,totalkeys = 0;
	float taxa = 0;
	printf("____________________________________________________________________________________________________\n");	
	printf("Testando com %d chaves\n",keys);

	for(int i = 1; i <= keys; i++){
		btree->root = insert(i, btree);
	}
	printf("* Essa árvore possui %d níveis de altura com %d chaves\n",btree->height,keys);

	for (int i = 0; i < btree->height; i++){
		numpages += pow(POINTERS,i);
	}

	totalkeys = (numpages * KEYS);
	taxa = (keys/totalkeys);
	
	printf("** A taxa de utilização da árvore é de %f com um total de %.0f slots disponíveis\n",taxa,totalkeys);
	printf("*** O disco foi acessado em média %f vezes para guardar %d chaves\n",(numpages/keys),keys);
	printf("O máximo de páginas que essa árvore B comporta é %.0f\n",numpages);

	printf("Raiz da Árvore:\n\n");
	print(btree->root);

	printf("Deseja pesquisar um número na Árvore ?\n");//Implementado porém não é importante para os testes
	printf("1 - Sim\n");
	printf("2 - Não\n");
	scanf("%d",&op);

	if(op == 1){
		printf("Digite seu número para pesquisa!\n");
		scanf("%d",&number);

		rs = search(number,btree->root);

		if(rs->found){
			printf("Seu número foi encontrado no nó abaixo:\n");
			printf("______________________________________\n");
			print(rs->node_pointer);
		}
		else
			printf("Índice não encontrado\n");
	}

	op = 0;
	number = 0;

	printf("Deseja deletar algum número ?\n");
	printf("1 - Sim\n");
	printf("2 - Não\n");
	scanf("%d",&op);

	if(op == 1){
		printf("Digite seu número para deletar!\n");
		scanf("%d",&number);

		btree->root = delete(number,btree);
	}

	print(btree->root);

	
}

int main(int argc, char *argv[]){

	printf("\nA ordem da árvore atualmente é %d, para alterar, modifique a linha 9 do código\n\n",ORDER);

	BTree* btree0 = create_btree();
	BTree* btree1 = create_btree();
	BTree* btree2 = create_btree();
	BTree* btree3 = create_btree();

	run_test(btree0,1000000);
	free(btree0);

	run_test(btree1,2000000);
	free(btree1);

	run_test(btree2,5000000);
	free(btree2);

	run_test(btree3,8000000);
	free(btree3);

	return 0;
}