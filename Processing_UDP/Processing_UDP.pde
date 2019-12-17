import hypermedia.net.*;

UDP udp;  // define the UDP object
String ip = "192.168.137.86";
int port = 57222;
final int ON = 1;
final int OFF = 0;
boolean man;


button buttons[];
trig grids[];
scale scales[];
PImage img;

void setup() {
  udp = new UDP(this);
  fullScreen(1);
  //size(1920, 1080);
  background(255, 255, 255);
  buttons = new button[13]; //Start/Stop | Silent/Sound | LED x 11
  buttons[0] = new button(width / 4.0, 2.0 * height / 3.0, width / 20.0, ON);
  buttons[1] = new button(2.0 * width / 4.0, 2.0 * height / 3.0, width / 20.0, OFF);
  buttons[2] = new button(3.0 * width / 4.0, height / 3.0, width / 20.0, OFF);
  for (int i = 3; i < 8; i++) {
    buttons[i] = new button(3.0 * width / 4.0 - (width / 10.0), height / 6.0 + (i * width / 20.0), width / 30.0, OFF);
  }
  for (int i = 8; i < 13; i++) {
    buttons[i] = new button(3.0 * width / 4.0 + (width / 10.0), height / 6.0 + ((i - 5) * width / 20.0), width / 30.0, OFF);
  }
  
  grids = new trig[2];
  grids[0] = new trig(width / 4.0, height / 3.0);
  grids[1] = new trig(3.0 * width / 4.0, 0.5 * height / 3.0);
  
  scales = new scale[12];
  scales[0] = new scale(width / 2.0, height / 3.0, height / 4.0, width / 20.0, 9999);
  scales[1] = new scale(3.0 * width / 4.0 - width / 20.0, height / 3.0 - height / 16.0 + width / 40.0, height / 8.0, width / 40.0, 255);
  for (int i = 2; i < 7; i++) {
    scales[i] = new scale(3.0 * width / 4.0 - (width / 10.0) - width / 30.0, height / 6.0 + ((i + 1) * width / 20.0) - (width / 45.0) + (width / 60.0), width / 22.5, width / 40.0, 255);
  }
  for (int i = 7; i < 12; i++) {
    scales[i] = new scale(3.0 * width / 4.0 + (width / 10.0) + width / 30.0, height / 6.0 + ((i - 4) * width / 20.0) - (width / 45.0) + (width / 60.0), width / 22.5, width / 40.0, 255);
  }
  img = loadImage("back.jpg");
  delay(1000);
  System.out.println("Sending: Manual");
  udp.send("Manual", ip, port);
  man = true;
  delay(200);
}

void draw() {
  background(img);
  if (man) {
    for (int i = 0; i < 13; i++) {
      buttons[i].check_me(mouseX, mouseY);
      buttons[i].draw_me();
    }
    for (int i = 0; i < 2; i++) {
      grids[i].check_me(mouseX, mouseY);
      grids[i].draw_me();
    }
    
    for (int i = 0; i < 12; i++) {
      scales[i].check_me(mouseX, mouseY);
      scales[i].draw_me();
    }
  } else {
    fill(255);
    textSize(50);
    text("CLICK ME!", width / 4.0 - 125, 1.8 * height / 3.0);
    buttons[0].check_me(mouseX, mouseY);
    buttons[0].draw_me();
  }
  delay(50);
}

void mousePressed() {
  int clicked = 0;
  for (; clicked < 13; clicked++) {
    if (buttons[clicked].check_me(mouseX, mouseY)) {
      break;
    }
  }
  
  if (clicked != 13) {
    int state = buttons[clicked].click();
    switch(clicked) {
      case 0:
        if (state == ON) {
          udp.send("Manual", ip, port);
          System.out.println("Sending: Manual");
          man = true;
        } else {
          udp.send("Auto", ip, port);
          System.out.println("Sending: Auto");  
          //buttons[0].on();
          for (int i = 1; i < 13; i++) {
            buttons[i].off();
          }
          man = false;
        }
        break;
      case 1:
        udp.send("Speaker", ip, port);
        System.out.println("Sending: Speaker");
        scales[0].reset(3000);
        break;
      case 2:
        udp.send("LED_K", ip, port);
        System.out.println("Sending: Main LED");
        scales[1].reset(0);
        break;
      default:
        String tmp = "LED_" + (char) (clicked - 3 + 'A');
        udp.send(tmp, ip, port);
        System.out.println("Sending: " + tmp);
        scales[clicked - 1].reset(0);
        break;
    }
    delay(200);
  } else {
    if (grids[0].check_me(mouseX, mouseY)) {
      String tmp = "c_" + nf(grids[0].get_r(mouseX, mouseY), 3) + "_" + nf(grids[0].get_g(mouseX, mouseY), 3) + "_" + nf(grids[0].get_r(mouseX, mouseY), 3);
      udp.send(tmp, ip, port);
    } else if (grids[1].check_me(mouseX, mouseY)) {
        String tmp = "m_" + nf(grids[1].get_r(mouseX, mouseY), 3) + "_" + nf(grids[1].get_g(mouseX, mouseY), 3) + "_" + nf(grids[1].get_r(mouseX, mouseY), 3);
        udp.send(tmp, ip, port);    
    } else {
      for (int i = 0; i < 12; i++) {
        if (scales[i].check_me(mouseX, mouseY)) {
          if (i == 0) {
            String tmp = "s_" + nf(scales[i].click(mouseY), 4);
            udp.send(tmp, ip, port);
          } else {
            char c = 'K';
            if (i != 1) {
              c = (char) ((int) 'A' + i - 2);
            }
            String tmp = "Dim_" + c + "_" + nf(scales[i].click(mouseY), 3);
            udp.send(tmp, ip, port);
          }
          break;
        }
      }
    }
  }
}

class scale {
  float top;
  float len;
  float wid;
  float cx;
  float cy;
  float bottom_left_x;
  float top_left_y;
  float scalar;
  float freq;
  boolean hover;
  
  scale(float x, float y, float l, float w, float s) {
    cx = x;
    cy = y;
    len = l;
    wid = w;
    scalar = s;
    top = 0;
    bottom_left_x = cx - wid / 2.0;
    top_left_y = cy - len / 2.0;
    hover = false;
  }
  
  void draw_me() {
    float bottom_left_y = top_left_y + len;
    stroke(0, 0, 0);
    if (hover) {
      strokeWeight(4);
    } else {
      strokeWeight(1);
    }
    fill(255, 255, 255);
    rect(bottom_left_x, top_left_y, wid, len, wid / 10.0);
    fill(0, 0, 0);
    quad(bottom_left_x, bottom_left_y, bottom_left_x + wid, bottom_left_y, bottom_left_x + wid, bottom_left_y + top, bottom_left_x, bottom_left_y + top);
    strokeWeight(1);
  }
  
  boolean check_me(int x, int y) {
    if (x >= cx - wid / 2.0 && x <= cx + wid / 2.0 && y >= cy - len / 2.0 && y <= cy + len / 2.0) {
      hover = true;
    } else {
      hover = false;
    }
    return hover;
  }
  
  int click(int y) {
    top = y - (cy + len / 2.0);
    return (int) -((top / len) * scalar);
  }
  
  void reset(int d) {
    top = (float) - d / scalar * len;
  }
}

class trig {
  float ax;
  float bx;
  float cx;
  float ay;
  float by;
  float cy;
  boolean hover;
  
  trig(float mid_x, float mid_y) {
    float mid_length = (255.0 / 2.0) / cos(PI / 6.0);
    float perp = mid_length * sin(PI / 6.0);
    ax = mid_x;
    ay = mid_y - mid_length;
    bx = mid_x - (255.0 / 2.0);
    by = mid_y + perp;
    cx = mid_x + (255.0 / 2.0);
    cy = mid_y + perp;
    hover = false;
  }
  
  void draw_me() {
    fill(0, 0, 0);
    if (hover) {
      strokeWeight(4);
      stroke(0, 0, 0);
    } else {
      noStroke();
    }
    triangle(ax, ay, bx, by, cx, cy);
    for (int x = (int) bx; x < cx; x++) {
      for (int y = (int) ay; y < by; y++) {
        if (get(x, y) == color(0, 0, 0)) {
          set(x, y, get_colour(x, y));
        }
      }
    }
    strokeWeight(1);
  }
  
  color get_colour(int x, int y) {
    int r = (int) 255 - (int) sqrt(sq(x - ax) + sq(y - ay));
    int g = (int) 255 - (int) sqrt(sq(x - bx) + sq(y - by));
    int b = (int) 255 - (int) sqrt(sq(x - cx) + sq(y - cy));
    return color(r, g, b);
  }
  
  int get_r(int x, int y) {
    return 255 - (int) sqrt(sq(x - ax) + sq(y - ay));
  }
  
  int get_g(int x, int y) {
    return 255 - (int) sqrt(sq(x - bx) + sq(y - by));
  }
  
  int get_b(int x, int y) {
    return 255 - (int) sqrt(sq(x - cx) + sq(y - cy));
  }
  
  boolean check_me(int x, int y) {
    int da = (int) sqrt(sq(x - ax) + sq(y - ay));
    int db = (int) sqrt(sq(x - bx) + sq(y - by));
    int dc = (int) sqrt(sq(x - cx) + sq(y - cy));
    if (da <= 255 && db <= 255 && dc <= 255) {
      hover = true;
    } else {
      hover = false;
    }
    return hover;
  }
}

class button {
  float cx;
  float cy;
  float size;
  color c;
  int state;
  boolean hover;
  
  button(float x, float y, float s, int st) {
    cx = x;
    cy = y;
    size = s;
    if (st == ON) {
      c = color(50, 255, 50);
    } else {
      c = color(255, 50, 50);
    }
    state = st;
    hover = false;
  }
  
  void draw_me() {
    if (hover) {
      stroke(0, 0, 0);
    } else {
      noStroke();
    }
    fill(c);
    circle(cx, cy, size);
  }
  
  boolean check_me(int x, int y) {
    if (sqrt(sq(x - cx) + sq(y - cy)) < size / 2.0) {
      hover = true;
    } else {
      hover = false;
    }
    return hover;
  }
  
  int click() {
    state++;
    state = state % 2;
    if (state == OFF) {
      c = color(255, 50, 50);
    } else {
      c = color(50, 255, 50);
    }
    return state;
  }
  
  void off() {
    state = OFF;
    c = color(255, 50, 50);
  }
  
  void on() {
    state = ON;
    c = color(50, 255, 50);
  }
}
