/**
 * Clases que definen botones.
 * Se basan en el ejemplo Topics > GUI > Buttons de Processing 1.5.
 *
 * http://processing.org/learning/topics/buttons.html
 *
 */

class Button
{
  int x, y;
  int size, size2;
  color basecolor, highlightcolor;
  color currentcolor,textcolor;
  boolean over = false;
  boolean pressed = false;   
  boolean locked = false;
  String caption = "Button";

  void update() 
  {
    if(over()) {
      currentcolor = highlightcolor;
    } 
    else {
      currentcolor = basecolor;
    }
  }

  boolean pressed() 
  {
    if(over) {
      locked = true;
      return true;
    } 
    else {
      locked = false;
      return false;
    }    
  }

  boolean over() 
  { 
    return true; 
  }

  boolean overRect(int x, int y, int width, int height) 
  {
    if (mouseX >= x && mouseX <= x+width && 
      mouseY >= y && mouseY <= y+height) {
      return true;
    } 
    else {
      return false;
    }
  }

  boolean overCircle(int x, int y, int diameter) 
  {
    float disX = x - mouseX;
    float disY = y - mouseY;
    if(sqrt(sq(disX) + sq(disY)) < diameter/2 ) {
      return true;
    } 
    else {
      return false;
    }
  }

}

class CircleButton extends Button
{ 
  CircleButton(int ix, int iy, int isize, color icolor, color ihighlight, color textcolor, String caption) 
  {
    x = ix;
    y = iy;
    size = isize;
    basecolor = icolor;
    highlightcolor = ihighlight;
    currentcolor = basecolor;
    this.textcolor = textcolor;
    this.caption = caption;
  }

  boolean over() 
  {
    if( overCircle(x, y, size) ) {
      over = true;
      return true;
    } 
    else {
      over = false;
      return false;
    }
  }

  void display() 
  {
    stroke(255);
    fill(currentcolor);
    ellipse(x, y, size, size);
    fill(textcolor);
    textAlign(CENTER,CENTER);
    text(caption, x+size/2, y+size/2);
  }
}

class RectButton extends Button
{
  RectButton(int ix, int iy, int isize, int isize2, color icolor, color ihighlight, color textcolor, String caption) 
  {
    x = ix;
    y = iy;
    size = isize;
    size2 = isize2;
    basecolor = icolor;
    highlightcolor = ihighlight;
    currentcolor = basecolor;
    this.textcolor = textcolor;
    this.caption = caption;
  }

  boolean over() 
  {
    if( overRect(x, y, size, size2) ) {
      over = true;
      return true;
    } 
    else {
      over = false;
      return false;
    }
  }

  void display() 
  {
    stroke(255);
    fill(currentcolor);
    rect(x, y, size, size2);
    fill(textcolor);
    textAlign(CENTER,CENTER);
    text(caption, x+size/2, y+size2/2);
  }
}
