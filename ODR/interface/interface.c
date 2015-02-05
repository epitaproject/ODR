#include <gtk/gtk.h>
#include<SDL/SDL.h>
#include<SDL/SDL_image.h>

#include"internet.h"

SDL_Surface *screen;

struct DLdata
{
  GtkWidget* text;
  GtkWidget* interval;
};

void dspl(GtkWidget *sender,gpointer data)
{
  displaysdl("dump.jpeg",screen);
}

void DL(GtkWidget *sender,gpointer data)
{
  struct DLdata *dt=data;
  char* text=gtk_entry_get_text((GtkEntry*)(dt->text));
  
  download(text,"dump.jpeg");
  displaysdl("dump.jpeg",screen);
}

void openmaingui()
{ 
  GtkWidget *window;
  window= gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit),NULL);
  
  GtkWidget *grid;
  grid=gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(window),grid);
  
  GtkWidget* text;
  text=gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid),text,0,0,1,1);
  
  GtkWidget* sleeper;
  sleeper=gtk_spin_button_new_with_range(0,100,1);
  gtk_grid_attach(GTK_GRID(grid),sleeper,1,0,1,1);
  
  GtkWidget* button2;
  button2=gtk_button_new();
  gtk_grid_attach(GTK_GRID (grid),button2,0,1,1,1);
  gtk_button_set_label((GtkButton*)button2,"rafrachir");
  g_signal_connect((GtkToggleButton*)button2,"clicked",G_CALLBACK(dspl),NULL);
  
 struct DLdata *data=malloc(sizeof(struct DLdata));
  data->text=text;
  data->interval=sleeper;
  GtkWidget* button;
  button=gtk_button_new();
  gtk_grid_attach(GTK_GRID (grid),button,1,1,1,1);
  gtk_button_set_label((GtkButton*)button,"nouvelle image");
  g_signal_connect((GtkToggleButton*)button,"clicked",G_CALLBACK(DL),data);
  
  
  gtk_widget_show_all (window);
}



int main (int argc,char *argv[])
{
  gtk_init (&argc,&argv);
  screen=initsdl();
  openmaingui(); 
  
  gtk_main();
  return 0;
}
