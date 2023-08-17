#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

int tokenNo = 0;
int pids[64],in=0;
int temp=-1;
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0;
  tokenNo=0 ;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

void handler(int n){
	int w=kill(temp,SIGINT);
	return;
}


int main(int argc, char* argv[]) {
	signal(SIGINT,handler);
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;


	while(1) {
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();
		line[strlen(line)] = '\n'; 
		tokens = tokenize(line);
		while(1) {
			int ret = waitpid(-1, NULL, WNOHANG);
			if(ret > 0) {
				printf("Shell: Background process finished\n");
			} else {
				break;
			}
		}
		if(tokens[0]==NULL){
			continue;
		}
		if(!strcmp(tokens[0],"cd")){
			if(tokenNo>2||chdir(tokens[1])<0){
				printf("Incorrect command\n");
				continue;
			}
			else{	
				continue;
			}
		}
		if(!strcmp(tokens[0],"exit")){
			for(int k=0;k<in;k++){
				int x=kill(pids[k],SIGINT);
			}
			while(wait(NULL)>0);
			for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
			}
			free(tokens);
			exit(0);
		}
		if(!strcmp(tokens[tokenNo-1],"&")){
			int fret=fork();
			pids[in++]=fret;
			if(fret<0){
				printf("fork failed");
			}
			else if(fret==0){
				tokens[tokenNo-1]=NULL;
				execvp(tokens[0],tokens);
				printf("Incorrect command\n");
				exit(0);
			}
			else{
				setpgid(fret,fret);
			}
			
			
		}
		
		else{	
			int fret=fork();
			pids[in++] = fret;
			
			if(fret<0){
				printf("fork failed");
			}
			else if(fret==0){	
				execvp(tokens[0],tokens);
				printf("Incorrect command\n");
				exit(0);
			}
			
			else{	
				setpgid(fret,fret);
				temp=fret;
				int t=waitpid(fret, NULL, 0);
			}
		}
	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
