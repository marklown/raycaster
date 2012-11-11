#include <SDL/SDL.h>
#include <math.h>

#define mapWidth 24
#define mapHeight 24

bool run = true;

int worldMap[mapWidth][mapHeight] =
{
  	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
 	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,2,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1},
	{1,0,0,0,2,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1},
	{1,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
	{1,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,2,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

void setpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	y = y * 640;
	Uint32 *target = (Uint32 *)surface->pixels + y + x;
	*target = pixel;
}

int main(void)
{
	double posX = 22, posY = 12;	// X and Y start position
	double dirX = -1, dirY = 0;		// Initial direction vector
	double plnX =  0, plnY = 0.66;	// The 2D raycaster version of the camera plane
	
	double time = 0, oldTime = 0;	// The time of the current and previous frames
	
	double oldDirX, oldPlaneX;
	
	bool keysHeld[4] = {false, false, false, false};
	
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{	
		printf("Error initializing SDL.");
		return 0;
	}
	
	// Create the SDL_Surface
	SDL_Surface *display;
	int w = 640, h = 480;
	display = SDL_SetVideoMode(w, h, 32, SDL_SWSURFACE);
	if (display == NULL) {
		printf("Error making screen.");
		return 0;
	}
	SDL_WM_SetCaption("Raycaster","Raycaster");
	
	// Main loop
	SDL_Event event;
	while(run)
	{
		// Check for messages
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				// Handle quit event
				case SDL_QUIT:
					break;
				// Handle key down event
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							run = false;
							break;
						case SDLK_LEFT:
							keysHeld[0] = true;
							break;
						case SDLK_RIGHT:
							keysHeld[1] = true;
							break;
						case SDLK_UP:
							keysHeld[2] = true;
							break;
						case SDLK_DOWN:
							keysHeld[3] = true;
							break;
					}
					break;
				// Handle key up event
				case SDL_KEYUP:
					switch(event.key.keysym.sym)
					{
						case SDLK_LEFT:
							keysHeld[0] = false;
							break;
						case SDLK_RIGHT:
							keysHeld[1] = false;
							break;
						case SDLK_UP:
							keysHeld[2] = false;
							break;
						case SDLK_DOWN:
							keysHeld[3] = false;
							break;
					}
					break;
			}
		}
		
		// Clear the screen
		SDL_FillRect(display, NULL, 0x000000);
		
		// Raycasting loop
		for (int x = 0;x < w;x++)
		{
			// Calculate ray position and direction
			double camX = 2 * x / double(w) - 1;	// X-coord in camera space
			double rayPosX = posX;
			double rayPosY = posY;
			double rayDirX = dirX + plnX * camX;
			double rayDirY = dirY + plnY * camX;
			
			// Which box of the map are we in
			int mapX = int(rayPosX);
			int mapY = int(rayPosY);
			
			// Length of ray from current position to next x or y side
			double sideDistX;
			double sideDistY;
			
			// Length of ray from one x or y side to the next x or y side
			double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
			double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
			double perpWallDist;
			
			// What direction to step in x or y (either +1 or -1)
			int stepX, stepY;
			
			int hit = 0;	// Was there a wall hit?
			int side;		// Was a NS or EW wall hit?
			
			// Calculate step and initial sideDist
			if (rayDirX < 0)
			{
				stepX = -1;
				sideDistX = (rayPosX - mapX) * deltaDistX;
			} 
			else
			{
				stepX = 1;
				sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
			}
			if (rayDirY < 0)
			{
				stepY = -1;
				sideDistY = (rayPosY - mapY) * deltaDistY;
			}
			else 
			{
				stepY = 1;
				sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
			}
			
			// Perform DDA (Digital Differental Analysis)
			while (hit == 0)
			{
				// Jump to next map square in either the x-direction or y-direction
				if (sideDistX < sideDistY)
				{
					sideDistX += deltaDistX;
					mapX += stepX;
					side = 0;
				}
				else
				{
					sideDistY += deltaDistY;
					mapY += stepY;
					side = 1;
				}
				
				// Check if ray has hit a wall
				if (worldMap[mapX][mapY] > 0)
				{
					hit = 1;
				}
			}
			
			// Calculate distance projected on camera direction
			if (side == 0)
			{
				perpWallDist = fabs((mapX - rayPosX + (1 - stepX) * 0.5) / rayDirX);
			}
			else 
			{
				perpWallDist = fabs((mapY - rayPosY + (1 - stepY) * 0.5) / rayDirY);
			}
			
			// Calculate the height of the line to draw on screen
			int lineHeight = abs(int(h / perpWallDist));
			
			// Calculate the lowest and heighest pixel to fill in current stripe
			int drawStart = -lineHeight * 0.5 + h * 0.5;
			if (drawStart < 0)
				drawStart = 0;
			int drawEnd = lineHeight * 0.5 + h * 0.5;
			if (drawEnd >= h)
				drawEnd = h - 1;
			
			// Set the color
			Uint32 color;
			switch(worldMap[mapX][mapY])
			{
				case 1:
					color = SDL_MapRGB(display->format, 255, 0, 0);
					break;
				case 2:
					color = SDL_MapRGB(display->format, 0, 255, 0);
					break;
				case 3:
					color = SDL_MapRGB(display->format, 0, 0, 255);
					break;
			}
			if (side == 1)
				color = color * .999;
				
			// Draw a single vertical line
			int y;
			for (int i = 0;i < drawEnd - drawStart;++i)
			{
				y = drawStart + i;
				setpixel(display, x, y, color);
			}
		}
		
		// Flip the buffer
		SDL_Flip(display);	
		
		// Timing for input and FPS counter
		oldTime = time;
		time = SDL_GetTicks();
	    double frameTime = (time - oldTime) * 0.001;

		// Speed modifiers
	    double moveSpeed = frameTime * 5.0; //the constant value is in squares/second
	    double rotSpeed = frameTime * 3.0;  //the constant value is in radians/second
	
		// Handle keyboard input
		if (keysHeld[0])
		{
		   	oldDirX = dirX;
		  	dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
		   	dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
		   	oldPlaneX = plnX;
		    plnX = plnX * cos(rotSpeed) - plnY * sin(rotSpeed);
		    plnY = oldPlaneX * sin(rotSpeed) + plnY * cos(rotSpeed);
		}
		if (keysHeld[1])
		{
			oldDirX = dirX;
	      	dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
	      	dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
			oldPlaneX = plnX;
	      	plnX = plnX * cos(-rotSpeed) - plnY * sin(-rotSpeed);
	      	plnY = oldPlaneX * sin(-rotSpeed) + plnY * cos(-rotSpeed);
		}
		if (keysHeld[2])
		{
			if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false)
				posX += dirX * moveSpeed;
			if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) 
				posY += dirY * moveSpeed;
		}
		if (keysHeld[3])
		{
			if(worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) 
				posX -= dirX * moveSpeed;
	      	if(worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) 
				posY -= dirY * moveSpeed;
		}
	}
	
	// Tell SDL to clean up
	SDL_Quit();
	
	return 0;
}