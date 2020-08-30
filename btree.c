#include <stdlib.h>
#include <stdio.h>

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

typedef struct node{
	int key_array[KEYS];//vetor de inteiros
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
	else{
		insert_nonfull(btree->root,key);//Caso não esteja, basta inserir o nó desejado
	}

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

	if(node->leaf){
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

void print(Node* node){

	printf("Array de chaves: ");
	for (int i = 0; i < KEYS; i++){
		printf("%d ",node->key_array[i]);
	}
	printf("\n");
	printf("\n");

	//printf("Indice %d\n",node->primary_key);
	printf("Número de chaves preenchidas: %d",node->key_index);
	printf("\n");

	if(node->leaf){
		printf("Esse nó é folha!\n");
	}
	else{
		printf("Esse nó não é folha\n");
	}
	printf("______________________________________\n");
}

void run_test(BTree* btree, int keys){

	Result* rs;
	int number;

	printf("Testando com %d de chaves\n",keys);

	for(int i = 1; i <= keys; i++){
		btree->root = insert(i, btree);
	}
	printf("Essa árvore tem %d níveis de altura com %d de chaves\n\n",btree->height,keys);

	printf("Raiz da Árvore:\n\n");
	print(btree->root);

	printf("Pesquise um número na Árvore!\n");
	scanf("%d",&number);

	rs = search(number,btree->root);

	if(rs->found){
		printf("Índice encontrado no nó abaixo:\n");
		printf("______________________________________\n");
		print(rs->node_pointer);
		//printf("%u",rs->depth);
	}
	else{
		printf("Índice não encontrado\n");
	}
}

int main(int argc, char *argv[]){

	printf("\nA ordem da árvore atualmente é %d, para alterar, modifique a linha 8 do código\n\n",ORDER);

	//BTree* btree0 = create_btree();
	//BTree* btree1 = create_btree();
	//BTree* btree2 = create_btree();
	BTree* btree3 = create_btree();

	//run_test(btree0,1000000);
	//free(btree0);

	//run_test(btree1,2000000);
	//free(btree1);

	//run_test(btree2,5000000);
	//free(btree2);

	run_test(btree3,8000000);
	free(btree3);

	return 0;
}