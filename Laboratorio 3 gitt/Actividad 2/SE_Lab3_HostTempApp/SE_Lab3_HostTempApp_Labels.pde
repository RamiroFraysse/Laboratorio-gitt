/**
 * Clase que define etiquetas textuales para la GUI de la aplicación.
 * Define una posición (esquina superior izquierda) para la etiqueta, el
 * contenido de la misma y el color para el renderizado del texto.
 *
 */

class Label
{
  int x, y;
  color textcolor;
  String caption;

  Label(int x, int y, color textcolor, String caption)
  {
    this.x = x;
    this.y = y;
    this.textcolor = textcolor;
    this.caption = caption;
  }
  
  void display() 
  {
    stroke(255);
    fill(textcolor);
    textAlign(LEFT,TOP);
    text(caption, x, y);
  }  
}
