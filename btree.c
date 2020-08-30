#include <stdlib.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define EMPTY 0

#define ORDER 32/*The degree of the tree.*/
#define POINTERS	(ORDER*2) //6
#define KEYS		(POINTERS-1) //5

typedef unsigned char bool;

typedef struct node{
	int key_array[KEYS];//vetor de inteiros
	struct node* child_array[POINTERS];//vetor de ponteiros que apontam para o mesmo tipo de nó
	unsigned int key_index;//aponta quantas posições estão ocupadas no vetor de inteiros
	bool leaf;//indica se é o não um nó folha
	//int primary_key;
}Node;

typedef struct result{
	Node* node_pointer;
	int key;
	bool found;
	unsigned int depth;
}Result;

typedef struct b_tree{
	Node *root;
	unsigned short order;
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
	//new->primary_key = cont;

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

void print(Node* node){

	printf("Key array:\n");
	for (int i = 0; i < KEYS; i++){
		printf("%d ",node->key_array[i]);
	}
	printf("\n");
	printf("\n");

	//printf("Indice %d\n",node->primary_key);
	printf("Key index %d",node->key_index);
	printf("\n");
	printf("______________\n");

	if(node->leaf){
		printf("É folha\n");
	}
	else{
		printf("Não é folha\n");
	}
	printf("\n");
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
		//new->primary_key = 0;
		split_child(new,1,root);
		insert_nonfull(new,key);
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
	ret->depth = 0;

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
		//result->depth = result->depth + 1;
		return search(key,node->child_array[i]);
	}
}

int main(int argc, char *argv[]){

	//int todelete[] = {15, 19};

	BTree* main = create_btree();
	int i;
	int number;
	Result* rs;

	/*for(i = 0; i < 100; i++){
		main->root = insert(i, main);
	}*/
	
	/*
	print(main->root->child_array[0]);
	printf("_________________________\n");
	print(main->root->child_array[1]);
	printf("_________________________\n");
	print(main->root->child_array[2]);
	*/

	for(i = 1; i <= 1000000; i++){
		main->root = insert(i, main);
	}
	print(main->root);
	printf("_________________________\n");
	/*for(i = 0; i < 5000000; i++){
		main->root = insert(i, main);
	}*/

	printf("Digite um número pra procurar\n");
	scanf("%d",&number);
	
	rs = search(number,main->root);

	if(rs->found){
		printf("Encontrado!\n");
		print(rs->node_pointer);
		//printf("%u",rs->depth);
	}

	/*for(i = 0; i < 100; i+=10){
		main->root = insert(i, main);
	}
	printf("Raiz da Árvore\n");
	print(main->root);
	printf("Filho 1 da raiz\n");
	print(main->root->child_array[0]);
	printf("Filho 2 da raiz\n");
	print(main->root->child_array[1]);
	printf("Filho 3 da raiz\n");
	print(main->root->child_array[2]);
	*/

	/*for(i = 0; i < 8000000; i++){
		main->root = insert(i, main);
	}*/

	free(main);

	return 0;
}