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
static gboolean invoked=FALSE;

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
	int CBBindex= gtk_combo_box_get_active(comboboxtext1);
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
	int CBBindex= gtk_combo_box_get_active(comboboxtext1);
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
	GtkComboBox*comboboxtext1=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext1");
	int CBBindex= gtk_combo_box_get_active(comboboxtext1);
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
			GtkComboBox*comboboxtext1=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext1");
			int CBBindex= gtk_combo_box_get_active(comboboxtext1);
			struct codedata*dataqueue=(struct codedata*)g_queue_peek_nth(Q,CBBindex);
			dataqueue->data=bufftext;
		}
}

void button3_callback()
{
	GtkComboBox*comboboxtext1=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext1");
	int CBBindex= gtk_combo_box_get_active(comboboxtext1);
	if(CBBindex!=-1)
		{
			struct codedata*queuepop=g_queue_peek_nth(Q,CBBindex);
			GtkEntry *entry2=(GtkEntry*)gtk_builder_get_object(interface,"entry2");
			strcpy(queuepop->name,gtk_entry_get_text(entry2));
		}
}


struct cocktaildata
{
	char*name;
	char*description;
	GQueue*list;
};

void button6_callback()
{
	GtkWidget*filechooserdialog1=(GtkWidget*)gtk_builder_get_object(interface,"filechooserdialog1");
	gtk_file_chooser_set_action(GTK_FILE_CHOOSER(filechooserdialog1),GTK_FILE_CHOOSER_ACTION_SAVE);
	GtkWidget*button8=(GtkWidget*)gtk_builder_get_object(interface,"button8");
	gtk_button_set_label ((GtkButton*)button8,"gtk-save");

	int respond=gtk_dialog_run(GTK_DIALOG(filechooserdialog1));
	if(respond!=1)
		{
			gtk_widget_hide(filechooserdialog1);
			return;
		}

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
		
		//cocktail marshall
		char*datacocktailstart="\t\t<COCKTAILS>\n";
		char*datacocktailend="\t\t</COCKTAILS>\n";
		char**Clist=malloc((g_queue_get_length(Q2)+1)*sizeof(char*));
		Clist[g_queue_get_length(Q2)]=NULL;
		for(unsigned int i=0;i<g_queue_get_length(Q2);i++)
		  {
		    //bind a single cocktail
		    char*cocktailstart="\t\t\t<COCKTAIL>\n";
		    char*cocktailend="\t\t\t</COCKTAIL>\n";
		    struct cocktaildata*cocktaildata=(struct cocktaildata*)g_queue_peek_nth(Q2,i);
		    char*name=cocktaildata->name;
		    char*XMLname=g_strdup_printf("\t\t\t\t<NAME SIZE=%u> %s </NAME>\n",strlen(name),name);
		    char*description=cocktaildata->description;
		    char*XMLdescription=g_strdup_printf("\t\t\t\t<DESCRIPTION SIZE=%u> %s </DESCRIPTION>\n",strlen(description),description);
		    //bind the list of drink for current cocktail
		    char*drinkliststart="\t\t\t\t<DRINKLIST>\n";
		    char*drinklistend="\t\t\t\t</DRINKLIST>\n";
		    GQueue*Q3=cocktaildata->list;
		    char**drinklist=malloc((g_queue_get_length(Q3)+1)*sizeof(char*));
		    drinklist[g_queue_get_length(Q3)]=NULL;
		    for(unsigned int i=0;i<g_queue_get_length(Q3);i++)
		      {
			//bind a single drink
			struct codedata*queuepop=g_queue_peek_nth(Q3,i);
			char*drink="\t\t\t\t\t<DRINK>\n";
			char*edrink="\t\t\t\t\t</DRINK>\n";
			char*codeid=g_strdup_printf("\t\t\t\t\t\t<CODEID> %u </CODEID>\n",queuepop->codeid);
			unsigned int l=strlen(queuepop->data);
			char*data=g_strdup_printf("\t\t\t\t\t\t<DATA SIZE=%u> %s </DATA>\n",l,queuepop->data);
			unsigned int j=strlen(queuepop->name);
			char*name=g_strdup_printf("\t\t\t\t\t\t<NAME SIZE=%u> %s </NAME>\n",j,queuepop->name);
			drinklist[i]=g_strconcat(drink,codeid,data,name,edrink,NULL);

			free(codeid);
			free(data);
			free(name);
		      }
		    
		    char*drinklistcat=g_strjoinv(NULL,drinklist);
		    g_strfreev(drinklist);
		    char*drinklistcatXML=g_strconcat(drinkliststart,drinklistcat,drinklistend,NULL);
		    Clist[i]=g_strconcat(cocktailstart,XMLname,XMLdescription,drinklistcatXML,cocktailend,NULL);
		    free(XMLname);
		    free(XMLdescription);
		    free(drinklistcat);
		    free(drinklistcatXML);
		  }
		char*cocktaildatastringfull=g_strjoinv(NULL,Clist);
		g_strfreev(Clist);
		char*cocktailXMLfull=g_strconcat(datacocktailstart,cocktaildatastringfull,datacocktailend,NULL);
		free(cocktaildatastringfull);		 
		//end of cocktail marshall
		fprintf(file,"%s%s%s%s",datastart,ls,cocktailXMLfull,dataend);
		free(cocktailXMLfull);
		free(ls);
		for(unsigned int i=0;i<k;i++)
		  {
		    free(list[i]);
		  }
		free(datastart);
		fclose(file);
		free(path);
		gtk_widget_hide(filechooserdialog1);
}

void codetab_callback(GtkWidget*widget,gpointer data)
{
	if(invoked){return;}
	unsigned int i=gtk_toggle_button_get_active((GtkToggleButton*)widget);
	char*btnlbl=(char*)gtk_button_get_label((GtkButton*)widget);
	GtkComboBox*comboboxtext2=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext2");
	int k=gtk_combo_box_get_active(comboboxtext2);
	GQueue*Q3=((struct cocktaildata*)g_queue_peek_nth(Q2,k))->list;
	if(i)
		{
			//add drink to current cocktail
			struct codedata*data=g_queue_peek_head(Q);
			if(!data)
				{
					return;
				}
			for(unsigned int i=0;i<g_queue_get_length(Q);i++)
				{
					data=(struct codedata*)g_queue_peek_nth(Q,i);
					char*name=data->name;
					if(!strcmp(btnlbl,name))
						{
							g_queue_push_head(Q3,data);
							return;
						}
				}
		}
	else
		{
			//remove drink from current cocktail
			for(unsigned int i=0;i<g_queue_get_length(Q3);i++)
				{
					struct codedata*codedata=(struct codedata*)g_queue_peek_nth(Q3,i);
					char*name=codedata->name;
					if(!strcmp(btnlbl,name))
						{
							g_queue_pop_nth(Q3,i);
							return;
						}
				}
		}
}

void GFdump(gpointer data,gpointer userdata)
{
	gtk_widget_destroy((GtkWidget*)data);
}

void button7_callback()
{
	GtkWidget*filechooserdialog1=(GtkWidget*)gtk_builder_get_object(interface,"filechooserdialog1");
	gtk_file_chooser_set_action(GTK_FILE_CHOOSER(filechooserdialog1),GTK_FILE_CHOOSER_ACTION_OPEN);
	GtkWidget*button8=(GtkWidget*)gtk_builder_get_object(interface,"button8");
	gtk_button_set_label ((GtkButton*)button8,"gtk-open");

	int respond=gtk_dialog_run(GTK_DIALOG(filechooserdialog1));
	if(respond!=1)
		{
			gtk_widget_hide(filechooserdialog1);
			return;
		}
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
	GtkComboBox*comboboxtext1=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext1");
	while(k)
		{
			gtk_combo_box_set_active(comboboxtext1,0);
			button2_callback();
		}
	char** regexS=g_regex_split_simple("<DRINK_LIST NUMBER_OF_ITEMS=([0-9]+)>(?:\n|\t)+(<DRINK>(?:.|\n)+</DRINK>)",data,0,0);
	sscanf(regexS[1],"%u",&l);
	char*trim1=malloc(strlen(regexS[2]));

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
	gtk_widget_hide(filechooserdialog1);
	comboboxtext1_callback();


//xml-unmarshall cocktail


	GtkContainer*vbox1=(GtkContainer*)gtk_builder_get_object(interface,"vbox1");
	GList*children=gtk_container_get_children(vbox1);
	g_list_foreach(children,GFdump,NULL);
	GtkWidget**codetab=malloc(l*sizeof(GtkWidget*));
	for(unsigned int i=0;i<l;i++)
		{
			struct codedata*codedata=(struct codedata*)g_queue_peek_nth(Q,i);
			char*name=codedata->name;
			codetab[i]=gtk_check_button_new_with_label(name);
			g_signal_connect(G_OBJECT(codetab[i]),"toggled",G_CALLBACK(codetab_callback),NULL);
			gtk_box_pack_start_defaults ((GtkBox*)vbox1,codetab[i]);
		}
	free(codetab);
	gtk_widget_show_all((GtkWidget*)vbox1);
}

//page3

unsigned int l=0;
void comboboxtext2_callback()
{
	GtkComboBox*comboboxtext2=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext2");
	int CBBindex= gtk_combo_box_get_active(comboboxtext2);
	if(CBBindex!=-1&&*gtk_combo_box_get_active_text(comboboxtext2)=='A')
		{
			//add cocktail
			char* text=g_strdup_printf("cocktail_%i",l);
			++l;
			gtk_combo_box_insert_text(comboboxtext2,CBBindex,text);
			struct cocktaildata*queuedata=malloc(sizeof(struct cocktaildata));
			queuedata->name=g_strdup(text);
			queuedata->description=g_strdup("");
			queuedata->list=g_queue_new();
			g_queue_push_nth (Q2,queuedata,CBBindex);
			gtk_combo_box_set_active(comboboxtext2,CBBindex);
			GtkContainer*vbox1=(GtkContainer*)gtk_builder_get_object(interface,"vbox1");
			GList*children=gtk_container_get_children(vbox1);
			while(children)
				{
					gtk_toggle_button_set_active((GtkToggleButton*)(children->data),FALSE);
					children=children->next;
				}
		}
	else if(CBBindex!=-1)
		{
			struct cocktaildata*queuepop=(struct cocktaildata*)g_queue_peek_nth(Q2,CBBindex);
			GtkTextBuffer*textbuffer2=(GtkTextBuffer*)gtk_builder_get_object(interface,"textbuffer2");
			gtk_text_buffer_set_text (textbuffer2,queuepop->description,strlen(queuepop->description));
			GtkEntry*entry3=(GtkEntry*)gtk_builder_get_object(interface,"entry3");
			gtk_entry_set_text(entry3,queuepop->name);
			GtkContainer*vbox1=(GtkContainer*)gtk_builder_get_object(interface,"vbox1");
			GList*children=gtk_container_get_children(vbox1);
			GQueue*Q3=(GQueue*)(((struct cocktaildata*)g_queue_peek_nth(Q2,CBBindex))->list);
			while(children)
				{
					GtkToggleButton*child=(GtkToggleButton*)(children->data);
					char*btnlbl=(char*)gtk_button_get_label((GtkButton*)child);
					unsigned int i=0;
					for(;i<g_queue_get_length(Q3);i++)
						{
							struct codedata*data=(struct codedata*)g_queue_peek_nth(Q3,i);
							char*name=data->name;
							if(!strcmp(name,btnlbl))
								{
									invoked=TRUE;
									gtk_toggle_button_set_active(child,TRUE);
									invoked=FALSE;
									break;
								}
						}
					if(i==g_queue_get_length(Q3))
						{
							invoked=TRUE;
							gtk_toggle_button_set_active(child,FALSE);
							invoked=FALSE;
						}
					children=children->next;
				}
		}
}
void freecodedata(gpointer data)
{
  struct codedata*codedata=(struct codedata*)data;
  free(codedata->data);
  free(codedata->name);
}
void button4_callback()
{
	GtkComboBox*comboboxtext2=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext2");
	int CBBindex= gtk_combo_box_get_active(comboboxtext2);
	if(CBBindex!=-1)
		{
			//delete cocktail
			struct cocktaildata*dataqueue=(struct cocktaildata*)g_queue_pop_nth(Q2,CBBindex);
			if(dataqueue)
			  {
			    free(dataqueue->name);
			    free(dataqueue->description);
			    g_queue_free_full(dataqueue->list,freecodedata);
			    g_queue_free(dataqueue->list);
			    free(dataqueue);//doesn't free ?
			  }

			if(g_queue_is_empty(Q2))
				l=0;

			gtk_combo_box_remove_text(comboboxtext2,CBBindex);
		}
}

void button5_callback()
{
	GtkComboBox*comboboxtext2=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext2");
	int CBBindex= gtk_combo_box_get_active(comboboxtext2);
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
			GtkComboBox*comboboxtext2=(GtkComboBox*)gtk_builder_get_object(interface,"comboboxtext2");
			int CBBindex= gtk_combo_box_get_active(comboboxtext2);
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
	GtkWidget*button10=(GtkWidget*)gtk_builder_get_object(interface,"button10");
	g_signal_connect(G_OBJECT(button10),"clicked",G_CALLBACK(button6_callback),NULL);
	GtkWidget*button11=(GtkWidget*)gtk_builder_get_object(interface,"button11");
	g_signal_connect(G_OBJECT(button11),"clicked",G_CALLBACK(button7_callback),NULL);

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
