#include <iostream>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#define SLATEGREY   al_map_rgb(112,128,144)
#define RED         al_map_rgb(255, 0, 0)
#define BACKGROUND  al_map_rgb(0xD6, 0xDC,0xF0)
#define PINK        al_map_rgb(255, 0, 255)
#define BLACK       al_map_rgb(0, 0, 0)

const int SCREEN_W = 1280;       // screen width
const int SCREEN_H = 720;       // screen height
const float FPS = 60;

ALLEGRO_DISPLAY *display;
ALLEGRO_TIMER *timer;
ALLEGRO_FONT *arial;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_BITMAP *image = nullptr;

struct Image {
    ALLEGRO_BITMAP *bitmap;               // picture
    int x, y;                             // position on screen
    int bbRight, bbLeft, bbTop, bbBottom; // boundary box (hitbox)
};

//prototypes
int initializeAllegro(int width, int height, const char title[]);
bool loadBitmap(Image &image, const char *filename);
void setPosition(Image &image, int xPos, int yPos);
void calcBounds(Image &a);
void drawBoundingBox(Image &image);
bool isCollision(Image &a, Image &b);

int main(int argc, char *argv[]) {
    ALLEGRO_EVENT_QUEUE *event_queue = nullptr;

    // Initialize Allegro
    initializeAllegro(SCREEN_W, SCREEN_H, "flappy bird test");

	// Declare a BITMAP called image, setting it's initial value to nullptr
    Image bg, pipe, pipe1, bird;

    if (!loadBitmap(bg, "background.jpg")){
        return 1;}
    if (!loadBitmap(pipe, "pipe.jpg")){
        return 1;}
    if (!loadBitmap(pipe1, "pipe.jpg")){
        return 1;}
    if (!loadBitmap(bird, "bird.png")){
        return 1;}

	// Display pictures
    setPosition(bg, 0, 0);
    setPosition(pipe, 600, 350);
    setPosition(pipe1, 200, 0);
	setPosition(bird, 0, 300);

    al_draw_bitmap(bg.bitmap, bg.x, bg.y, 0); //Draw background
    al_draw_bitmap(pipe.bitmap, pipe.x, pipe.y, 0); //Draw pipe
    al_draw_bitmap(pipe1.bitmap, pipe1.x, pipe1.y, ALLEGRO_FLIP_VERTICAL); //Draw another pipe upside down
    al_draw_bitmap(bird.bitmap, bird.x, bird.y, 0); //Draw bird

    //set up timer
    timer = al_create_timer(1.0 / FPS);
   	if (!timer) {
   		al_show_native_message_box(display, "Error", "Error", "Failed to create timer!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }


    // set up event queue
	event_queue = al_create_event_queue();
	if (!event_queue) {
		al_show_native_message_box(display, "Error", "Error", "Failed to create event_queue!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
      	return -1;
	}

	// need to register events for event queue
	al_register_event_source(event_queue, al_get_display_event_source(display));
 	al_register_event_source(event_queue, al_get_keyboard_event_source());
 	al_register_event_source(event_queue, al_get_timer_event_source(timer));


	// int dx = 100;
	// int dy = 100;
    // al_draw_bitmap(bird.bitmap, dx, dy, 0);

    //write display to screen
	al_flip_display();

    //  start timer
    al_start_timer(timer);

	int show_bbox = false;
	bool doexit = false;
    while (!doexit) {

    // move bird around with arrow keys
    ALLEGRO_EVENT ev;
    al_wait_for_event(event_queue, &ev);

    if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        	doexit = true;
    }
    else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch(ev.keyboard.keycode) {
            case ALLEGRO_KEY_UP:
               // dy -= 8;
                bird.y -=10;
                break;
            case ALLEGRO_KEY_DOWN:
                //dy += 8;
                bird.y +=10;
                break;
            case ALLEGRO_KEY_LEFT:
                //dx -= 8;
                bird.x -= 10;
                break;
            case ALLEGRO_KEY_RIGHT:
                //dx += 8;
                bird.x += 10;
                break;
            case ALLEGRO_KEY_ESCAPE:
                doexit = true;
                break;
            case ALLEGRO_KEY_SPACE:
                show_bbox = !show_bbox;
                break;
            default: ;
         	}
    }
    // move pipe with previously set up timer
    else if (ev.type == ALLEGRO_EVENT_TIMER){
        if(pipe.x > 0) {
            pipe.x -= 2;
        }
        else{
            pipe.x = 1200;
        }

            al_draw_bitmap(bg.bitmap, 0, 0, 0);
            al_draw_bitmap(pipe.bitmap, pipe.x, pipe.y, 0); //600 350
             al_draw_bitmap(pipe1.bitmap, pipe1.x, pipe1.y, ALLEGRO_FLIP_VERTICAL);
		 	// al_draw_bitmap(bird.bitmap, dx, dy, 0);

            /* drawing obounding box if the player is hitting space.
           it turns off if a character hits a key not in use*/
            if (show_bbox) {
			// Draw images bounding box.
                drawBoundingBox(bird);
                drawBoundingBox(pipe);
                drawBoundingBox(pipe1);
        }


        // collision detection statement
		if (isCollision(bird, pipe)) {
			 al_draw_text(arial, PINK, 0, 0, 0, "Collision!!!!!!!!!!");
			 setPosition(bird, 0, 300);
			 bird.x = 0;
			 bird.y = 300;
			 al_draw_bitmap(bird.bitmap, 0, 300, 0);
			 al_flip_display();
		} else if(isCollision(bird, pipe1)) {
			 al_draw_text(arial, PINK, 0, 0, 0, "Collision!!!!!!!!!!");
			 setPosition(bird, 0, 300);
			 bird.x = 0;
			 bird.y = 300;
			 al_draw_bitmap(bird.bitmap, 0, 300, 0);
			 al_flip_display();
		}
		else {
		// convert coodinates to string to print to screen
            al_draw_bitmap(bird.bitmap, bird.x, bird.y, 0);
            char birdx[40] = "";
            char birdy[20] = "";
            char birdseperate [2] = "+";
		    sprintf(birdx, "%d", bird.x);
            sprintf(birdy, "%d", bird.y);
		    strcat(birdx, birdseperate);
		    strcat(birdx, birdy);
		    al_draw_text(arial, PINK, 0, 0, 0, birdx);

		    char pipex[10];
            char pipey[10];
            char pipeseperate [3] = "+";
		    sprintf(pipex, "%d", pipe.x);
		    sprintf(pipey, "%d", pipe.y);
		    strcat(pipex, pipeseperate);
		    strcat(pipex, pipey);
		    al_draw_text(arial, PINK, 0, 30, 0, pipex);

            // win condition and text on top
		    if(bird.x>1000){
             al_draw_text(arial, PINK, 1000, 0, 0, "You win 1 recycle bin!!!!!");
		    }

		}
		 	al_flip_display();
		}
	}

    al_flip_display();

    // Wait 10 seconds
	al_rest(10);

	// Free memory and return with successful return code..
    al_destroy_bitmap(bg.bitmap);
    al_destroy_bitmap(pipe.bitmap);
    al_destroy_bitmap(pipe1.bitmap);
    al_destroy_bitmap(bird.bitmap);
    al_destroy_display(display);
	return 0;
}

int initializeAllegro(int width, int height, const char title[]) {

    ALLEGRO_COLOR background = al_map_rgb(255, 255, 255);       // make background white.

    // Initialize Allegro
	al_init();

	// initialize display
	display = al_create_display(width, height);
	if (!display) {
    	al_show_native_message_box(display, "Error", "Error", "Failed to initialize display!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
       	return -1;
	}
	al_set_window_title(display, title);

   	// Initialize keyboard routines
	if (!al_install_keyboard()) {
	    al_show_native_message_box(display, "Error", "Error", "failed to initialize the keyboard!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
     	return -1;
   	}

	// need to add image processor
 	if (!al_init_image_addon()) {
    	al_show_native_message_box(display, "Error", "Error", "Failed to initialize image addon!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
    	return -1;
	}

	// setup timer
	timer = al_create_timer(1.0 / FPS);
   	if (!timer) {
   		al_show_native_message_box(display, "Error", "Error", "Failed to create timer!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    // Add fonts
   al_init_font_addon(); // initialize the font addon
   al_init_ttf_addon();// initialize the ttf (True Type Font) addon

   arial = al_load_ttf_font("C:/Windows/Fonts/arial.ttf", 16, 0);
   if (!arial){
      al_show_native_message_box(display, "Error", "Error", "Could not load arial.ttf",
                                    nullptr, ALLEGRO_MESSAGEBOX_ERROR);
      return -1;
   }

    // Initialize primative add on
 	if (!al_init_primitives_addon()) {
    	al_show_native_message_box(display, "Error", "Error", "Failed to initialize primatives addon!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
    	return -1;
	}

	// set up event queue
	event_queue = al_create_event_queue();
	if (!event_queue) {
		al_show_native_message_box(display, "Error", "Error", "Failed to create event_queue!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
      	return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
 	al_register_event_source(event_queue, al_get_keyboard_event_source());

//l_register_event_source(event_queue, al_get_timer_event_source(timer));	*/

// 3. register timer events
	al_flip_display();
	al_start_timer(timer);
//	std::cout << "Init return zero " << std::endl;
	return 0;
}

//load bitmap to instance
bool loadBitmap(Image &image, const char *filename) {

    image.bitmap = al_load_bitmap(filename);
    if (image.bitmap == nullptr) {
		al_show_native_message_box(display, "Error", filename, "Could not load ",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
		return false;
	}
	//al_convert_mask_to_alpha(image.bitmap, PINK);
	return true;
}

//set position of bitmap
void setPosition(Image &image, int xPos, int yPos) {
    image.x = xPos; //Set the x position of image 1 to 0.
	image.y = yPos; //Set the y position of image 1 to 0.
}

// hitbox
void calcBounds(Image &a) {
    a.bbLeft = a.x;
	a.bbTop  = a.y;
	a.bbRight = a.bbLeft + al_get_bitmap_width(a.bitmap);
	a.bbBottom = a.bbTop + al_get_bitmap_height(a.bitmap);
}

void drawBoundingBox(Image &image) {
    calcBounds(image);
	al_draw_line(image.bbLeft, image.bbTop, image.bbRight, image.bbTop, RED, 1);
    al_draw_line(image.bbLeft, image.bbBottom, image.bbRight, image.bbBottom, RED, 1);
	al_draw_line(image.bbLeft, image.bbTop, image.bbLeft, image.bbBottom, RED, 1);
	al_draw_line(image.bbRight, image.bbTop, image.bbRight, image.bbBottom, RED,1);
}

// collision detection
bool isCollision(Image &a, Image &b) {
    calcBounds(a);
    calcBounds(b);
    if (a.bbBottom < b.bbTop) {
        return false;
    }
    else if (a.bbTop > b.bbBottom) {
        return false;
    }
    else if (a.bbRight < b.bbLeft) {
        return false;
    }
    else if (a.bbLeft > b.bbRight) {
        return false;
    }
    return true;
}

// end
