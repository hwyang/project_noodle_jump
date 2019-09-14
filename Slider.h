#ifndef SLIDER_H_INCLUDED
#define SLIDER_H_INCLUDED
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
class Object {
public:
    Object() {}
    ~Object() {}

    // pure function for drawing object
    virtual void Draw() = 0;

//    int getX() { return circle->x; }
//    int getY() { return circle->y; }
//    int getRadius() { return circle->r; }
//
//    Circle* getCircle() const { return circle; }
//
protected:
    //Circle *circle;

};


class Slider : public Object
{
public:
    Slider(int, int);
    ~Slider();

    void Draw();
    void set_color(ALLEGRO_COLOR);
    void set_label_content(const char*);
    void toggleDrag() { dragged = !dragged; }

    float Drag(int, int);
    float getDegree() { return degree; }

    int getLength() { return lengthOftrack; }

    bool isClicked(int, int);
    bool isDragged() { return dragged; }

private:
    int target_x, target_y;
    int track_x, track_y;
    int lengthOftrack = 200;
    float degree = 1.0;
    bool dragged = false;
    char label[20];
    ALLEGRO_COLOR target_color;
    ALLEGRO_FONT *font;
};

#endif // SLIDER_H_INCLUDED
