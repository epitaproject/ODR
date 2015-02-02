#include <gtk/gtk.h>
#include<SDL/SDL.h>
#include<SDL/SDL_image.h>

#include"internet.h"
void dsplsdl(GtkWidget *sender, gpointer data)
{
  GtkEntry *text=data;
  int i=0;
  i=gtk_toggle_button_get_active((GtkToggleButton*)sender)==TRUE;
  
  if(i)
  {
    displaysdl(gtk_entry_get_text(text));
  }
  
  else
  {
  SDL_Quit();
  }
  
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
  
  GtkWidget* button;
  button=gtk_toggle_button_new();
  gtk_grid_attach(GTK_GRID (grid),button,0,1,1,1);
  gtk_button_set_label((GtkButton*)button,"fenêtre SDL");
  g_signal_connect((GtkToggleButton*)button,"toggled",G_CALLBACK(dsplsdl),text);
  
  
  
  gtk_widget_show_all (window);
}



int main (int   argc,
      char *argv[])
{
 gtk_init (&argc,&argv);
 openmaingui(); 
 
 gtk_main();
  return 0;
}
