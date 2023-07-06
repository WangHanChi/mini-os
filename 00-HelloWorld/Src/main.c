/*
 *  Filename   : main.c
 *  Created on : 2023/07/06
 *  Author     : WangHanChi
 */
#include <stdint.h>
#include <stdio.h>
#include "myusart.h"

int main(void)
{
	//printf("Start\n");
	MYUSART_Init();
	while(1){
		char data[1000] = {0};
		printf("iinput a data!!\n");
		scanf("%s", data);
		printf("data = %s\n", data);		
		fflush(stdin);	/*clear buffer*/
	}
	return 0;
}