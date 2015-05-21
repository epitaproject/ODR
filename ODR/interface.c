#include<gtk/gtk.h>
#include<stdio.h>
#include<stdlib.h>
#include<glib.h>
#include<string.h>
#include<errno.h>

static GtkBuilder* interface;
static GtkToggleButton** code;
static GQueue* Q;
static GQueue*Q2;
static int CBBindex=-1;

//page1
void button1_callback()
{
	GtkEntry*entry1=(GtkEntry*)gtk_builder_get_object(interface,"entry1");
	char*path=(char*)gtk_entry_get_text(entry1);
	GtkRange*hscale1=(GtkRange*)gtk_builder_get_object(interface,"hscale1");
	unsigned int min=(int)gtk_range_get_value(hscale1);
	GtkRange*hscale2=(GtkRange*)gtk_builder_get_object(interface,"hscale2");
	unsigned int max=(int)gtk_range_get_value(hscale2);
	FILE*file=fopen("./config","w");
	fprintf(file,"%s %i %i",path,min,max);
	fclose(file);
}

//page2
struct codedata
{
	unsigned int codeid;
	char* data;
	char* name;
};

static int k=0;
void comboboxtext1_callback()
{
	GtkComboBox*comboboxtext1=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext1");
	CBBindex= gtk_combo_box_get_active(comboboxtext1);
	if(CBBindex!=-1&&*gtk_combo_box_get_active_text(comboboxtext1)=='A')
		{
			//add drink
			char* text=g_strdup_printf("QR_%i",k);
			++k;
			gtk_combo_box_insert_text(comboboxtext1,CBBindex,text);
			struct codedata*queuedata=malloc(sizeof(struct codedata));
			queuedata->codeid=0;
			queuedata->data="";
			queuedata->name=text;
			g_queue_push_nth (Q,queuedata,CBBindex);
			gtk_combo_box_set_active(comboboxtext1,CBBindex);
		}
	else if(CBBindex!=-1)
		{
			struct codedata*queuepop=(struct codedata*)g_queue_peek_nth(Q,CBBindex);
			unsigned int id=queuepop->codeid;
			for(unsigned int i=15;i>0;i--)
				{
					gtk_toggle_button_set_active (code[i],id&0x1);
					id>>=1;
				}
			GtkTextBuffer*textbuffer1=(GtkTextBuffer*)gtk_builder_get_object(interface,"textbuffer1");
			gtk_text_buffer_set_text (textbuffer1,queuepop->data,strlen(queuepop->data));
			GtkEntry*entry2=(GtkEntry*)gtk_builder_get_object(interface,"entry2");
			gtk_entry_set_text(entry2,queuepop->name);
		}
}

void button2_callback()
{
	GtkComboBox*comboboxtext1=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext1");
	if(CBBindex!=-1)
		{
			//delete drink
			struct codedata*dataqueue=(struct codedata*)g_queue_pop_nth(Q,CBBindex);
			if(dataqueue)
					free(dataqueue);

			if(g_queue_is_empty(Q))
				k=0;

			gtk_combo_box_remove_text(comboboxtext1,CBBindex);
		}
}

void code_callback()
{
	gtk_toggle_button_set_active(code[0],TRUE);
	gtk_toggle_button_set_active(code[3],TRUE);
	gtk_toggle_button_set_active(code[12],TRUE);
	gtk_toggle_button_set_active(code[15],FALSE);
	if(CBBindex!=-1)
		{
			unsigned int no=1;
			for(unsigned int j=0;j<15;j++)
				{
					no+=gtk_toggle_button_get_active(code[j]);
					no<<=1;
				}
		no+=(int)gtk_toggle_button_get_active(code[15]);
		if(!g_queue_is_empty(Q))
			{
				struct codedata*dataqueue=(struct codedata*)g_queue_peek_nth(Q,CBBindex);
				dataqueue->codeid=no;
			}
		}
}

void textbuffer1_callback()
{
	GtkTextBuffer*textbuffer1=(GtkTextBuffer*)gtk_builder_get_object(interface,"textbuffer1");
	GtkTextIter*start=malloc(sizeof(GtkTextIter));
	GtkTextIter*end=malloc(sizeof(GtkTextIter));
	gtk_text_buffer_get_bounds (textbuffer1,start,end);
	char*bufftext=gtk_text_buffer_get_text(textbuffer1,start,end,TRUE);
	free(start);
	free(end);
	if(!g_queue_is_empty(Q))
		{
			struct codedata*dataqueue=(struct codedata*)g_queue_peek_nth(Q,CBBindex);
			dataqueue->data=bufftext;
		}
}

void button3_callback()
{
	if(CBBindex!=-1)
		{
			struct codedata*queuepop=g_queue_peek_nth(Q,CBBindex);
			GtkEntry *entry2=(GtkEntry*)gtk_builder_get_object(interface,"entry2");
			strcpy(queuepop->name,gtk_entry_get_text(entry2));
		}
}

void button6_callback()
{
	GtkWidget*filechooserdialog1=(GtkWidget*)gtk_builder_get_object(interface,"filechooserdialog1");
	gtk_file_chooser_set_action(GTK_FILE_CHOOSER(filechooserdialog1),GTK_FILE_CHOOSER_ACTION_SAVE);
	GtkWidget*button8=(GtkWidget*)gtk_builder_get_object(interface,"button8");
	gtk_button_set_label ((GtkButton*)button8,"gtk-save");

	int respond=gtk_dialog_run(GTK_DIALOG(filechooserdialog1));
	if(respond==1)
		{
			GFile*gfile=gtk_file_chooser_get_file(GTK_FILE_CHOOSER(filechooserdialog1));
			char*path=g_file_get_path(gfile);
			FILE*file=fopen(path,"w");
			//xml-marshall
			unsigned int k=g_queue_get_length(Q);
			char*datastart=g_strdup_printf("<ODR_DATA>\n\t<DRINK_LIST NUMBER_OF_ITEMS=%i>\n",k);
			char*dataend="\t</DRINK_LIST>\n</ODR_DATA>";
			char**list=malloc(k+1*sizeof(char*));
			list[k]=NULL;
			for(unsigned int i=0;i<k;i++)
				{
					struct codedata*queuepop=g_queue_peek_nth(Q,i);
					char*drink="\t\t<DRINK>\n";
					char*edrink="\t\t</DRINK>\n";
					char*codeid=g_strdup_printf("\t\t\t<CODEID> %u </CODEID>\n",queuepop->codeid);
					unsigned int l=strlen(queuepop->data);
					char*data=g_strdup_printf("\t\t\t<DATA SIZE=%u> %s </DATA>\n",l,queuepop->data);
					unsigned int j=strlen(queuepop->name);
					char*name=g_strdup_printf("\t\t\t<NAME SIZE=%u> %s </NAME>\n",j,queuepop->name);
					list[i]=g_strconcat(drink,codeid,data,name,edrink,NULL);

					free(codeid);
					free(data);
					free(name);
				}
				char*ls=g_strjoinv(NULL,list);
			fprintf(file,"%s%s%s",datastart,ls,dataend);
			free(ls);
			for(unsigned int i=0;i<k;i++)
				{
					free(list[i]);
				}
			free(datastart);
			fclose(file);
			free(path);
		}
	gtk_widget_hide(filechooserdialog1);
}

void button7_callback()
{
	GtkWidget*filechooserdialog1=(GtkWidget*)gtk_builder_get_object(interface,"filechooserdialog1");
	gtk_file_chooser_set_action(GTK_FILE_CHOOSER(filechooserdialog1),GTK_FILE_CHOOSER_ACTION_OPEN);
	GtkWidget*button8=(GtkWidget*)gtk_builder_get_object(interface,"button8");
	gtk_button_set_label ((GtkButton*)button8,"gtk-open");

	int respond=gtk_dialog_run(GTK_DIALOG(filechooserdialog1));
	if(respond==1)
		{
			GFile*gfile=gtk_file_chooser_get_file(GTK_FILE_CHOOSER(filechooserdialog1));
			char*path=g_file_get_path(gfile);
			FILE*file=fopen(path,"r");
			fseek(file,0,SEEK_END);
			unsigned long int l=ftell(file);
			rewind(file);
			free(path);
			char*data=malloc(l);
			fread((void*)data,l,1,file);
			fclose(file);
			//xml-unmarshall
			while(k)
				{
					CBBindex=0;
					button2_callback();
				}
			char** regexS=g_regex_split_simple("<DRINK_LIST NUMBER_OF_ITEMS=([0-9]+)>(?:\n|\t)+(<DRINK>(?:.|\n)+</DRINK>)",data,0,0);
			sscanf(regexS[1],"%u",&l);
			char*trim1=malloc(strlen(regexS[2]));
			GtkComboBox*comboboxtext1=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext1");

			for(unsigned int i=0;i<l;i++)
			{
				gtk_combo_box_set_active (comboboxtext1,i);
			}

			strcpy(trim1,regexS[2]);
			g_strfreev(regexS);

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
			char**trim2=malloc((l+1)*sizeof(char*));
			trim2[l]=NULL;
			for(unsigned int i=0;i<l;i++)
			{
				trim2[i]=malloc(strlen(regexS[i+1]));
				strcpy(trim2[i],regexS[i+1]);
			}
			g_strfreev(regexS);

			GRegex*regexL=g_regex_new("<DRINK>(?:\n|\t)+<CODEID> ([0-9]+) </CODEID>(?:\n|\t)+<DATA SIZE=([0-9]+)> ((?:\n|.)*) </DATA>(?:\n|\t)+<NAME SIZE=([0-9]+)> (.*) </NAME>(?:\n|\t)+</DRINK>",0,0,NULL);
			unsigned int j;
			for(unsigned int i=0;i<l;i++)
				{
					regexS=g_regex_split(regexL,trim2[i],0);
					struct codedata*data=g_queue_peek_nth(Q,i);
					sscanf(regexS[1],"%u",&j);
					data->codeid=j;
					sscanf(regexS[2],"%u",&j);
					data->data=malloc(j);
					strcpy(data->data,regexS[3]);
					sscanf(regexS[4],"%u",&j);
					data->name=malloc(j);
					strcpy(data->name,regexS[5]);
					g_strfreev(regexS);
				}
			g_strfreev(trim2);
			g_regex_unref(regexL);
		}
	gtk_widget_hide(filechooserdialog1);
}

//page3
struct cocktaildata
{
	char*name;
	char*description;
	GQueue*list;
};

unsigned int l=0;
void comboboxtext2_callback()
{
	GtkComboBox*comboboxtext2=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext2");
	CBBindex= gtk_combo_box_get_active(comboboxtext2);
	if(CBBindex!=-1&&*gtk_combo_box_get_active_text(comboboxtext2)=='A')
		{
			//add cocktail
			char* text=malloc(10);
			g_strdup_printf(text,"cocktail_%i",l);
			++l;
			gtk_combo_box_insert_text(comboboxtext2,CBBindex,text);
			struct cocktaildata*queuedata=malloc(sizeof(struct cocktaildata));
			queuedata->name=text;
			queuedata->description="";
			queuedata->list=g_queue_new();
			g_queue_push_nth (Q2,queuedata,CBBindex);
			gtk_combo_box_set_active(comboboxtext2,CBBindex);
		}
	else if(CBBindex!=-1)
		{
			struct cocktaildata*queuepop=(struct cocktaildata*)g_queue_peek_nth(Q2,CBBindex);
			GtkTextBuffer*textbuffer2=(GtkTextBuffer*)gtk_builder_get_object(interface,"textbuffer2");
			gtk_text_buffer_set_text (textbuffer2,queuepop->description,strlen(queuepop->description));
			GtkEntry*entry3=(GtkEntry*)gtk_builder_get_object(interface,"entry3");
			gtk_entry_set_text(entry3,queuepop->name);
		}
}

void button4_callback()
{
	GtkComboBox*comboboxtext2=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext2");
	if(CBBindex!=-1)
		{
			//delete cocktail
			struct cocktaildata*dataqueue=(struct cocktaildata*)g_queue_pop_nth(Q2,CBBindex);
			if(dataqueue)
				free(dataqueue);

			if(g_queue_is_empty(Q2))
				l=0;

			gtk_combo_box_remove_text(comboboxtext2,CBBindex);
		}
}

void button5_callback()
{
	if(CBBindex!=-1)
		{
			struct cocktaildata*queuepop=g_queue_peek_nth(Q2,CBBindex);
			GtkEntry *entry3=(GtkEntry*)gtk_builder_get_object(interface,"entry3");
			strcpy(queuepop->name,gtk_entry_get_text(entry3));
		}
}

void textbuffer2_callback()
{
	GtkTextBuffer*textbuffer2=(GtkTextBuffer*)gtk_builder_get_object(interface,"textbuffer2");
	GtkTextIter*start=malloc(sizeof(GtkTextIter));
	GtkTextIter*end=malloc(sizeof(GtkTextIter));
	gtk_text_buffer_get_bounds (textbuffer2,start,end);
	char*bufftext=gtk_text_buffer_get_text(textbuffer2,start,end,TRUE);
	free(start);
	free(end);
	if(!g_queue_is_empty(Q2))
		{
			struct cocktaildata*dataqueue=(struct cocktaildata*)g_queue_peek_nth(Q2,CBBindex);
			dataqueue->description=bufftext;
		}
}

void main_window()
{
	interface=gtk_builder_new();
	gtk_builder_add_from_file(interface,"./interface.ui",NULL);
	GtkWidget *window1;
	window1=(GtkWidget*)gtk_builder_get_object(interface,"window1");
	g_signal_connect(G_OBJECT(window1), "destroy",G_CALLBACK(gtk_main_quit), NULL);

	//page1
	GtkWidget*button1=(GtkWidget*)gtk_builder_get_object(interface,"button1");
	g_signal_connect(G_OBJECT(button1),"clicked",G_CALLBACK(button1_callback),NULL);

	//page2
	GtkTable*table1=(GtkTable*)gtk_builder_get_object(interface,"table1");
	code=malloc(16*sizeof(GtkToggleButton*));
	unsigned k=0;
	for(unsigned int i=0;i<4;i++)
		{
			for(unsigned int j=0;j<4;j++)
				{
					code[k]=(GtkToggleButton*)gtk_toggle_button_new();
					gtk_table_attach_defaults(table1,(GtkWidget*)code[k],i,i+1,j,j+1);
					g_signal_connect(G_OBJECT(code[k]),"toggled",code_callback,NULL);
					k++;
				}
		}
	gtk_toggle_button_set_active(code[0],TRUE);
	gtk_toggle_button_set_active(code[3],TRUE);
	gtk_toggle_button_set_active(code[12],TRUE);
	k=0;
	GtkWidget*comboboxtext1=(GtkWidget*)gtk_builder_get_object(interface,"comboboxtext1");
	g_signal_connect(G_OBJECT(comboboxtext1),"changed",G_CALLBACK(comboboxtext1_callback),NULL);
	GtkWidget*button2=(GtkWidget*)gtk_builder_get_object(interface,"button2");
	g_signal_connect(G_OBJECT(button2),"clicked",G_CALLBACK(button2_callback),NULL);
	GtkTextBuffer*textbuffer1=(GtkTextBuffer*)gtk_builder_get_object(interface,"textbuffer1");
	g_signal_connect(G_OBJECT(textbuffer1),"changed",G_CALLBACK(textbuffer1_callback),NULL);
	GtkWidget*button3=(GtkWidget*)gtk_builder_get_object(interface,"button3");
	g_signal_connect(G_OBJECT(button3),"clicked",G_CALLBACK(button3_callback),NULL);

	//page3
	GtkWidget*button4=(GtkWidget*)gtk_builder_get_object(interface,"button4");
	g_signal_connect(G_OBJECT(button4),"clicked",G_CALLBACK(button4_callback),NULL);
	GtkWidget*button5=(GtkWidget*)gtk_builder_get_object(interface,"button5");
	g_signal_connect(G_OBJECT(button5),"clicked",G_CALLBACK(button5_callback),NULL);
	GtkWidget*comboboxtext2=(GtkWidget*)gtk_builder_get_object(interface,"comboboxtext2");
	g_signal_connect(G_OBJECT(comboboxtext2),"changed",G_CALLBACK(comboboxtext2_callback),NULL);
	GtkTextBuffer*textbuffer2=(GtkTextBuffer*)gtk_builder_get_object(interface,"textbuffer2");
	g_signal_connect(G_OBJECT(textbuffer2),"changed",G_CALLBACK(textbuffer2_callback),NULL);

	//save/load dialog
	GtkWidget*button6=(GtkWidget*)gtk_builder_get_object(interface,"button6");
	g_signal_connect(G_OBJECT(button6),"clicked",G_CALLBACK(button6_callback),NULL);
	GtkWidget*button7=(GtkWidget*)gtk_builder_get_object(interface,"button7");
	g_signal_connect(G_OBJECT(button7),"clicked",G_CALLBACK(button7_callback),NULL);


	Q= g_queue_new();
	Q2=g_queue_new();
	gtk_widget_show_all(window1);
}

int main()
{
	gtk_init(NULL,NULL);
	main_window();
	gtk_main();
}
