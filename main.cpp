// Developed for MSYS2 for Windows (https://www.msys2.org/)
// Install Toolchain (From MSYS2 MSYS terminal):
// pacman -S --needed base-devel mingw-w64-x86_64-toolchain
// Build (From MSYS2 MinGW 64-bit terminal):
//g++ ./main.cpp -o main.exe -mwindows -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

// Copy all dll dependencies to current directory (run from project directory after building)
// (from MSYS2 MinGW 64-bit terminal):
//ldd main.exe | grep '\/mingw.*\.dll' -o | xargs -I{} cp "{}" .

// Run:
//./main.exe
// or double-click from windows file manager.

#include <iostream>
#include <sstream>

// Install required libraries (From MSYS2 MSYS terminal):
// pacman -S mingw64/mingw-w64-x86_64-SDL2
#include <SDL2/SDL.h>
// pacman -S mingw64/mingw-w64-x86_64-SDL2_image
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_main.h>

class AnimManager {
	public:
		int frameCount;
		int currentFrame;
		int imageX;
		int imageY;
		int velX;
		int velY;
		int destImageWidth;
		int destImageHeight;
		AnimManager(int frameCount, int destImageWidth, int destImageHeight);
		void Tick(SDL_Window* window);
		void Reset();
};

AnimManager::AnimManager(int frameCount, int destImageWidth, int destImageHeight) {
	this->frameCount = frameCount;
	this->currentFrame = 0;
	// X and Y velocity of the image. Expressed in pixels/Tick.
	this->velX = 3;
	this->velY = 3;
	this->destImageWidth = destImageWidth;
	this->destImageHeight = destImageHeight;
	// Start with the image offscreen.
	this->imageX = 0 - this->destImageWidth;
	this->imageY = 0 - this->destImageHeight;
}

// Updates the target render location and advances the animation frame.
void AnimManager::Tick(SDL_Window* window) {
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	int maxWidth = windowWidth - destImageWidth;
	int maxHeight = windowHeight - destImageHeight;
	
	int imageXNew = imageX + velX;
	int imageYNew = imageY + velY;
	
	// Bounce detection for x-axis.
	if (velX < 0) {
		// Image is moving left.
		if (imageXNew < 0) {
			// Image would be at least partially off the left of the window.
			// Bounce to the corrected position.
			imageXNew = -imageXNew;
			// Reverse direction.
			velX = -velX;
		}
	} else if (velX > 0) {
		// Image is moving right.
		if (imageXNew >= maxWidth) {
			// Image would be at least partially off the right of the window.
			int overflow = (imageXNew - maxWidth);
			// Bounce to the corrected position.
			imageXNew = maxWidth + overflow;
			// Reverse direction.
			velX = -velX;
		}
	}
	
	// Bounce detection for y-axis.
	if (velY < 0) {
		// Image is moving up.
		if (imageYNew < 0) {
			// Image would be at least partially above the window.
			// Bounce to the corrected position.
			imageYNew = -imageYNew;
			// Reverse direction.
			velY = -velY;
		}
	} else if (velY > 0) {
		// Image is moving down.
		if (imageYNew >= maxHeight) {
			// Image would be at least partially below the window.
			int overflow = (imageYNew - maxHeight);
			// Bounce to the corrected position.
			imageYNew = maxHeight + overflow;
			// Reverse direction.
			velY = -velY;
		}
	}
	
	// Clamp to bounds if necessary.
	// (Can occur when window is resized to something smaller.)
	imageXNew = (imageXNew >= maxWidth) ? maxWidth : imageXNew;
	imageYNew = (imageYNew >= maxHeight) ? maxHeight : imageYNew;
	
	// Update to new values.
	imageX = imageXNew;
	imageY = imageYNew;
	
	int nextFrameIndex = currentFrame + 1;
	// Wrap back to the first frame after the end.
	currentFrame = (nextFrameIndex < frameCount) ? nextFrameIndex : 0;
}

// Resets the animation to the default position and frame.
void AnimManager::Reset() {
	// Start at the first frame.
	currentFrame = 0;
	// Start with the image offscreen.
	imageX = 0 - destImageWidth;
	imageY = 0 - destImageHeight;
	// Reset velocity.
	this->velX = 3;
	this->velY = 3;
}

// These dimensions are updated once the animation images are loaded.
// Source images for the animation are assumed to all be the same size.
int sourceImageWidth = 1;
int sourceImageHeight = 1;
// Target width and height that the animation will be rendered at.
int destImageWidth = 1;
int destImageHeight = 1;

const int numAnimFrames = 8;
SDL_Texture* animFrames[numAnimFrames];

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

AnimManager* animManager = NULL;

// Rendering loop runs at 30Hz.  Event loop runs more frequently.
int framerate = 30;

// Returns true while the idle animation should be played.
bool canWeContinue = true;
bool CanWeContinue() {
	// Indicate idle animation should be played.
	// TODO: Replace with final logic once available.
	return canWeContinue;
}

// Draws the frame at (zero-based) index "frame".
// x and y are the coordinates that the image's top-left corner will appear.
// Assumes screen coordinates (0,0) denote the top-left corner of the screen.
void Render(int frame, int x, int y) {
	SDL_Rect srcRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = sourceImageWidth;
	srcRect.h = sourceImageHeight;
	SDL_Rect destRect;
	destRect.x = x;
	destRect.y = y;
	destRect.w = animManager->destImageWidth;
	destRect.h = animManager->destImageHeight;
	
	if (frame < numAnimFrames) {
		// Valid frame available for rendering.
		SDL_Texture* texture = animFrames[frame];
		SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
	}
}

void RenderAnimFrame(AnimManager* animManager) {
	Render(animManager->currentFrame, animManager->imageX, animManager->imageY);
}

// Handle loading textures into a suitable format for SDL.
SDL_Texture* PrepareImage(SDL_Renderer* renderer, std::string imageFilePath) {
	SDL_Texture* texture = NULL;
	
	// Load image from file to surface.
	SDL_Surface* surface = IMG_Load(imageFilePath.c_str());
	if (NULL == surface) {
		std::cout << "Image load failure." << std::endl;
		std::cout << imageFilePath << std::endl;
		return NULL;
	}
	// Convent surface to a renderable texture.
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	// Free memory for the original surface.
	SDL_FreeSurface(surface);
	
	return texture;
}

int main(int argc, char* argv[]) {
	// Indicates the main event loop should continue running,
	// when set to false the application exits at the start of the next loop.
	bool keepRunning = true;
	
	int screenWidth = 800;
	int screenHeight = 600;
	
	
	SDL_Init(SDL_INIT_EVERYTHING);
	if (0 == IMG_Init(IMG_INIT_PNG)) {
		std::cout << "PNG image library error." << std::endl;
		return 1;
	}
	
	window = SDL_CreateWindow(
		"Screen Saver Demo", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (NULL != window) {
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (NULL == renderer) {
			std::cout << "Renderer creation failure." << std::endl;
			return 4;
		}
	} else {
		std::cout << "Window creation error." << std::endl;
		return 2;
	}

	for (int i = 0; i < numAnimFrames; ++i) {
		std::stringstream ss;
		// Frames are numbered starting from 1 with no leading zeros.
		int imageNumber = (i + 1);
		ss << "assets/Test" << imageNumber << ".png";
		std::string imageFilePath = ss.str();
		SDL_Texture* imageNew = PrepareImage(renderer, imageFilePath);
		
		if (NULL == imageNew) {
			std::cout << "Failed to load animation frame: " << imageNumber << ". Exiting." << std::endl;
			return 3;
		}
		
		animFrames[i] = imageNew;
	}
	
	// Get image attributes.
	if (0 == numAnimFrames) {
		std::cout << "Zero animation frames defined for screen saver logo." << std::endl;
		return 5;
	};
	
	SDL_Texture* texture = animFrames[0];
	SDL_QueryTexture(texture, NULL, NULL,  &sourceImageWidth, &sourceImageHeight);
	//std::cout << "Animation image dimensions: " << textureRect.w << " x " << textureRect.h << std::endl;
	
	// Destination image dimensions will match the source image.
	destImageWidth = sourceImageWidth;
	destImageHeight = sourceImageHeight;
	animManager = new AnimManager(numAnimFrames, destImageWidth, destImageHeight);
	
	// Initialise timing.
	unsigned int ticksPrevious = SDL_GetTicks();
	unsigned int ticksCurrent;
	int deltaThreshold = 1000 / framerate;
	while (keepRunning) {
		ticksCurrent = SDL_GetTicks();
		int timeDelta = ticksCurrent - ticksPrevious;
		// Process event queue.
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
				case SDL_KEYDOWN:
					// Toggle screensaver mode when any key is pressed.
					canWeContinue = !canWeContinue;
					if (canWeContinue) {
						animManager->Reset();
					}
					break;
				case SDL_QUIT:
				    // Signal to exit.
					keepRunning = false;
					break;
				default:
					break;
			}
		}
		
		// Check keyboard input.
		// Any keypress will toggle the rendering mode.
		
		
		// Advance state.
		if (timeDelta < deltaThreshold) {
			//std::cout << timeDelta << std::endl;
			SDL_Delay(1);
			continue;
		}
		
		// Check render mode.
		if (CanWeContinue()) {
			// Render the idle mode animation.
			animManager->Tick(window);
			
			SDL_SetRenderDrawColor(renderer, 64, 128, 128, 255);
			SDL_RenderClear(renderer);
			
			// Render animation frame.
			RenderAnimFrame(animManager);
		} else {
			// Render normally.
			// Clear to black.
			SDL_SetRenderDrawColor(renderer, 64, 128, 128, 255);
			SDL_RenderClear(renderer);
			// TODO: Render normal mode here.
		}
		
		// Swap display buffers.
		SDL_RenderPresent(renderer);
		
		// Manage timing.
		ticksPrevious = ticksCurrent;
	}
	
    SDL_Quit();
	std::cout << "Exiting." << std::endl;
	return 0;
}