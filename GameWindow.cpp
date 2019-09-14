#include "GameWindow.h"
#include <iostream>
#define ORANGE_LIGHT al_map_rgb(255, 196, 87)

const float FPS = 60;

struct point{int x, y;};
const int h=200;

point randpos[20];
platform plat[20];
Doodle doodle;
Ninja ninja;

Score score;
int plat3_dx = -3;
static float jump_vol = 1.0;
static float fail_vol = 1.0;

ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h)
{
    ALLEGRO_BITMAP *resized_bmp, *loaded_bmp, *prev_target;

    resized_bmp = al_create_bitmap(w, h);
    if (!resized_bmp) return NULL;

    loaded_bmp = al_load_bitmap(filename);
    if (!loaded_bmp)
    {
        al_destroy_bitmap(resized_bmp);
        return NULL;
    }

    prev_target = al_get_target_bitmap();
    al_set_target_bitmap(resized_bmp);

    al_draw_scaled_bitmap(loaded_bmp,
                          0, 0,
                          al_get_bitmap_width(loaded_bmp),
                          al_get_bitmap_height(loaded_bmp),
                          0, 0,
                          w, h, 0 );

    al_set_target_bitmap(prev_target);
    al_destroy_bitmap(loaded_bmp);

    return resized_bmp;
}

void
GameWindow::game_init()
{
    icon = al_load_bitmap("./icon.png");
    background = load_bitmap_at_size("./Background.png", 400, 533);
    start_pic = load_bitmap_at_size("start_pic.jpg", 400, 533);
    plat_norm = al_load_bitmap("plat_norm.png");
    plat_sponge = load_bitmap_at_size("plat_sponge.png", 68, 16);
    plat_move = al_load_bitmap("plat_move.png");
    doodle_pic_left = load_bitmap_at_size("doodle_left.png", 80, 80);
    doodle_pic_right = load_bitmap_at_size("doodle_right.png", 80, 80);
    ninja_left = load_bitmap_at_size("ninja_left.png", 80, 80);
    ninja_right = load_bitmap_at_size("ninja_right.png", 80, 80);
    end_pic = load_bitmap_at_size("end_pic.png", 400, 533);
    option_pic = al_load_bitmap("./Settings.png");

    al_set_display_icon(display, icon);

    al_reserve_samples(4);
    sample = al_load_sample("fall.wav");
    failSound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(failSound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(failSound, al_get_default_mixer());

    sample = al_load_sample("jump.wav");
    jumpSound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(jumpSound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(jumpSound, al_get_default_mixer());

    al_reserve_samples(3);
    sample = al_load_sample("sponge.wav");
    spongeSound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(spongeSound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(spongeSound, al_get_default_mixer());


    sample = al_load_sample("bgm.wav");
    backgroundSound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(backgroundSound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(backgroundSound, al_get_default_mixer());


    doodle.x=100.0,doodle.y=100.0;
    doodle.dy=0.0;
    doodle.init_dy_norm = -10;
    doodle.init_dy_sponge = -20;

}


void
GameWindow::game_play()
{
    int msg;

    srand(time(NULL));

    msg = -1;
    game_start_scene();


    srand(time(0));
    for (int i=0;i<10;i++){
        randpos[i].x=rand()%400;
        randpos[i].y=rand()%533;
        plat[i].setPosition(randpos[i].x, randpos[i].y);
    }


    game_begin();

    while(msg != GAME_EXIT){
        if (msg == GAME_FAIL) {
            game_end_scene();
            game_reset();
            game_init();
            game_play();
        }
        msg = game_run();
    }

    show_err_msg(msg);
}

void GameWindow::game_start_scene(){
    while(true){
        al_draw_bitmap(start_pic, 0, 0, 0);
        al_draw_bitmap(option_pic, 350, 480, 0);

        if ( fmod (al_get_time(), 2) < 1 ) {
            al_draw_text(Medium_font, al_map_rgb(0, 0, 0), 200, 470, ALLEGRO_ALIGN_CENTER, "Press S to start !");
        }

        al_flip_display();

        al_get_next_event(event_queue, &event);
        if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if(event.mouse.x>=350&&event.mouse.x<=400&&event.mouse.y>=480&&event.mouse.y<=530){
                adjust_volume();
            }
        }
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_S) {
                return;
            }
        }
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            exit(6);
        }
    }

}

void GameWindow::show_err_msg(int msg)
{
    if(msg == GAME_TERMINATE)
        fprintf(stderr, "Game Terminated...");
    else
        fprintf(stderr, "unexpected msg: %d", msg);

    game_destroy();
    exit(9);
}

GameWindow::GameWindow()
{
    if (!al_init())
        show_err_msg(-1);

    printf("Game Initializing...\n");

    display = al_create_display(window_width, window_height);
    event_queue = al_create_event_queue();

    timer = al_create_timer(1.0 / FPS);

    if(timer == NULL)
        show_err_msg(-1);

    if (display == NULL || event_queue == NULL)
        show_err_msg(-1);

    al_init_primitives_addon();
    al_init_font_addon(); // initialize the font addon
    al_init_ttf_addon(); // initialize the ttf (True Type Font) addon
    al_init_image_addon(); // initialize the image addon
    al_init_acodec_addon(); // initialize acodec addon

    al_install_keyboard(); // install keyboard event
    al_install_mouse();    // install mouse event
    al_install_audio();    // install audio event

    /*font = al_load_ttf_font("Caviar_Dreams_Bold.ttf",12,0); // load small font
    Medium_font = al_load_ttf_font("Caviar_Dreams_Bold.ttf",24,0); //load medium font
    Large_font = al_load_ttf_font("Caviar_Dreams_Bold.ttf",36,0); //load large font*/
    title = al_load_font("Amatic-Bold.ttf", 80, NULL);
    subtitle = al_load_font("Amatic-Bold.ttf", 30, NULL);
    Medium_font = al_load_ttf_font("Caviar_Dreams_Bold.ttf",24,0); //load medium font

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    // Initializing resources that tower game needs.
    game_init();
}

void
GameWindow::game_begin()
{
    draw_running_map();

    /*al_play_sample_instance(startSound);
    while(al_get_sample_instance_playing(startSound));*/
    al_play_sample_instance(backgroundSound);

    al_start_timer(timer);
}

int
GameWindow::game_run()
{
    int error = GAME_CONTINUE;

    if (!al_is_event_queue_empty(event_queue)) {

        error = process_event();
    }
    return error;
}

void GameWindow::game_end_scene(){
    while(true){
        al_draw_bitmap(end_pic, 0, 0, 0);
        al_draw_textf(title, al_map_rgb(178,34,34),250, 400,  ALLEGRO_ALIGN_LEFT, "%d", score.score);
        al_flip_display();

        al_wait_for_event(event_queue, &event);
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_R) {
                return;
            }
        }
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            exit(6);
        }

    }

}
int
GameWindow::game_update()
{
    doodle.dy += 0.2;
    doodle.y += doodle.dy;
    //left right bound checking
    if(doodle.x < 0) doodle.x = 400;
    if(doodle.x > 400) doodle.x = 0;
    //dead checking
    if (doodle.y>500){
        al_play_sample_instance(failSound);
        while(al_get_sample_instance_playing(failSound));
        //game_end_scene();
        return GAME_FAIL;
    }
    //overlap checking
    for (int i=0;i<10;i++){
        if ((doodle.x+50>plat[i].x) && (doodle.x+20<plat[i].x+68)&& (doodle.y+70>plat[i].y) && (doodle.y+70<plat[i].y+14) && (doodle.dy>0)){
            if(plat[i].plat_type == 8 && (doodle.x+20<plat[i].x+34)){
                doodle.dy = doodle.init_dy_sponge;
                al_play_sample_instance(spongeSound);
            }
            else {
                doodle.dy = doodle.init_dy_norm;
                al_play_sample_instance(jumpSound);
            }
        }
    }
    //for scrolling
    if (doodle.y<h){
        for (int i=0;i<10;i++){
            doodle.y = h;
            plat[i].y = plat[i].y - doodle.dy;
            if (plat[i].y>533){
                plat[i].plat_type = 0;
                plat[i].setPosition(rand()%400, 0);
                if(score.score > 100 && score.score <= 400) plat[i].plat_type = rand()%9;
                else if(score.score > 400 && score.score <= 1000) plat[i].plat_type = rand()%10;
                else if(score.score > 1000) plat[i].plat_type = rand()%16;
                score.score += rand()%50;

            }
        }
    }
    if(score.score > 1000){
        doodle.init_dy_norm = -20;
        doodle.init_dy_sponge = -40;
    }

    return GAME_CONTINUE;
}

void
GameWindow::game_reset()
{
    mute = false;
    redraw = false;
    //stop sample instance
    al_stop_sample_instance(backgroundSound);
    al_stop_sample_instance(failSound);
    al_stop_sample_instance(jumpSound);
    score.score = 0;
    // stop timer
    al_stop_timer(timer);
}

void
GameWindow::game_destroy()
{
    game_reset();

    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_font(font);
    al_destroy_font(Medium_font);
    al_destroy_font(Large_font);
    al_destroy_font(title);
    al_destroy_font(subtitle);

    al_destroy_timer(timer);

    al_destroy_bitmap(icon);
    al_destroy_bitmap(background);
    al_destroy_bitmap(start_pic);
    al_destroy_bitmap(plat_norm);
    al_destroy_bitmap(plat_sponge);
    al_destroy_bitmap(plat_move);
    al_destroy_bitmap(doodle_pic_left);
    al_destroy_bitmap(doodle_pic_right);
    al_destroy_bitmap(end_pic);
    al_destroy_bitmap(option_pic);
    al_destroy_bitmap(ninja_left);
    al_destroy_bitmap(ninja_right);
    al_destroy_sample(sample);
    al_destroy_sample_instance(jumpSound);
    al_destroy_sample_instance(spongeSound);
    al_destroy_sample_instance(failSound);
    al_destroy_sample_instance(backgroundSound);
}

int
GameWindow::process_event()
{
    int instruction = GAME_CONTINUE;

    al_wait_for_event(event_queue, &event);
    redraw = false;

    if(event.type == ALLEGRO_EVENT_TIMER) {
        if(event.timer.source == timer) {
            redraw = true;
        }
    }
    else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        return GAME_EXIT;
    }
    //doodle controlling
    else if(event.type == ALLEGRO_EVENT_KEY_CHAR){
        if(event.keyboard.keycode == ALLEGRO_KEY_A){
            doodle.x-=30;
            doodle.left_right = false;
        }
        if(event.keyboard.keycode == ALLEGRO_KEY_D){
            doodle.x+=30;
            doodle.left_right = true;
        }
    }
    else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
        if(event.mouse.button == 1 && event.mouse.x >= 360 && event.mouse.x <= 390 && event.mouse.y >= 0 && event.mouse.y <= 30){
            if(pause){
                    al_start_timer(timer);
                }
                else{
                    al_stop_timer(timer);
                }
                pause = !pause;
        }
        if(event.mouse.x>=350&&event.mouse.x<=400&&event.mouse.y>=480&&event.mouse.y<=530){
            adjust_volume();
        }
    }
    if(redraw) {
        // update each object in game
        instruction = game_update();

        // Re-draw map
        draw_running_map();
        redraw = false;
    }

    return instruction;
}

void GameWindow::adjust_volume()
{
    Slider S = Slider(100,175);
    S.set_label_content("Background Sound");
    Slider S2 = Slider(100,250);
    S2.set_label_content("Effect Sound");

    while (true) {
        al_clear_to_color(ORANGE_LIGHT);
        S.Draw();
        S2.Draw();
        al_draw_text(Medium_font, al_map_rgb(0, 0, 0), 200, 470, ALLEGRO_ALIGN_CENTER, "Press space to return !");
        al_flip_display();

        al_wait_for_event(event_queue,  &event);
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_SPACE ) {
                return;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE )
                return;
        }
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            exit(6);
        }
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (event.mouse.button == 1) {
                if (S.isClicked(mouse_x, mouse_y)) {
                    //dragged = !dragged;
                    S.toggleDrag();
                }
                if (S2.isClicked(mouse_x, mouse_y)){
                    S2.toggleDrag();
                }
            }
        }
        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = event.mouse.x;
            mouse_y = event.mouse.y;
            if (S.isDragged()) {
                S.Drag(mouse_x, mouse_y);
                al_set_sample_instance_gain(backgroundSound, S.getDegree());
            }
            if (S2.isDragged()) {
                S2.Drag(mouse_x, mouse_y);
                jump_vol = S2.getDegree();
                al_set_sample_instance_gain(jumpSound, jump_vol);
                fail_vol = S2.getDegree();
                al_set_sample_instance_gain(failSound, fail_vol);
            }
        }
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (S.isDragged())
                S.toggleDrag();
            if (S2.isDragged())
                S2.toggleDrag();
        }

    }

}

void GameWindow::draw_running_map()
{
    al_clear_to_color(al_map_rgb(100, 100, 100));
    al_draw_bitmap(background, 0, 0, 0);
    al_draw_bitmap(option_pic, 350, 480, 0);

    al_draw_textf(subtitle, al_map_rgb(0,0,0),15, 0,  ALLEGRO_ALIGN_LEFT, "%d", score.score);

    for (int i = 0; i < 10; i++){
        if(score.score > 400 && plat[i].plat_type >= 9){
            plat[i].x += plat3_dx;
            if(plat[i].x == 330) plat3_dx = -3;
            if(plat[i].x == 0) plat3_dx = 3;
        }
    }


    for (int i = 0; i < 10; i++){
        if(score.score > 100 && plat[i].plat_type == 8)al_draw_bitmap(plat_sponge, plat[i].x, plat[i].y, 0);
        else if(score.score > 400 && plat[i].plat_type == 9)al_draw_bitmap(plat_move, plat[i].x, plat[i].y, 0);
        else if(score.score > 1000 && plat[i].plat_type >= 9)al_draw_bitmap(plat_move, plat[i].x, plat[i].y, 0);
        else al_draw_bitmap(plat_norm, plat[i].x, plat[i].y, 0);
    }

	//doodle.setPosition(x,y);
    if(score.score <= 1000 && doodle.left_right == false)al_draw_bitmap(doodle_pic_left, doodle.x, doodle.y, 0);
    if(score.score <= 1000 && doodle.left_right == true)al_draw_bitmap(doodle_pic_right, doodle.x, doodle.y, 0);
    if(score.score > 1000 && doodle.left_right == false)al_draw_bitmap(ninja_left, doodle.x, doodle.y, 0);
    if(score.score > 1000 && doodle.left_right == true)al_draw_bitmap(ninja_right, doodle.x, doodle.y, 0);
    al_flip_display();
}
