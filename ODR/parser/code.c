#include<stdio.h>
#include<stdlib.h>
#include<gtk/gtk.h>
#include<glib.h>
#include<string.h>

static GQueue* Q;

struct codedata
{
	unsigned int codeid;
	char* data;
       	char* name;
};

void charger(char* path)
{
	FILE*file=fopen(path,"r");
	fseek(file,0,SEEK_END);
	unsigned long int l=ftell(file);
	rewind(file);
	char*data=malloc(l);
	fread((void*)data,l,1,file);
	fclose(file);
	//xml-unmarshall
	//pass 1
	char** regexS=g_regex_split_simple("<DRINK_LIST NUMBER_OF_ITEMS=([0-9]+)>(?:\n|\t)+(<DRINK>(?:.|\n)+</DRINK>)",data,0,0);
	sscanf(regexS[1],"%u",&l);

	char*trim1=malloc(strlen(regexS[2]));
	strcpy(trim1,regexS[2]);
	g_strfreev(regexS);

	for(unsigned int i=0;i<l;i++)
	{
		//add drink
		struct codedata*queuedata=malloc(sizeof(struct codedata));
		queuedata->codeid=0;
		queuedata->data="";
		queuedata->name="";
		g_queue_push_nth (Q,queuedata,0);
	}

	//pass 2
	char*patern="(<DRINK>(?:\t|\n)*<CODEID> [0-9]+ </CODEID>(?:\t|\n)*<DATA SIZE=[0-9]+>(?:.|\n)*</DATA>(?:\n|\t)*<NAME SIZE=[0-9]+> (?:.|\n)*</DRINK>(?:\n|\t)*)";
	char*regexB=malloc(l*strlen(patern));
	char*t;
	t=regexB;
	for(unsigned int i=0;i<l;i++)
		{
			strcpy(t,patern);
			t+=strlen(patern);
		}

	regexS=g_regex_split_simple(regexB,trim1,0,0);
	free(regexB);

	char**trim2=malloc(l*sizeof(char*));
	for(unsigned int i=0;i<l;i++)
	{
		strcpy(trim2[i],regexS[i+1]);
	}

	g_strfreev(regexS);


	//pass3
	GRegex*regexL=g_regex_new("<DRINK>(?:\t|\n)+<CODEID> ([0-9]+) </CODEID>(?:\t|\n)+<DATA SIZE=([0-9]+)> ((?:.|\n)*) </DATA>(?:\n|\t)+<NAME SIZE=([0-9]+)> ((?:.|\n)*) </NAME>(?:\t|\n)*</DRINK>",0,0,NULL);
	for(unsigned int i=0;i<l;i++)
		{
			regexS=g_regex_split(regexL,trim2[i],0);
			struct codedata*data=g_queue_peek_nth(Q,i);
			unsigned int j=0;
			sscanf(regexS[1],"%u",&j);
			data->codeid=j;
			sscanf(regexS[2],"%u",&j);
			data->data=malloc(j);
			data->data=regexS[3];
			sscanf(regexS[4],"%u",&j);
			data->name=malloc(j);
			data->name=regexS[5];
			g_strfreev(regexS);
		}
	g_regex_unref(regexL);
	g_strfreev(trim2);
}

int main()
{
	Q= g_queue_new();
	printf("chemin ? : \n");
	char chemin[300];
	scanf("%s",&chemin);
	charger(chemin);
}
