#include<gtk/gtk.h>
#include<stdlib.h>
#include<limits.h>

struct callback_button_data{
  GtkWidget* path;
  GtkWidget* min;
  GtkWidget* max;
};

void exita(GtkWidget*widget,gpointer data)
{
  struct callback_button_data*dat=(struct callback_button_data*)data;
  
  GtkWidget*entry=dat->path;
  char*path=gtk_entry_get_text((GtkEntry*)entry);
  
  GtkWidget*minw=dat->min;
  int min=gtk_range_get_value((GtkRange*)minw);
  
  GtkWidget*maxw=dat->max;
  int max=gtk_range_get_value((GtkRange*)maxw);
  
  FILE* config=fopen("./config","w");
  fprintf(config,"%s %i %i 1",path,min,max);
  fclose(config);
}

void callback_button(GtkWidget*widget,gpointer data)
{
  struct callback_button_data*dat=(struct callback_button_data*)data;
  
  GtkWidget*entry=dat->path;
  char*path=gtk_entry_get_text((GtkEntry*)entry);
  
  GtkWidget*minw=dat->min;
  int min=gtk_range_get_value((GtkRange*)minw);
  
  GtkWidget*maxw=dat->max;
  int max=gtk_range_get_value((GtkRange*)maxw);
  
  FILE* config=fopen("./config","w");
  fprintf(config,"%s %i %i 0",path,min,max);
  fclose(config);
}

void main_window()
{  
  GtkWidget *window;
  window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(gtk_main_quit), NULL);
  GtkWidget*table;
  table=gtk_table_new(5,1,TRUE);
  gtk_container_add(GTK_CONTAINER(window),table);
  
  struct callback_button_data*data=malloc(sizeof(struct callback_button_data));
  
  GtkWidget*entry;
  entry=gtk_entry_new();
  gtk_table_attach_defaults((GtkTable*)table,entry,0,1,0,1);
  data->path=entry;
  
  GtkWidget*button=gtk_button_new();
  gtk_button_set_label((GtkButton*)button,"valider");
  g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(callback_button),data);
  gtk_table_attach_defaults((GtkTable*)table,button,0,1,3,4);
  
  GtkWidget*min;
  min=gtk_hscale_new_with_range (0,500,1);
  gtk_table_attach_defaults((GtkTable*)table,min,0,1,1,2);
  data->min=min;
  
   GtkWidget*max;
  max=gtk_hscale_new_with_range (0,500,1);
  gtk_table_attach_defaults((GtkTable*)table,max,0,1,2,3);
  data->max=max;
  
  GtkWidget*exit_button=gtk_button_new();
  gtk_button_set_label((GtkButton*)exit_button,"quitter");
  g_signal_connect(G_OBJECT(exit_button),"clicked",G_CALLBACK(exita),data);
  gtk_table_attach_defaults((GtkTable*)table,exit_button,0,1,4,5);
  
  gtk_widget_show_all(window);
}

int main()
{
 gtk_init(NULL,NULL);
 
 main_window();
 
 gtk_main();
}